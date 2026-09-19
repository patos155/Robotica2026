import { computed, onBeforeUnmount, ref } from 'vue'

const ACTION_NAME = '/execute_test'
const ACTION_TYPE = 'robot_interfaces/action/ExecuteTest'
const MAX_RECONNECT_DELAY_MS = 8000

export function useRosbridge() {
  const connectionState = ref('disconnected')
  const feedback = ref('Esperando una prueba')
  const qrResult = ref('')
  const lastError = ref('')
  const isTestRunning = ref(false)
  const events = ref([])

  let socket = null
  let socketUrl = ''
  let activeGoalId = ''
  let reconnectTimer = null
  let reconnectDelayMs = 1000
  let shouldReconnect = true

  const isConnected = computed(() => connectionState.value === 'connected')

  function addEvent(message, level = 'info') {
    events.value = [
      { id: `${Date.now()}-${Math.random()}`, message, level, time: new Date() },
      ...events.value,
    ].slice(0, 6)
  }

  function send(payload) {
    if (!socket || socket.readyState !== WebSocket.OPEN) {
      lastError.value = 'ROS no está conectado.'
      return false
    }
    socket.send(JSON.stringify(payload))
    return true
  }

  function scheduleReconnect() {
    if (!shouldReconnect || reconnectTimer) return
    connectionState.value = 'reconnecting'
    reconnectTimer = window.setTimeout(() => {
      reconnectTimer = null
      openSocket()
    }, reconnectDelayMs)
    reconnectDelayMs = Math.min(reconnectDelayMs * 2, MAX_RECONNECT_DELAY_MS)
  }

  function parseJsonField(value, fallback = {}) {
    if (typeof value !== 'string') return value ?? fallback
    try {
      return JSON.parse(value)
    } catch {
      return fallback
    }
  }

  function handleMessage(event) {
    let message
    try {
      message = JSON.parse(event.data)
    } catch {
      addEvent('ROS envió un mensaje no válido.', 'error')
      return
    }

    if (message.op === 'action_feedback' && message.id === activeGoalId) {
      const data = parseJsonField(message.values?.feedback_json)
      feedback.value = data.status || 'Procesando imagen…'
      return
    }

    if (message.op === 'action_result' && message.id === activeGoalId) {
      isTestRunning.value = false
      const result = message.values || {}
      const data = parseJsonField(result.result_json)

      if (message.result && result.success) {
        qrResult.value = data.qr_text || 'QR detectado sin texto'
        feedback.value = 'Lectura terminada'
        addEvent(`QR detectado: ${qrResult.value}`, 'success')
      } else {
        const reason = data.reason || data.error || 'La prueba no terminó correctamente'
        feedback.value = reason
        addEvent(`Prueba QR: ${reason}`, 'warning')
      }
      activeGoalId = ''
      return
    }

    if (message.op === 'status' && message.level === 'error') {
      lastError.value = message.msg || 'Error reportado por rosbridge.'
      addEvent(lastError.value, 'error')
    }
  }

  function openSocket() {
    if (!socketUrl) return

    if (socket && [WebSocket.OPEN, WebSocket.CONNECTING].includes(socket.readyState)) {
      socket.close()
    }

    connectionState.value = 'connecting'
    lastError.value = ''
    const currentSocket = new WebSocket(socketUrl)
    socket = currentSocket

    currentSocket.addEventListener('open', () => {
      if (socket !== currentSocket) return
      connectionState.value = 'connected'
      reconnectDelayMs = 1000
      addEvent(`Conectado a ${socketUrl}`, 'success')
    })

    currentSocket.addEventListener('message', handleMessage)

    currentSocket.addEventListener('error', () => {
      if (socket !== currentSocket) return
      lastError.value = `No se pudo conectar con ${socketUrl}`
    })

    currentSocket.addEventListener('close', () => {
      if (socket !== currentSocket) return
      socket = null
      isTestRunning.value = false
      activeGoalId = ''
      addEvent('Conexión con ROS interrumpida.', 'warning')
      scheduleReconnect()
    })
  }

  function connect(url) {
    socketUrl = url
    shouldReconnect = true
    reconnectDelayMs = 1000
    if (reconnectTimer) {
      window.clearTimeout(reconnectTimer)
      reconnectTimer = null
    }
    openSocket()
  }

  function disconnect() {
    shouldReconnect = false
    if (reconnectTimer) {
      window.clearTimeout(reconnectTimer)
      reconnectTimer = null
    }
    if (socket) {
      const currentSocket = socket
      socket = null
      currentSocket.close()
    }
    connectionState.value = 'disconnected'
    isTestRunning.value = false
    activeGoalId = ''
    addEvent('Conexión cerrada por el operador.')
  }

  function startQrTest() {
    const goalId = globalThis.crypto?.randomUUID?.() || `qr-${Date.now()}`
    const sent = send({
      op: 'send_action_goal',
      id: goalId,
      action: ACTION_NAME,
      action_type: ACTION_TYPE,
      args: { test_name: 'qr' },
      feedback: true,
    })
    if (!sent) return

    activeGoalId = goalId
    isTestRunning.value = true
    qrResult.value = ''
    feedback.value = 'Buscando código QR…'
    addEvent('Prueba QR iniciada.')
  }

  function cancelQrTest() {
    if (!activeGoalId) return
    if (send({ op: 'cancel_action_goal', id: activeGoalId, action: ACTION_NAME })) {
      feedback.value = 'Cancelando prueba…'
      addEvent('Cancelación solicitada.', 'warning')
    }
  }

  onBeforeUnmount(disconnect)

  return {
    connectionState,
    feedback,
    qrResult,
    lastError,
    isTestRunning,
    isConnected,
    events,
    connect,
    disconnect,
    startQrTest,
    cancelQrTest,
  }
}
