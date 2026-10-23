/*
 * Emulador Motorola 68HC11
 * Copyright (C) 2026 Julio Cruz
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "aboutdialog.h"

#include <QApplication>
#include <QDesktopServices>
#include <QDialogButtonBox>
#include <QFont>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QUrl>
#include <QVBoxLayout>

AboutDialog::AboutDialog(QWidget *parent) : QDialog(parent) {
  setWindowTitle(tr("Acerca de Emulador 68HC11"));
  setModal(true);

  setMinimumWidth(520);
  resize(560, 420);

  auto *mainLayout = new QVBoxLayout(this);

  /*
   * Título
   */
  auto *titleLabel = new QLabel(tr("Emulador Motorola 68HC11"), this);

  QFont titleFont = titleLabel->font();
  titleFont.setPointSize(18);
  titleFont.setBold(true);

  titleLabel->setFont(titleFont);
  titleLabel->setAlignment(Qt::AlignCenter);

  /*
   * Versión
   */
  // auto *versionLabel = new QLabel(tr("Versión 1.0.0"), this);
  auto *versionLabel = new QLabel(
      tr("Versión %1").arg(QApplication::applicationVersion()), this);

  versionLabel->setAlignment(Qt::AlignCenter);

  /*
   * Descripción
   */
  auto *descriptionLabel = new QLabel(
      tr("Emulador del microprocesador Motorola 68HC11 desarrollado "
         "principalmente con fines educativos y académicos.\n\n"
         "Su objetivo es servir como herramienta de apoyo para el "
         "aprendizaje de arquitectura de computadoras, microprocesadores "
         "y lenguaje ensamblador.\n\n"
         "El proyecto está orientado especialmente a estudiantes de la "
         "Facultad de Ingeniería de la Universidad Nacional Autónoma "
         "de México (UNAM)."),
      this);

  descriptionLabel->setWordWrap(true);
  descriptionLabel->setAlignment(Qt::AlignJustify);

  /*
   * Autor
   */
  auto *authorLabel =
      new QLabel(tr("<b>Autor y desarrollador:</b> Julio Cesar Cruz Estrada<br>"
                    "© 2026 Julio Cesar Cruz Estrada"),
                 this);

  authorLabel->setTextFormat(Qt::RichText);

  /*
   * Aviso institucional
   */
  auto *disclaimerLabel = new QLabel(
      tr("<i>"
         "Este es un proyecto independiente y no constituye software "
         "oficial de la Universidad Nacional Autónoma de México ni "
         "implica respaldo institucional por parte de la UNAM."
         "</i>"),
      this);

  disclaimerLabel->setWordWrap(true);
  disclaimerLabel->setTextFormat(Qt::RichText);

  /*
   * Licencia
   */
  auto *licenseLabel =
      new QLabel(tr("<b>Licencia:</b> GNU General Public License v3.0 "
                    "(GPL-3.0-only)"),
                 this);

  licenseLabel->setTextFormat(Qt::RichText);

  /*
   * Licencia segunda parte
   */
  auto *licenseLabel2 =
      new QLabel(tr("El software se proporciona sin garantía, conforme a los "
                    "términos establecidos por la licenca."),
                 this);

  licenseLabel2->setWordWrap(true);
  licenseLabel2->setTextFormat(Qt::RichText);

  /*
   * Botones
   */
  auto *repositoryButton = new QPushButton(tr("Repositorio en GitHub"), this);

  auto *licenseButton = new QPushButton(tr("Ver licencia"), this);

  auto *closeButton = new QPushButton(tr("Cerrar"), this);

  closeButton->setDefault(true);

  /*
   * Layout de botones
   */
  auto *buttonLayout = new QHBoxLayout;

  buttonLayout->addWidget(repositoryButton);
  buttonLayout->addStretch();
  buttonLayout->addWidget(licenseButton);
  buttonLayout->addWidget(closeButton);

  /*
   * Layout principal
   */
  mainLayout->addWidget(titleLabel);
  mainLayout->addWidget(versionLabel);

  mainLayout->addSpacing(10);

  mainLayout->addWidget(descriptionLabel);

  mainLayout->addSpacing(10);

  mainLayout->addWidget(authorLabel);

  mainLayout->addSpacing(10);

  mainLayout->addWidget(disclaimerLabel);

  mainLayout->addSpacing(10);

  mainLayout->addWidget(licenseLabel);

  mainLayout->addSpacing(10);

  mainLayout->addWidget(licenseLabel2);

  mainLayout->addStretch();

  mainLayout->addLayout(buttonLayout);

  /*
   * Señales
   */
  connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);

  connect(licenseButton, &QPushButton::clicked, this,
          &AboutDialog::showLicense);

  connect(repositoryButton, &QPushButton::clicked, this,
          &AboutDialog::openRepository);
}

void AboutDialog::showLicense() {
  QMessageBox licenseBox(this);

  licenseBox.setWindowTitle(tr("Licencia GNU GPL v3"));

  licenseBox.setIcon(QMessageBox::Information);

  licenseBox.setText(tr("<b>GNU General Public License version 3</b><br><br>"
                        "Este programa es software libre: puede redistribuirlo "
                        "y/o modificarlo bajo los términos de la GNU General "
                        "Public License versión 3.<br><br>"
                        "El programa se distribuye con la esperanza de que sea "
                        "útil, pero SIN NINGUNA GARANTÍA.<br><br>"
                        "Consulte el archivo LICENSE incluido con el código "
                        "fuente para conocer los términos completos."));

  licenseBox.exec();
}

void AboutDialog::openRepository() {
  QDesktopServices::openUrl(
      QUrl("https://github.com/JulioCCruzE/emulador68HC11"));
}
