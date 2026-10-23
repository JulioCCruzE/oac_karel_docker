/*
 * Karel ROS
 * Copyright (C) 2026 Julio Cruz
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <algorithm>
#include <cmath>
#include <limits>

#include <QApplication>
#include <QCheckBox>
#include <QCloseEvent>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <QScrollArea>
#include <QShortcut>
#include <QSlider>
#include <QSpinBox>
#include <QTimer>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWidget>

#include <gazebo_msgs/SetModelState.h>
#include <geometry_msgs/Twist.h>
#include <nav_msgs/Odometry.h>
#include <ros/ros.h>
#include <sensor_msgs/Range.h>
#include <std_msgs/Bool.h>
#include <std_msgs/Float32.h>
#include <std_msgs/String.h>
#include <std_srvs/Empty.h>
#include <std_srvs/Trigger.h>

class KarelWindow : public QMainWindow {
public:
  explicit KarelWindow(ros::NodeHandle &node)
      : publisher_(node.advertise<geometry_msgs::Twist>(
            "/hardware/mobile_base/cmd_vel", 1)),
        odometry_subscriber_(node.subscribe(
            "/karel/odom", 1, &KarelWindow::odometryCallback, this)),
        range_subscriber_(node.subscribe(
            "/hardware/ultrasonic", 1, &KarelWindow::frontRangeCallback, this)),
        back_range_subscriber_(node.subscribe("/hardware/ultrasonic/back", 1,
                                              &KarelWindow::backRangeCallback,
                                              this)),
        left_range_subscriber_(node.subscribe("/hardware/ultrasonic/left", 1,
                                              &KarelWindow::leftRangeCallback,
                                              this)),
        right_range_subscriber_(node.subscribe("/hardware/ultrasonic/right", 1,
                                               &KarelWindow::rightRangeCallback,
                                               this)),
        score_subscriber_(
            node.subscribe("/score", 1, &KarelWindow::scoreCallback, this)),
        score_started_subscriber_(node.subscribe(
            "/score/started", 1, &KarelWindow::scoreStartedCallback, this)),
        score_elapsed_subscriber_(
            node.subscribe("/score/elapsed_seconds", 1,
                           &KarelWindow::scoreElapsedCallback, this)),
        score_timed_out_subscriber_(node.subscribe(
            "/score/timed_out", 1, &KarelWindow::scoreTimedOutCallback, this)),
        message_subscriber_(
            node.subscribe("/message", 1, &KarelWindow::messageCallback, this)),
        set_model_state_client_(node.serviceClient<gazebo_msgs::SetModelState>(
            "/gazebo/set_model_state")),
        start_score_client_(
            node.serviceClient<std_srvs::Trigger>("/karel_score/start")),
        reset_score_client_(
            node.serviceClient<std_srvs::Empty>("/karel_score/reset")),
        movement_(Stopped),
        initial_pose_available_(
            node.getParam("/karel/initial_pose/x", initial_pose_x_) &&
            node.getParam("/karel/initial_pose/y", initial_pose_y_) &&
            node.getParam("/karel/initial_pose/yaw", initial_pose_yaw_)) {
    setWindowTitle("Karel - Control manual");
    setMinimumSize(440, 500);
    resize(500, 700);

    QScrollArea *scroll_area = new QScrollArea(this);
    scroll_area->setWidgetResizable(true);
    scroll_area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroll_area->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    QWidget *central = new QWidget(scroll_area);
    QVBoxLayout *root = new QVBoxLayout(central);
    root->setSpacing(14);

    QLabel *title = new QLabel("KAREL", central);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet(
        "font-size: 28px; font-weight: 700; color: #1769aa; padding: 8px;");
    root->addWidget(title);

    QGroupBox *movement_box = new QGroupBox("Movimiento", central);
    QGridLayout *movement_layout = new QGridLayout(movement_box);
    movement_layout->setSpacing(8);

    QPushButton *forward = movementButton("Adelante", Forward);
    QPushButton *left = movementButton("Girar izquierda", TurnLeft);
    QPushButton *right = movementButton("Girar derecha", TurnRight);
    QPushButton *backward = movementButton("Atras", Backward);
    QPushButton *stop = new QPushButton("STOP", movement_box);
    stop->setFixedHeight(46);
    stop->setStyleSheet(
        "QPushButton { background: #b71c1c; color: white; font-size: 18px; "
        "font-weight: 700; border-radius: 6px; } "
        "QPushButton:pressed { background: #7f0000; }");
    connect(stop, &QPushButton::clicked, this, [this]() { stopRobot(); });

    movement_layout->addWidget(forward, 0, 1);
    movement_layout->addWidget(left, 1, 0);
    movement_layout->addWidget(stop, 1, 1);
    movement_layout->addWidget(right, 1, 2);
    movement_layout->addWidget(backward, 2, 1);
    root->addWidget(movement_box);

    QWidget *speed_content = NULL;
    QWidget *speed_section =
        collapsibleSection("Velocidad", central, speed_content);
    QGridLayout *speed_layout = new QGridLayout(speed_content);
    linear_slider_ = slider(5, 40, 20);
    angular_slider_ = slider(20, 180, 90);
    linear_value_ = new QLabel(speed_content);
    angular_value_ = new QLabel(speed_content);
    speed_layout->addWidget(new QLabel("Lineal", speed_content), 0, 0);
    speed_layout->addWidget(linear_slider_, 0, 1);
    speed_layout->addWidget(linear_value_, 0, 2);
    speed_layout->addWidget(new QLabel("Angular", speed_content), 1, 0);
    speed_layout->addWidget(angular_slider_, 1, 1);
    speed_layout->addWidget(angular_value_, 1, 2);
    connect(linear_slider_, &QSlider::valueChanged, this,
            [this](int) { updateSpeedLabels(); });
    connect(angular_slider_, &QSlider::valueChanged, this,
            [this](int) { updateSpeedLabels(); });
    updateSpeedLabels();
    root->addWidget(speed_section);

    QWidget *telemetry_content = NULL;
    QWidget *telemetry_section =
        collapsibleSection("Telemetria", central, telemetry_content);
    QGridLayout *telemetry_layout = new QGridLayout(telemetry_content);
    telemetry_layout->setColumnStretch(1, 1);
    pose_value_ = new QLabel("Esperando /karel/odom", telemetry_content);
    pose_value_->setWordWrap(true);
    range_value_ = new QLabel("Esperando sensor frontal", telemetry_content);
    back_range_value_ =
        new QLabel("Esperando sensor trasero", telemetry_content);
    left_range_value_ =
        new QLabel("Esperando sensor izquierdo", telemetry_content);
    right_range_value_ =
        new QLabel("Esperando sensor derecho", telemetry_content);
    telemetry_layout->addWidget(new QLabel("Pose", telemetry_content), 0, 0);
    telemetry_layout->addWidget(pose_value_, 0, 1);
    telemetry_layout->addWidget(
        new QLabel("Ultrasonido frontal", telemetry_content), 1, 0);
    telemetry_layout->addWidget(range_value_, 1, 1);
    telemetry_layout->addWidget(
        new QLabel("Ultrasonido trasero", telemetry_content), 2, 0);
    telemetry_layout->addWidget(back_range_value_, 2, 1);
    telemetry_layout->addWidget(
        new QLabel("Ultrasonido izquierdo", telemetry_content), 3, 0);
    telemetry_layout->addWidget(left_range_value_, 3, 1);
    telemetry_layout->addWidget(
        new QLabel("Ultrasonido derecho", telemetry_content), 4, 0);
    telemetry_layout->addWidget(right_range_value_, 4, 1);
    root->addWidget(telemetry_section);

    QGroupBox *score_box = new QGroupBox("Evaluacion", central);
    QGridLayout *score_layout = new QGridLayout(score_box);
    score_value_ = new QLabel("0 puntos", score_box);
    score_value_->setStyleSheet(
        "font-size: 24px; font-weight: 700; color: #1769aa;");
    score_message_ = new QLabel("Esperando /score", score_box);
    score_message_->setWordWrap(true);
    score_layout->addWidget(score_value_, 0, 0);
    score_layout->addWidget(score_message_, 1, 0);
    score_elapsed_ = new QLabel("0.00 s", score_box);
    score_state_ = new QLabel("Medicion detenida", score_box);
    score_layout->addWidget(new QLabel("Tiempo", score_box), 2, 0);
    score_layout->addWidget(score_elapsed_, 2, 1);
    score_layout->addWidget(score_state_, 3, 0, 1, 2);
    score_time_limit_checkbox_ =
        new QCheckBox("Sin limite de tiempo", score_box);
    score_time_limit_minutes_ = new QSpinBox(score_box);
    score_time_limit_minutes_->setRange(1, std::numeric_limits<int>::max());
    score_time_limit_minutes_->setSuffix(" min");
    bool time_limit_enabled = true;
    int time_limit_minutes = 10;
    node.param("/karel_score/time_limit_enabled", time_limit_enabled, true);
    node.param("/karel_score/time_limit_minutes", time_limit_minutes, 10);
    score_time_limit_checkbox_->setChecked(!time_limit_enabled);
    score_time_limit_minutes_->setValue(
        time_limit_minutes > 0 ? time_limit_minutes : 10);
    score_time_limit_minutes_->setEnabled(time_limit_enabled);
    score_layout->addWidget(score_time_limit_checkbox_, 4, 0, 1, 2);
    score_layout->addWidget(new QLabel("Limite", score_box), 5, 0);
    score_layout->addWidget(score_time_limit_minutes_, 5, 1);
    connect(score_time_limit_checkbox_, &QCheckBox::toggled, this,
            [this](bool no_limit) {
              score_time_limit_minutes_->setEnabled(!no_limit);
              updateScoreTimeDisplay();
            });
    connect(score_time_limit_minutes_,
            static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this,
            [this](int) { updateScoreTimeDisplay(); });
    updateScoreTimeDisplay();
    QPushButton *start_score = new QPushButton("Iniciar", score_box);
    connect(start_score, &QPushButton::clicked, this,
            [this]() { startScore(); });
    score_layout->addWidget(start_score, 6, 0);
    QPushButton *reset_score =
        new QPushButton("Reiniciar evaluacion", score_box);
    connect(reset_score, &QPushButton::clicked, this,
            [this]() { resetScore(); });
    score_layout->addWidget(reset_score, 6, 1);
    root->addWidget(score_box);

    QPushButton *reset_position = new QPushButton("Volver al inicio", central);
    reset_position->setMinimumHeight(48);
    connect(reset_position, &QPushButton::clicked, this,
            [this]() { resetPosition(); });
    root->addWidget(reset_position);

    action_status_ = new QLabel("Listo", central);
    action_status_->setWordWrap(true);
    root->addWidget(action_status_);

    QLabel *help =
        new QLabel("Mantenga pulsado un boton para mover. Suelte para detener. "
                   "La barra espaciadora activa STOP.",
                   central);
    help->setWordWrap(true);
    help->setStyleSheet("color: #555; padding: 4px;");
    root->addWidget(help);
    root->addStretch();
    scroll_area->setWidget(central);
    setCentralWidget(scroll_area);

    QShortcut *emergency_stop =
        new QShortcut(QKeySequence(Qt::Key_Space), this);
    connect(emergency_stop, &QShortcut::activated, this,
            [this]() { stopRobot(); });

    connect(&ros_timer_, &QTimer::timeout, this, [this]() {
      if (!ros::ok()) {
        close();
        return;
      }
      ros::spinOnce();
      if (movement_ != Stopped)
        publishCommand();
    });
    ros_timer_.start(50);
  }

  ~KarelWindow() override { publishStop(); }

protected:
  void closeEvent(QCloseEvent *event) override {
    publishStop();
    QMainWindow::closeEvent(event);
  }

private:
  enum Movement { Stopped, Forward, Backward, TurnLeft, TurnRight };

  QWidget *collapsibleSection(const QString &title, QWidget *parent,
                              QWidget *&content) {
    QWidget *section = new QWidget(parent);
    QVBoxLayout *layout = new QVBoxLayout(section);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(4);

    QToolButton *toggle = new QToolButton(section);
    toggle->setText(title);
    toggle->setCheckable(true);
    toggle->setChecked(false);
    toggle->setArrowType(Qt::RightArrow);
    toggle->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    toggle->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    toggle->setStyleSheet(
        "QToolButton { font-weight: 600; text-align: left; padding: 6px; }");

    content = new QWidget(section);
    content->setVisible(false);
    layout->addWidget(toggle);
    layout->addWidget(content);

    connect(toggle, &QToolButton::toggled, this,
            [toggle, content](bool expanded) {
              toggle->setArrowType(expanded ? Qt::DownArrow
                                            : Qt::RightArrow);
              content->setVisible(expanded);
            });
    return section;
  }

  QPushButton *movementButton(const QString &text, Movement movement) {
    QPushButton *button = new QPushButton(text, this);
    button->setMinimumWidth(105);
    button->setFixedHeight(44);
    button->setStyleSheet(
        "QPushButton { background: #1769aa; color: white; font-weight: 600; "
        "border-radius: 6px; padding: 8px; } "
        "QPushButton:pressed { background: #003c7a; }");
    connect(button, &QPushButton::pressed, this,
            [this, movement]() { movement_ = movement; });
    connect(button, &QPushButton::released, this, [this]() { stopRobot(); });
    return button;
  }

  QSlider *slider(int minimum, int maximum, int value) {
    QSlider *result = new QSlider(Qt::Horizontal, this);
    result->setRange(minimum, maximum);
    result->setValue(value);
    return result;
  }

  void updateSpeedLabels() {
    linear_value_->setText(QString::number(linearSpeed(), 'f', 2) + " m/s");
    angular_value_->setText(QString::number(angular_slider_->value()) +
                            " deg/s");
  }

  double linearSpeed() const { return linear_slider_->value() / 100.0; }
  double angularSpeed() const {
    return angular_slider_->value() * M_PI / 180.0;
  }

  void publishCommand() {
    geometry_msgs::Twist command;
    if (movement_ == Forward)
      command.linear.x = linearSpeed();
    if (movement_ == Backward)
      command.linear.x = -linearSpeed();
    if (movement_ == TurnLeft)
      command.angular.z = angularSpeed();
    if (movement_ == TurnRight)
      command.angular.z = -angularSpeed();
    publisher_.publish(command);
  }

  void publishStop() {
    geometry_msgs::Twist command;
    publisher_.publish(command);
  }

  void stopRobot() {
    movement_ = Stopped;
    publishStop();
  }

  void resetPosition() {
    stopRobot();
    if (!initial_pose_available_) {
      action_status_->setText(
          "No se puede volver al inicio: falta la pose inicial de Gazebo.");
      ROS_ERROR("Initial Karel pose parameters are not available");
      return;
    }

    if (!set_model_state_client_.waitForExistence(ros::Duration(1.0))) {
      action_status_->setText(
          "No se puede volver al inicio: Gazebo no esta disponible.");
      ROS_ERROR("Gazebo set_model_state service is not available");
      return;
    }

    gazebo_msgs::SetModelState service;
    service.request.model_state.model_name = "karel";
    service.request.model_state.reference_frame = "world";
    service.request.model_state.pose.position.x = initial_pose_x_;
    service.request.model_state.pose.position.y = initial_pose_y_;
    service.request.model_state.pose.position.z = 0.0;
    service.request.model_state.pose.orientation.z =
        std::sin(initial_pose_yaw_ / 2.0);
    service.request.model_state.pose.orientation.w =
        std::cos(initial_pose_yaw_ / 2.0);
    if (!set_model_state_client_.call(service)) {
      action_status_->setText(
          "Error al solicitar el reposicionamiento del robot.");
      ROS_ERROR("Failed to call Gazebo set_model_state service");
      return;
    }
    if (!service.response.success) {
      action_status_->setText(
          QString("Gazebo no pudo reposicionar el robot: %1")
              .arg(QString::fromStdString(service.response.status_message)));
      ROS_ERROR("Gazebo rejected robot reset: %s",
                service.response.status_message.c_str());
      return;
    }

    action_status_->setText("Robot en su posicion inicial.");
  }

  void startScore() {
    const bool time_limit_enabled = !score_time_limit_checkbox_->isChecked();
    const int time_limit_minutes = score_time_limit_minutes_->value();
    ros::param::set("/karel_score/time_limit_enabled", time_limit_enabled);
    ros::param::set("/karel_score/time_limit_minutes", time_limit_minutes);
    bool configured_time_limit_enabled = !time_limit_enabled;
    int configured_time_limit_minutes = 0;
    if (!ros::param::get("/karel_score/time_limit_enabled",
                         configured_time_limit_enabled) ||
        !ros::param::get("/karel_score/time_limit_minutes",
                         configured_time_limit_minutes) ||
        configured_time_limit_enabled != time_limit_enabled ||
        configured_time_limit_minutes != time_limit_minutes) {
      action_status_->setText(
          "No se pudo aplicar la configuracion del limite de tiempo.");
      ROS_ERROR("Failed to configure the Karel score time limit");
      return;
    }

    if (!start_score_client_.waitForExistence(ros::Duration(1.0))) {
      action_status_->setText(
          "No se puede iniciar la medicion: score no esta disponible.");
      ROS_ERROR("Karel score start service is not available");
      return;
    }

    std_srvs::Trigger service;
    if (!start_score_client_.call(service)) {
      action_status_->setText("Error al solicitar el inicio de la medicion.");
      ROS_ERROR("Failed to call Karel score start service");
      return;
    }
    if (!service.response.success) {
      action_status_->setText(
          QString("No se inicio la medicion: %1")
              .arg(QString::fromStdString(service.response.message)));
      ROS_WARN("Karel score did not start: %s",
               service.response.message.c_str());
      return;
    }

    action_status_->setText("Medicion iniciada.");
  }

  void resetScore() {
    if (!reset_score_client_.waitForExistence(ros::Duration(1.0))) {
      action_status_->setText(
          "No se puede reiniciar la evaluacion: score no esta disponible.");
      ROS_ERROR("Karel score reset service is not available");
      return;
    }

    std_srvs::Empty service;
    if (!reset_score_client_.call(service)) {
      action_status_->setText(
          "Error al solicitar el reinicio de la evaluacion.");
      ROS_ERROR("Failed to call Karel score reset service");
      return;
    }

    action_status_->setText("Evaluacion reiniciada.");
  }

  void odometryCallback(const nav_msgs::Odometry::ConstPtr &message) {
    const geometry_msgs::Point &position = message->pose.pose.position;
    const geometry_msgs::Quaternion &q = message->pose.pose.orientation;
    const double yaw = std::atan2(2.0 * (q.w * q.z + q.x * q.y),
                                  1.0 - 2.0 * (q.y * q.y + q.z * q.z));
    pose_value_->setText(QString("x %1 m | y %2 m | yaw %3 deg")
                             .arg(position.x, 0, 'f', 2)
                             .arg(position.y, 0, 'f', 2)
                             .arg(yaw * 180.0 / M_PI, 0, 'f', 1));
  }

  void updateRange(QLabel *value, const sensor_msgs::Range::ConstPtr &message) {
    if (std::isfinite(message->range)) {
      value->setText(QString::number(message->range, 'f', 2) + " m");
    } else {
      value->setText("Fuera de rango");
    }
  }

  void frontRangeCallback(const sensor_msgs::Range::ConstPtr &message) {
    updateRange(range_value_, message);
  }

  void backRangeCallback(const sensor_msgs::Range::ConstPtr &message) {
    updateRange(back_range_value_, message);
  }

  void leftRangeCallback(const sensor_msgs::Range::ConstPtr &message) {
    updateRange(left_range_value_, message);
  }

  void rightRangeCallback(const sensor_msgs::Range::ConstPtr &message) {
    updateRange(right_range_value_, message);
  }

  void scoreCallback(const std_msgs::Float32::ConstPtr &message) {
    score_value_->setText(QString::number(message->data, 'f', 0) + " puntos");
  }

  void scoreStartedCallback(const std_msgs::Bool::ConstPtr &message) {
    score_running_ = message->data;
    score_state_->setText(
        score_running_ ? "Cronometro activo"
                       : (score_timed_out_ ? "Tiempo agotado"
                                           : "Cronometro detenido"));
    score_time_limit_checkbox_->setEnabled(!score_running_);
    score_time_limit_minutes_->setEnabled(
        !score_running_ && !score_time_limit_checkbox_->isChecked());
  }

  void scoreElapsedCallback(const std_msgs::Float32::ConstPtr &message) {
    score_elapsed_seconds_ = message->data;
    updateScoreTimeDisplay();
  }

  void scoreTimedOutCallback(const std_msgs::Bool::ConstPtr &message) {
    score_timed_out_ = message->data;
    score_state_->setText(
        score_running_ ? "Cronometro activo"
                       : (score_timed_out_ ? "Tiempo agotado"
                                           : "Cronometro detenido"));
  }

  void updateScoreTimeDisplay() {
    if (score_time_limit_checkbox_->isChecked()) {
      score_elapsed_->setText(
          QString("Transcurrido %1 s")
              .arg(score_elapsed_seconds_, 0, 'f', 2));
      return;
    }

    const double seconds_left =
        score_time_limit_minutes_->value() * 60.0 - score_elapsed_seconds_;
    const int remaining =
        static_cast<int>(std::ceil(std::max(0.0, seconds_left)));
    score_elapsed_->setText(
        QString("Restante %1:%2")
            .arg(remaining / 60, 2, 10, QLatin1Char('0'))
            .arg(remaining % 60, 2, 10, QLatin1Char('0')));
  }

  void messageCallback(const std_msgs::String::ConstPtr &message) {
    score_message_->setText(QString::fromStdString(message->data));
  }

  ros::Publisher publisher_;
  ros::Subscriber odometry_subscriber_;
  ros::Subscriber range_subscriber_;
  ros::Subscriber back_range_subscriber_;
  ros::Subscriber left_range_subscriber_;
  ros::Subscriber right_range_subscriber_;
  ros::Subscriber score_subscriber_;
  ros::Subscriber score_started_subscriber_;
  ros::Subscriber score_elapsed_subscriber_;
  ros::Subscriber score_timed_out_subscriber_;
  ros::Subscriber message_subscriber_;
  ros::ServiceClient set_model_state_client_;
  ros::ServiceClient start_score_client_;
  ros::ServiceClient reset_score_client_;
  Movement movement_;
  bool initial_pose_available_;
  double initial_pose_x_;
  double initial_pose_y_;
  double initial_pose_yaw_;
  QTimer ros_timer_;
  QSlider *linear_slider_;
  QSlider *angular_slider_;
  QLabel *linear_value_;
  QLabel *angular_value_;
  QLabel *pose_value_;
  QLabel *range_value_;
  QLabel *back_range_value_;
  QLabel *left_range_value_;
  QLabel *right_range_value_;
  QLabel *score_value_;
  QLabel *score_message_;
  QLabel *score_elapsed_;
  QLabel *score_state_;
  QLabel *action_status_;
  QCheckBox *score_time_limit_checkbox_;
  QSpinBox *score_time_limit_minutes_;
  double score_elapsed_seconds_ = 0.0;
  bool score_running_ = false;
  bool score_timed_out_ = false;
};

int main(int argc, char **argv) {
  ros::init(argc, argv, "karel_gui");
  ros::NodeHandle node;
  QApplication application(argc, argv);
  KarelWindow window(node);
  window.show();
  const int result = application.exec();
  ros::shutdown();
  return result;
}
