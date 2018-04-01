#include "stdafx.h"
#include "Programs.h"

#include "Utils.h"

#include "opencv2\opencv.hpp"
using namespace cv;

#include <vector>  
#include <iostream> 


#include <boost/filesystem.hpp>
using namespace std;
using namespace boost::filesystem;


#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/iostreams/stream.hpp>
#include <numeric>

#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/calib3d/calib3d.hpp"



int Programs::main_display()
{
	namedWindow("Display window", WINDOW_AUTOSIZE);// Create a window for display.

	std::vector<Utils::Instance> instances = Utils::LoadDataset();

	int _heigth = 50, _width = 25;

	std::for_each(instances.begin(), instances.end(), [_width, _heigth](const Utils::Instance& instance) {
		Mat image = imread(instance.url, CV_LOAD_IMAGE_COLOR);

		Size size(_width, _heigth);//the dst image size,e.g.100x100
		Mat dst;//dst image
		cv::resize(image, dst, size);//resize image


		imshow("Display window", dst);                   // Show our image inside it.
		waitKey(0);
	});

	cvDestroyWindow("Display window");
	system("pause");

	return 0;
}

int Programs::main_detect_rectangle()
{
	namedWindow("main", 0);
	namedWindow("aligned", 1);
	Utils::for_each_captured_images([&](Mat& frame)->bool {
	
		auto res = Utils::find_rectangles(frame);
		std::vector<std::vector<cv::Point>> rectangles = std::get<0>(res);
		vector<Vec4i> hierarchy = std::get<1>(res);
		
		double max_area = -1;
		std::vector<cv::Point>* biggest_rectangle = nullptr;
		std::for_each(rectangles.begin(), rectangles.end(), [&biggest_rectangle, &max_area]( std::vector<cv::Point>& rectangle) {
			double curr_contour_area = cv::contourArea(rectangle);
			if (max_area < curr_contour_area)
			{
				max_area = curr_contour_area;
				biggest_rectangle = &rectangle;
			}
		});

		if (biggest_rectangle)
		{
			std::vector<std::vector<cv::Point>> rectangles_filtered;
			rectangles_filtered.push_back(*biggest_rectangle);
			Scalar color = Scalar(0, 255, 255);
			drawContours(frame, rectangles_filtered, 0, color, 2, 8, hierarchy, 0, Point());
			Mat warped(500, 500, frame.type(), Scalar(0, 0, 0));
			Utils::warp(*biggest_rectangle, frame, warped);
			imshow("aligned", warped);
		}


		
		//GaussianBlur(edges, edges, Size(7, 7), 1.5, 1.5);
		//Canny(edges, edges, 0, 30, 3);
		imshow("main", frame);
		return (waitKey(30) <= 0);
	}
	
	);

	return 0;
}


int Programs::main_iterate_images_folder()
{
	namedWindow("Display window", WINDOW_AUTOSIZE);// Create a window for display.

	std::vector<std::string> instances = Utils::LoadImagesFromFolder(".\\somelabels");

	int _heigth = 500, _width = 500;

	std::for_each(instances.begin(), instances.end(), [_width, _heigth](const std::string& url) {
		Mat image = imread(url, CV_LOAD_IMAGE_COLOR);

		Size size(_width, _heigth);//the dst image size,e.g.100x100
		Mat dst;//dst image
		cv::resize(image, dst, size);//resize image

		Mat gray;
		cv::cvtColor(dst, gray, COLOR_BGR2GRAY);
		//cv::equalizeHist(gray, gray);

		Mat img_bw;
		cv::threshold(gray, img_bw, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);

		imshow("Display window", img_bw);                   // Show our image inside it.
		waitKey(0);
	});

	cvDestroyWindow("Display window");
	system("pause");

	return 0;
}