#include "stdafx.h"
#include "Utils.h"

#include "opencv2\opencv.hpp"
using namespace cv;
using namespace std;

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



std::vector<Utils::Instance> Utils::LoadDatasetFromFolder(std::string path_dir)
{
	boost::filesystem::path full_path(boost::filesystem::current_path());

	std::vector<Utils::Instance> res;

	path p(path_dir);
	std::vector<directory_entry> v; // To save the file names in a vector.
	copy(directory_iterator(p), directory_iterator(), back_inserter(v));
	for (std::vector<directory_entry>::const_iterator it = v.begin(); it != v.end(); ++it)
	{
		path p2((*it).path().string());
		std::vector<directory_entry> v2; // To save the file names in a vector.
		copy(directory_iterator(p2), directory_iterator(), back_inserter(v2));
		for (std::vector<directory_entry>::const_iterator it2 = v2.begin(); it2 != v2.end(); ++it2)
		{
			

			std::cout << full_path.string() << (*it2).path().string().substr(1) << endl;
			//std::vector<string> partial = getSourceText((*it).path().string());
			//res.insert(res.end(), partial.begin(), partial.end());*/
			Utils::Instance curr;
			curr.url = full_path.string() + (*it2).path().string().substr(1);			
			path dd(curr.url);
			std::string name = (*it2).path().string().substr(path_dir.length()+1);
			name = name.substr(0, name.find("_"));
			curr.label = std::stof(name);

			res.push_back(curr);
		}
	}


	return res;
}


std::vector<std::string> Utils::LoadImagesFromFolder(std::string path_dir)
{
	boost::filesystem::path full_path(boost::filesystem::current_path());

	std::vector<std::string> res;

	path p(path_dir);
	std::vector<directory_entry> v; // To save the file names in a vector.
	copy(directory_iterator(p), directory_iterator(), back_inserter(v));
	for (std::vector<directory_entry>::const_iterator it = v.begin(); it != v.end(); ++it)
	{
			std::cout << full_path.string() << (*it).path().string().substr(1) << endl;
			res.push_back(full_path.string() + (*it).path().string().substr(1));
	}
	return res;
}



std::vector<Utils::Instance> Utils::LoadDataset()
{
	return LoadDatasetFromFolder(".\\trainingimages");
}

void Utils::for_each_captured_images(std::function<bool(cv::Mat& mat)> afunc)
{
	VideoCapture cap(0); // open the default camera
	if (!cap.isOpened())  // check if we succeeded
		return ;

	cv::Mat edges;
	
	cv::Mat frame;
	do
	{
		cap >> frame; // get a new frame from camera		
		
	} while (afunc(frame));
}


std::tuple<std::vector<std::vector<cv::Point>>, vector<cv::Vec4i> >  Utils::find_rectangles(cv::Mat & frame)
{

	double MORPH = 7;
	double	CANNY = 250;
	std::vector<std::vector<cv::Point>>   res;

	Mat gray;
	cv::cvtColor(frame, gray, COLOR_BGR2GRAY);
	Mat gray2;
	cv::bilateralFilter(gray, gray2, 1, 10, 120);
	Mat edges;
	cv::Canny(gray2, edges, 10, CANNY);
cv:Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Point(MORPH, MORPH));
	cv::Mat closed;
	cv::morphologyEx(edges, closed, cv::MORPH_CLOSE, kernel);
	vector<vector<Point> > contours;
	vector<Vec4i> h;
	cv::findContours(closed, contours, h, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

	std::for_each(contours.begin(), contours.end(), [&](const vector<Point>& contour) {
		//if (cv::contourArea(contour) > 5000)
		{
			double arc_len = cv::arcLength(contour, true);
			vector<Point> approx;
			cv::approxPolyDP(contour, approx, 0.1 * arc_len, true);
			if (approx.size() == 4)
			{
				res.push_back(approx);
				/*pts_src = np.array(approx, np.float32)
				h, status = cv2.findHomography(pts_src, pts_dst)
				out = cv2.warpPerspective(rgb, h, (int(_width + _margin * 2), int(_height + _margin * 2)))
				cv2.drawContours(rgb, [approx], -1, (255, 0, 0), 2)*/

				//h, status = cv::findHomography(pts_src, pts_dst)
			}
		}
	});



	return  std::make_tuple(res,h);
}


struct st1 {
	cv::Point2f point;
	double angle;
};

/*std::vector<cv::Point> clockwise(std::vector<cv::Point>& src)
{
	std::vector<st1> temp_buff;
	cv::Point2f mean(0,0);
	std::for_each(src.begin(), src.end(), [&mean, &temp_buff](const cv::Point& pt) {
		mean.x += pt.x;
		mean.y += pt.y;
	});

	std::for_each(src.begin(), src.end(), [&mean, &temp_buff](const cv::Point& pt) {
		double dx = pt.x - mean.x;
		double dy = pt.y - mean.y;
		double atan = (dx == 0 ? 1000000 : (dy/dx));
		temp_buff.push_back({ pt, 0.0 });
	});

	if (src.size() > 0)
	{
		mean.x /= src.size();
		mean.y /= src.size();
	}

	
}*/

void Utils::warp(std::vector<cv::Point>& rectangle, cv::Mat& src, cv::Mat& dst)
{
	std::vector<cv::Point2f> border = {cv::Point2f(0,0),
		cv::Point2f(0, dst.rows-1),
		cv::Point2f(dst.cols-1, dst.rows-1), 
		cv::Point2f(dst.cols-1, 0)};

	std::vector<cv::Point2f> orig;
	std::for_each(rectangle.begin(), rectangle.end(), [&](const cv::Point& pt) {
		orig.push_back(cv::Point2f(pt.x, pt.y));
	});

	/*std::vector<cv::Point2f> border = { cv::Point2f(0,0),
		cv::Point2f(0, 1),
		cv::Point2f(10, 10),
		cv::Point2f(10, 0) };*/


	cv::Mat warpMatrix = cv::getPerspectiveTransform(orig, border);
	cv::warpPerspective(src, dst, warpMatrix, dst.size());
}