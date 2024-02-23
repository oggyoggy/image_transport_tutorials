// Copyright 2021, Open Source Robotics Foundation, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "cv_bridge/cv_bridge.h"
#include "image_transport/image_transport.hpp"
#include "opencv2/highgui.hpp"
#include "rclcpp/logging.hpp"
#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/image.hpp"

void imageCallback(const sensor_msgs::msg::Image::ConstSharedPtr & msg)
{
// ------------- oggy from here ------------------//
// After the image is compressed, the imaged is resized to for a certain display size 
  cv::Mat image_before_resize = cv_bridge::toCvShare(msg, "bgr8")->image;
  double subsampling_factor = 7.0;
  int new_width = image_before_resize.cols / subsampling_factor + 0.5;
  int new_height = image_before_resize.rows / subsampling_factor + 0.5;

  cv::Mat image_after_resize;
  cv::resize(image_before_resize, image_after_resize, cv::Size(new_width, new_height));

  try {
//    cv::imshow("view", cv_bridge::toCvShare(msg, "bgr8")->image);
    cv::imshow("compressed", image_after_resize);
// ------------- oggy to here ------------------//
    cv::waitKey(10);
  } catch (const cv_bridge::Exception & e) {
    auto logger = rclcpp::get_logger("my_subscriber");
    RCLCPP_ERROR(logger, "Could not convert from '%s' to 'bgr8'.", msg->encoding.c_str());
  }
}

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  rclcpp::NodeOptions options;
  rclcpp::Node::SharedPtr node = rclcpp::Node::make_shared("image_listener", options);
  // TransportHints does not actually declare the parameter
  node->declare_parameter<std::string>("image_transport", "raw");
  cv::namedWindow("compressed");
  cv::startWindowThread();
  image_transport::ImageTransport it(node);
  image_transport::TransportHints hints(node.get());
//  image_transport::Subscriber sub = it.subscribe("camera/image", 1, imageCallback, &hints);
  image_transport::Subscriber sub = it.subscribe("camera/color/image_raw", 1, imageCallback, &hints);
  rclcpp::spin(node);
  cv::destroyWindow("compressed");

  return 0;
}
