# Karel ROS
# Copyright (C) 2026 Julio Cruz
# SPDX-License-Identifier: GPL-3.0-only

cmake_minimum_required(VERSION 2.8.3)
project(karel_teleop)

find_package(catkin REQUIRED COMPONENTS geometry_msgs rospy sensor_msgs)

catkin_package(CATKIN_DEPENDS geometry_msgs rospy sensor_msgs)

catkin_install_python(PROGRAMS scripts/karel_joystick.py
  DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)
