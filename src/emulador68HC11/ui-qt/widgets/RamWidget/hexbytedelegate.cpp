/*
 * Emulador Motorola 68HC11
 * Copyright (C) 2026 Julio Cruz
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "hexbytedelegate.h"

HexByteDelegate::HexByteDelegate(QObject *parent)
    : QStyledItemDelegate(parent) {}

QWidget *HexByteDelegate::createEditor(QWidget *parent,
                                       const QStyleOptionViewItem &,
                                       const QModelIndex &index) const {
  if (index.column() == 0)
    return nullptr;

  auto *editor = new QLineEdit(parent);

  editor->setMaxLength(2);

  editor->setAlignment(Qt::AlignCenter);

  editor->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));

  editor->setValidator(new QRegularExpressionValidator(
      QRegularExpression("[0-9A-Fa-f]{1,2}"), editor));

  return editor;
}

void HexByteDelegate::setEditorData(QWidget *editor,
                                    const QModelIndex &index) const {
  auto *lineEdit = qobject_cast<QLineEdit *>(editor);

  if (!lineEdit)
    return;

  QString value = index.model()->data(index, Qt::DisplayRole).toString();

  lineEdit->setText(value);

  lineEdit->selectAll();
}

void HexByteDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                   const QModelIndex &index) const {
  auto *lineEdit = qobject_cast<QLineEdit *>(editor);

  if (!lineEdit)
    return;

  QString value = lineEdit->text().trimmed().toUpper();

  model->setData(index, value, Qt::EditRole);
}

void HexByteDelegate::updateEditorGeometry(QWidget *editor,
                                           const QStyleOptionViewItem &option,
                                           const QModelIndex &) const {
  editor->setGeometry(option.rect);
}
