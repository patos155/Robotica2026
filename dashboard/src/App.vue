<script setup>
import { computed, onMounted, ref, watch } from 'vue'
import { useRosbridge } from './composables/useRosbridge'

const envHost = import.meta.env.VITE_ROS_HOST
const defaultHost = envHost || window.location.hostname || 'localhost'

const host = ref(localStorage.getItem('robot-host') || defaultHost)
const rosbridgePort = ref(import.meta.env.VITE_ROSBRIDGE_PORT || '9090')
const videoPort = ref(import.meta.env.VITE_VIDEO_PORT || '8080')
const videoTopic = ref(import.meta.env.VITE_VIDEO_TOPIC || '/camera/image_raw')
const videoReady = ref(false)
const videoFailed = ref(false)
const videoRevision = ref(0)

const {
  connectionState,
  feedback,
  testResult,
  lastError,
  isTestRunning,
  isConnected,
  events,
  connect,
  disconnect,
  startTest,
  cancelTest,
} = useRosbridge()

const resultDialog = ref(null)

const socketUrl = computed(() => `ws://${host.value.trim()}:${rosbridgePort.value}`)
const videoUrl = computed(() => {
  // ros_compressed reenvia el JPEG de image_compressor tal cual (sin
  // recomprimir, sin plugin de image_transport); width/quality no aplican.
  // sensor_data = BEST_EFFORT, igual que CAMERA_QOS; con el default (RELIABLE)
  // el subscriber nunca recibe frames.
  const query = new URLSearchParams({
    topic: videoTopic.value,
    type: 'ros_compressed',
    qos_profile: 'sensor_data',
    client_id: `dashboard-${videoRevision.value}`,
  })
  // web_video_server (Humble) no decodifica %2F en el nombre del topic y lo
  // rechaza como invalido; un "/" literal es valido en un query string.
  return `http://${host.value.trim()}:${videoPort.value}/stream?${query.toString().replaceAll('%2F', '/')}`
})

const statusLabel = computed(() => ({
  connected: 'ROS conectado',
  connecting: 'Conectando…',
  reconnecting: 'Reconectando…',
  disconnected: 'Sin conexión',
}[connectionState.value]))

const formattedEvents = computed(() => events.value.map((event) => ({
  ...event,
  displayTime: event.time.toLocaleTimeString('es-MX', {
    hour: '2-digit',
    minute: '2-digit',
    second: '2-digit',
  }),
})))

function applyConnection() {
  localStorage.setItem('robot-host', host.value.trim())
  videoReady.value = false
  videoFailed.value = false
  videoRevision.value += 1
  connect(socketUrl.value)
}

function retryVideo() {
  videoReady.value = false
  videoFailed.value = false
  videoRevision.value += 1
}

watch(videoUrl, () => {
  videoReady.value = false
  videoFailed.value = false
})

// flush 'post': el contenido del <dialog> (v-if) ya esta renderizado al abrirlo.
watch(testResult, (result) => {
  if (result && !resultDialog.value.open) resultDialog.value.showModal()
}, { flush: 'post' })

onMounted(applyConnection)
</script>

<template>
  <main class="app-shell">
    <header class="topbar">
      <div class="brand">
        <span class="brand-mark" aria-hidden="true">RC</span>
        <div>
          <p class="eyebrow">TMR 2027 · SISTEMA DE INSPECCIÓN</p>
          <h1>Robot Vision</h1>
        </div>
      </div>

      <div class="connection-pill" :class="connectionState" role="status">
        <span class="status-dot" aria-hidden="true"></span>
        {{ statusLabel }}
      </div>
    </header>

    <div v-if="!isConnected" class="connection-banner" role="alert">
      <strong>No hay comunicación con el robot.</strong>
      <span>La navegación local no se detiene; revisa la dirección o inicia rosbridge.</span>
    </div>

    <section class="dashboard-grid">
      <article class="camera-card panel">
        <div class="panel-heading">
          <div>
            <p class="section-label">CÁMARA PRINCIPAL</p>
            <h2>Inspección en vivo</h2>
          </div>
          <span class="live-label" :class="{ active: videoReady }">
            <span></span>{{ videoReady ? 'EN VIVO' : 'SIN VIDEO' }}
          </span>
        </div>

        <div class="video-stage">
          <img
            :key="videoRevision"
            :src="videoUrl"
            alt="Transmisión de la cámara de inspección del robot"
            @load="videoReady = true; videoFailed = false"
            @error="videoReady = false; videoFailed = true"
          />
          <div v-if="!videoReady" class="video-placeholder">
            <span class="scanner-frame" aria-hidden="true"></span>
            <p>{{ videoFailed ? 'No se encontró el servidor de video' : 'Esperando señal de cámara…' }}</p>
            <button v-if="videoFailed" class="text-button" type="button" @click="retryVideo">
              Reintentar video
            </button>
          </div>
          <div class="camera-meta">
            <span>TOPIC</span>
            <code>{{ videoTopic }}</code>
          </div>
        </div>
      </article>

      <aside class="control-stack">
        <article class="panel qr-panel">
          <div class="panel-heading compact">
            <div>
              <p class="section-label">LECTOR ÓPTICO</p>
              <h2>Prueba QR</h2>
            </div>
            <span class="qr-icon" aria-hidden="true">⌗</span>
          </div>

          <div class="test-status" :class="{ scanning: isTestRunning, success: testResult?.found }">
            <span class="test-indicator" aria-hidden="true"></span>
            <div>
              <small>ESTADO</small>
              <p>{{ feedback }}</p>
            </div>
          </div>

          <div class="button-row">
            <button
              class="primary-button"
              type="button"
              :disabled="!isConnected || isTestRunning"
              @click="startTest('qr')"
            >
              {{ isTestRunning ? 'Escaneando…' : 'Iniciar lectura QR' }}
            </button>
            <button
              v-if="isTestRunning"
              class="secondary-button danger"
              type="button"
              @click="cancelTest"
            >
              Cancelar
            </button>
          </div>
        </article>

        <article class="panel settings-panel">
          <div class="panel-heading compact">
            <div>
              <p class="section-label">ENLACE DE RED</p>
              <h2>Conexión ROS</h2>
            </div>
          </div>

          <form @submit.prevent="applyConnection">
            <label>
              IP o nombre del robot
              <input v-model="host" name="host" autocomplete="off" placeholder="192.168.4.1" />
            </label>
            <div class="port-grid">
              <label>
                WebSocket
                <input v-model="rosbridgePort" name="rosbridgePort" inputmode="numeric" />
              </label>
              <label>
                Video HTTP
                <input v-model="videoPort" name="videoPort" inputmode="numeric" />
              </label>
            </div>
            <div class="button-row">
              <button class="secondary-button" type="submit">Aplicar y conectar</button>
              <button v-if="isConnected" class="text-button" type="button" @click="disconnect">
                Desconectar
              </button>
            </div>
          </form>
          <p v-if="lastError" class="error-copy">{{ lastError }}</p>
        </article>
      </aside>
    </section>

    <section class="panel activity-panel">
      <div class="panel-heading compact">
        <div>
          <p class="section-label">DIAGNÓSTICO</p>
          <h2>Actividad reciente</h2>
        </div>
        <code>{{ socketUrl }}</code>
      </div>
      <ul v-if="formattedEvents.length" class="event-list">
        <li v-for="event in formattedEvents" :key="event.id" :class="event.level">
          <time>{{ event.displayTime }}</time>
          <span>{{ event.message }}</span>
        </li>
      </ul>
      <p v-else class="empty-state">Los eventos de conexión y lectura aparecerán aquí.</p>
    </section>

    <dialog ref="resultDialog" class="result-modal" aria-labelledby="result-title">
      <template v-if="testResult">
        <div class="panel-heading compact">
          <div>
            <p class="section-label">RESULTADO</p>
            <h2 id="result-title">{{ testResult.label }}</h2>
          </div>
        </div>
        <img v-if="testResult.image" :src="testResult.image" alt="Captura anotada de la prueba" />
        <div class="result-text" :class="{ empty: !testResult.found }" role="status">
          <small>{{ testResult.found ? 'CONTENIDO DETECTADO' : 'SIN RESULTADO' }}</small>
          <strong>{{ testResult.text }}</strong>
        </div>
        <form method="dialog" class="button-row">
          <button class="primary-button" autofocus>Cerrar</button>
        </form>
      </template>
    </dialog>
  </main>
</template>
