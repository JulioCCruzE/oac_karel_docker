/*
 * Emulador Motorola 68HC11
 * Copyright (C) 2026 Julio Cruz
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef ACCESSREGISTER_H
#define ACCESSREGISTER_H

#include <cstdint>

class AddressBus16; // forward declaration

class AccessRegister {
public:
  AccessRegister() = default;

  void reset();

  void loadFromBus(const AddressBus16 &abus, uint8_t ncbd);

  uint16_t read() const;

private:
  uint16_t address = 0;
};

#endif // ACCESSREGISTER_H
