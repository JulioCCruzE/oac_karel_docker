/*
 * Emulador Motorola 68HC11
 * Copyright (C) 2026 Julio Cruz
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "CPU68HC11.h"
#include "aboutdialog.h"
#include "memory/Memory.h"
#include "sequencer/LogicaInterna.h"
#include "sequencer/SelectorPrueba.h"
#include "widgets/CentralWidget/centralwidget.h"
#include "widgets/RamWidget/ramwidget.h"
#include "widgets/RegistersWidget/registerswidget.h"
#include "widgets/RomWidget/romwidget.h"
#include <QCloseEvent>
#include <QMainWindow>
#include <QTimer>
#include <array>
#include <cstdint>
#include <deque>
#include <geometry_msgs/Twist.h>
#include <ros/ros.h>
#include <sensor_msgs/Range.h>
#include <std_msgs/Bool.h>
#include <std_msgs/Float32.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(ros::NodeHandle &node);
  ~MainWindow();

  void publishVelocityCommand(const geometry_msgs::Twist &command);
  bool hasReceivedScore() const;
  float score() const;
  bool hasReceivedScoreStarted() const;
  bool scoreStarted() const;
  void publishCommand();
  void frontRangeCallback(const sensor_msgs::Range::ConstPtr &message);
  void backRangeCallback(const sensor_msgs::Range::ConstPtr &message);
  void leftRangeCallback(const sensor_msgs::Range::ConstPtr &message);
  void rightRangeCallback(const sensor_msgs::Range::ConstPtr &message);

private slots:
  void on_actionCarga_RAM_triggered();

  void on_actionCarga_ROM_triggered();

  void on_actionReset_triggered();

  void on_actionRun_triggered();

  void on_actionAcerca_de_triggered();

  void on_actionAcerca_de_Qt_triggered();

protected:
  void closeEvent(QCloseEvent *event) override;

private:
  struct TimedRangeReading {
    TimedRangeReading(std::uint8_t reading, std::uint64_t stamp)
        : value(reading), stampNs(stamp) {}

    std::uint8_t value;
    std::uint64_t stampNs;
  };

  Ui::MainWindow *ui;
  Memory mem;
  LogicaInterna li;
  SelectorPrueba sp;
  CPU68HC11 cpu;

  RegistersWidget *m_registersWidget;
  RamWidget *m_ramWidget;
  RomWidget *m_romWidget;
  CentralWidget *m_centralWidget;

  ros::Publisher publisher_;
  ros::Subscriber scoreSubscriber_;
  ros::Subscriber scoreStartedSubscriber_;
  ros::Subscriber range_subscriber_;
  ros::Subscriber back_range_subscriber_;
  ros::Subscriber left_range_subscriber_;
  ros::Subscriber right_range_subscriber_;

  QTimer rosTimer_;
  std::array<std::deque<TimedRangeReading>, 4> pendingRangeReadings_;
  std::array<std::uint64_t, 4> lastAppliedRangeStampNs_;
  std::uint64_t ioPeriodNs_;
  std::uint64_t lastSimTimeNs_;
  std::uint64_t nextIoTimeNs_;
  bool ioTimelineInitialized_;
  bool hasSeenSimTime_;
  float score_;
  bool scoreStarted_;
  bool hasReceivedScore_;
  bool hasReceivedScoreStarted_;
  bool inputMemoryChanged_;
  bool pendingScoreStarted_;
  bool hasPendingScoreStarted_;
  std::uint64_t pendingScoreStartedStampNs_;

  void refreshUI();
  void processRosEvents();
  void processIoTick(std::uint64_t tickNs);
  void applyInputSamples(std::uint64_t tickNs);
  void resetIoTimeline(std::uint64_t simTimeNs);
  void stopRobot();
  void updateRangeReading(std::size_t sensorIndex,
                          const sensor_msgs::Range::ConstPtr &message);
  void publishStop();
  void scoreCallback(const std_msgs::Float32::ConstPtr &message);
  void scoreStartedCallback(const std_msgs::Bool::ConstPtr &message);
};

#endif // MAINWINDOW_H
