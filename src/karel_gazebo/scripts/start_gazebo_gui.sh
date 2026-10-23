#!/bin/bash
# Karel ROS
# Copyright (C) 2026 Julio Cruz
# SPDX-License-Identifier: GPL-3.0-only

set -eo pipefail

for attempt in {1..60}; do
  if rosservice call /gazebo/get_world_properties >/dev/null 2>&1; then
    break
  fi
  sleep 1
done

if ! rosservice call /gazebo/get_world_properties >/dev/null 2>&1; then
  echo "Gazebo server did not become ready within 60 seconds." >&2
  exit 1
fi

for attempt in {1..30}; do
  if glxinfo -B >/tmp/karel-glxinfo.log 2>&1; then
    exec gzclient "$@"
  fi
  sleep 1
done

echo "OpenGL is unavailable on DISPLAY=${DISPLAY:-unset}; Gazebo server will remain running." >&2
cat /tmp/karel-glxinfo.log >&2 || true
exit 1
