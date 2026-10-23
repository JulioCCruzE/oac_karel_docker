/*
 * Emulador Motorola 68HC11
 * Copyright (C) 2026 Julio Cruz
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef HEXBYTEDELEGATE_H
#define HEXBYTEDELEGATE_H

#include <QFontDatabase>
#include <QLineEdit>
#include <QStyledItemDelegate>

class HexByteDelegate : public QStyledItemDelegate {
  Q_OBJECT
public:
  explicit HexByteDelegate(QObject *parent = nullptr);

  QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                        const QModelIndex &index) const override;

  void setEditorData(QWidget *editor, const QModelIndex &index) const override;

  void setModelData(QWidget *editor, QAbstractItemModel *model,
                    const QModelIndex &index) const override;

  void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                            const QModelIndex &index) const override;
};

#endif // HEXBYTEDELEGATE_H
