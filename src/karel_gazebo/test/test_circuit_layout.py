from __future__ import division

import os
import sys
import unittest
import xml.etree.ElementTree as ET


SCRIPTS_DIR = os.path.abspath(os.path.join(
    os.path.dirname(__file__), "..", "scripts"))
sys.path.insert(0, SCRIPTS_DIR)

from circuit_layout import (  # noqa: E402
    X_SECTIONS,
    generate_obstacle_positions,
    has_safe_bypass,
)


START_X = -0.85
GOAL_X = 0.85
TRACK_HALF_WIDTH = 0.25
ROBOT_HALF_LENGTH = 0.06
ROBOT_HALF_WIDTH = 0.07
OBSTACLE_HALF_LENGTH = 0.04
OBSTACLE_HALF_WIDTH = 0.05
SAFETY_CLEARANCE = 0.01


def straight_bypass_exists(positions):
    center_limit = TRACK_HALF_WIDTH - ROBOT_HALF_WIDTH - SAFETY_CLEARANCE
    top_lane = center_limit
    bottom_lane = -center_limit

    top_is_clear = all(
        top_lane - ROBOT_HALF_WIDTH - SAFETY_CLEARANCE >=
        y + OBSTACLE_HALF_WIDTH
        for _, y in positions)
    bottom_is_clear = all(
        bottom_lane + ROBOT_HALF_WIDTH + SAFETY_CLEARANCE <=
        y - OBSTACLE_HALF_WIDTH
        for _, y in positions)

    first_x = min(x for x, _ in positions)
    last_x = max(x for x, _ in positions)
    approach_is_clear = (
        first_x - OBSTACLE_HALF_LENGTH - ROBOT_HALF_LENGTH -
        SAFETY_CLEARANCE > START_X)
    exit_is_clear = (
        last_x + OBSTACLE_HALF_LENGTH + ROBOT_HALF_LENGTH +
        SAFETY_CLEARANCE < GOAL_X)
    return (top_is_clear or bottom_is_clear) and approach_is_clear and exit_is_clear


class CircuitLayoutTest(unittest.TestCase):

    def test_every_generated_layout_has_a_safe_route(self):
        for count in range(1, len(X_SECTIONS) + 1):
            for seed in range(10000):
                positions = generate_obstacle_positions(seed, count)
                self.assertTrue(
                    straight_bypass_exists(positions),
                    "No route for seed={0}, count={1}: {2}".format(
                        seed, count, positions))

    def test_generation_is_reproducible(self):
        self.assertEqual(generate_obstacle_positions(9, 3),
                         generate_obstacle_positions(9, 3))
        self.assertNotEqual(generate_obstacle_positions(9, 3),
                            generate_obstacle_positions(10, 3))

    def test_safe_bypass_boundary_cases(self):
        self.assertTrue(has_safe_bypass(((-0.4, 0.04), (0.0, -0.10))))
        self.assertTrue(has_safe_bypass(((-0.4, -0.04), (0.0, 0.10))))
        self.assertFalse(has_safe_bypass(((-0.4, 0.05), (0.0, -0.05))))

    def test_circuit_has_four_overlapping_walls(self):
        world_path = os.path.join(
            os.path.dirname(__file__), "..", "worlds",
            "circuit_straight.world")
        root = ET.parse(world_path).getroot()
        models = dict((model.get("name"), model)
                      for model in root.findall("./world/model"))

        expected = {
            "circuit_wall_left": ((0.0, 0.28), (2.0, 0.06)),
            "circuit_wall_right": ((0.0, -0.28), (2.0, 0.06)),
            "circuit_wall_start": ((-1.0, 0.0), (0.06, 0.62)),
            "circuit_wall_goal": ((1.0, 0.0), (0.06, 0.62)),
        }
        for name, (expected_pose, expected_size) in expected.items():
            self.assertIn(name, models)
            pose = tuple(float(value) for value in
                         models[name].find("pose").text.split()[:2])
            size = tuple(float(value) for value in models[name].find(
                "./link/collision/geometry/box/size").text.split()[:2])
            self.assertEqual(expected_pose, pose)
            self.assertEqual(expected_size, size)

        side_half_length = expected["circuit_wall_left"][1][0] / 2.0
        end_half_thickness = expected["circuit_wall_start"][1][0] / 2.0
        self.assertGreaterEqual(-1.0 + end_half_thickness,
                                -side_half_length)
        self.assertLessEqual(1.0 - end_half_thickness, side_half_length)


if __name__ == "__main__":
    unittest.main()
