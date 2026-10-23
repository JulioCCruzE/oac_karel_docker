/*
 * Emulador Motorola 68HC11
 * Copyright (C) 2026 Julio Cruz
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "rammodel.h"

#include <algorithm>

RamModel::RamModel(Memory *memory, CPU68HC11 *cpu, QObject *parent)
    : QAbstractItemModel(parent), memory(memory), cpu(cpu) {
  std::copy_n(memory->ramData(), previousMemory.size(), previousMemory.begin());
  previousRDIR = cpu->getRDIR();
  previousPC = cpu->getPC();
  previousAP = cpu->getAP();
  previousX = cpu->getX(0);
  previousY = cpu->getX(1);
  modifiedAddresses.reserve(64);
  highlightedAddresses.reserve(64);
  changedCells.reserve(128);
}

QVariant RamModel::headerData(int section, Qt::Orientation orientation,
                              int role) const {
  if (role != Qt::DisplayRole)
    return QVariant();

  if (orientation == Qt::Horizontal) {
    if (section == 0)
      return "Addr";

    return QString("%1").arg(section - 1, 2, 16, QChar('0')).toUpper();
  }

  // Encabezado vertical
  int address = section * 16;

  return QString("%1").arg(address, 4, 16, QChar('0')).toUpper();
}

QModelIndex RamModel::index(int row, int column,
                            const QModelIndex &parent) const {
  if (parent.isValid())
    return QModelIndex();

  if (row < 0 || row >= rowCount() || column < 0 || column >= columnCount())
    return QModelIndex();

  return createIndex(row, column);
}

QModelIndex RamModel::parent(const QModelIndex &index) const {
  Q_UNUSED(index);
  return QModelIndex();
}

int RamModel::rowCount(const QModelIndex &parent) const {
  if (parent.isValid())
    return 0;
  // qDebug() << "rowCount";
  return 4096;
}

int RamModel::columnCount(const QModelIndex &parent) const {
  if (parent.isValid())
    return 0;

  // qDebug() << "columnCount";
  return 17;
}

QVariant RamModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid())
    return QVariant();

  uint16_t address = index.row() * 16;

  if (index.column() > 0) {
    address += index.column() - 1;
  }

  switch (role) {
  case Qt::DisplayRole:
    return getDisplayText(address, index.column());

  case Qt::BackgroundRole:
    if (index.column() == 0)
      return normalColor;
    else
      return getBackgroundColor(address);

  case Qt::FontRole:
    return getFont(address);

    // INFO: there are more variants
    // case Qt::TextAlignmentRole:
    //   return getAlignment();

  default:
    return {};
  }

  // return QVariant();
}

Qt::ItemFlags RamModel::flags(const QModelIndex &index) const {
  if (!index.isValid())
    return Qt::NoItemFlags;

  // Columna de direcciones
  if (index.column() == 0) {
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
  }

  // Bytes de memoria
  return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

bool RamModel::setData(const QModelIndex &index, const QVariant &value,
                       int role) {
  if (!index.isValid())
    return false;

  if (role != Qt::EditRole)
    return false;

  if (index.column() == 0)
    return false;

  uint16_t address = index.row() * 16 + (index.column() - 1);

  QString text = value.toString().trimmed().toUpper();

  bool ok;

  int byte = text.toInt(&ok, 16);

  if (!ok)
    return false;

  if (byte < 0 || byte > 0xFF)
    return false;

  if (!writeByte(address, static_cast<uint8_t>(byte)))
    return false;

  refresh();

  return true;
}

bool RamModel::writeByte(uint16_t address, uint8_t value) {

  // INFO: add  more logic and rules for write in memory
  memory->writeFromEditor(address, value);

  return true;
}

QString RamModel::getDisplayText(uint16_t address, int column) const {

  if (column == 0) {
    return QString("%1").arg(address, 4, 16, QChar('0')).toUpper();
  }
  uint8_t value = memory->readRam(address);
  return QString("%1").arg(value, 2, 16, QChar('0')).toUpper();
}

QColor RamModel::getBackgroundColor(uint16_t address) const {

  if (cpu->getRDIR() == address) {
    return rdirColor;
  } else if (cpu->getPC() == address) {
    return pcColor;
  } else if (cpu->getAP() == address) {
    return spColor;
  } else if (cpu->getX(0) == address) {
    return xColor;
  } else if (cpu->getX(1) == address) {
    return yColor;
  } else if (changedMemory[address]) {
    return changedColor;
  } else {
    return normalColor;
  }
}

QFont RamModel::getFont(uint16_t address) const {
  QFont f;

  f.setBold(address == cpu->getPC());

  return f;
}

void RamModel::markAddressChanged(uint16_t address, bool markAddressColumn) {
  int row = address / 16;
  changedCells.push_back(row * columnCount() + address % 16 + 1);
  if (markAddressColumn && address % 16 == 0)
    changedCells.push_back(row * columnCount());
}

bool RamModel::refresh() {
  changedCells.clear();

  for (uint16_t address : highlightedAddresses) {
    changedMemory[address] = false;
    markAddressChanged(address);
  }
  highlightedAddresses.clear();

  memory->takeModifiedAddresses(modifiedAddresses);
  for (uint16_t address : modifiedAddresses) {
    uint8_t value = memory->readRam(address);
    bool memoryChanged = previousMemory[address] != value;
    previousMemory[address] = value;
    changedMemory[address] = memoryChanged;
    if (memoryChanged) {
      highlightedAddresses.push_back(address);
      markAddressChanged(address);
    }
  }

  uint16_t currentRDIR = cpu->getRDIR();
  uint16_t currentPC = cpu->getPC();
  uint16_t currentAP = cpu->getAP();
  uint16_t currentX = cpu->getX(0);
  uint16_t currentY = cpu->getX(1);

  if (previousRDIR != currentRDIR) {
    markAddressChanged(previousRDIR);
    markAddressChanged(currentRDIR);
  }
  if (previousPC != currentPC) {
    markAddressChanged(previousPC, true);
    markAddressChanged(currentPC, true);
  }
  if (previousAP != currentAP) {
    markAddressChanged(previousAP);
    markAddressChanged(currentAP);
  }
  if (previousX != currentX) {
    markAddressChanged(previousX);
    markAddressChanged(currentX);
  }
  if (previousY != currentY) {
    markAddressChanged(previousY);
    markAddressChanged(currentY);
  }

  bool rdirChanged = firstRefresh || previousRDIR != currentRDIR;
  firstRefresh = false;
  previousRDIR = currentRDIR;
  previousPC = currentPC;
  previousAP = currentAP;
  previousX = currentX;
  previousY = currentY;

  std::sort(changedCells.begin(), changedCells.end());
  changedCells.erase(std::unique(changedCells.begin(), changedCells.end()),
                     changedCells.end());

  size_t cell = 0;
  while (cell < changedCells.size()) {
    int firstCell = changedCells[cell];
    int row = firstCell / columnCount();
    int lastCell = firstCell;

    while (cell + 1 < changedCells.size() &&
           changedCells[cell + 1] == lastCell + 1 &&
           changedCells[cell + 1] / columnCount() == row) {
      ++cell;
      lastCell = changedCells[cell];
    }

    emit dataChanged(index(row, firstCell % columnCount()),
                     index(row, lastCell % columnCount()));
    ++cell;
  }

  return rdirChanged;
}

uint16_t RamModel::getRDIR() const { return cpu->getRDIR(); }
