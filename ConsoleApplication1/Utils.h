#pragma once

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

#include <vector> 
#include <functional>
#include <iostream>

namespace Utils
{

	struct Instance{
		std::string url;
		double label;
	};

	std::vector<Instance> LoadDatasetFromFolder(std::string path_dir);

	std::vector<Instance> LoadDataset();

	void for_each_captured_images(std::function<bool(cv::Mat& mat)> afunc);

	std::tuple<std::vector<std::vector<cv::Point>>, std::vector<cv::Vec4i> > find_rectangles(cv::Mat& frame);

	void warp(std::vector<cv::Point>& rectangle, cv::Mat& src, cv::Mat& dst);

	std::vector<std::string> LoadImagesFromFolder(std::string path_dir);


}

