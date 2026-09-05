FROM ros:jazzy-ros-base

# Base ROS2 + herramientas de desarrollo (igual que en robot_ws_demo)
# más lo que necesita robot_vision específicamente: cv_bridge para
# convertir entre frames de OpenCV y sensor_msgs/Image, y libzbar0
# porque pyzbar es un wrapper de Python sobre la librería nativa ZBar.
RUN apt-get update && apt-get install -y --no-install-recommends \
    python3-colcon-common-extensions \
    python3-rosdep \
    python3-pip \
    ros-dev-tools \
    ros-jazzy-cv-bridge \
    ros-jazzy-image-transport \
    ros-jazzy-rosbridge-suite \
    ros-jazzy-web-video-server \
    libzbar0 \
    git \
    ros-jazzy-foxglove-bridge \
    && rm -rf /var/lib/apt/lists/*

# pyzbar no está en el índice de rosdep (no es un paquete ROS), por eso
# va aparte con pip y no en package.xml como exec_depend. qrcode es
# solo para los tests (generar un QR en memoria sin depender de un
# archivo externo en el repo).
RUN pip3 install --no-cache-dir --break-system-packages pyzbar "qrcode[pil]"

RUN rosdep init || true && rosdep update

RUN echo "source /opt/ros/jazzy/setup.bash" >> /etc/bash.bashrc && \
    echo "if [ -f /workspace/robot_ws2/install/setup.bash ]; then source /workspace/robot_ws2/install/setup.bash; fi" >> /etc/bash.bashrc

WORKDIR /workspace
