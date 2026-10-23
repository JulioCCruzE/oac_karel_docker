/*
 * Emulador Motorola 68HC11
 * Copyright (C) 2026 Julio Cruz
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "romwidget.h"
#include "ui_romwidget.h"

RomWidget::RomWidget(Memory *memory, CPU68HC11 *cpu, QWidget *parent)
    : QWidget(parent), memory(memory), cpu(cpu), ui(new Ui::RomWidget) {
  ui->setupUi(this);

  table = new QTableWidget(this);
  initializeTable();

  auto *layout = new QVBoxLayout(this);

  layout->addWidget(table);
  setLayout(layout);

  // previousMemory.fill(0);
  updateMemory();
}

RomWidget::~RomWidget() { delete ui; }

void RomWidget::initializeTable() {

  int totalCol = 50;
  table->setRowCount(4096); // 64 KB
  table->setColumnCount(totalCol);

  QStringList headers;

  headers << "Estado";
  headers << "Prueba";
  headers << "VF";
  headers << "MI";
  headers << "LIGA";
  headers << "ncri";
  headers << "eb";
  headers << "nwb";
  headers << "ea";
  headers << "nwa";
  headers << "selbus";
  headers << "dest";
  headers << "functn";
  headers << "source";
  headers << "noeupa";
  headers << "ndupa";
  headers << "selmux";
  headers << "nex";
  headers << "x";
  headers << "enay";
  headers << "nera";
  headers << "ra";
  headers << "neap";
  headers << "ap";
  headers << "nepc";
  headers << "pc";
  headers << "ncbd";
  headers << "int";
  headers << "nas";
  headers << "nrw";
  headers << "bd";
  headers << "dint";
  headers << "hint";
  headers << "sirq";
  headers << "sxirq";
  headers << "b9";
  headers << "b8b7b6";
  headers << "b5b4b3";
  headers << "b2b1";
  headers << "b0";
  headers << "cc";
  headers << "cn";
  headers << "cv";
  headers << "cz";
  headers << "ci";
  headers << "ch";
  headers << "cx";
  headers << "cs";
  headers << "nhb";
  headers << "accsec";

  table->setHorizontalHeaderLabels(headers);

  table->verticalHeader()->setVisible(false);
  table->verticalHeader()->setDefaultSectionSize(22);

  table->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);

  table->setEditTriggers(QAbstractItemView::NoEditTriggers);
  // table->setEditTriggers(QAbstractItemView::DoubleClicked |
  //                        QAbstractItemView::EditKeyPressed);

  table->setSelectionMode(QAbstractItemView::SingleSelection);

  table->setSelectionBehavior(QAbstractItemView::SelectItems);

  for (int row = 0; row < 4096; row++) {
    for (int col = 0; col < totalCol; col++) {
      QTableWidgetItem *item = new QTableWidgetItem();
      table->setItem(row, col, item);
      if (col == 0) {
        table->setColumnWidth(col, 50);
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
      } else if (col == 10 || col == 12 || col == 13 || col == 14 ||
                 col == 15 || col == 16 || col == 49) {
        table->setColumnWidth(col, 50);
      } else if (col == 36 || col == 37) {
        table->setColumnWidth(col, 55);
      } else {
        table->setColumnWidth(col, 35);
      }
    }
  }

  // tableView->setShowGrid(true);
  QFont font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
  table->setFont(font);

  // connect(table, &QTableWidget::itemChanged, this,
  // &RamWidget::onItemChanged);
}

void RomWidget::updateCell(int row) {

  // QTableWidgetItem *item = table->item(row, 1);

  table->item(row, 1)->setText(
      QString("%1").arg(prueba, 2, 16, QChar('0')).toUpper());
  table->item(row, 2)->setText(
      QString("%1").arg(vf, 1, 2, QChar('0')).toUpper());
  table->item(row, 3)->setText(
      QString("%1").arg(mi, 1, 16, QChar('0')).toUpper());
  table->item(row, 4)->setText(
      QString("%1").arg(liga, 3, 16, QChar('0')).toUpper());
  table->item(row, 5)->setText(
      QString("%1").arg(ncri, 1, 16, QChar('0')).toUpper());
  table->item(row, 6)->setText(
      QString("%1").arg(eb, 1, 16, QChar('0')).toUpper());
  table->item(row, 7)->setText(
      QString("%1").arg(nwb, 1, 16, QChar('0')).toUpper());
  table->item(row, 8)->setText(
      QString("%1").arg(ea, 1, 16, QChar('0')).toUpper());
  table->item(row, 9)->setText(
      QString("%1").arg(nwa, 1, 16, QChar('0')).toUpper());
  table->item(row, 10)->setText(
      QString("%1").arg(selbus, 1, 16, QChar('0')).toUpper());
  table->item(row, 11)->setText(
      QString("%1").arg(dest, 1, 16, QChar('0')).toUpper());
  table->item(row, 12)->setText(
      QString("%1").arg(functn, 1, 16, QChar('0')).toUpper());
  table->item(row, 13)->setText(
      QString("%1").arg(source, 1, 16, QChar('0')).toUpper());
  table->item(row, 14)->setText(
      QString("%1").arg(noeupa, 1, 16, QChar('0')).toUpper());
  table->item(row, 15)->setText(
      QString("%1").arg(ndupa, 1, 16, QChar('0')).toUpper());
  table->item(row, 16)->setText(
      QString("%1").arg(selmux, 1, 16, QChar('0')).toUpper());
  table->item(row, 17)->setText(
      QString("%1").arg(nex, 1, 16, QChar('0')).toUpper());

  table->item(row, 18)->setText(
      QString("%1").arg(x, 1, 16, QChar('0')).toUpper());
  table->item(row, 19)->setText(
      QString("%1").arg(enay, 1, 16, QChar('0')).toUpper());
  table->item(row, 20)->setText(
      QString("%1").arg(nera, 1, 16, QChar('0')).toUpper());
  table->item(row, 21)->setText(
      QString("%1").arg(ra, 1, 16, QChar('0')).toUpper());
  table->item(row, 22)->setText(
      QString("%1").arg(neap, 1, 16, QChar('0')).toUpper());
  table->item(row, 23)->setText(
      QString("%1").arg(ap, 1, 16, QChar('0')).toUpper());
  table->item(row, 24)->setText(
      QString("%1").arg(nepc, 1, 16, QChar('0')).toUpper());
  table->item(row, 25)->setText(
      QString("%1").arg(pc, 1, 16, QChar('0')).toUpper());
  table->item(row, 26)->setText(
      QString("%1").arg(ncbd, 1, 16, QChar('0')).toUpper());
  table->item(row, 27)->setText(
      QString("%1").arg(interrupt, 1, 16, QChar('0')).toUpper());
  table->item(row, 28)->setText(
      QString("%1").arg(nas, 1, 16, QChar('0')).toUpper());
  table->item(row, 29)->setText(
      QString("%1").arg(nrw, 1, 16, QChar('0')).toUpper());
  table->item(row, 30)->setText(
      QString("%1").arg(bd, 1, 16, QChar('0')).toUpper());
  table->item(row, 31)->setText(
      QString("%1").arg(dint, 1, 16, QChar('0')).toUpper());
  table->item(row, 32)->setText(
      QString("%1").arg(hint, 1, 16, QChar('0')).toUpper());
  table->item(row, 33)->setText(
      QString("%1").arg(sirq, 1, 16, QChar('0')).toUpper());
  table->item(row, 34)->setText(
      QString("%1").arg(sxirq, 1, 16, QChar('0')).toUpper());
  table->item(row, 35)->setText(
      QString("%1").arg(b9, 1, 16, QChar('0')).toUpper());
  table->item(row, 36)->setText(
      QString("%1").arg(b8b7b6, 1, 16, QChar('0')).toUpper());
  table->item(row, 37)->setText(
      QString("%1").arg(b5b4b3, 1, 16, QChar('0')).toUpper());
  table->item(row, 38)->setText(
      QString("%1").arg(b2b1, 1, 16, QChar('0')).toUpper());
  table->item(row, 39)->setText(
      QString("%1").arg(b0, 1, 16, QChar('0')).toUpper());
  table->item(row, 40)->setText(
      QString("%1").arg(cc, 1, 16, QChar('0')).toUpper());
  table->item(row, 41)->setText(
      QString("%1").arg(cn, 1, 16, QChar('0')).toUpper());
  table->item(row, 42)->setText(
      QString("%1").arg(cv, 1, 16, QChar('0')).toUpper());
  table->item(row, 43)->setText(
      QString("%1").arg(cz, 1, 16, QChar('0')).toUpper());
  table->item(row, 44)->setText(
      QString("%1").arg(ci, 1, 16, QChar('0')).toUpper());
  table->item(row, 45)->setText(
      QString("%1").arg(ch, 1, 16, QChar('0')).toUpper());
  table->item(row, 46)->setText(
      QString("%1").arg(cx, 1, 16, QChar('0')).toUpper());
  table->item(row, 47)->setText(
      QString("%1").arg(cs, 1, 16, QChar('0')).toUpper());
  table->item(row, 48)->setText(
      QString("%1").arg(nhb, 1, 16, QChar('0')).toUpper());
  table->item(row, 49)->setText(
      QString("%1").arg(accsec, 1, 16, QChar('0')).toUpper());

  if (cpu->getmPC() == row) {
    oldmPC = cpu->getmPC();
    table->item(row, 0)->setBackground(mpcColor);
  } else {
    table->item(row, 0)->setBackground(normalColor);
  }
}

void RomWidget::refreshmPC() {
  uint16_t row = cpu->getmPC();

  for (int col = 0; col < 50; col++) {
    table->item(oldmPC, col)->setBackground(normalColor);
  }

  // get the full word at mPC address
  state = memory->fetchRom(row);
  getState(state);

  // paint each cell
  table->item(row, 0)->setBackground(mpcColor);

  if (!ncri)
    table->item(row, 5)->setBackground(activeColor);
  if (eb != 0)
    table->item(row, 6)->setBackground(activeColor);
  if (!nwb)
    table->item(row, 7)->setBackground(activeColor);
  if (ea != 0)
    table->item(row, 8)->setBackground(activeColor);
  if (!nwa)
    table->item(row, 9)->setBackground(activeColor);
  if (selbus)
    table->item(row, 10)->setBackground(activeColor);
  if (dest != 0)
    table->item(row, 11)->setBackground(activeColor);
  if (functn != 0)
    table->item(row, 12)->setBackground(activeColor);
  if (source != 0)
    table->item(row, 13)->setBackground(activeColor);
  if (!noeupa)
    table->item(row, 14)->setBackground(activeColor);
  if (!ndupa)
    table->item(row, 15)->setBackground(activeColor);
  if (selmux != 0)
    table->item(row, 16)->setBackground(activeColor);
  if (nex != 7)
    table->item(row, 17)->setBackground(activeColor);
  if (x != 0)
    table->item(row, 18)->setBackground(activeColor);
  if (enay != 0)
    table->item(row, 19)->setBackground(activeColor);

  if (nera != 7)
    table->item(row, 20)->setBackground(activeColor);
  if (ra != 0)
    table->item(row, 21)->setBackground(activeColor);
  if (neap != 7)
    table->item(row, 22)->setBackground(activeColor);
  if (ap != 0)
    table->item(row, 23)->setBackground(activeColor);
  if (nepc != 7)
    table->item(row, 24)->setBackground(activeColor);
  if (pc != 0)
    table->item(row, 25)->setBackground(activeColor);
  if (!ncbd)
    table->item(row, 26)->setBackground(activeColor);
  if (interrupt != 15)
    table->item(row, 27)->setBackground(activeColor);
  if (!nas)
    table->item(row, 28)->setBackground(activeColor);
  if (!nrw)
    table->item(row, 29)->setBackground(activeColor);
  if (bd != 0)
    table->item(row, 30)->setBackground(activeColor);
  if (dint != 0)
    table->item(row, 31)->setBackground(activeColor);
  if (hint != 0)
    table->item(row, 32)->setBackground(activeColor);
  if (sirq != 0)
    table->item(row, 33)->setBackground(activeColor);
  if (sxirq != 0)
    table->item(row, 34)->setBackground(activeColor);
  if (b9 != 0)
    table->item(row, 35)->setBackground(activeColor);
  if (b8b7b6 != 0)
    table->item(row, 36)->setBackground(activeColor);
  if (b5b4b3 != 0)
    table->item(row, 37)->setBackground(activeColor);
  if (b2b1 != 0)
    table->item(row, 38)->setBackground(activeColor);
  if (b0 != 0)
    table->item(row, 39)->setBackground(activeColor);
  if (cc != 0)
    table->item(row, 40)->setBackground(activeColor);
  if (cn != 0)
    table->item(row, 41)->setBackground(activeColor);
  if (cv != 0)
    table->item(row, 42)->setBackground(activeColor);
  if (cz != 0)
    table->item(row, 43)->setBackground(activeColor);
  if (ci != 0)
    table->item(row, 44)->setBackground(activeColor);
  if (ch != 0)
    table->item(row, 45)->setBackground(activeColor);
  if (cx != 0)
    table->item(row, 46)->setBackground(activeColor);
  if (cs != 0)
    table->item(row, 47)->setBackground(activeColor);
  if (!nhb)
    table->item(row, 48)->setBackground(activeColor);
  if (accsec != 0)
    table->item(row, 49)->setBackground(activeColor);

  // center on mPC cell
  QTableWidgetItem *item = table->item(row, 0);

  if (item) {
    table->scrollToItem(item, QAbstractItemView::PositionAtCenter);
  }

  oldmPC = row;
}

void RomWidget::updateMemory() {
  int row = 0;
  // updatingMemory = true;

  for (uint16_t address = 0x0; address <= 0x0FFF; address++) {
    // TODO: en el futuro solo mostrar el row correspondiente al mpc
    table->item(row, 0)->setText(
        QString("%1").arg(address, 3, 16, QChar('0')).toUpper());

    state = memory->fetchRom(address);
    getState(state);

    updateCell(row);

    row++;
  }
  // firstUpdate = false;
  // updatingMemory = false;
}

uint32_t RomWidget::getBits(const RomWord &w, int shift, uint32_t mask) {
  if (shift >= 64) {
    return (w.high >> (shift - 64)) & mask;
  } else {
    return (w.low >> shift) & mask;
  }
}

void RomWidget::getState(const RomWord &state) {

  // Get Control Lines
  prueba = getBits(state, PRUEBA_SHIFT, VECTOR5_MASK);
  vf = getBits(state, VF_SHIFT, BIT_MASK);
  mi = getBits(state, MI_SHIFT, VECTOR2_MASK);
  liga = getBits(state, LIGA_SHIFT, VECTOR12_MASK);
  ncri = getBits(state, NCRI_SHIFT, BIT_MASK);
  eb = getBits(state, EB_SHIFT, VECTOR2_MASK);
  nwb = getBits(state, NWB_SHIFT, BIT_MASK);
  ea = getBits(state, EA_SHIFT, VECTOR2_MASK);
  nwa = getBits(state, NWA_SHIFT, BIT_MASK);
  selbus = getBits(state, SELBUS_SHIFT, BIT_MASK);
  dest = getBits(state, DESTUPA_SHIFT, VECTOR3_MASK);
  functn = getBits(state, FUNCTNUPA_SHIFT, VECTOR3_MASK);
  // Low part of my structure
  source = getBits(state, SOURCEUPA_SHIFT, VECTOR4_MASK);
  noeupa = getBits(state, NOEUPA_SHIFT, BIT_MASK);
  ndupa = getBits(state, NDUPA_SHIFT, BIT_MASK);
  selmux = getBits(state, SELMUX_SHIFT, BIT_MASK);
  nex = getBits(state, NEX_SHIFT, VECTOR3_MASK);
  x = getBits(state, X_SHIFT, VECTOR3_MASK);
  enay = getBits(state, ENAY_SHIFT, BIT_MASK);
  nera = getBits(state, NERA_SHIFT, VECTOR3_MASK);
  ra = getBits(state, RA_SHIFT, VECTOR3_MASK);
  neap = getBits(state, NEAP_SHIFT, VECTOR3_MASK);
  ap = getBits(state, AP_SHIFT, VECTOR3_MASK);
  nepc = getBits(state, NEPC_SHIFT, VECTOR3_MASK);
  pc = getBits(state, PC_SHIFT, VECTOR3_MASK);
  ncbd = getBits(state, NCBD_SHIFT, BIT_MASK);
  interrupt = getBits(state, INTERRUPT_SHIFT, VECTOR4_MASK);
  nas = getBits(state, NAS_SHIFT, BIT_MASK);
  nrw = getBits(state, NRW_SHIFT, BIT_MASK);
  bd = getBits(state, BD_SHIFT, BIT_MASK);
  dint = getBits(state, DINT_SHIFT, BIT_MASK);
  hint = getBits(state, HINT_SHIFT, BIT_MASK);
  sirq = getBits(state, SIRQ_SHIFT, BIT_MASK);
  sxirq = getBits(state, SXIRQ_SHIFT, BIT_MASK);
  b9 = getBits(state, B9_SHIFT, BIT_MASK);
  b8b7b6 = getBits(state, B8B7B6_SHIFT, VECTOR3_MASK);
  b5b4b3 = getBits(state, B5B4B3_SHIFT, VECTOR3_MASK);
  b2b1 = getBits(state, B2B1_SHIFT, VECTOR2_MASK);
  b0 = getBits(state, B2B1_SHIFT, BIT_MASK);
  cc = getBits(state, CC_SHIFT, BIT_MASK);
  cn = getBits(state, CN_SHIFT, BIT_MASK);
  cv = getBits(state, CV_SHIFT, BIT_MASK);
  cz = getBits(state, CZ_SHIFT, BIT_MASK);
  ci = getBits(state, CI_SHIFT, BIT_MASK);
  ch = getBits(state, CH_SHIFT, BIT_MASK);
  cx = getBits(state, CX_SHIFT, BIT_MASK);
  cs = getBits(state, CS_SHIFT, BIT_MASK);
  nhb = getBits(state, NHB_SHIFT, BIT_MASK);
  accsec = getBits(state, ACCSEC_SHIFT, BIT_MASK);
}
