#!/bin/bash
# Karel ROS
# Copyright (C) 2026 Julio Cruz
# SPDX-License-Identifier: GPL-3.0-only

set -eo pipefail

source /catkin_ws/devel/setup.bash

exec roslaunch karel_gazebo karel.launch gui:="${KAREL_GUI:-true}"
