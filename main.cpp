/*
 * main.cpp
 *
 *  Created on: Dec 9, 2018
 *      Author: joey
 */

#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <iostream>
#include <fstream>
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/core/utility.hpp"
#include <opencv2/video/background_segm.hpp>
#include <raspicam/raspicam_cv.h>
#include <iomanip>
#include <ctime>
#include <chrono>
#include <thread>
#include <queue>          // std::queue
#include <unistd.h>
#include "platform.h"
#include "net.h"
#include "mat.h"

extern "C" {
#include "c_gpio.h"
#include "pwm.h"
}

#define TIMING 0
#define WHOLE_IMG_WIDTH 160
#define WHOLE_IMG_HEIGHT 120

using namespace cv;

int main(int argc, char** argv) {

  // get esc key init
  struct termios initial_settings, new_settings;
  unsigned char key;
  tcgetattr(0, &initial_settings);

  new_settings = initial_settings;
  new_settings.c_lflag &= ~ICANON;
  new_settings.c_lflag &= ~ECHO;
  new_settings.c_lflag &= ~ISIG;
  new_settings.c_cc[VMIN] = 0;
  new_settings.c_cc[VTIME] = 0;

  tcsetattr(0, TCSANOW, &new_settings);

  // PWM init
  set_loglevel(999);
  int throttle_pwm_pin = 12, steering_pwm_pin = 18; //pin32, pin12
  int pw_incr_us = 2;
  pwm_setup(pw_incr_us, DELAY_VIA_PWM);
  int channel = 0;
  int subcycle_time_us = SUBCYCLE_TIME_US_DEFAULT; //20ms;
  init_channel(channel, subcycle_time_us);
  print_channel(channel);
  add_channel_pulse(channel, throttle_pwm_pin, 0, 1500 / pw_incr_us); // 1500 is middle value
  add_channel_pulse(channel, steering_pwm_pin, 0, 1500 / pw_incr_us);

  // Camera init
  raspicam::RaspiCam_Cv Camera;
//	Camera.set( CV_CAP_PROP_FORMAT, CV_8UC3 );
  Camera.set(CV_CAP_PROP_FRAME_WIDTH, WHOLE_IMG_WIDTH);
  Camera.set(CV_CAP_PROP_FRAME_HEIGHT, WHOLE_IMG_HEIGHT);
  Camera.set(CV_CAP_PROP_FPS, 120);
  if (!Camera.open()) {
    std::cerr << "Error opening the camera" << std::endl;
    return -1;
  }
  Camera.grab();
  cv::Mat frame;
  std::chrono::steady_clock::time_point t1, t_frame;

  // NCNN init
  ncnn::Net net;
  net.load_param("/home/pi/mycar/models/ncnn_mypilot.param");
  net.load_model("/home/pi/mycar/models/ncnn_mypilot.bin");
  cv::Mat test_img;
  int w; // image width
  int h; // image height
  ncnn::Mat angle_out_m, throttle_out_m;
  ncnn::Mat in;

  int count = 0;
  while (Camera.isOpened()) {

    int input;
    input = getchar();
    if (input == 27) {  // ESC key to quit
      break;
    }

#if TIMING == 1
    std::chrono::duration<double> time_span1 = std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::steady_clock::now() - t_frame);
    t_frame = std::chrono::steady_clock::now();
    std::cout << "Cycle Time: " << time_span1.count() * 1000 << "ms" << std::endl;
    t1 = std::chrono::steady_clock::now();
#endif

    Camera.retrieve(frame);
#if TIMING == 1
    std::chrono::duration<double> time_span3 = std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::steady_clock::now() - t1);
    std::cout << "camera retrieve Time: " << time_span3.count() * 1000 << "ms" << std::endl;
    t1 = std::chrono::steady_clock::now();
#endif
    Camera.grab();
#if TIMING == 1
    std::chrono::duration<double> time_span4 = std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::steady_clock::now() - t1);
    std::cout << "grab Time: " << time_span4.count() * 1000 << "ms" << std::endl;
#endif
//		printf("frame width: %d\n", frame.cols);
    count++;

    // offline continues image test
//		if(count % 2 == 0){
//		  test_img = cv::imread("/home/pi/codes/FastDonkey/1030_cam-image_array_.jpg");
//		  printf("1\n");
//		}else{
//		  test_img = cv::imread("/home/pi/codes/FastDonkey/0_cam-image_array_.jpg");
//		  printf("2\n");
//		}
    test_img = frame;
#if TIMING == 1
    t1 = std::chrono::steady_clock::now();
#endif
    //cv::resize(frame, test_img, cv::Size(160, 120));
    in = ncnn::Mat::from_pixels(test_img.data, ncnn::Mat::PIXEL_BGR, test_img.cols,
      test_img.rows);
    ncnn::Extractor ex = net.create_extractor();
//    ex.set_num_threads(1);
    ex.set_light_mode(true);
    ex.input("data", in);
    ex.extract("angle_out", angle_out_m);
    ex.extract("throttle_out", throttle_out_m);
#if TIMING == 1
    std::chrono::duration<double> time_span2 = std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::steady_clock::now() - t1);
    std::cout << "net Time: " << time_span2.count() * 1000 << "ms" << std::endl;
#endif
    assert(angle_out_m.w == 1);
//    printf("angle_out: %f\n", angle_out_m[0]);
    assert(throttle_out_m.w == 1);
//    printf("throttle_out: %f\n", throttle_out_m[0]);

    int angle_int, throttle_int;
    if (angle_out_m[0] > 1.0) {
      angle_out_m[0] = 1.0;
    } else if (angle_out_m[0] < -1.0) {
      angle_out_m[0] = -1.0;
    } else {
    }
    if (throttle_out_m[0] > 1.0) {
      throttle_out_m[0] = 1.0;
    } else if (throttle_out_m[0] < -1.0) {
      throttle_out_m[0] = -1.0;
    } else {
    }

    // speed down when steering
    if (fabs(angle_out_m[0]) > 0.5) {
      throttle_out_m[0] = throttle_out_m[0] * 0.5;
    }
    if (fabs(angle_out_m[0] > 0.8)) {
      throttle_out_m[0] = throttle_out_m[0] * 0.6;
    }

    angle_int = -500 * angle_out_m[0] + 1500; // calibrate this value accordingly
    throttle_int = 520 * throttle_out_m[0] + 1500;
//    printf("angle_int: %d\n", angle_int);
//    printf("throttle_int: %d\n", throttle_int);

    // update PWM output
    add_channel_pulse(channel, throttle_pwm_pin, 0, throttle_int / pw_incr_us);
    add_channel_pulse(channel, steering_pwm_pin, 0, angle_int / pw_incr_us);
  }

  // restore terminal settings
  tcsetattr(0, TCSANOW, &initial_settings);

  // stop the car
  add_channel_pulse(channel, throttle_pwm_pin, 0, 1500 / pw_incr_us);
  add_channel_pulse(channel, steering_pwm_pin, 0, 1500 / pw_incr_us);
  std::cout << "Stop camera..." << std::endl;
  Camera.release();

}
