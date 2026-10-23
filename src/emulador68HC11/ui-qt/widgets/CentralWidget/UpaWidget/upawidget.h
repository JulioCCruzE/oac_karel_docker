/*
 * Emulador Motorola 68HC11
 * Copyright (C) 2026 Julio Cruz
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef UPAWIDGET_H
#define UPAWIDGET_H

#include "CPU68HC11.h"
#include <QWidget>

namespace Ui {
class UpaWidget;
}

class UpaWidget : public QWidget {
  Q_OBJECT

public:
  explicit UpaWidget(CPU68HC11 *cpu, QWidget *parent = nullptr);
  ~UpaWidget();

  void refresh();

private:
  Ui::UpaWidget *ui;
  CPU68HC11 *m_cpu;
};

#endif // UPAWIDGET_H
