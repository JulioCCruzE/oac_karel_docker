# Karel ROS
# Copyright (C) 2026 Julio Cruz
# SPDX-License-Identifier: GPL-3.0-only

cmake_minimum_required(VERSION 2.8.3)
project(karel_gui)

add_compile_options(-std=c++11)

find_package(catkin REQUIRED COMPONENTS
  gazebo_msgs
  geometry_msgs
  nav_msgs
  roscpp
  sensor_msgs
  std_msgs
  std_srvs
)
find_package(Qt5Widgets REQUIRED)

catkin_package(
  CATKIN_DEPENDS gazebo_msgs geometry_msgs nav_msgs roscpp sensor_msgs std_msgs std_srvs
)

include_directories(${catkin_INCLUDE_DIRS})

add_executable(karel_gui_node src/karel_gui_node.cpp)
target_link_libraries(karel_gui_node Qt5::Widgets ${catkin_LIBRARIES})

install(TARGETS karel_gui_node
  RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)
install(DIRECTORY launch
  DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}
)
