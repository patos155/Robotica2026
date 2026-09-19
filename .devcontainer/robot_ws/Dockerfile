FROM ros:humble-ros-base

# Base ROS2 + herramientas de desarrollo (igual que en robot_ws_demo)
# más lo que necesita robot_vision específicamente: cv_bridge para
# convertir entre frames de OpenCV y sensor_msgs/Image, libzbar0 porque
# pyzbar es un wrapper de Python sobre la librería nativa ZBar, y el
# puente hacia el dashboard (rosbridge_suite + web_video_server).
# launch-xml es dependencia del launch file de robot_vision, que incluye
# el rosbridge_websocket_launch.xml de rosbridge_server.
RUN apt-get update && apt-get install -y --no-install-recommends \
    python3-colcon-common-extensions \
    python3-rosdep \
    python3-pip \
    ros-dev-tools \
    ros-humble-cv-bridge \
    ros-humble-image-transport \
    ros-humble-rosbridge-suite \
    ros-humble-web-video-server \
    ros-humble-launch-xml \
    libzbar0 \
    git \
    ros-humble-foxglove-bridge \
    && rm -rf /var/lib/apt/lists/*

# pyzbar no está en el índice de rosdep (no es un paquete ROS), por eso
# va aparte con pip y no en package.xml como exec_depend. qrcode es
# solo para los tests (generar un QR en memoria sin depender de un
# archivo externo en el repo).
RUN pip3 install --no-cache-dir --break-system-packages pyzbar "qrcode[pil]"

RUN rosdep init || true && rosdep update

RUN echo "source /opt/ros/humble/setup.bash" >> /etc/bash.bashrc && \
    echo "if [ -f /workspace/robot_ws/install/setup.bash ]; then source /workspace/robot_ws/install/setup.bash; fi" >> /etc/bash.bashrc

WORKDIR /workspace
