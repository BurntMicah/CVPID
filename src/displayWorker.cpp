#include "displayWorker.hpp"
#include "mailBox.hpp"
#include "processingWorker.hpp"
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#define _USE_MATH_DEFINES
#include <cmath>
#include <sstream>

const char *colour_view = "CVPID Vision";
const char *binary_view = "Threshold Mask";

static void draw_box(cv::Mat &img, cv::Point loc, double circle_area){
	double radius = sqrt(circle_area / M_PI);
	cv::Point top_left(loc.x - radius, loc.y - radius);
	cv::Point bot_right(loc.x + radius, loc.y + radius);
	cv::Scalar colour(0,255,0);
	int thickness = 2;
	cv::rectangle(img, top_left, bot_right, colour, thickness, cv::LINE_4, 0);
	std::stringstream coords;
	coords << "(" << loc.x << "," << loc.y << ")";
	cv::putText(img, coords.str(), cv::Point(loc.x - radius, loc.y + radius + 20), cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(0, 255, 0));
}

static void draw_target(cv::Mat &img, int x, int y){
	const int thickness = 2;
	const int lineType = cv::LINE_8;
	const int length = 40;
	cv::Scalar colour(0, 0, 255);
	cv::line(img, cv::Point(x, y - length), cv::Point(x, y + length), colour, thickness, lineType);
	cv::line(img, cv::Point(x - length, y), cv::Point(x + length, y), colour, thickness, lineType);
}

void display(MailBox<BlobInfo> &display_in, ProcParams &params, bool &running){
	// Create a window
	cv::namedWindow(colour_view, 1);
	// Create a window
	cv::namedWindow(binary_view, 1);
	const int hue_slider_max = 179 - 10;
	const int SV_slider_max = 254;
	const int min_A_slider_max = 1000;
	const int set_point_x_max = 640;
	const int set_point_y_max = 360;
	params.hue_center = 108;
	params.min_S = 209;
	params.min_V = 64;
	params.min_A = 107;
	params.set_point_x = set_point_x_max / 2;
	params.set_point_y = set_point_y_max / 2;
	cv::createTrackbar("Hue", binary_view, &params.hue_center, hue_slider_max, NULL);
	cv::createTrackbar("Min Saturation", binary_view, &params.min_S, SV_slider_max, NULL);
	cv::createTrackbar("Min Value", binary_view, &params.min_V, SV_slider_max, NULL);
	cv::createTrackbar("Min Area", binary_view, &params.min_A, min_A_slider_max, NULL);
	cv::createTrackbar("X", colour_view, &params.set_point_x, set_point_x_max, NULL);
	cv::createTrackbar("Y", colour_view, &params.set_point_y, set_point_y_max, NULL);
	
	while(running){
		BlobInfo display = display_in.get();
		if(display.found){
			draw_box(display.colour, display.p, display.circle_area);
		}
		draw_target(display.colour, params.set_point_x, params.set_point_y);
		cv::putText(display.colour, "fps: " + std::to_string(1.0 / display.dt), cv::Point(20, 20), cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(0, 255, 0));
		cv::imshow(colour_view, display.colour);
		cv::imshow(binary_view, display.binary);
	
		char keypress = (char)cv::waitKey(1);
		if(keypress == (char)27)
			running = false;
	}
}
