/*
 * Emulador Motorola 68HC11
 * Copyright (C) 2026 Julio Cruz
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "DataBus16.h"

void DataBus16::clear() {
  value = 0;
  DBH = 0;
  DBL = 0;
  drivenL = false;
  drivenH = false;
}

void DataBus16::driveL(uint8_t v) {
  if (drivenL) {
    std::cerr << "ERROR: Data Bus Low contention\n";
  }
  DBL = v;
  drivenL = true;
}

uint16_t DataBus16::readL() const { return DBL; }

bool DataBus16::isDrivenL() const { return drivenL; }

void DataBus16::driveH(uint8_t v) {
  if (drivenH) {
    std::cerr << "ERROR: Data Bus Hight contention\n";
  }
  DBH = v;
  drivenH = true;
}

uint16_t DataBus16::readH() const { return DBH; }

bool DataBus16::isDrivenH() const { return drivenH; }
