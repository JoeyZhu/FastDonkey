#include <stdio.h>
#include <algorithm>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "platform.h"
#include "net.h"
#include "mat.h"

int main(int argc, char** argv)
{
	ncnn::Net net;
	net.load_param("/home/pi/mycar/models/ncnn_mypilot.param");
	net.load_model("/home/pi/mycar/models/ncnn_mypilot.bin");


	cv::Mat test_img;
	int w;// image width
	int h;// image height
	test_img = cv::imread("/home/pi/codes/FastDonkey/1030_cam-image_array_.jpg");
	ncnn::Mat in = ncnn::Mat::from_pixels(test_img.data, ncnn::Mat::PIXEL_BGR, test_img.cols, test_img.rows);

	ncnn::Mat angle_out, throttle_out;
	ncnn::Extractor ex = net.create_extractor();
	ex.set_light_mode(true);
	ex.input("data", in);
	ex.extract("angle_out", angle_out);
	ex.extract("throttle_out", throttle_out);
	for (int j=0; j<angle_out.w; j++)
	{
		printf("angle_out: %f\n", angle_out[j]);
	}
	for (int j=0; j<throttle_out.w; j++)
	{
		printf("angle_out: %f\n", throttle_out[j]);
	}

	net.clear();

}
