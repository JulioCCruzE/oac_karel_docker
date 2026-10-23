# Karel ROS
# Copyright (C) 2026 Julio Cruz
# SPDX-License-Identifier: GPL-3.0-only

cmake_minimum_required(VERSION 2.8.3)
project(karel_evaluation)

add_compile_options(-std=c++11)

find_package(catkin REQUIRED COMPONENTS
  gazebo_msgs
  roscpp
  std_msgs
  std_srvs
)
find_package(gazebo REQUIRED)

catkin_package(CATKIN_DEPENDS gazebo_msgs roscpp std_msgs std_srvs)

include_directories(${catkin_INCLUDE_DIRS} ${GAZEBO_INCLUDE_DIRS})
link_directories(${GAZEBO_LIBRARY_DIRS})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GAZEBO_CXX_FLAGS}")

add_executable(karel_score_counter src/karel_score_counter.cpp)
target_link_libraries(karel_score_counter ${catkin_LIBRARIES} ${GAZEBO_LIBRARIES})

install(TARGETS karel_score_counter
  RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)
install(DIRECTORY config launch
  DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}
)
