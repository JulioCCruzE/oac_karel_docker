FROM ros:melodic-ros-base@sha256:b4366ca0536e2ad3654ed66fb4475bc43b7a0c15bd8a5be91fd5ac9dfaf935d0

SHELL ["/bin/bash", "-c"]
ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install --no-install-recommends -y \
    gazebo9 \
    libgazebo9-dev \
    mesa-utils \
    ros-melodic-gazebo-plugins \
    ros-melodic-gazebo-ros \
    ros-melodic-robot-state-publisher \
    ros-melodic-rostopic \
    ros-melodic-xacro \
    supervisor \
    && rm -rf /var/lib/apt/lists/*

RUN groupadd --gid 1000 karel \
    && useradd --uid 1000 --gid 1000 --create-home --shell /bin/bash karel \
    && mkdir -p /catkin_ws/src \
    && chown -R karel:karel /catkin_ws
WORKDIR /catkin_ws

COPY dockerfile/entrypoint.sh /entrypoint.sh
COPY --chmod=755 dockerfile/start-simulator.sh /start-simulator.sh
COPY dockerfile/supervisord.conf /etc/supervisor/supervisord.conf

USER karel
ENTRYPOINT ["/entrypoint.sh"]
CMD ["/usr/bin/supervisord", "-n", "-c", "/etc/supervisor/supervisord.conf"]
