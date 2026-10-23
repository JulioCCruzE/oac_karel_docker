/*
 * Emulador Motorola 68HC11
 * Copyright (C) 2026 Julio Cruz
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef RAMMODEL_H
#define RAMMODEL_H

#include <QAbstractItemModel>
#include <QColor>
#include <QFont>
#include <array>
#include <vector>

#include "CPU68HC11.h"
#include "memory/Memory.h"

class RamModel : public QAbstractItemModel {
  Q_OBJECT

public:
  explicit RamModel(Memory *memory, CPU68HC11 *cpu, QObject *parent = nullptr);

  // Header:
  QVariant headerData(int section, Qt::Orientation orientation,
                      int role = Qt::DisplayRole) const override;

  // Basic functionality:
  QModelIndex index(int row, int column,
                    const QModelIndex &parent = QModelIndex()) const override;
  QModelIndex parent(const QModelIndex &index) const override;

  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  int columnCount(const QModelIndex &parent = QModelIndex()) const override;

  QVariant data(const QModelIndex &index,
                int role = Qt::DisplayRole) const override;

  Qt::ItemFlags flags(const QModelIndex &index) const;

  bool setData(const QModelIndex &index, const QVariant &value, int role);

  bool writeByte(uint16_t address, uint8_t value);

  bool refresh();

  uint16_t getRDIR() const;

private:
  Memory *memory;
  CPU68HC11 *cpu;

  std::array<uint8_t, 65536> previousMemory;
  std::array<bool, 65536> changedMemory{};
  std::vector<uint16_t> modifiedAddresses;
  std::vector<uint16_t> highlightedAddresses;
  std::vector<int> changedCells;
  uint16_t previousRDIR = 0;
  uint16_t previousPC = 0;
  uint16_t previousAP = 0;
  uint16_t previousX = 0;
  uint16_t previousY = 0;
  bool firstRefresh = true;

  const QColor normalColor = Qt::white;
  const QColor changedColor = QColor(255, 255, 120);
  const QColor pcColor = QColor(255, 117, 184);   // rosa
  const QColor spColor = QColor(71, 173, 254);    // azul
  const QColor xColor = QColor(156, 160, 237);    // morado
  const QColor yColor = QColor(0, 255, 133);      // verde 2
  const QColor rdirColor = QColor(180, 255, 180); // verde

  QString getDisplayText(uint16_t address, int column) const;
  QColor getBackgroundColor(uint16_t address) const;
  QFont getFont(uint16_t address) const;
  void markAddressChanged(uint16_t address, bool markAddressColumn = false);
  // Qt::Alignment getAlignment() const;
};

#endif // RAMMODEL_H
