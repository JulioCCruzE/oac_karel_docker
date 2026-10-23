/*
 * Emulador Motorola 68HC11
 * Copyright (C) 2026 Julio Cruz
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "centralwidget.h"
#include "ui_centralwidget.h"

CentralWidget::CentralWidget(RegistersWidget *regW, RamWidget *ramW,
                             RomWidget *romW, CPU68HC11 *cpu, QWidget *parent)
    : QWidget(parent), m_regW(regW), m_ramW(ramW), m_romW(romW), m_cpu(cpu),
      m_execWidget(nullptr), m_upaWidget(nullptr), ui(new Ui::CentralWidget) {
  ui->setupUi(this);

  // status widget
  // m_cpuStatusWidget = new CpuStatusWidget(m_cpu, this);
  // ui->verticalLayout->replaceWidget(ui->cpuStatusPlaceHolder,
  //                                   m_cpuStatusWidget);
  // ui->cpuStatusPlaceHolder->deleteLater();

  // exec widget
  m_execWidget = new ExecWidget(m_regW, m_ramW, m_romW, m_cpu, this);
  ui->verticalLayout_2->addWidget(m_execWidget);

  // upa widget
  m_upaWidget = new UpaWidget(m_cpu, this);
  ui->verticalLayout_3->addWidget(m_upaWidget);

  connect(m_execWidget, &ExecWidget::stepExecuted, this,
          &CentralWidget::refresh);
  connect(m_execWidget, &ExecWidget::executionStopped, this,
          &CentralWidget::executionStopped);
}

CentralWidget::~CentralWidget() { delete ui; }

void CentralWidget::refresh() {
  m_execWidget->refreshInfo();
  m_upaWidget->refresh();
}

void CentralWidget::runSelectedMode() { m_execWidget->runSelectedMode(); }

void CentralWidget::stopExecution() { m_execWidget->stopExecution(); }

void CentralWidget::advanceContinuousExecution(std::uint64_t simTimeNs,
                                               bool clockAvailable) {
  m_execWidget->advanceContinuousExecution(simTimeNs, clockAvailable);
}
