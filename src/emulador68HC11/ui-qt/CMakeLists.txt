# Emulador Motorola 68HC11
# Copyright (C) 2026 Julio Cruz
# Licensed under the GNU GPL v3.

cmake_minimum_required(VERSION 3.0.2)
project(emulador68hc11)

add_compile_options(-std=c++11)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(catkin REQUIRED COMPONENTS
  cpu68hc11
  geometry_msgs
  roscpp
  std_msgs
  sensor_msgs
)
find_package(Qt5Widgets REQUIRED)

catkin_package(
  CATKIN_DEPENDS cpu68hc11 geometry_msgs roscpp std_msgs
)

include_directories(
  ${catkin_INCLUDE_DIRS}
)

add_executable(emulador68hc11
  main.cpp
  mainwindow.cpp
  mainwindow.h
  mainwindow.ui
  aboutdialog.cpp
  aboutdialog.h
  widgets/RegistersWidget/registerswidget.h
  widgets/RegistersWidget/registerswidget.cpp
  widgets/RegistersWidget/registerswidget.ui
  widgets/RamWidget/ramwidget.h
  widgets/RamWidget/ramwidget.cpp
  widgets/RamWidget/ramwidget.ui
  widgets/RamWidget/rammodel.h
  widgets/RamWidget/rammodel.cpp
  widgets/RamWidget/hexbytedelegate.h
  widgets/RamWidget/hexbytedelegate.cpp
  widgets/RomWidget/romwidget.h
  widgets/RomWidget/romwidget.cpp
  widgets/RomWidget/romwidget.ui
  widgets/CentralWidget/centralwidget.h
  widgets/CentralWidget/centralwidget.cpp
  widgets/CentralWidget/centralwidget.ui
  widgets/CentralWidget/ExecWidget/execwidget.h
  widgets/CentralWidget/ExecWidget/execwidget.cpp
  widgets/CentralWidget/ExecWidget/execwidget.ui
  widgets/CentralWidget/UpaWidget/upawidget.h
  widgets/CentralWidget/UpaWidget/upawidget.cpp
  widgets/CentralWidget/UpaWidget/upawidget.ui
)

target_link_libraries(emulador68hc11
  Qt5::Widgets
  ${catkin_LIBRARIES}
)

install(TARGETS emulador68hc11
  RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)
