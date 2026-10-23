/*
 * Emulador Motorola 68HC11
 * Copyright (C) 2026 Julio Cruz
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef RAMWIDGET_H
#define RAMWIDGET_H

#include <QTableWidget>
#include <QWidget>
#include <array>

#include "CPU68HC11.h"
#include "memory/Memory.h"
#include "hexbytedelegate.h"
#include "rammodel.h"

namespace Ui {
class RamWidget;
}

class RamWidget : public QWidget {
  Q_OBJECT

public:
  explicit RamWidget(Memory *memory, CPU68HC11 *cpu, QWidget *parent = nullptr);
  ~RamWidget();

  void updateMemory();

  void refresh();

private slots:

  void onItemChanged(QTableWidgetItem *item);

private:
  Ui::RamWidget *ui;

  Memory *memory;
  CPU68HC11 *cpu;
  QTableWidget *table;

  QTableView *tableView;
  RamModel *model;
  HexByteDelegate *delegate;

  std::array<uint8_t, 65536> previousMemory;
  bool firstUpdate = true;
  bool updatingMemory = false;

  const QColor normalColor = Qt::white;
  const QColor changedColor = QColor(255, 255, 120);
  const QColor pcColor = QColor(255, 117, 184);   // rosa
  const QColor spColor = QColor(71, 173, 254);    // azul
  const QColor xColor = QColor(156, 160, 237);    // morado
  const QColor yColor = QColor(0, 255, 133);      // verde 2
  const QColor rdirColor = QColor(180, 255, 180); // verde

  void initializeTable();
  void updateCell(uint16_t address, uint8_t value, int row, int col);

  void centerOnAddress(uint16_t address);
};

#endif // RAMWIDGET_H
