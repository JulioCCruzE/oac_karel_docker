# Karel ROS
# Copyright (C) 2026 Julio Cruz
# SPDX-License-Identifier: GPL-3.0-only

cmake_minimum_required(VERSION 2.8.3)
project(karel_gazebo)

find_package(catkin REQUIRED)

catkin_package()

install(DIRECTORY launch urdf worlds
  DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}
)

catkin_install_python(PROGRAMS
  scripts/list_worlds.py
  scripts/spawn_circuit_obstacles.py
  DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)

install(FILES scripts/circuit_layout.py
  DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)

install(PROGRAMS scripts/start_gazebo_gui.sh
  DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)

if(CATKIN_ENABLE_TESTING)
  catkin_add_nosetests(test/test_circuit_layout.py)
endif()
