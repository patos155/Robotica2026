<script setup>
import { computed, onMounted, ref, watch } from 'vue'
import { useRosbridge } from './composables/useRosbridge'

const envHost = import.meta.env.VITE_ROS_HOST
const defaultHost = envHost || window.location.hostname || 'localhost'

const host = ref(localStorage.getItem('robot-host') || defaultHost)
const rosbridgePort = ref(import.meta.env.VITE_ROSBRIDGE_PORT || '9090')
const videoPort = ref(import.meta.env.VITE_VIDEO_PORT || '8080')
const videoTopic = ref(import.meta.env.VITE_VIDEO_TOPIC || '/inspection/image_processed')
const videoReady = ref(false)
const videoFailed = ref(false)
const videoRevision = ref(0)

const {
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
} = useRosbridge()

const socketUrl = computed(() => `ws://${host.value.trim()}:${rosbridgePort.value}`)
const videoUrl = computed(() => {
  const query = new URLSearchParams({
    topic: videoTopic.value,
    type: 'mjpeg',
    width: '1280',
    quality: '78',
    client_id: `dashboard-cam-${videoRevision.value}`,
  })
  return `http://${host.value.trim()}:${videoPort.value}/stream?${query}`
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

          <div class="test-status" :class="{ scanning: isTestRunning, success: qrResult }">
            <span class="test-indicator" aria-hidden="true"></span>
            <div>
              <small>ESTADO</small>
              <p>{{ feedback }}</p>
            </div>
          </div>

          <div v-if="qrResult" class="qr-result" aria-live="polite">
            <small>CONTENIDO DETECTADO</small>
            <strong>{{ qrResult }}</strong>
          </div>

          <div class="button-row">
            <button
              class="primary-button"
              type="button"
              :disabled="!isConnected || isTestRunning"
              @click="startQrTest"
            >
              {{ isTestRunning ? 'Escaneando…' : 'Iniciar lectura QR' }}
            </button>
            <button
              v-if="isTestRunning"
              class="secondary-button danger"
              type="button"
              @click="cancelQrTest"
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
  </main>
</template>
