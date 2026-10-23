/*
 * Karel ROS
 * Copyright (C) 2026 Julio Cruz
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <cmath>
#include <iomanip>
#include <map>
#include <mutex>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include <gazebo/gazebo_client.hh>
#include <gazebo/msgs/msgs.hh>
#include <gazebo/transport/transport.hh>
#include <gazebo_msgs/ModelStates.h>
#include <ros/ros.h>
#include <std_msgs/Bool.h>
#include <std_msgs/Float32.h>
#include <std_msgs/String.h>
#include <std_srvs/Empty.h>
#include <std_srvs/Trigger.h>

namespace {

std::string modelName(const std::string& collision) {
  const std::size_t separator = collision.find("::");
  return separator == std::string::npos ? collision
                                         : collision.substr(0, separator);
}

bool startsWith(const std::string& value, const std::string& prefix) {
  return value.compare(0, prefix.size(), prefix) == 0;
}

struct ObstacleState {
  double x = 0.0;
  double y = 0.0;
  double min_distance = 1e9;
  bool touched = false;
  bool awarded = false;
};

struct ContactState {
  bool active = false;
  ros::WallTime last_seen;
};

}  // namespace

class KarelScoreCounter {
 public:
  KarelScoreCounter()
      : private_node_("~"), robot_pose_available_(false), robot_x_(0.0),
        robot_y_(0.0), score_(0.0), goal_reached_(false), run_started_(false),
        timed_out_(false), elapsed_seconds_(0.0), time_limit_enabled_(true),
        time_limit_minutes_(10) {
    private_node_.param<std::string>("robot_model", robot_model_, "karel");
    private_node_.param<std::string>("obstacle_prefix", obstacle_prefix_,
                                     "circuit_obstacle_");
    private_node_.param("goal_x", goal_x_, 0.85);
    private_node_.param("goal_y", goal_y_, 0.0);
    private_node_.param("goal_line_half_width", goal_line_half_width_, 0.25);
    private_node_.param("goal_line_tolerance", goal_line_tolerance_, 0.10);
    private_node_.param("pass_margin", pass_margin_, 0.08);
    private_node_.param("evade_close_radius", evade_close_radius_, 0.20);
    private_node_.param("collision_release_seconds", collision_release_seconds_,
                        0.40);
    private_node_.param("score_goal", score_goal_, 100.0);
    private_node_.param("score_evade_per_obstacle", score_evade_, 20.0);
    private_node_.param("score_evade_close_bonus", score_close_bonus_, 10.0);
    private_node_.param("penalty_collision", collision_penalty_, 30.0);
    private_node_.param("require_obstacles_for_goal",
                        require_obstacles_for_goal_, true);
    private_node_.param("time_limit_enabled", time_limit_enabled_, true);
    private_node_.param("time_limit_minutes", time_limit_minutes_, 10);

    score_publisher_ = node_.advertise<std_msgs::Float32>("/score", 1, true);
    message_publisher_ = node_.advertise<std_msgs::String>("/message", 1, true);
    detail_publisher_ = node_.advertise<std_msgs::String>("/score/detail", 1, true);
    started_publisher_ = node_.advertise<std_msgs::Bool>("/score/started", 1, true);
    elapsed_publisher_ =
        node_.advertise<std_msgs::Float32>("/score/elapsed_seconds", 1, true);
    timed_out_publisher_ =
        node_.advertise<std_msgs::Bool>("/score/timed_out", 1, true);
    model_subscriber_ = node_.subscribe("/gazebo/model_states", 1,
                                         &KarelScoreCounter::modelCallback, this);
    reset_service_ = private_node_.advertiseService(
        "reset", &KarelScoreCounter::resetCallback, this);
    start_service_ = node_.advertiseService(
        "/karel_score/start", &KarelScoreCounter::startCallback, this);
    timer_ = node_.createTimer(ros::Duration(0.1),
                                &KarelScoreCounter::timerCallback, this);

    gazebo_node_.reset(new gazebo::transport::Node());
    gazebo_node_->Init();
    contact_subscriber_ = gazebo_node_->Subscribe(
        "~/physics/contacts", &KarelScoreCounter::contactCallback, this);

    publishMessage("Evaluacion lista; esperando obstaculos");
    publishScore();
    publishStarted();
    publishElapsed();
    publishTimedOut(false);
  }

 private:
  void modelCallback(const gazebo_msgs::ModelStates::ConstPtr& message) {
    std::set<std::string> visible_obstacles;
    for (std::size_t index = 0; index < message->name.size(); ++index) {
      const std::string& name = message->name[index];
      if (name == robot_model_) {
        robot_x_ = message->pose[index].position.x;
        robot_y_ = message->pose[index].position.y;
        robot_pose_available_ = true;
      } else if (startsWith(name, obstacle_prefix_)) {
        visible_obstacles.insert(name);
        ObstacleState& obstacle = obstacles_[name];
        obstacle.x = message->pose[index].position.x;
        obstacle.y = message->pose[index].position.y;
      }
    }

    for (std::map<std::string, ObstacleState>::iterator it = obstacles_.begin();
         it != obstacles_.end();) {
      if (visible_obstacles.count(it->first) == 0) {
        it = obstacles_.erase(it);
      } else {
        ++it;
      }
    }
  }

  void contactCallback(ConstContactsPtr& message) {
    const ros::WallTime now = ros::WallTime::now();
    std::lock_guard<std::mutex> lock(contact_mutex_);
    for (int index = 0; index < message->contact_size(); ++index) {
      const gazebo::msgs::Contact& contact = message->contact(index);
      const std::string first = modelName(contact.collision1());
      const std::string second = modelName(contact.collision2());
      std::string obstacle;
      if (first == robot_model_ && startsWith(second, obstacle_prefix_)) {
        obstacle = second;
      } else if (second == robot_model_ && startsWith(first, obstacle_prefix_)) {
        obstacle = first;
      }
      if (obstacle.empty()) continue;

      ContactState& state = contacts_[obstacle];
      if (!state.active) {
        state.active = true;
        collision_events_.push_back(obstacle);
      }
      state.last_seen = now;
    }
  }

  void timerCallback(const ros::TimerEvent&) {
    if (run_started_) {
      elapsed_seconds_ = (ros::Time::now() - run_start_time_).toSec();
      publishElapsed();
    }
    if (run_started_) {
      const bool within_time_limit =
          !time_limit_enabled_ ||
          elapsed_seconds_ <= time_limit_minutes_ * 60.0;
      if (within_time_limit) {
        processContacts();
        if (robot_pose_available_) {
          scoreEvasions();
          scoreGoal();
        }
      }
      if (run_started_ && time_limit_enabled_ &&
          elapsed_seconds_ >= time_limit_minutes_ * 60.0) {
        run_started_ = false;
        timed_out_ = true;
        publishMessage("Tiempo agotado; la medicion ha terminado");
        publishStarted();
        publishTimedOut(timed_out_);
        ROS_INFO("[Karel score] Tiempo agotado; la medicion ha terminado");
      }
    }
    publishScore();
  }

  void processContacts() {
    std::vector<std::string> events;
    const ros::WallTime now = ros::WallTime::now();
    {
      std::lock_guard<std::mutex> lock(contact_mutex_);
      events.swap(collision_events_);
      for (std::map<std::string, ContactState>::iterator it = contacts_.begin();
           it != contacts_.end(); ++it) {
        if (it->second.active &&
            (now - it->second.last_seen).toSec() > collision_release_seconds_) {
          it->second.active = false;
        }
      }
    }

    for (std::size_t index = 0; index < events.size(); ++index) {
      ObstacleState& obstacle = obstacles_[events[index]];
      obstacle.touched = true;
      score_ -= collision_penalty_;
      std::ostringstream message;
      message << "Colision con " << events[index] << ": -"
              << collision_penalty_ << " puntos";
      publishMessage(message.str());
      ROS_WARN("[Karel score] %s", message.str().c_str());
    }
  }

  void scoreEvasions() {
    for (std::map<std::string, ObstacleState>::iterator it = obstacles_.begin();
         it != obstacles_.end(); ++it) {
      ObstacleState& obstacle = it->second;
      const double distance = std::hypot(robot_x_ - obstacle.x,
                                         robot_y_ - obstacle.y);
      obstacle.min_distance = std::min(obstacle.min_distance, distance);
      if (!obstacle.awarded && robot_x_ > obstacle.x + pass_margin_) {
        obstacle.awarded = true;
        if (!obstacle.touched) {
          double gained = score_evade_;
          if (obstacle.min_distance <= evade_close_radius_) {
            gained += score_close_bonus_;
          }
          score_ += gained;
          std::ostringstream message;
          message << "Obstaculo " << it->first << " evadido: +" << gained
                  << " puntos";
          publishMessage(message.str());
          ROS_INFO("[Karel score] %s", message.str().c_str());
        }
      }
    }
  }

  void scoreGoal() {
    if (!run_started_ || goal_reached_) return;
    if (require_obstacles_for_goal_ && obstacles_.empty()) return;
    if (std::abs(robot_x_ - goal_x_) > goal_line_tolerance_ ||
        std::abs(robot_y_ - goal_y_) > goal_line_half_width_) {
      return;
    }

    goal_reached_ = true;
    run_started_ = false;
    elapsed_seconds_ = (ros::Time::now() - run_start_time_).toSec();
    score_ += score_goal_;
    std::ostringstream message;
    message << "Meta alcanzada en " << std::fixed << std::setprecision(2)
            << elapsed_seconds_ << " s: +" << score_goal_ << " puntos";
    publishMessage(message.str());
    publishStarted();
    publishElapsed();
    ROS_INFO("[Karel score] %s", message.str().c_str());
  }

  bool resetCallback(std_srvs::Empty::Request&, std_srvs::Empty::Response&) {
    score_ = 0.0;
    goal_reached_ = false;
    run_started_ = false;
    timed_out_ = false;
    elapsed_seconds_ = 0.0;
    time_limit_enabled_ = true;
    time_limit_minutes_ = 10;
    private_node_.param("time_limit_enabled", time_limit_enabled_, true);
    private_node_.param("time_limit_minutes", time_limit_minutes_, 10);
    for (std::map<std::string, ObstacleState>::iterator it = obstacles_.begin();
         it != obstacles_.end(); ++it) {
      it->second.min_distance = 1e9;
      it->second.touched = false;
      it->second.awarded = false;
    }
    {
      std::lock_guard<std::mutex> lock(contact_mutex_);
      contacts_.clear();
      collision_events_.clear();
    }
    publishMessage("Puntuacion reiniciada");
    publishStarted();
    publishElapsed();
    publishTimedOut(false);
    publishScore();
    return true;
  }

  bool startCallback(std_srvs::Trigger::Request&,
                     std_srvs::Trigger::Response& response) {
    if (run_started_) {
      response.message = "La medicion ya esta en curso";
      return true;
    }
    if (goal_reached_) {
      response.message =
          "Reinicia la evaluacion antes de comenzar otra medicion";
      return true;
    }
    if (timed_out_) {
      response.message =
          "Reinicia la evaluacion antes de comenzar otra medicion";
      return true;
    }
    if (!ros::Time::isValid() || ros::Time::now().isZero()) {
      response.message = "El reloj de ROS aun no esta disponible";
      return true;
    }

    bool configured_time_limit_enabled = time_limit_enabled_;
    int configured_time_limit_minutes = time_limit_minutes_;
    if ((private_node_.hasParam("time_limit_enabled") &&
         !private_node_.getParam("time_limit_enabled",
                                 configured_time_limit_enabled)) ||
        (private_node_.hasParam("time_limit_minutes") &&
         !private_node_.getParam("time_limit_minutes",
                                 configured_time_limit_minutes))) {
      response.message = "La configuracion del limite de tiempo no es valida";
      return true;
    }
    if (configured_time_limit_enabled && configured_time_limit_minutes <= 0) {
      response.message = "El limite de tiempo debe ser mayor que cero";
      return true;
    }
    time_limit_enabled_ = configured_time_limit_enabled;
    time_limit_minutes_ = configured_time_limit_minutes;

    run_start_time_ = ros::Time::now();
    elapsed_seconds_ = 0.0;
    run_started_ = true;
    timed_out_ = false;
    publishTimedOut(timed_out_);
    {
      std::lock_guard<std::mutex> lock(contact_mutex_);
      contacts_.clear();
      collision_events_.clear();
    }
    response.success = true;
    response.message = "Medicion iniciada";
    publishMessage(response.message);
    publishStarted();
    publishElapsed();
    return true;
  }

  void publishMessage(const std::string& text) {
    last_message_ = text;
    std_msgs::String message;
    message.data = text;
    message_publisher_.publish(message);
  }

  void publishScore() {
    std_msgs::Float32 score;
    score.data = score_;
    score_publisher_.publish(score);

    int evaded = 0;
    int touched = 0;
    for (std::map<std::string, ObstacleState>::const_iterator it = obstacles_.begin();
         it != obstacles_.end(); ++it) {
      if (it->second.awarded && !it->second.touched) ++evaded;
      if (it->second.touched) ++touched;
    }
    std::ostringstream stream;
    stream << std::fixed << std::setprecision(0) << "total=" << score_
           << " goal=" << (goal_reached_ ? "yes" : "no")
           << " evaded=" << evaded << "/" << obstacles_.size()
           << " touched=" << touched;
    std_msgs::String detail;
    detail.data = stream.str();
    detail_publisher_.publish(detail);
  }

  void publishStarted() {
    std_msgs::Bool message;
    message.data = run_started_;
    started_publisher_.publish(message);
  }

  void publishElapsed() {
    std_msgs::Float32 message;
    message.data = static_cast<float>(elapsed_seconds_);
    elapsed_publisher_.publish(message);
  }

  void publishTimedOut(bool timed_out) {
    std_msgs::Bool message;
    message.data = timed_out;
    timed_out_publisher_.publish(message);
  }

  ros::NodeHandle node_;
  ros::NodeHandle private_node_;
  ros::Publisher score_publisher_;
  ros::Publisher message_publisher_;
  ros::Publisher detail_publisher_;
  ros::Publisher started_publisher_;
  ros::Publisher elapsed_publisher_;
  ros::Publisher timed_out_publisher_;
  ros::Subscriber model_subscriber_;
  ros::ServiceServer reset_service_;
  ros::ServiceServer start_service_;
  ros::Timer timer_;

  gazebo::transport::NodePtr gazebo_node_;
  gazebo::transport::SubscriberPtr contact_subscriber_;
  std::mutex contact_mutex_;
  std::map<std::string, ContactState> contacts_;
  std::vector<std::string> collision_events_;

  std::string robot_model_;
  std::string obstacle_prefix_;
  std::map<std::string, ObstacleState> obstacles_;
  bool robot_pose_available_;
  double robot_x_;
  double robot_y_;
  double score_;
  bool goal_reached_;
  bool run_started_;
  bool timed_out_;
  ros::Time run_start_time_;
  double elapsed_seconds_;
  bool time_limit_enabled_;
  int time_limit_minutes_;
  std::string last_message_;

  double goal_x_;
  double goal_y_;
  double goal_line_half_width_;
  double goal_line_tolerance_;
  double pass_margin_;
  double evade_close_radius_;
  double collision_release_seconds_;
  double score_goal_;
  double score_evade_;
  double score_close_bonus_;
  double collision_penalty_;
  bool require_obstacles_for_goal_;
};

int main(int argc, char** argv) {
  ros::init(argc, argv, "karel_score");
  gazebo::client::setup(argc, argv);
  {
    KarelScoreCounter score_counter;
    ros::spin();
  }
  gazebo::client::shutdown();
  return 0;
}
