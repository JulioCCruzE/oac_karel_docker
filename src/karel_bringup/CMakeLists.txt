# Karel ROS
# Copyright (C) 2026 Julio Cruz
# SPDX-License-Identifier: GPL-3.0-only

cmake_minimum_required(VERSION 2.8.3)
project(karel_bringup)

find_package(catkin REQUIRED)
catkin_package()

install(DIRECTORY launch rviz
  DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}
)
