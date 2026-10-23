/*
 * Emulador Motorola 68HC11
 * Copyright (C) 2026 Julio Cruz
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef CPUSTATUSWIDGET_H
#define CPUSTATUSWIDGET_H

#include "CPU68HC11.h"
#include <QWidget>

namespace Ui {
class CpuStatusWidget;
}

class CpuStatusWidget : public QWidget {
  Q_OBJECT

public:
  explicit CpuStatusWidget(CPU68HC11 *cpu, QWidget *parent = nullptr);
  ~CpuStatusWidget();

  void refreshUCP();

private:
  Ui::CpuStatusWidget *ui;
  CPU68HC11 *m_cpu;

  QString hex8(uint8_t value) const;
  QString hex16(uint16_t value) const;
};

#endif // CPUSTATUSWIDGET_H
