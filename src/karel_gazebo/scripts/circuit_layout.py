from __future__ import division

import random


X_SECTIONS = ((-0.55, -0.35), (-0.12, 0.12), (0.35, 0.55))
Y_RANGE = (-0.10, 0.10)

TRACK_HALF_WIDTH = 0.25
OBSTACLE_WIDTH = 0.10
ROBOT_WIDTH = 0.14
SAFETY_CLEARANCE = 0.01


def bypass_y_limit():
    required_gap = ROBOT_WIDTH + 2.0 * SAFETY_CLEARANCE
    return TRACK_HALF_WIDTH - OBSTACLE_WIDTH / 2.0 - required_gap


def has_safe_bypass(positions):
    if not positions:
        return True

    limit = bypass_y_limit()
    top_is_clear = all(y <= limit for _, y in positions)
    bottom_is_clear = all(y >= -limit for _, y in positions)
    return top_is_clear or bottom_is_clear


def generate_obstacle_positions(seed, count):
    if count < 1 or count > len(X_SECTIONS):
        raise ValueError("count must be between 1 and {0}".format(
            len(X_SECTIONS)))

    randomizer = random.Random(seed)
    clear_side = randomizer.choice(("top", "bottom"))
    limit = bypass_y_limit()
    if clear_side == "top":
        y_range = (Y_RANGE[0], limit)
    else:
        y_range = (-limit, Y_RANGE[1])

    positions = []
    for section in X_SECTIONS[:count]:
        positions.append((randomizer.uniform(section[0], section[1]),
                          randomizer.uniform(y_range[0], y_range[1])))
    return positions
