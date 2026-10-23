# -*- coding: utf-8 -*-

import base64
import math
import subprocess

import rospy
from geometry_msgs.msg import Twist
from IPython.display import HTML, Image, display
from nav_msgs.msg import Odometry
from sensor_msgs.msg import Range


CMD_VEL_TOPIC = "/hardware/mobile_base/cmd_vel"
ODOMETRY_TOPIC = "/karel/odom"
ULTRASONIC_TOPIC = "/hardware/ultrasonic"

_base_vel_pub = None


def screen_shot():
    """Captura la pantalla del simulador y la muestra en el notebook."""
    path = "/tmp/karel-screen.png"
    subprocess.check_call(["import", "-window", "root", path])
    display(Image(filename=path))


def screen_cast(sec):
    """Graba la pantalla del simulador y muestra el GIF en el notebook."""
    if sec <= 0:
        raise ValueError("sec debe ser mayor que cero")

    path = "/tmp/karel-screencast.gif"
    subprocess.check_call(["byzanz-record", "-d", str(sec), path])
    with open(path, "rb") as gif_file:
        encoded = base64.b64encode(gif_file.read()).decode("ascii")
    display(HTML('<img src="data:image/gif;base64,%s">' % encoded))


def _get_base_vel_publisher():
    global _base_vel_pub
    if not rospy.core.is_initialized():
        raise RuntimeError("Ejecuta rospy.init_node(...) antes de mover el robot")
    if _base_vel_pub is None:
        _base_vel_pub = rospy.Publisher(CMD_VEL_TOPIC, Twist, queue_size=1)
        rospy.sleep(0.2)
    return _base_vel_pub


def move_base_vel(vx, vy, vw):
    """Publica velocidad lineal [m/s] y angular [grados/s] para Karel."""
    if vy != 0:
        raise ValueError("Karel no admite movimiento lateral; vy debe ser cero")

    command = Twist()
    command.linear.x = vx
    command.angular.z = math.radians(vw)
    _get_base_vel_publisher().publish(command)


def stop_base():
    """Detiene la base móvil."""
    _get_base_vel_publisher().publish(Twist())


def get_current_time_sec():
    """Devuelve el tiempo ROS actual en segundos."""
    return rospy.Time.now().to_sec()


def read_odometry(timeout=2.0):
    """Espera y devuelve el siguiente mensaje de odometría de Karel."""
    return rospy.wait_for_message(ODOMETRY_TOPIC, Odometry, timeout=timeout)


def read_ultrasonic(timeout=2.0):
    """Espera y devuelve la siguiente medición ultrasónica frontal."""
    return rospy.wait_for_message(ULTRASONIC_TOPIC, Range, timeout=timeout)
