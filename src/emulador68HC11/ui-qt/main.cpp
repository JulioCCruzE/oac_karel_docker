/*
 * Emulador Motorola 68HC11
 * Copyright (C) 2026 Julio Cruz
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <QApplication>

#include "mainwindow.h"

int main(int argc, char *argv[]) {
  ros::init(argc, argv, "emulador68hc11_gui");
  ros::NodeHandle node;

  QApplication app(argc, argv);

  QApplication::setApplicationName("Emulador 68HC11");

  QApplication::setApplicationVersion("1.0.0");

  QApplication::setOrganizationName("Julio Cesar Cruz Estrada");

  int result;
  {
    MainWindow w(node);
    w.show();
    result = app.exec();
  }
  ros::shutdown();
  return result;
}
