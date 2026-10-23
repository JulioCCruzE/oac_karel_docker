/*
 * Emulador Motorola 68HC11
 * Copyright (C) 2026 Julio Cruz
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef ROMWIDGET_H
#define ROMWIDGET_H

#include <QFontDatabase>
#include <QHeaderView>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QWidget>

#include "CPU68HC11.h"
#include "common/Constants.h"
#include "common/Types.h"
#include "memory/Memory.h"

namespace Ui {
class RomWidget;
}

class RomWidget : public QWidget {
  Q_OBJECT

public:
  explicit RomWidget(Memory *memory, CPU68HC11 *cpu, QWidget *parent = nullptr);
  ~RomWidget();

  void updateMemory();
  void refreshmPC();

private:
  Ui::RomWidget *ui;

  Memory *memory;
  CPU68HC11 *cpu;
  QTableWidget *table;

  RomWord state;
  uint16_t oldmPC;

  uint8_t prueba;
  uint8_t vf;
  uint8_t mi;
  uint16_t liga;
  uint8_t ncri;
  uint8_t eb;
  uint8_t nwb;
  uint8_t ea;
  uint8_t nwa;
  uint8_t selbus;
  uint8_t dest;
  uint8_t functn;
  // Low part of my structure
  uint8_t source;
  uint8_t noeupa;
  uint8_t ndupa;
  uint8_t selmux;
  uint8_t nex;
  uint8_t x;
  uint8_t enay;
  uint8_t nera;
  uint8_t ra;
  uint8_t neap;
  uint8_t ap;
  uint8_t nepc;
  uint8_t pc;
  uint8_t ncbd;
  uint8_t interrupt;
  uint8_t nas;
  uint8_t nrw;
  uint8_t bd;
  uint8_t dint;
  uint8_t hint;
  uint8_t sirq;
  uint8_t sxirq;
  uint8_t b9;
  uint8_t b8b7b6;
  uint8_t b5b4b3;
  uint8_t b2b1;
  uint8_t b0;
  uint8_t cc;
  uint8_t cn;
  uint8_t cv;
  uint8_t cz;
  uint8_t ci;
  uint8_t ch;
  uint8_t cx;
  uint8_t cs;
  uint8_t nhb;
  uint8_t accsec;

  const QColor normalColor = Qt::white;
  const QColor changedColor = QColor(255, 255, 120);
  const QColor mpcColor = QColor(255, 117, 184);    // rosa
  const QColor spColor = QColor(71, 173, 254);      // azul
  const QColor xColor = QColor(156, 160, 237);      // morado
  const QColor yColor = QColor(0, 255, 133);        // verde 2
  const QColor activeColor = QColor(180, 255, 180); // verde

  void initializeTable();
  void updateCell(int row);
  uint32_t getBits(const RomWord &w, int shift, uint32_t mask);
  void getState(const RomWord &state);
};

#endif // ROMWIDGET_H
