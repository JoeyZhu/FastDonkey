#include <stdio.h>
#include <iostream>
#include <algorithm>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <ctime>
#include <chrono>

#include "platform.h"
#include "net.h"
#include "mat.h"

#define TIMING 1


int main(int argc, char** argv)
{
	ncnn::Net net;
	net.load_param("/home/pi/mycar/models/ncnn_mypilot.param");
	net.load_model("/home/pi/mycar/models/ncnn_mypilot.bin");


	cv::Mat test_img;
	int w;// image width
	int h;// image height

  ncnn::Mat angle_out, throttle_out;
  ncnn::Mat in;
  ncnn::Extractor ex = net.create_extractor();

  std::chrono::steady_clock::time_point t1, t_frame;


	for(int i = 0 ; i < 100; i++){
	  test_img = cv::imread("/home/pi/codes/FastDonkey/1030_cam-image_array_.jpg");
	  in = ncnn::Mat::from_pixels(test_img.data, ncnn::Mat::PIXEL_BGR, test_img.cols, test_img.rows);


	  ex.set_light_mode(true);
	  ex.input("data", in);
	  ex.extract("angle_out", angle_out);
	  ex.extract("throttle_out", throttle_out);
	  assert(angle_out.w == 1);
	  printf("angle_out: %f\n", angle_out[0]);
	  assert(throttle_out.w == 1);
	  printf("throttle_out: %f\n", throttle_out[0]);


#if TIMING == 1
    std::chrono::duration<double> time_span1 = std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::steady_clock::now() - t_frame);
    t_frame = std::chrono::steady_clock::now();
    std::cout << "Cycle Time: " << time_span1.count() * 1000 << "ms" << std::endl;
    t1 = std::chrono::steady_clock::now();
#endif

	}


	net.clear();

}
