/*
 * Emulador Motorola 68HC11
 * Copyright (C) 2026 Julio Cruz
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef CENTRALWIDGET_H
#define CENTRALWIDGET_H

#include "../RamWidget/ramwidget.h"
#include "../RegistersWidget/registerswidget.h"
#include "../RomWidget/romwidget.h"
#include "CPU68HC11.h"
#include "ExecWidget/execwidget.h"
#include "UpaWidget/upawidget.h"
#include <QWidget>
#include <cstdint>

namespace Ui {
class CentralWidget;
}

class CentralWidget : public QWidget {
  Q_OBJECT

public:
  explicit CentralWidget(RegistersWidget *regW, RamWidget *ramW,
                         RomWidget *romW, CPU68HC11 *cpu,
                         QWidget *parent = nullptr);
  ~CentralWidget();

  void refresh();
  void runSelectedMode();
  void stopExecution();
  void advanceContinuousExecution(std::uint64_t simTimeNs,
                                  bool clockAvailable);

signals:
  void executionStopped();

private:
  Ui::CentralWidget *ui;
  CPU68HC11 *m_cpu;
  RamWidget *m_ramW;
  RomWidget *m_romW;
  RegistersWidget *m_regW;

  ExecWidget *m_execWidget;
  UpaWidget *m_upaWidget;
};

#endif // CENTRALWIDGET_H
