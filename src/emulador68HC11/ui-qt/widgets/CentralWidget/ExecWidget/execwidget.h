/*
 * Emulador Motorola 68HC11
 * Copyright (C) 2026 Julio Cruz
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef EXECWIDGET_H
#define EXECWIDGET_H

#include "../../RamWidget/ramwidget.h"
#include "../../RegistersWidget/registerswidget.h"
#include "../../RomWidget/romwidget.h"
#include "CPU68HC11.h"
#include <QWidget>
#include <cstdint>

namespace Ui {
class ExecWidget;
}

enum class LogType { Normal, Info, Remark, Error };

class ExecWidget : public QWidget {
  Q_OBJECT

public:
  explicit ExecWidget(RegistersWidget *regW, RamWidget *ramW, RomWidget *romW,
                      CPU68HC11 *cpu, QWidget *parent = nullptr);
  ~ExecWidget();

  void refreshInfo();
  void generateLog();
  void runSelectedMode();
  void stopExecution();
  void advanceContinuousExecution(std::uint64_t simTimeNs,
                                  bool clockAvailable);

public slots:
  void addLog(const QString &message, LogType type);
  void clearLog();

signals:
  void stepExecuted();
  void executionStopped();

private slots:
  void on_btnRun_clicked();
  void on_btnStop_clicked();
  void on_btnReset_clicked();
  void refreshUI();

  QString genActiveLines();
  void genBusLog();

private:
  Ui::ExecWidget *ui;
  CPU68HC11 *m_cpu;
  RamWidget *m_ramW;
  RomWidget *m_romW;
  RegistersWidget *m_regW;
  bool m_continuousRunning;
  bool m_hasLastSimTime;
  bool m_waitingForClockLogged;
  bool m_backlogWarningLogged;
  std::uint64_t m_cycleIntervalNs;
  std::uint64_t m_lastSimTimeNs;
  std::uint64_t m_accumulatedSimTimeNs;

  void executeStep();
  void startContinuousExecution();
  void updateExecutionControls(bool running);
};

#endif // EXECWIDGET_H
