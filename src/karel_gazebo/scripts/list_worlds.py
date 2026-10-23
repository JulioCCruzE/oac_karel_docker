#!/usr/bin/env python2
# Karel ROS
# Copyright (C) 2026 Julio Cruz
# SPDX-License-Identifier: GPL-3.0-only


from __future__ import print_function

import glob
import os

import rospkg


def list_package_worlds(package_name):
    package_path = rospkg.RosPack().get_path(package_name)
    for path in sorted(glob.glob(os.path.join(package_path, "worlds", "*.world"))):
        print("{0:28s} {1}".format(os.path.splitext(os.path.basename(path))[0], path))


if __name__ == "__main__":
    print("Karel circuits:")
    list_package_worlds("karel_gazebo")
