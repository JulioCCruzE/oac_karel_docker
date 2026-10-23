/*
 * Emulador Motorola 68HC11
 * Copyright (C) 2026 Julio Cruz
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "upawidget.h"
#include "ui_upawidget.h"

UpaWidget::UpaWidget(CPU68HC11 *cpu, QWidget *parent)
    : QWidget(parent), m_cpu(cpu), ui(new Ui::UpaWidget) {
  ui->setupUi(this);
}

UpaWidget::~UpaWidget() { delete ui; }

void UpaWidget::refresh() {
  // flags
  ui->lbCValue->setText(
      QString("%1").arg(m_cpu->getCUPA(), 1, 16, QChar('0')).toUpper());
  ui->lbVValue->setText(
      QString("%1").arg(m_cpu->getVUPA(), 1, 16, QChar('0')).toUpper());
  ui->lbZValue->setText(
      QString("%1").arg(m_cpu->getZUPA(), 1, 16, QChar('0')).toUpper());
  ui->lbNValue->setText(
      QString("%1").arg(m_cpu->getNUPA(), 1, 16, QChar('0')).toUpper());
  ui->lbHValue->setText(
      QString("%1").arg(m_cpu->getHUPA(), 1, 16, QChar('0')).toUpper());

  // registros
  ui->lbYValue->setText(
      QString("%1").arg(m_cpu->getYUPA(), 2, 16, QChar('0')).toUpper());
  ui->lbQValue->setText(
      QString("%1").arg(m_cpu->getQUPA(), 2, 16, QChar('0')).toUpper());
  ui->lbCounterValue->setText(
      QString("%1").arg(m_cpu->getCounterUPA(), 1, 16, QChar('0')).toUpper());

  // operation
  ui->lbPreValue->setText("");
  ui->lbR->setText("R");
  ui->lbS->setText("S");
  ui->lbSufValue->setText("");

  ui->lbRValue->setText(
      QString("%1").arg(m_cpu->getRUPA(), 2, 16, QChar('0')).toUpper());
  ui->lbSValue->setText(
      QString("%1").arg(m_cpu->getSUPA(), 2, 16, QChar('0')).toUpper());
  ui->lbFValue->setText(
      QString("%1").arg(m_cpu->getFUPA(), 2, 16, QChar('0')).toUpper());

  ui->lbCinValue->setText(
      QString("%1").arg(m_cpu->getCinUPA(), 1, 16, QChar('0')).toUpper());
  ui->lbCin->setEnabled(false);
  ui->lbCinValue->setEnabled(false);

  switch (m_cpu->getOpUPA()) {
  case 0:
    // F = R + S + cin;
    ui->lbFunctnValue->setText("+");
    ui->lbSufValue->setText("   +");
    ui->lbCin->setEnabled(true);
    ui->lbCinValue->setEnabled(true);
    break;
  case 1:
    // F = S - R - !cin;
    ui->lbFunctnValue->setText("-");
    ui->lbR->setText("S");
    ui->lbS->setText("R");
    ui->lbSufValue->setText("   -");
    ui->lbRValue->setText(
        QString("%1").arg(m_cpu->getSUPA(), 2, 16, QChar('0')).toUpper());
    ui->lbSValue->setText(
        QString("%1").arg(m_cpu->getRUPA(), 2, 16, QChar('0')).toUpper());
    ui->lbCinValue->setText(
        "¬" +
        QString("%1").arg(m_cpu->getCinUPA(), 1, 16, QChar('0')).toUpper());
    ui->lbCin->setEnabled(true);
    ui->lbCinValue->setEnabled(true);
    break;
  case 2:
    // F = R - S - !cin;
    ui->lbFunctnValue->setText("-");
    ui->lbSufValue->setText("   -");
    ui->lbCinValue->setText(
        "¬" +
        QString("%1").arg(m_cpu->getCinUPA(), 1, 16, QChar('0')).toUpper());
    ui->lbCin->setEnabled(true);
    ui->lbCinValue->setEnabled(true);
    break;
  case 3:
    // F = R | S;
    ui->lbFunctnValue->setText("v");
    break;
  case 4:
    // F = R & S;
    ui->lbFunctnValue->setText("^");
    break;
  case 5:
    // F = ~R & S;
    ui->lbPreValue->setText("¬");
    ui->lbFunctnValue->setText("^");
    break;
  case 6:
    // F = R ^ S;
    ui->lbFunctnValue->setText("xor");
    break;
  case 7:
    // F = ~(R ^ S);
    ui->lbPreValue->setText("¬(");
    ui->lbFunctnValue->setText("xor");
    ui->lbSufValue->setText(")");
  default:
    break;
  }
}
