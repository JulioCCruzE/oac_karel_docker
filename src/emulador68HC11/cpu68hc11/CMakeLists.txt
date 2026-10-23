# Emulador Motorola 68HC11
# Copyright (C) 2026 Julio Cruz
# Licensed under the GNU GPL v3.

cmake_minimum_required(VERSION 3.0.2)
project(cpu68hc11)

find_package(catkin REQUIRED COMPONENTS
  roscpp
)

catkin_package(
  INCLUDE_DIRS src
  LIBRARIES ${PROJECT_NAME}
  CATKIN_DEPENDS roscpp
)

add_library(${PROJECT_NAME}
  src/CPU68HC11.cpp
  src/bus/AddressBus16.cpp
  src/bus/DataBus16.cpp
  src/bus/DataBusQueries.cpp
  src/bus/DataBusUpa.cpp
  src/memory/AccessRegister.cpp
  src/memory/Memory.cpp
  src/register/Ccr.cpp
  src/register/Counter16.cpp
  src/register/Index16.cpp
  src/register/Registro8.cpp
  src/sequencer/InstructionRegister.cpp
  src/sequencer/LogicaInterna.cpp
  src/sequencer/SelectorPrueba.cpp
  src/uci/Uci.cpp
  src/upa/Upa.cpp
)

target_include_directories(${PROJECT_NAME}
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${catkin_INCLUDE_DIRS}
)
