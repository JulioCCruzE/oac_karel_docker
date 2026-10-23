#!/usr/bin/env python2
# Karel ROS
# Copyright (C) 2026 Julio Cruz
# SPDX-License-Identifier: GPL-3.0-only


from __future__ import print_function

import argparse
import time

import rospy
from gazebo_msgs.srv import DeleteModel, GetWorldProperties, SpawnModel
from geometry_msgs.msg import Pose
from std_srvs.srv import Empty

from circuit_layout import X_SECTIONS, generate_obstacle_positions


OBSTACLE_PREFIX = "circuit_obstacle_"
GAZEBO_SERVICES = (
    "/gazebo/get_world_properties",
    "/gazebo/delete_model",
    "/gazebo/spawn_sdf_model",
)


def obstacle_sdf(name, dynamic):
    static = "false" if dynamic else "true"
    inertial = """
        <inertial>
          <mass>0.10</mass>
          <inertia><ixx>0.00017</ixx><iyy>0.00014</iyy><izz>0.00021</izz></inertia>
        </inertial>""" if dynamic else ""
    return """<?xml version="1.0"?>
<sdf version="1.5">
  <model name="{name}">
    <static>{static}</static>
    <link name="link">
      {inertial}
      <collision name="collision">
        <geometry><box><size>0.08 0.10 0.10</size></box></geometry>
      </collision>
      <visual name="visual">
        <geometry><box><size>0.08 0.10 0.10</size></box></geometry>
        <material><ambient>0.95 0.55 0.05 1</ambient><diffuse>0.95 0.55 0.05 1</diffuse></material>
      </visual>
    </link>
  </model>
</sdf>""".format(name=name, static=static, inertial=inertial)


def model_names(world):
    response = world()
    if not response.success:
        raise RuntimeError("Could not inspect Gazebo world: {0}".format(
            response.status_message))
    return response.model_names


def wait_for_model(world, name, timeout):
    deadline = time.time() + timeout
    while time.time() < deadline and not rospy.is_shutdown():
        if name in model_names(world):
            return
        time.sleep(0.1)
    raise RuntimeError("Timed out waiting for Gazebo model '{0}'".format(name))


def clear_obstacles(world, delete, timeout):
    deleted = []
    for name in model_names(world):
        if name.startswith(OBSTACLE_PREFIX):
            response = delete(name)
            if not response.success:
                raise RuntimeError("Could not delete {0}: {1}".format(
                    name, response.status_message))
            deleted.append(name)

    if not deleted:
        return 0

    remaining = set(deleted)
    deadline = time.time() + timeout
    while time.time() < deadline and not rospy.is_shutdown():
        remaining = set(deleted).intersection(model_names(world))
        if not remaining:
            return len(deleted)
        time.sleep(0.1)
    raise RuntimeError("Timed out deleting Gazebo models: {0}".format(
        ", ".join(sorted(remaining))))


def main():
    parser = argparse.ArgumentParser(description="Spawn Karel circuit obstacles.")
    parser.add_argument("--seed", type=int, default=1)
    parser.add_argument("--count", type=int, default=3)
    parser.add_argument("--dynamic", action="store_true")
    parser.add_argument("--clear", action="store_true",
                        help="Only remove existing circuit obstacles.")
    parser.add_argument("--wait-for-model",
                        help="Wait for this Gazebo model before modifying the world.")
    parser.add_argument("--timeout", type=float, default=30.0,
                        help="Maximum seconds to wait for Gazebo operations.")
    args = parser.parse_args(rospy.myargv()[1:])

    if args.count < 1 or args.count > len(X_SECTIONS):
        parser.error("--count must be between 1 and {0}".format(len(X_SECTIONS)))
    if args.timeout <= 0:
        parser.error("--timeout must be greater than zero")

    rospy.init_node("spawn_circuit_obstacles", anonymous=True)
    for service_name in GAZEBO_SERVICES:
        try:
            rospy.wait_for_service(service_name, timeout=args.timeout)
        except rospy.ROSException:
            raise RuntimeError("Timed out waiting for service {0}".format(
                service_name))
    world = rospy.ServiceProxy("/gazebo/get_world_properties", GetWorldProperties)
    delete = rospy.ServiceProxy("/gazebo/delete_model", DeleteModel)
    spawn = rospy.ServiceProxy("/gazebo/spawn_sdf_model", SpawnModel)

    if args.wait_for_model:
        wait_for_model(world, args.wait_for_model, args.timeout)

    removed = clear_obstacles(world, delete, args.timeout)
    if args.clear:
        rospy.loginfo("Circuit obstacles removed: %d", removed)
    else:
        positions = generate_obstacle_positions(args.seed, args.count)
        for index, position in enumerate(positions):
            name = OBSTACLE_PREFIX + str(index)
            pose = Pose()
            pose.position.x = position[0]
            pose.position.y = position[1]
            pose.position.z = 0.05
            pose.orientation.w = 1.0
            response = spawn(name, obstacle_sdf(name, args.dynamic), "", pose, "world")
            if not response.success:
                raise RuntimeError("Could not spawn {0}: {1}".format(name, response.status_message))
            rospy.loginfo("Spawned %s at x=%.3f y=%.3f", name,
                          pose.position.x, pose.position.y)

    try:
        rospy.wait_for_service("/karel_score/reset", timeout=2.0)
        rospy.ServiceProxy("/karel_score/reset", Empty)()
    except (rospy.ROSException, rospy.ServiceException):
        rospy.logwarn("Score reset service is not available")


if __name__ == "__main__":
    main()
