#!/bin/bash
# Karel ROS
# Copyright (C) 2026 Julio Cruz
# SPDX-License-Identifier: GPL-3.0-only

set -eo pipefail

source "/opt/ros/${ROS_DISTRO}/setup.bash"
if [[ -f /workspace/devel/setup.bash ]]; then
  source /workspace/devel/setup.bash
fi

args=(
  --ip=0.0.0.0
  --no-browser
  --NotebookApp.notebook_dir=/workspace
)

if [[ -n "${JUPYTER_TOKEN:-}" ]]; then
  args+=(--NotebookApp.token="$JUPYTER_TOKEN" --NotebookApp.password='')
else
  echo "JUPYTER_TOKEN is not set; Jupyter will generate a random token." >&2
fi

exec jupyter notebook "${args[@]}"
