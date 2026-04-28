# Imagen de ros1 kinetic
FROM osrf/ros:kinetic-desktop-full
ENV DEBIAN_FRONTEND=noninteractive

#  actualiza e instala cosas necesarias
RUN apt-get update && apt-get upgrade -y && apt-get install -y \
    git \
    nano \
    python-pip \
    python-rosinstall \
    build-essential \
    && rm -rf /var/lib/apt/lists/*

RUN rosdep update

#Crea el directorio
WORKDIR root/baxter_ws

RUN mkdir src && \
    wstool init src && \
    wstool merge -t src https://raw.githubusercontent.com/RethinkRobotics/baxter/master/baxter_sdk.rosinstall && \
    wstool update -t src

RUN cp src/baxter/baxter.sh .

# inicializar entorno ROS automático
RUN echo "source /opt/ros/kinetic/setup.bash" >> ~/.bashrc


CMD ["bash"]
