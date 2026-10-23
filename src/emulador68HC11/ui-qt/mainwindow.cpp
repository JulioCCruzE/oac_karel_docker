/*
 * Emulador Motorola 68HC11
 * Copyright (C) 2026 Julio Cruz
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDockWidget>
#include <QFileDialog>
#include <QMessageBox>
#include <algorithm>
#include <cmath>
#include <limits>

namespace {
std::uint64_t saturatedAdd(std::uint64_t left, std::uint64_t right) {
  if (right > std::numeric_limits<std::uint64_t>::max() - left)
    return std::numeric_limits<std::uint64_t>::max();
  return left + right;
}
} // namespace

MainWindow::MainWindow(ros::NodeHandle &node)
    : ui(new Ui::MainWindow), mem(), li(), sp(), cpu(mem, li, sp),
      publisher_(node.advertise<geometry_msgs::Twist>(
          "/hardware/mobile_base/cmd_vel", 1)),
      range_subscriber_(node.subscribe("/hardware/ultrasonic", 1,
                                       &MainWindow::frontRangeCallback, this)),
      back_range_subscriber_(node.subscribe("/hardware/ultrasonic/back", 1,
                                            &MainWindow::backRangeCallback,
                                            this)),
      left_range_subscriber_(node.subscribe("/hardware/ultrasonic/left", 1,
                                            &MainWindow::leftRangeCallback,
                                            this)),
      right_range_subscriber_(node.subscribe("/hardware/ultrasonic/right", 1,
                                             &MainWindow::rightRangeCallback,
                                             this)),
      pendingRangeReadings_(), lastAppliedRangeStampNs_(),
      ioPeriodNs_(50000000), lastSimTimeNs_(0), nextIoTimeNs_(0),
      ioTimelineInitialized_(false), hasSeenSimTime_(false), score_(0.0f),
      scoreStarted_(false), hasReceivedScore_(false),
      hasReceivedScoreStarted_(false), inputMemoryChanged_(false),
      pendingScoreStarted_(false), hasPendingScoreStarted_(false),
      pendingScoreStartedStampNs_(0) {
  ui->setupUi(this);

  ros::NodeHandle privateNode("~");
  double ioPeriodSeconds = 0.05;
  privateNode.param("io_period", ioPeriodSeconds, ioPeriodSeconds);
  if (!std::isfinite(ioPeriodSeconds) || ioPeriodSeconds <= 0.0 ||
      ioPeriodSeconds > 3600.0) {
    ROS_WARN("Invalid ~io_period; using 0.05 seconds of Sim Time");
    ioPeriodSeconds = 0.05;
  }
  ioPeriodNs_ = std::max<std::uint64_t>(
      1, static_cast<std::uint64_t>(ioPeriodSeconds * 1000000000.0));

  auto *registersDock = new QDockWidget("Estado del procesador", this);
  auto *ramDock = new QDockWidget("RAM", this);
  auto *romDock = new QDockWidget("ROM", this);

  m_registersWidget = new RegistersWidget(cpu, this);
  m_ramWidget = new RamWidget(&mem, &cpu, this);
  m_romWidget = new RomWidget(&mem, &cpu, this);

  registersDock->setWidget(m_registersWidget);
  ramDock->setWidget(m_ramWidget);
  romDock->setWidget(m_romWidget);

  addDockWidget(Qt::LeftDockWidgetArea, registersDock);
  addDockWidget(Qt::RightDockWidgetArea, ramDock);
  addDockWidget(Qt::TopDockWidgetArea, romDock);

  m_centralWidget = new CentralWidget(m_registersWidget, m_ramWidget,
                                      m_romWidget, &cpu, this);
  setCentralWidget(m_centralWidget);
  connect(m_centralWidget, &CentralWidget::executionStopped, this,
          &MainWindow::stopRobot);

  scoreSubscriber_ =
      node.subscribe("/score", 1, &MainWindow::scoreCallback, this);
  scoreStartedSubscriber_ = node.subscribe(
      "/score/started", 1, &MainWindow::scoreStartedCallback, this);

  refreshUI();

  connect(&rosTimer_, &QTimer::timeout, this, &MainWindow::processRosEvents);
  rosTimer_.setTimerType(Qt::PreciseTimer);
  rosTimer_.start(10);
}

MainWindow::~MainWindow() {
  publishStop();
  delete ui;
}

void MainWindow::processRosEvents() {
  if (!ros::ok()) {
    close();
    return;
  }
  ros::spinOnce();
  ros::Time simTime;
  bool clockAvailable = false;
  if (ros::Time::isSimTime() && ros::Time::isValid()) {
    simTime = ros::Time::now();
    clockAvailable = !simTime.isZero();
  }

  if (!clockAvailable) {
    if (hasSeenSimTime_) {
      resetIoTimeline(0);
      hasSeenSimTime_ = false;
    }
    ioTimelineInitialized_ = false;
    m_centralWidget->advanceContinuousExecution(0, false);
    if (inputMemoryChanged_) {
      inputMemoryChanged_ = false;
      m_ramWidget->refresh();
    }
    return;
  }

  const std::uint64_t simTimeNs = simTime.toNSec();
  hasSeenSimTime_ = true;
  if (!ioTimelineInitialized_) {
    lastSimTimeNs_ = simTimeNs;
    nextIoTimeNs_ = saturatedAdd(simTimeNs, ioPeriodNs_);
    ioTimelineInitialized_ = true;
    m_centralWidget->advanceContinuousExecution(simTimeNs, true);
    return;
  }

  if (simTimeNs < lastSimTimeNs_) {
    m_centralWidget->advanceContinuousExecution(simTimeNs, true);
    resetIoTimeline(simTimeNs);
  } else {
    lastSimTimeNs_ = simTimeNs;
    if (simTimeNs >= nextIoTimeNs_) {
      const std::uint64_t tickNs =
          nextIoTimeNs_ +
          ((simTimeNs - nextIoTimeNs_) / ioPeriodNs_) * ioPeriodNs_;
      m_centralWidget->advanceContinuousExecution(tickNs, true);
      processIoTick(tickNs);
      nextIoTimeNs_ = saturatedAdd(tickNs, ioPeriodNs_);
    }
    m_centralWidget->advanceContinuousExecution(simTimeNs, true);
  }

  if (inputMemoryChanged_) {
    inputMemoryChanged_ = false;
    m_ramWidget->refresh();
  }
}

void MainWindow::processIoTick(std::uint64_t tickNs) {
  applyInputSamples(tickNs);
  if (scoreStarted())
    publishCommand();
}

void MainWindow::applyInputSamples(std::uint64_t tickNs) {
  static const std::array<std::uint16_t, 4> addresses = {
      {0xC002, 0xC003, 0xC004, 0xC005}};

  for (std::size_t index = 0; index < pendingRangeReadings_.size(); ++index) {
    std::deque<TimedRangeReading> &readings = pendingRangeReadings_[index];
    if (readings.empty() || readings.front().stampNs > tickNs)
      continue;

    TimedRangeReading reading = readings.front();
    readings.pop_front();
    while (!readings.empty() && readings.front().stampNs <= tickNs) {
      reading = readings.front();
      readings.pop_front();
    }

    if (mem.readRam(addresses[index]) != reading.value) {
      mem.writeRam(addresses[index], reading.value);
      inputMemoryChanged_ = true;
    }
    lastAppliedRangeStampNs_[index] = reading.stampNs;
  }

  if (hasPendingScoreStarted_ && pendingScoreStartedStampNs_ <= tickNs) {
    scoreStarted_ = pendingScoreStarted_;
    if (mem.readRam(0xC006) != static_cast<std::uint8_t>(scoreStarted_)) {
      mem.writeRam(0xC006, scoreStarted_);
      inputMemoryChanged_ = true;
    }
    hasPendingScoreStarted_ = false;
  }
}

void MainWindow::resetIoTimeline(std::uint64_t simTimeNs) {
  for (std::size_t index = 0; index < pendingRangeReadings_.size(); ++index)
    pendingRangeReadings_[index].clear();
  lastAppliedRangeStampNs_.fill(0);
  score_ = 0.0f;
  scoreStarted_ = false;
  hasReceivedScore_ = false;
  hasReceivedScoreStarted_ = false;
  hasPendingScoreStarted_ = false;
  pendingScoreStartedStampNs_ = 0;
  lastSimTimeNs_ = simTimeNs;
  nextIoTimeNs_ = saturatedAdd(simTimeNs, ioPeriodNs_);

  mem.writeRam(0xC000, 0);
  mem.writeRam(0xC001, 0);
  for (std::uint16_t address = 0xC002; address <= 0xC005; ++address)
    mem.writeRam(address, 0xFF);
  mem.writeRam(0xC006, 0);
  inputMemoryChanged_ = true;
  publishStop();
}

void MainWindow::publishVelocityCommand(const geometry_msgs::Twist &command) {
  publisher_.publish(command);
}

void MainWindow::publishStop() { publisher_.publish(geometry_msgs::Twist()); }

void MainWindow::stopRobot() {
  mem.writeRam(0xC000, 0);
  mem.writeRam(0xC001, 0);
  publishStop();
  m_ramWidget->refresh();
}

void MainWindow::scoreCallback(const std_msgs::Float32::ConstPtr &message) {
  score_ = message->data;
  hasReceivedScore_ = true;
}

void MainWindow::scoreStartedCallback(const std_msgs::Bool::ConstPtr &message) {
  if (scoreStarted_ && !message->data)
    stopRobot();
  hasReceivedScoreStarted_ = true;
  pendingScoreStarted_ = message->data;
  hasPendingScoreStarted_ = true;
  pendingScoreStartedStampNs_ = ros::Time::isSimTime() && ros::Time::isValid()
                                    ? ros::Time::now().toNSec()
                                    : 0;
}

void MainWindow::closeEvent(QCloseEvent *event) {
  publishStop();
  QMainWindow::closeEvent(event);
}

bool MainWindow::hasReceivedScore() const { return hasReceivedScore_; }

float MainWindow::score() const { return score_; }

bool MainWindow::hasReceivedScoreStarted() const {
  return hasReceivedScoreStarted_;
}

bool MainWindow::scoreStarted() const { return scoreStarted_; }

void MainWindow::refreshUI() {
  m_registersWidget->updateRegisters();
  // m_ramWidget->updateMemory();
  m_ramWidget->refresh();
  m_romWidget->refreshmPC();
  m_centralWidget->refresh();
}

void MainWindow::on_actionCarga_RAM_triggered() {
  // mem.loadRamFromMIF("mem_content.mif");
  QString fileName = QFileDialog::getOpenFileName(
      this, tr("Seleccionar archivo de memoria RAM"), "",
      tr("Memory Initialization Files (*.mif);;Todos los archivos (*)"));

  if (fileName.isEmpty())
    return;

  // if (!mem.loadRamFromMIF(fileName.toStdString())) {
  //   QMessageBox::critical(this, tr("Error"),
  //                         tr("No fue posible cargar el archivo de
  //                         memoria."));
  mem.loadRamFromMIF(fileName.toStdString());
  // m_ramWidget->updateMemory();
  m_ramWidget->refresh();
  return;
}

void MainWindow::on_actionCarga_ROM_triggered() {
  // mem.loadRomFromVHDL("memory.vhd");
  QString fileName = QFileDialog::getOpenFileName(
      this, tr("Seleccionar archivo de memoria ROM"), "",
      tr("Memory Initialization Files (*.vhd);;Todos los archivos (*)"));

  if (fileName.isEmpty())
    return;

  // if (!mem.loadRamFromMIF(fileName.toStdString())) {
  //   QMessageBox::critical(this, tr("Error"),
  //                         tr("No fue posible cargar el archivo de
  //                         memoria."));
  mem.loadRomFromVHDL(fileName.toStdString());
  m_romWidget->updateMemory();
  m_romWidget->refreshmPC();
  return;
  // }
}

void MainWindow::on_actionReset_triggered() {
  m_centralWidget->stopExecution();
  cpu.reset();
  refreshUI();
  // m_romWidget->refreshmPC();
}

void MainWindow::on_actionRun_triggered() {
  m_centralWidget->runSelectedMode();
}

void MainWindow::on_actionAcerca_de_triggered() {
  AboutDialog dialog(this);
  dialog.exec();
}

void MainWindow::on_actionAcerca_de_Qt_triggered() { QApplication::aboutQt(); }

void MainWindow::publishCommand() {
  geometry_msgs::Twist command;
  int linear_value = mem.readRam(0xC000);
  int angular_value = mem.readRam(0xC001);
  if (linear_value > 0x7F)
    linear_value -= 0x100;
  if (angular_value > 0x7F)
    angular_value -= 0x100;

  command.linear.x = linear_value / 100.0;
  command.angular.z = 0.0;

  // giro si no hay movimiento lineal
  if (linear_value == 0)
    command.angular.z = angular_value * M_PI / 180.0;
  publisher_.publish(command);
}

void MainWindow::frontRangeCallback(
    const sensor_msgs::Range::ConstPtr &message) {
  updateRangeReading(0, message);
}

void MainWindow::backRangeCallback(
    const sensor_msgs::Range::ConstPtr &message) {
  updateRangeReading(1, message);
}

void MainWindow::leftRangeCallback(
    const sensor_msgs::Range::ConstPtr &message) {
  updateRangeReading(2, message);
}

void MainWindow::rightRangeCallback(
    const sensor_msgs::Range::ConstPtr &message) {
  updateRangeReading(3, message);
}

void MainWindow::updateRangeReading(
    std::size_t sensorIndex, const sensor_msgs::Range::ConstPtr &message) {
  if (sensorIndex >= pendingRangeReadings_.size())
    return;

  std::uint8_t value = 0xFF;
  if (std::isfinite(message->range) && message->range >= 0.0) {
    const double centimeters = message->range * 100.0;
    value = centimeters >= 254.0 ? 254 : static_cast<std::uint8_t>(centimeters);
  }

  ros::Time stamp = message->header.stamp;
  if (stamp.isZero()) {
    if (!ros::Time::isSimTime() || !ros::Time::isValid())
      return;
    stamp = ros::Time::now();
    if (stamp.isZero())
      return;
  }

  const std::uint64_t stampNs = stamp.toNSec();
  if (stampNs <= lastAppliedRangeStampNs_[sensorIndex])
    return;

  std::deque<TimedRangeReading> &readings = pendingRangeReadings_[sensorIndex];
  std::deque<TimedRangeReading>::iterator position = readings.begin();
  while (position != readings.end() && position->stampNs < stampNs)
    ++position;
  if (position != readings.end() && position->stampNs == stampNs) {
    position->value = value;
  } else {
    readings.insert(position, TimedRangeReading(value, stampNs));
  }

  const std::size_t maxPendingReadings = 16;
  while (readings.size() > maxPendingReadings)
    readings.pop_back();
}
