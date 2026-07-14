# Imagen de ros1 indigo oficial de OSRF
FROM osrf/ros:indigo-desktop-full
ENV DEBIAN_FRONTEND=noninteractive

# 1. Limpieza total de cualquier configuración de red vieja de Ubuntu y ROS
RUN rm -rf /etc/apt/sources.list.d/* && echo "" > /etc/apt/sources.list

# 2. Forzar TODOS los repositorios históricos necesarios de Ubuntu Trusty
RUN echo "deb http://old-releases.ubuntu.com/ubuntu trusty main restricted universe multiverse" >> /etc/apt/sources.list && \
    echo "deb http://old-releases.ubuntu.com/ubuntu trusty-updates main restricted universe multiverse" >> /etc/apt/sources.list && \
    echo "deb http://old-releases.ubuntu.com/ubuntu trusty-security main restricted universe multiverse" >> /etc/apt/sources.list

# 3. Forzar el repositorio histórico de ROS Indigo 
RUN echo "deb http://snapshots.ros.org/indigo/final/ubuntu trusty main" >> /etc/apt/sources.list.d/ros-latest.list

# 4. Actualizar ignorando las llaves expiradas y certificados viejos
RUN apt-get update -o Acquire::Check-Valid-Until=false --allow-unauthenticated

# 5. Instalar las herramientas críticas para el Baxter
RUN apt-get install -y --allow-unauthenticated \
    git \
    nano \
    python-pip \
    python-rosinstall \
    build-essential \
    && rm -rf /var/lib/apt/lists/*

# 6. Actualizar las dependencias de ROS
RUN rosdep update

# Configuración del entorno de trabajo del robot
WORKDIR /root/baxter_ws

RUN mkdir src && \
    wstool init src && \
    wstool merge -t src https://raw.githubusercontent.com/RethinkRobotics/baxter/master/baxter_sdk.rosinstall && \
    wstool update -t src

RUN cp src/baxter/baxter.sh .

RUN echo "source /opt/ros/indigo/setup.bash" >> ~/.bashrc

CMD ["bash"]