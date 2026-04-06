FROM osrf/ros:kinetic-desktop-full

ENV DEBIAN_FRONTEND=noninteractive

# actualizar sistema
RUN apt-get update && apt-get upgrade -y

# herramientas básicas (nada específico aún)
RUN apt-get install -y \
    git \
    nano \
    python-pip \
    build-essential

# inicializar entorno ROS automático
RUN echo "source /opt/ros/kinetic/setup.bash" >> ~/.bashrc


CMD ["bash"]
