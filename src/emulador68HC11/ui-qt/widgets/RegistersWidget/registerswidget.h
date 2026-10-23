/*
 * Emulador Motorola 68HC11
 * Copyright (C) 2026 Julio Cruz
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef REGISTERSWIDGET_H
#define REGISTERSWIDGET_H

#include "CPU68HC11.h"
#include <QWidget>

namespace Ui {
class RegistersWidget;
}

class RegistersWidget : public QWidget {
  Q_OBJECT

public:
  explicit RegistersWidget(CPU68HC11 &cpu, QWidget *parent = nullptr);
  ~RegistersWidget();

  void updateRegisters();

private:
  Ui::RegistersWidget *ui;
  CPU68HC11 &m_cpu;

  QString hex8(uint8_t value) const;
  QString hex16(uint16_t value) const;
};

#endif // REGISTERSWIDGET_H
