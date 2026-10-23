/*
 * Emulador Motorola 68HC11
 * Copyright (C) 2026 Julio Cruz
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "SelectorPrueba.h"

void SelectorPrueba::reset() { inputs = 0; }

void SelectorPrueba::setInput(int index, bool value) {
  if (value)
    inputs |= (1u << index);
  else
    inputs &= ~(1u << index);
}

bool SelectorPrueba::getOutput(uint8_t prueba) const {
  return (prueba < 25) ? (inputs & (1u << prueba)) : false;
}

bool SelectorPrueba::getNcc(uint8_t prueba, uint8_t vf) {
  bool out = getOutput(prueba);
  bool vf_bit = (vf != 0); // normalizar vf a 0 o 1

  return !(out ^ vf_bit);
}

bool SelectorPrueba::getOutput(uint8_t prueba, uint32_t queries) const {
  return (prueba < 25) ? (queries & (1u << prueba)) : false;
}

bool SelectorPrueba::getNcc(uint8_t prueba, uint8_t vf, uint32_t queries) {
  bool out = getOutput(prueba, queries);
  bool vf_bit = (vf != 0); // normalizar vf a 0 o 1

  return !(out ^ vf_bit);
}
