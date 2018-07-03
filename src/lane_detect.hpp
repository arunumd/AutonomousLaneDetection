/*
 * lane_detect.hpp
 *
 *  Created on: Jun 12, 2018
 *      Author: arun
 */

#ifndef SRC_LANE_DETECT_HPP_
#define SRC_LANE_DETECT_HPP_

#include <iostream>
#include <fstream>
#include <ostream>
#include <vector>
#include "opencv2/opencv.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

class lanedetect
{
public: ~lanedetect(); //destructor to delete lanedetect


private:
cv::Mat cameraMatrix = cv::Mat::zeros(cv::Size(3, 3), CV_64FC4);// Matrix for Camera Matrix with size 3x3 initialized with zeros
cv::Mat distCoeffs = cv::Mat::zeros(cv::Size(1,8), CV_64FC4);// Matrix (essentially a vector for distortion coefficients (k1, k2, p1, p2, k3, k4, k5, k6) with size 1x8 initialized with zeros
cv::Mat undistortedimage; //Matrix for undistorted image
std::string videolocation; //String for video path from user input
std::string filestring; //Filename obtained from path
};

#endif /* SRC_LANE_DETECT_HPP_ */
