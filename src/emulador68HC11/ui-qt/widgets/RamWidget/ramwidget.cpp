/*
 * Emulador Motorola 68HC11
 * Copyright (C) 2026 Julio Cruz
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "ramwidget.h"
#include "ui_ramwidget.h"

#include <QHeaderView>
#include <QVBoxLayout>

RamWidget::RamWidget(Memory *memory, CPU68HC11 *cpu, QWidget *parent)
    : QWidget(parent), ui(new Ui::RamWidget) {
  ui->setupUi(this);

  // table = new QTableWidget(this);
  // Quitando la primer version
  // initializeTable();
  //
  // auto *layout = new QVBoxLayout(this);
  // layout->addWidget(table);
  //
  // setLayout(layout);
  //
  // previousMemory.fill(0);
  // updateMemory();

  // add model and delegate
  tableView = new QTableView(this);
  model = new RamModel(memory, cpu, this);
  delegate = new HexByteDelegate(this);

  // connect
  tableView->setModel(model);
  // qDebug() << "Modelo:" << tableView->model();

  tableView->setItemDelegate(delegate);

  // inicializa tabla
  tableView->verticalHeader()->setVisible(false);
  tableView->setColumnWidth(0, 70);
  for (int i = 1; i < 17; i++)
    tableView->setColumnWidth(i, 35);
  tableView->setShowGrid(true);
  tableView->setSelectionMode(QAbstractItemView::SingleSelection);
  tableView->setSelectionBehavior(QAbstractItemView::SelectItems);
  tableView->verticalHeader()->setDefaultSectionSize(22);
  QFont font = QFontDatabase::systemFont(QFontDatabase::FixedFont);

  tableView->setFont(font);

  auto *layout = new QVBoxLayout(this);
  layout->addWidget(tableView);

  setLayout(layout);
}

RamWidget::~RamWidget() { delete ui; }

void RamWidget::initializeTable() {

  table->setRowCount(4096);  // 64 KB / 16 bytes
  table->setColumnCount(17); // Dirección + 16 bytes

  QStringList headers;

  headers << "Addr";

  for (int i = 0; i < 16; i++)
    headers << QString("%1").arg(i, 1, 16).toUpper();

  table->setHorizontalHeaderLabels(headers);

  table->verticalHeader()->setVisible(false);

  table->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);

  // table->setEditTriggers(QAbstractItemView::NoEditTriggers);
  table->setEditTriggers(QAbstractItemView::DoubleClicked |
                         QAbstractItemView::EditKeyPressed);

  table->setSelectionMode(QAbstractItemView::SingleSelection);

  table->setSelectionBehavior(QAbstractItemView::SelectItems);

  for (int row = 0; row < 4096; row++) {
    for (int col = 0; col < 17; col++) {
      QTableWidgetItem *item = new QTableWidgetItem();
      table->setItem(row, col, item);
      if (col == 0) {
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
      }
    }
  }

  connect(table, &QTableWidget::itemChanged, this, &RamWidget::onItemChanged);
}

void RamWidget::updateCell(uint16_t address, uint8_t value, int row, int col) {

  QTableWidgetItem *item = table->item(row, col + 1);

  item->setText(QString("%1").arg(value, 2, 16, QChar('0')).toUpper());

  if (!firstUpdate) {
    if (cpu->getRDIR() == address + col) {
      item->setBackground(rdirColor);
    } else if (cpu->getPC() == address + col) {
      item->setBackground(pcColor);
    } else if (cpu->getAP() == address + col) {
      item->setBackground(spColor);
    } else if (cpu->getX(0) == address + col) {
      item->setBackground(xColor);
    } else if (cpu->getX(1) == address + col) {
      item->setBackground(yColor);
    } else if (previousMemory[address + col] != value) {
      item->setBackground(changedColor);
    } else {
      item->setBackground(normalColor);
    }
    // item->setBackground(previousMemory[address + col] != value ? changedColor
    // : normalColor);
  }

  previousMemory[address + col] = value;
}

void RamWidget::updateMemory() {
  int row = 0;
  updatingMemory = true;

  for (uint32_t address = 0x0; address <= 0xFFFF; address += 16) {
    table->item(row, 0)->setText(
        QString("%1").arg(address, 4, 16, QChar('0')).toUpper());

    for (int col = 0; col < 16; col++) {
      // uint8_t value = memory->read(address + col);
      uint8_t value = memory->readRam(address + col);

      updateCell(address, value, row, col);
    }
    row++;
  }
  firstUpdate = false;
  updatingMemory = false;
}

void RamWidget::onItemChanged(QTableWidgetItem *item) {
  if (updatingMemory)
    return;

  int row = item->row();
  int column = item->column();

  if (column == 0)
    return;

  uint16_t address = row * 16 + (column - 1);

  QString text = item->text().trimmed().toUpper();

  bool ok;

  int value = text.toInt(&ok, 16);

  if (!ok || value < 0 || value > 0xFF) {
    updateMemory();
    return;
  }

  memory->writeFromEditor(address, static_cast<uint8_t>(value));

  updateMemory();

  return;
}

void RamWidget::centerOnAddress(uint16_t address) {
  int row = address / 16;
  int column = (address % 16) + 1;

  QModelIndex index = model->index(row, column);
  // tableView->scrollTo(index, QAbstractItemView::PositionAtCenter);
  //
  QRect rect = tableView->visualRect(index);

  if (!tableView->viewport()->rect().contains(rect)) {
    tableView->scrollTo(index, QAbstractItemView::PositionAtCenter);
  }
}

void RamWidget::refresh() {
  if (model->refresh())
    centerOnAddress(model->getRDIR());
}
