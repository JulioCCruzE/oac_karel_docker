/*
 * Emulador Motorola 68HC11
 * Copyright (C) 2026 Julio Cruz
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "cpustatuswidget.h"
#include "ui_cpustatuswidget.h"

CpuStatusWidget::CpuStatusWidget(CPU68HC11 *cpu, QWidget *parent)
    : QWidget(parent), m_cpu(cpu), ui(new Ui::CpuStatusWidget) {
  ui->setupUi(this);
}

CpuStatusWidget::~CpuStatusWidget() { delete ui; }

QString CpuStatusWidget::hex8(uint8_t value) const {
  return QString("%1").arg(value, 2, 16, QLatin1Char('0')).toUpper();
}

QString CpuStatusWidget::hex16(uint16_t value) const {
  return QString("%1").arg(value, 4, 16, QLatin1Char('0')).toUpper();
}

void CpuStatusWidget::refreshUCP() {

  // registers
  ui->lbAPValue->setText(hex16(m_cpu->getAP()));
  ui->lbPCValue->setText(hex16(m_cpu->getPC()));
  ui->lbRDIRValue->setText(hex16(m_cpu->getRDIR()));

  // flags
  // ui->lbZPCValue->setText(hex8(m_cpu.getZPC()));
  // ui->lbNPCValue->setText(hex8(m_cpu.getNPC()));
}
