/*
 * Emulador Motorola 68HC11
 * Copyright (C) 2026 Julio Cruz
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef DATABUSQUERIES_H
#define DATABUSQUERIES_H

#include <cstdint>
#include <iostream>

class DataBusQueries {
public:
  void clear();

  void setInput(int index, bool value);
  uint32_t read() const;

private:
  uint32_t inputs = 0;
};

#endif
