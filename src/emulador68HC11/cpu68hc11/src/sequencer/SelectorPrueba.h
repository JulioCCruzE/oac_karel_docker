/*
 * Emulador Motorola 68HC11
 * Copyright (C) 2026 Julio Cruz
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef SELECTORPRUEBA_H
#define SELECTORPRUEBA_H

#include <cstdint>

class SelectorPrueba {
public:
  void reset();

  void setInput(int index, bool value);

  bool getOutput(uint8_t prueba) const;

  bool getNcc(uint8_t pruena, uint8_t vf);

  bool getOutput(uint8_t prueba, uint32_t queries) const;

  bool getNcc(uint8_t pruena, uint8_t vf, uint32_t queries);

private:
  uint32_t inputs = 0;
};

#endif // SELECTORPRUEBA_H
