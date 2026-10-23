#!/usr/bin/env python2
# Karel ROS
# Copyright (C) 2026 Julio Cruz
# SPDX-License-Identifier: GPL-3.0-only


import rospy
from geometry_msgs.msg import Twist
from sensor_msgs.msg import Joy


class KarelJoystick(object):
    def __init__(self):
        self.linear_axis = rospy.get_param("~linear_axis", 4)
        self.angular_axis = rospy.get_param("~angular_axis", 3)
        self.enable_button = rospy.get_param("~enable_button", 4)
        self.linear_scale = rospy.get_param("~linear_scale", 0.25)
        self.angular_scale = rospy.get_param("~angular_scale", 1.5)
        self.timeout = rospy.Duration(rospy.get_param("~timeout", 0.5))
        self.last_joy = rospy.Time(0)

        self.publisher = rospy.Publisher(
            "/hardware/mobile_base/cmd_vel", Twist, queue_size=1
        )
        rospy.Subscriber("joy", Joy, self.joy_callback, queue_size=1)
        rospy.Timer(rospy.Duration(0.1), self.watchdog_callback)

    def joy_callback(self, message):
        self.last_joy = rospy.Time.now()
        command = Twist()
        if self._button(message, self.enable_button):
            command.linear.x = self.linear_scale * self._axis(message, self.linear_axis)
            command.angular.z = self.angular_scale * self._axis(message, self.angular_axis)
        self.publisher.publish(command)

    def watchdog_callback(self, _event):
        if rospy.Time.now() - self.last_joy > self.timeout:
            self.publisher.publish(Twist())

    @staticmethod
    def _axis(message, index):
        return message.axes[index] if 0 <= index < len(message.axes) else 0.0

    @staticmethod
    def _button(message, index):
        return 0 <= index < len(message.buttons) and message.buttons[index] == 1


if __name__ == "__main__":
    rospy.init_node("karel_joystick")
    KarelJoystick()
    rospy.spin()
