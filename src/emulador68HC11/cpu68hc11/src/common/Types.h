/*
 * Emulador Motorola 68HC11
 * Copyright (C) 2026 Julio Cruz
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef TYPES_H
#define TYPES_H

#include <cstddef>
#include <cstdint>
#include <string>

struct RomWord {
  uint64_t low;  // bits 0..63
  uint64_t high; // bits 64..127
};

#endif
