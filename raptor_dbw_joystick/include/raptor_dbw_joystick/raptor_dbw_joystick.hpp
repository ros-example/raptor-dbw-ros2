//  Copyright (c) 2018-2021 New Eagle, Copyright (c) 2015-2018, Dataspeed Inc.
// All rights reserved.
//
// Software License Agreement (BSD License 2.0)
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above
//    copyright notice, this list of conditions and the following
//    disclaimer in the documentation and/or other materials provided
//    with the distribution.
//  * Neither the name of {copyright_holder} nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
// COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
// BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
// LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
// ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

/** \brief This file defines the RaptorDbwJoystick class.
 * \copyright Copyright 2021 New Eagle LLC
 * \file raptor_dbw_joystick.hpp
 */

#ifndef RAPTOR_DBW_JOYSTICK__RAPTOR_DBW_JOYSTICK_HPP_
#define RAPTOR_DBW_JOYSTICK__RAPTOR_DBW_JOYSTICK_HPP_

#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/joy.hpp>
#include <std_msgs/msg/empty.hpp>

#include <raptor_dbw_msgs/msg/accelerator_pedal_cmd.hpp>
#include <raptor_dbw_msgs/msg/brake_cmd.hpp>
#include <raptor_dbw_msgs/msg/gear_cmd.hpp>
#include <raptor_dbw_msgs/msg/global_enable_cmd.hpp>
#include <raptor_dbw_msgs/msg/misc_cmd.hpp>
#include <raptor_dbw_msgs/msg/steering_cmd.hpp>

#include <chrono>

using namespace std::chrono_literals;  // NOLINT

using sensor_msgs::msg::Joy;
using std_msgs::msg::Empty;

using raptor_dbw_msgs::msg::AcceleratorPedalCmd;
using raptor_dbw_msgs::msg::BrakeCmd;
using raptor_dbw_msgs::msg::Gear;
using raptor_dbw_msgs::msg::GearCmd;
using raptor_dbw_msgs::msg::GlobalEnableCmd;
using raptor_dbw_msgs::msg::MiscCmd;
using raptor_dbw_msgs::msg::SteeringCmd;
using raptor_dbw_msgs::msg::TurnSignal;

namespace raptor_dbw_joystick
{
typedef struct
{
  std::chrono::time_point<std::chrono::steady_clock> stamp;
  float brake_joy;
  float accelerator_pedal_joy;
  float steering_joy;
  bool steering_mult;
  int gear_cmd;
  int turn_signal_cmd;
  bool joy_accelerator_pedal_valid;
  bool joy_brake_valid;
} JoystickDataStruct;

/** \brief Class for sending control commands to NE Raptor DBW with a joystick. */
class RaptorDbwJoystick : public rclcpp::Node
{
public:
/** \brief Default constructor.
 * \param[in] options The options for this node.
 * \param[in] ignore Whether driver overrides should be ignored
 * \param[in] enable Whether joystick node can control enable/disable
 * \param[in] svel Steering angle velocity, deg/s
 * \param[in] max_steer_angle Maximum steering angle allowed, deg
 */
  explicit RaptorDbwJoystick(
    const rclcpp::NodeOptions & options,
    bool ignore,
    bool enable,
    double svel,
    float max_steer_angle);

private:
  rclcpp::Clock m_clock;
  static constexpr int64_t CLOCK_1_SEC = 1000;  // duration in milliseconds

  /** \brief Convert the joystick input from the joystick hardware into ROS messages.
   * \param[in] msg The joystick input received from the hardware.
   */
  void recvJoy(const Joy::SharedPtr msg);

  /** \brief Send the translated commands to the Drive By Wire node
   *    via published ROS messages.
   */
  void cmdCallback();

  // Topics
  rclcpp::Subscription<Joy>::SharedPtr sub_joy_;

  rclcpp::Publisher<AcceleratorPedalCmd>::SharedPtr pub_accelerator_pedal_;
  rclcpp::Publisher<BrakeCmd>::SharedPtr pub_brake_;
  rclcpp::Publisher<GearCmd>::SharedPtr pub_gear_;
  rclcpp::Publisher<GlobalEnableCmd>::SharedPtr pub_global_enable_;
  rclcpp::Publisher<MiscCmd>::SharedPtr pub_misc_;
  rclcpp::Publisher<SteeringCmd>::SharedPtr pub_steering_;
  rclcpp::Publisher<Empty>::SharedPtr pub_enable_;
  rclcpp::Publisher<Empty>::SharedPtr pub_disable_;

  // Parameters
  bool ignore_;     // Ignore driver overrides
  bool enable_;     // Use enable and disable buttons
  double svel_;     // Steering command speed
  float max_steer_angle_;  // Maximum steering angle allowed

  // Variables
  rclcpp::TimerBase::SharedPtr timer_;
  JoystickDataStruct data_;
  Joy joy_;
  uint8_t counter_;

  enum
  {
    BTN_PARK = 3,
    BTN_REVERSE = 1,
    BTN_NEUTRAL = 2,
    BTN_DRIVE = 0,
    BTN_ENABLE = 5,
    BTN_DISABLE = 4,
    BTN_STEER_MULT_1 = 6,
    BTN_STEER_MULT_2 = 7,
    BTN_COUNT = 11,
    AXIS_ACCELERATOR_PEDAL = 5,
    AXIS_BRAKE = 2,
    AXIS_STEER_1 = 0,
    AXIS_STEER_2 = 3,
    AXIS_TURN_SIG = 6,
    AXIS_COUNT = 8,
  };
};
}  // namespace raptor_dbw_joystick

#endif  // RAPTOR_DBW_JOYSTICK__RAPTOR_DBW_JOYSTICK_HPP_
