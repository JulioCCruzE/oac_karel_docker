/*
 * Emulador Motorola 68HC11
 * Copyright (C) 2026 Julio Cruz
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "execwidget.h"
#include "ui_execwidget.h"

#include <algorithm>
#include <limits>

ExecWidget::ExecWidget(RegistersWidget *regW, RamWidget *ramW, RomWidget *romW,
                       CPU68HC11 *cpu, QWidget *parent)
    : QWidget(parent), ui(new Ui::ExecWidget), m_cpu(cpu), m_ramW(ramW),
      m_romW(romW), m_regW(regW), m_continuousRunning(false),
      m_hasLastSimTime(false), m_waitingForClockLogged(false),
      m_backlogWarningLogged(false), m_cycleIntervalNs(0),
      m_lastSimTimeNs(0), m_accumulatedSimTimeNs(0) {
  ui->setupUi(this);
  ui->textEdit->setReadOnly(true);
  ui->textEdit->setLineWrapMode(QTextEdit::NoWrap);
  ui->btnStop->setEnabled(false);
}

ExecWidget::~ExecWidget() { delete ui; }

void ExecWidget::refreshInfo() {
  ui->lbEPValue->setText(
      QString("%1").arg(m_cpu->getOldmPC(), 3, 16, QChar('0')).toUpper());
  ui->lbESValue->setText(
      QString("%1").arg(m_cpu->getmPC(), 3, 16, QChar('0')).toUpper());
  ui->lbCyclesValue->setText(QString::number(m_cpu->getCycleCount()));
}

void ExecWidget::refreshUI() {
  m_ramW->refresh();
  m_romW->refreshmPC();
  m_regW->updateRegisters();
}

void ExecWidget::runSelectedMode() {
  if (ui->cmbExecMode->currentIndex() == 0) {
    if (!m_continuousRunning)
      executeStep();
    return;
  }

  startContinuousExecution();
}

void ExecWidget::startContinuousExecution() {
  if (m_continuousRunning)
    return;

  m_cycleIntervalNs = std::max<std::uint64_t>(
      1, static_cast<std::uint64_t>(
             qRound64(ui->sbInterval->value() * 1000000000.0)));
  m_continuousRunning = true;
  m_hasLastSimTime = false;
  m_waitingForClockLogged = false;
  m_backlogWarningLogged = false;
  m_accumulatedSimTimeNs = 0;
  updateExecutionControls(true);
}

void ExecWidget::stopExecution() {
  m_continuousRunning = false;
  m_hasLastSimTime = false;
  m_accumulatedSimTimeNs = 0;
  updateExecutionControls(false);
  emit executionStopped();
}

void ExecWidget::advanceContinuousExecution(std::uint64_t simTimeNs,
                                            bool clockAvailable) {
  if (!m_continuousRunning)
    return;

  if (!clockAvailable) {
    m_hasLastSimTime = false;
    m_accumulatedSimTimeNs = 0;
    if (!m_waitingForClockLogged) {
      addLog("Esperando el reloj de simulacion de Gazebo...", LogType::Remark);
      m_waitingForClockLogged = true;
    }
    return;
  }

  if (!m_hasLastSimTime) {
    m_lastSimTimeNs = simTimeNs;
    m_hasLastSimTime = true;
    m_waitingForClockLogged = false;
    addLog("Ejecucion continua sincronizada con Sim Time.", LogType::Remark);
    return;
  }

  if (simTimeNs < m_lastSimTimeNs) {
    m_lastSimTimeNs = simTimeNs;
    m_accumulatedSimTimeNs = 0;
    m_backlogWarningLogged = false;
    addLog("Sim Time retrocedio; se reinicio la referencia temporal.",
           LogType::Remark);
    return;
  }

  const std::uint64_t elapsedNs = simTimeNs - m_lastSimTimeNs;
  m_lastSimTimeNs = simTimeNs;
  if (elapsedNs == 0)
    return;

  if (elapsedNs > std::numeric_limits<std::uint64_t>::max() -
                      m_accumulatedSimTimeNs) {
    m_accumulatedSimTimeNs = std::numeric_limits<std::uint64_t>::max();
  } else {
    m_accumulatedSimTimeNs += elapsedNs;
  }

  const std::uint64_t pendingCycles =
      m_accumulatedSimTimeNs / m_cycleIntervalNs;
  if (pendingCycles == 0)
    return;

  const std::uint64_t maxCyclesPerUpdate = 100;
  const std::uint64_t maxPendingCycles = 1000;
  if (pendingCycles > maxPendingCycles) {
    addLog("No es posible alcanzar Sim Time; ejecucion detenida.",
           LogType::Error);
    stopExecution();
    return;
  }

  const std::uint64_t cyclesToExecute =
      std::min(pendingCycles, maxCyclesPerUpdate);
  for (std::uint64_t cycle = 0; cycle < cyclesToExecute; ++cycle) {
    m_cpu->step();
    generateLog();
  }
  m_accumulatedSimTimeNs -= cyclesToExecute * m_cycleIntervalNs;

  if (pendingCycles > maxCyclesPerUpdate && !m_backlogWarningLogged) {
    addLog("El emulador esta recuperando microciclos pendientes.",
           LogType::Remark);
    m_backlogWarningLogged = true;
  } else if (pendingCycles <= maxCyclesPerUpdate) {
    m_backlogWarningLogged = false;
  }

  refreshUI();
  refreshInfo();
  emit stepExecuted();
}

void ExecWidget::updateExecutionControls(bool running) {
  ui->btnStop->setEnabled(running);
  ui->cmbExecMode->setEnabled(!running);
  ui->sbInterval->setEnabled(!running);
}

void ExecWidget::executeStep() {
  m_cpu->step();
  refreshUI();
  refreshInfo();
  generateLog();
  emit stepExecuted();
}

void ExecWidget::addLog(const QString &message, LogType type) {
  QTextCharFormat format;

  switch (type) {
  case LogType::Normal:
    break;

  case LogType::Info:
    format.setFontWeight(QFont::Bold);
    break;

  case LogType::Remark:
    format.setForeground(Qt::magenta);
    break;

  case LogType::Error:
    format.setForeground(Qt::red);
    format.setFontWeight(QFont::Bold);
    break;
  }

  QTextCursor cursor = ui->textEdit->textCursor();
  cursor.movePosition(QTextCursor::End);

  cursor.insertText(message + "\n", format);
  ui->textEdit->setTextCursor(cursor);
  ui->textEdit->ensureCursorVisible();
}

QString ExecWidget::genActiveLines() {
  QString aL = "";

  if (!m_cpu->get_ncri())
    aL.append(
        " ncri=" +
        QString("%1").arg(m_cpu->get_ncri(), 1, 16, QChar('0')).toUpper());
  if (m_cpu->get_eb() != 0)
    aL.append(" eb=" +
              QString("%1").arg(m_cpu->get_eb(), 1, 16, QChar('0')).toUpper());
  if (!m_cpu->get_nwb())
    aL.append(" nwb=" +
              QString("%1").arg(m_cpu->get_nwb(), 1, 16, QChar('0')).toUpper());
  if (m_cpu->get_ea() != 0)
    aL.append(" ea=" +
              QString("%1").arg(m_cpu->get_ea(), 1, 16, QChar('0')).toUpper());
  if (!m_cpu->get_nwa())
    aL.append(" nwa=" +
              QString("%1").arg(m_cpu->get_nwa(), 1, 16, QChar('0')).toUpper());
  if (m_cpu->get_selbus())
    aL.append(
        " selbus=" +
        QString("%1").arg(m_cpu->get_selbus(), 1, 16, QChar('0')).toUpper());
  if (m_cpu->get_dest() != 0)
    aL.append(
        " dest=" +
        QString("%1").arg(m_cpu->get_dest(), 1, 16, QChar('0')).toUpper());
  if (m_cpu->get_functn() != 0)
    aL.append(
        " functn=" +
        QString("%1").arg(m_cpu->get_functn(), 1, 16, QChar('0')).toUpper());
  if (m_cpu->get_source() != 0)
    aL.append(
        " source=" +
        QString("%1").arg(m_cpu->get_source(), 1, 16, QChar('0')).toUpper());
  if (!m_cpu->get_noeupa())
    aL.append(
        " noeupa=" +
        QString("%1").arg(m_cpu->get_noeupa(), 1, 16, QChar('0')).toUpper());
  if (!m_cpu->get_ndupa())
    aL.append(
        " ndupa=" +
        QString("%1").arg(m_cpu->get_ndupa(), 1, 16, QChar('0')).toUpper());
  if (m_cpu->get_selmux() != 0)
    aL.append(
        " selmux=" +
        QString("%1").arg(m_cpu->get_selmux(), 1, 16, QChar('0')).toUpper());
  if (m_cpu->get_nex() != 7)
    aL.append(" nex=" +
              QString("%1").arg(m_cpu->get_nex(), 1, 16, QChar('0')).toUpper());
  if (m_cpu->get_x() != 0)
    aL.append(" x=" +
              QString("%1").arg(m_cpu->get_x(), 1, 16, QChar('0')).toUpper());
  if (m_cpu->get_enay() != 0)
    aL.append(
        " enay=" +
        QString("%1").arg(m_cpu->get_enay(), 1, 16, QChar('0')).toUpper());

  if (m_cpu->get_nera() != 7)
    aL.append(
        " nera=" +
        QString("%1").arg(m_cpu->get_nera(), 1, 16, QChar('0')).toUpper());
  if (m_cpu->get_ra() != 0)
    aL.append(" ra=" +
              QString("%1").arg(m_cpu->get_ra(), 1, 16, QChar('0')).toUpper());
  if (m_cpu->get_neap() != 7)
    aL.append(
        " neap=" +
        QString("%1").arg(m_cpu->get_neap(), 1, 16, QChar('0')).toUpper());
  if (m_cpu->get_ap() != 0)
    aL.append(" ap=" +
              QString("%1").arg(m_cpu->get_ap(), 1, 16, QChar('0')).toUpper());
  if (m_cpu->get_nepc() != 7)
    aL.append(
        " nepc=" +
        QString("%1").arg(m_cpu->get_nepc(), 1, 16, QChar('0')).toUpper());
  if (m_cpu->get_pc() != 0)
    aL.append(" pc=" +
              QString("%1").arg(m_cpu->get_pc(), 1, 16, QChar('0')).toUpper());
  if (!m_cpu->get_ncbd())
    aL.append(
        " ncbd=" +
        QString("%1").arg(m_cpu->get_ncbd(), 1, 16, QChar('0')).toUpper());
  if (m_cpu->get_interrupt() != 15)
    aL.append(
        " interrupt=" +
        QString("%1").arg(m_cpu->get_interrupt(), 1, 16, QChar('0')).toUpper());
  if (!m_cpu->get_nas())
    aL.append(" nas=" +
              QString("%1").arg(m_cpu->get_nas(), 1, 16, QChar('0')).toUpper());
  if (!m_cpu->get_nrw())
    aL.append(" nrw=" +
              QString("%1").arg(m_cpu->get_nrw(), 1, 16, QChar('0')).toUpper());
  if (m_cpu->get_bd() != 0)
    aL.append(" bd=" +
              QString("%1").arg(m_cpu->get_bd(), 1, 16, QChar('0')).toUpper());
  if (m_cpu->get_dint() != 0)
    aL.append(
        " dint=" +
        QString("%1").arg(m_cpu->get_dint(), 1, 16, QChar('0')).toUpper());
  if (m_cpu->get_hint() != 0)
    aL.append(
        " hint=" +
        QString("%1").arg(m_cpu->get_hint(), 1, 16, QChar('0')).toUpper());
  if (m_cpu->get_sirq() != 0)
    aL.append(
        " sirq=" +
        QString("%1").arg(m_cpu->get_sirq(), 1, 16, QChar('0')).toUpper());
  if (m_cpu->get_sxirq() != 0)
    aL.append(
        " sxirq=" +
        QString("%1").arg(m_cpu->get_sxirq(), 1, 16, QChar('0')).toUpper());
  if (m_cpu->get_b9() != 0)
    aL.append(" b9=" +
              QString("%1").arg(m_cpu->get_b9(), 1, 16, QChar('0')).toUpper());
  if (m_cpu->get_b8b7b6() != 0)
    aL.append(
        " b8b7b6=" +
        QString("%1").arg(m_cpu->get_b8b7b6(), 1, 16, QChar('0')).toUpper());
  if (m_cpu->get_b5b4b3() != 0)
    aL.append(
        " b5b4b3=" +
        QString("%1").arg(m_cpu->get_b5b4b3(), 1, 16, QChar('0')).toUpper());
  if (m_cpu->get_b2b1() != 0)
    aL.append(
        " b2b1=" +
        QString("%1").arg(m_cpu->get_b2b1(), 1, 16, QChar('0')).toUpper());
  if (m_cpu->get_b0() != 0)
    aL.append(" b0=" +
              QString("%1").arg(m_cpu->get_b0(), 1, 16, QChar('0')).toUpper());
  if (m_cpu->get_cc() != 0)
    aL.append(" cc=" +
              QString("%1").arg(m_cpu->get_cc(), 1, 16, QChar('0')).toUpper());
  if (m_cpu->get_cn() != 0)
    aL.append(" cn=" +
              QString("%1").arg(m_cpu->get_cn(), 1, 16, QChar('0')).toUpper());
  if (m_cpu->get_cv() != 0)
    aL.append(" cv=" +
              QString("%1").arg(m_cpu->get_cv(), 1, 16, QChar('0')).toUpper());
  if (m_cpu->get_cz() != 0)
    aL.append(" cz=" +
              QString("%1").arg(m_cpu->get_cz(), 1, 16, QChar('0')).toUpper());
  if (m_cpu->get_ci() != 0)
    aL.append(" ci=" +
              QString("%1").arg(m_cpu->get_ci(), 1, 16, QChar('0')).toUpper());
  if (m_cpu->get_ch() != 0)
    aL.append(" ch=" +
              QString("%1").arg(m_cpu->get_ch(), 1, 16, QChar('0')).toUpper());
  if (m_cpu->get_cx() != 0)
    aL.append(" cx=" +
              QString("%1").arg(m_cpu->get_cx(), 1, 16, QChar('0')).toUpper());
  if (m_cpu->get_cs() != 0)
    aL.append(" cs=" +
              QString("%1").arg(m_cpu->get_cs(), 1, 16, QChar('0')).toUpper());
  if (!m_cpu->get_nhb())
    aL.append(" nhb=" +
              QString("%1").arg(m_cpu->get_nhb(), 1, 16, QChar('0')).toUpper());
  if (m_cpu->get_accsec() != 0)
    aL.append(
        " accsec=" +
        QString("%1").arg(m_cpu->get_accsec(), 1, 16, QChar('0')).toUpper());

  return aL;
}

void ExecWidget::genBusLog() {
  bool drivenH = false;
  bool drivenL = false;
  bool drivenA = false;

  // AccA
  if (m_cpu->get_nwa()) {
    if (m_cpu->get_ea() == 1) {
      if (!drivenL) {
        addLog("AccA esta usando la parte baja del bus de datos",
               LogType::Normal);
        drivenL = true;
      } else {
        addLog("[Error AccA]: La parte baja del bus de datos ya esta en uso",
               LogType::Error);
      }
    } else if (m_cpu->get_ea() == 2) {
      if (!drivenH) {
        addLog("AccA esta usando la parte alta del bus de datos",
               LogType::Normal);
        drivenH = true;
      } else {
        addLog("[Error AccA]: La parte alta del bus de datos ya esta en uso",
               LogType::Error);
      }
    }
  }

  // AccB
  if (m_cpu->get_nwb()) {
    if (m_cpu->get_eb() == 1) {
      if (!drivenL) {
        addLog("AccB esta usando la parte baja del bus de datos",
               LogType::Normal);
        drivenL = true;
      } else {
        addLog("[Error AccB]: La parte baja del bus de datos ya esta en uso",
               LogType::Error);
      }
    } else if (m_cpu->get_eb() == 2) {
      if (!drivenH) {
        addLog("AccB esta usando la parte alta del bus de datos",
               LogType::Normal);
        drivenH = true;
      } else {
        addLog("[Error AccB]: La parte alta del bus de datos ya esta en uso",
               LogType::Error);
      }
    }
  }

  // UPA
  if (!m_cpu->get_noeupa()) {
    if (!drivenL) {
      addLog("UPA esta usando la parte baja del bus de datos", LogType::Normal);
      drivenL = true;
    } else {
      addLog("[Error UPA]: La parte baja del bus de datos ya esta en uso",
             LogType::Error);
    }
    if (!drivenH) {
      addLog("UPA esta usando la parte alta del bus de datos", LogType::Normal);
      drivenH = true;
    } else {
      addLog("[Error UPA]: La parte alta del bus de datos ya esta en uso",
             LogType::Error);
    }
  }

  // X
  if (!m_cpu->get_x()) {
    if (m_cpu->get_nex() == 6 || m_cpu->get_nex() == 4 ||
        m_cpu->get_nex() == 2 || m_cpu->get_nex() == 0) {
      if (!drivenL) {
        addLog("X esta usando la parte baja del bus de datos", LogType::Normal);
        drivenL = true;
      } else {
        addLog("[Error X]: La parte baja del bus de datos ya esta en uso",
               LogType::Error);
      }
    }
    if (m_cpu->get_nex() == 5 || m_cpu->get_nex() == 4 ||
        m_cpu->get_nex() == 1 || m_cpu->get_nex() == 0) {
      if (!drivenH) {
        addLog("X esta usando la parte alta del bus de datos", LogType::Normal);
        drivenH = true;
      } else {
        addLog("[Error X]: La parte alta del bus de datos ya esta en uso",
               LogType::Error);
      }
    }
    if (m_cpu->get_nex() < 4) {
      if (!drivenA) {
        addLog("X esta usando el bus de direcciones", LogType::Normal);
        drivenA = true;
      } else {
        addLog("[Error X]: El bus de direcciones ya esta en uso",
               LogType::Error);
      }
    }
  }

  // RA
  if (!m_cpu->get_ra()) {
    if (m_cpu->get_nera() == 6 || m_cpu->get_nera() == 4 ||
        m_cpu->get_nera() == 2 || m_cpu->get_nera() == 0) {
      if (!drivenL) {
        addLog("RA esta usando la parte baja del bus de datos",
               LogType::Normal);
        drivenL = true;
      } else {
        addLog("[Error RA]: La parte baja del bus de datos ya esta en uso",
               LogType::Error);
      }
    }
    if (m_cpu->get_nera() == 5 || m_cpu->get_nera() == 4 ||
        m_cpu->get_nera() == 1 || m_cpu->get_nera() == 0) {
      if (!drivenH) {
        addLog("RA esta usando la parte alta del bus de datos",
               LogType::Normal);
        drivenH = true;
      } else {
        addLog("[Error RA]: La parte alta del bus de datos ya esta en uso",
               LogType::Error);
      }
    }
    if (m_cpu->get_nera() < 4) {
      if (!drivenA) {
        addLog("RA esta usando el bus de direcciones", LogType::Normal);
        drivenA = true;
      } else {
        addLog("[Error RA]: El bus de direcciones ya esta en uso",
               LogType::Error);
      }
    }
  }

  // AP
  if (!m_cpu->get_ap()) {
    if (m_cpu->get_neap() == 6 || m_cpu->get_neap() == 4 ||
        m_cpu->get_neap() == 2 || m_cpu->get_neap() == 0) {
      if (!drivenL) {
        addLog("AP esta usando la parte baja del bus de datos",
               LogType::Normal);
        drivenL = true;
      } else {
        addLog("[Error AP]: La parte baja del bus de datos ya esta en uso",
               LogType::Error);
      }
    }
    if (m_cpu->get_neap() == 5 || m_cpu->get_neap() == 4 ||
        m_cpu->get_neap() == 1 || m_cpu->get_neap() == 0) {
      if (!drivenH) {
        addLog("AP esta usando la parte alta del bus de datos",
               LogType::Normal);
        drivenH = true;
      } else {
        addLog("[Error AP]: La parte alta del bus de datos ya esta en uso",
               LogType::Error);
      }
    }
    if (m_cpu->get_neap() < 4) {
      if (!drivenA) {
        addLog("AP esta usando el bus de direcciones", LogType::Normal);
        drivenA = true;
      } else {
        addLog("[Error AP]: El bus de direcciones ya esta en uso",
               LogType::Error);
      }
    }
  }

  // PC
  if (!m_cpu->get_pc()) {
    if (m_cpu->get_nepc() == 6 || m_cpu->get_nepc() == 4 ||
        m_cpu->get_nepc() == 2 || m_cpu->get_nepc() == 0) {
      if (!drivenL) {
        addLog("PC esta usando la parte baja del bus de datos",
               LogType::Normal);
        drivenL = true;
      } else {
        addLog("[Error PC]: La parte baja del bus de datos ya esta en uso",
               LogType::Error);
      }
    }
    if (m_cpu->get_nepc() == 5 || m_cpu->get_nepc() == 4 ||
        m_cpu->get_nepc() == 1 || m_cpu->get_nepc() == 0) {
      if (!drivenH) {
        addLog("PC esta usando la parte alta del bus de datos",
               LogType::Normal);
        drivenH = true;
      } else {
        addLog("[Error PC]: La parte alta del bus de datos ya esta en uso",
               LogType::Error);
      }
    }
    if (m_cpu->get_nepc() < 4) {
      if (!drivenA) {
        addLog("PC esta usando el bus de direcciones", LogType::Normal);
        drivenA = true;
      } else {
        addLog("[Error PC]: El bus de direcciones ya esta en uso",
               LogType::Error);
      }
    }
  }

  // memory
  if (!m_cpu->get_nas() && m_cpu->get_nrw() == 1) {
    if (!m_cpu->get_bd()) {
      if (!drivenL) {
        addLog("Memoria externa esta usando la parte baja del bus de datos",
               LogType::Normal);
        drivenL = true;
      } else {
        addLog("[Error Memoria]: La parte baja del bus de datos ya esta en uso",
               LogType::Error);
      }
    }
    if (m_cpu->get_bd() == 1) {
      if (!drivenH) {
        addLog("Memoria externa esta usando la parte alta del bus de datos",
               LogType::Normal);
        drivenH = true;
      } else {
        addLog("[Error Memoria]: La parte alta del bus de datos ya esta en uso",
               LogType::Error);
      }
    }
  }

  // uci (vector address)
  if (m_cpu->get_interrupt() == 1) {
    if (!drivenL) {
      addLog("UCI esta usando la parte baja del bus de datos", LogType::Normal);
      drivenL = true;
    } else {
      addLog("[Error UCI]: La parte baja del bus de datos ya esta en uso",
             LogType::Error);
    }
    if (!drivenH) {
      addLog("UCI esta usando la parte alta del bus de datos", LogType::Normal);
      drivenH = true;
    } else {
      addLog("[Error UCI]: La parte alta del bus de datos ya esta en uso",
             LogType::Error);
    }
  }

  // ccr
  if (!m_cpu->get_nhb()) {
    if (!drivenL) {
      addLog("CCR esta usando la parte baja del bus de datos", LogType::Normal);
      drivenL = true;
    } else {
      addLog("[Error CCR]: La parte baja del bus de datos ya esta en uso",
             LogType::Error);
    }
  }

  return;
}

void ExecWidget::generateLog() {

  // current state and active lines
  addLog(
      "[" + QString("%1").arg(m_cpu->getOldmPC(), 3, 16, QChar('0')).toUpper() +
          "] prueba=" +
          QString("%1").arg(m_cpu->get_prueba(), 2, 16, QChar('0')).toUpper() +
          " vf=" +
          QString("%1").arg(m_cpu->get_vf(), 1, 16, QChar('0')).toUpper() +
          " MI=" +
          QString("%1").arg(m_cpu->get_mi(), 2, 16, QChar('0')).toUpper() +
          " liga=" +
          QString("%1").arg(m_cpu->get_liga(), 3, 16, QChar('0')).toUpper(),
      LogType::Info);

  // Active lines messages
  QString activeLines = genActiveLines();
  addLog(activeLines, LogType::Normal);

  // Bus messages
  genBusLog();

  // next state
  addLog("eSiguiente=" +
             QString("%1").arg(m_cpu->getmPC(), 3, 16, QChar('0')).toUpper(),
         LogType::Remark);
}

void ExecWidget::clearLog() { ui->textEdit->clear(); }

void ExecWidget::on_btnRun_clicked() {
  runSelectedMode();
}

void ExecWidget::on_btnStop_clicked() {
  stopExecution();
}

void ExecWidget::on_btnReset_clicked() {
  stopExecution();
  m_cpu->reset();
  refreshUI();
  refreshInfo();
  clearLog();
}
