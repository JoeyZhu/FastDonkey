/*
 * main.cpp
 *
 *  Created on: Dec 9, 2018
 *      Author: joey
 */

#include <stdio.h>
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

extern "C" {
#include "c_gpio.h"
#include "pwm.h"
}

#define TIMING 1
#define WHOLE_IMG_WIDTH 640
#define WHOLE_IMG_HEIGHT 480

using namespace cv;

int main(int argc, char** argv){

  set_loglevel(999);
  int heading_gpio = 18, pitch_gpio = 12, mode_gpio = 13, laser_gpio = 23,
      laser_switch = 0;


	raspicam::RaspiCam_Cv Camera;
//	Camera.set( CV_CAP_PROP_FORMAT, CV_8UC3 );
	Camera.set(CV_CAP_PROP_FRAME_WIDTH, WHOLE_IMG_WIDTH);    //  1280x720
	Camera.set(CV_CAP_PROP_FRAME_HEIGHT, WHOLE_IMG_HEIGHT);
	Camera.set(CV_CAP_PROP_FPS, 120);
	if (!Camera.open()) {
		std::cerr << "Error opening the camera" << std::endl;
		return -1;
	}
	Camera.grab();
	Mat frame;
	std::chrono::steady_clock::time_point t1, t_frame;
/*
ncnn::Net net;
net.load_param("alexnet.param");
net.load_model("alexnet.bin");
net.clear();
*/
	while (Camera.isOpened()) {

#if TIMING == 1
		std::chrono::duration<double> time_span1 = std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::steady_clock::now() - t_frame);
		t_frame = std::chrono::steady_clock::now();
		std::cout << "Cycle Time: " << time_span1.count() * 1000 << "ms" << std::endl;
		t1 = std::chrono::steady_clock::now();
#endif


		Camera.retrieve(frame);
		Camera.grab();
	
	}

	std::cout << "Stop camera..." << std::endl;
	Camera.release();


}
