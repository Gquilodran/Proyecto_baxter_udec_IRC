FROM osrf/ros:kinetic-desktop-full

ENV DEBIAN_FRONTEND=noninteractive

# actualizar sistema
RUN apt-get update && apt-get upgrade -y

# herramientas básicas (nada específico aún)
RUN apt-get install -y \
    git \
    nano \
    python-pip \
    python-rosinstall \
    build-essential

RUN sudo rosdep init \
    rosdep update

#Crea el directorio
RUN mkdir -p ~/baxter_ws/src  \
    wstool init .
    wstool merge https://raw.githubusercontent.com/RethinkRobotics/baxter/master/baxter_sdk.rosinstall \
    wstool update   

RUN cd baxter_ws \
    cp src/baxter /baxter.sh ./

# inicializar entorno ROS automático
RUN echo "source /opt/ros/kinetic/setup.bash" >> ~/.bashrc


CMD ["bash"]
