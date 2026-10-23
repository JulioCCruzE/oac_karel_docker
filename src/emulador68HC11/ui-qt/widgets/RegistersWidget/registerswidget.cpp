/*
 * Emulador Motorola 68HC11
 * Copyright (C) 2026 Julio Cruz
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "registerswidget.h"
#include "ui_registerswidget.h"

RegistersWidget::RegistersWidget(CPU68HC11 &cpu, QWidget *parent)
    : QWidget(parent), ui(new Ui::RegistersWidget), m_cpu(cpu) {
  ui->setupUi(this);
}

RegistersWidget::~RegistersWidget() { delete ui; }

QString RegistersWidget::hex8(uint8_t value) const {
  return QString("%1").arg(value, 2, 16, QLatin1Char('0')).toUpper();
}

QString RegistersWidget::hex16(uint16_t value) const {
  return QString("%1").arg(value, 4, 16, QLatin1Char('0')).toUpper();
}

void RegistersWidget::updateRegisters() {

  // registers
  ui->lbAValue->setText(hex8(m_cpu.getA()));
  ui->lbBValue->setText(hex8(m_cpu.getB()));
  ui->lbRAValue->setText(hex16(m_cpu.getRA()));
  ui->lbXValue->setText(hex16(m_cpu.getX(0)));
  ui->lbYValue->setText(hex16(m_cpu.getX(1)));
  ui->lbSPValue->setText(hex16(m_cpu.getAP()));
  ui->lbPCValue->setText(hex16(m_cpu.getPC()));
  ui->lbCCRValue->setText(hex8(m_cpu.getCCR()));
  ui->lbRDIRValue->setText(hex16(m_cpu.getRDIR()));
  ui->lbDBHValue->setText(hex8(m_cpu.getDBH()));
  ui->lbDBLValue->setText(hex8(m_cpu.getDBL()));

  // flags
  ui->lbZAValue->setText(hex8(m_cpu.getZA()));
  ui->lbNAValue->setText(hex8(m_cpu.getNA()));
  ui->lbZBValue->setText(hex8(m_cpu.getZB()));
  ui->lbNBValue->setText(hex8(m_cpu.getNB()));
  ui->lbZXValue->setText(hex8(m_cpu.getZX()));
  ui->lbNXValue->setText(hex8(m_cpu.getNX()));
  ui->lbZPCValue->setText(hex8(m_cpu.getZPC()));
  ui->lbNPCValue->setText(hex8(m_cpu.getNPC()));
}
