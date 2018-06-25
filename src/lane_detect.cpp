/*
 * lane_detect.cpp
 *
 *  Created on: May 24, 2018
 *      Author: arun
 */

#include <iostream>
#include <ostream>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <string>
#include <vector>
#include "lane_detect.hpp"
#include "opencv2/opencv.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;
using namespace cv;
using namespace std;
// Ref: https://stackoverflow.com/questions/3024197/what-does-int-argc-char-argv-mean
int main(int argc, char *argv[]){
   /**************************************************************************
	*                                                                        *
	* Defining the Camera Parameters and Distortion coefficients obtained    *
	* from the given text file                                               *
	*                                                                        *
	**************************************************************************/
    string videolocation;
    string filestring;
    cv::Mat cleanimage;
	// Hard-coding the Camera Parameters below
	cv::Mat cameraParameters = (cv::Mat_<double>(3,3) << 1.15422732e+03, 0.00000000e+00, 6.71627794e+02, 0.00000000e+00, 1.14818221e+03, 3.86046312e+02, 0.00000000e+00, 0.00000000e+00, 1.00000000e+00);
	// Hard-coding the distortion coefficients below
	cv::Mat distCoeffs = (cv::Mat_<double>(1,8) << -2.42565104e-01, -4.77893070e-02, -1.31388084e-03, -8.79107779e-05, 2.20573263e-02, 0, 0, 0);

	/**************************************************************************
	 *                                                                        *
	 * Code for opening video file based on full path of file from user input.*
	 * This code relies on boost::filesystem from the boost library for C++.  *
	 * More information about boost library can be found on this website :    *
	 * https://www.boost.org/doc/libs/1_67_0/libs/filesystem/doc/index.htm    *
	 *                                                                        *
	 **************************************************************************/
	//Lines 45 to 71 are only for user input of video filepath and file verification
	if (argc < 2){
		std::cout<<"You have not given the input video filepath in command-line arguments\nPlease provide the video filepath below:\n";
		std::getline (std::cin,videolocation);//User input prompt for file path
		fs::path p(videolocation);//Boost filesystem object for path
		filestring = p.string();
		while(!is_regular_file(p)){ //Boost filesystem function for checking if there is a real file in the mentioned path
			std::cout<<"You have not given a valid input video filepath in command-line arguments\nPlease provide the video filepath below:\n";
			std::getline (std::cin,videolocation);//User input prompt for file path
			fs::path p(videolocation);//Boost filesystem object for path
			filestring = p.string();
		}
	}
	else if (argc = 2){
		fs::path p(argv[1]);
		filestring = p.string();
		while(!is_regular_file(p)){ //Boost filesystem function for checking if there is a real file in the mentioned path
			std::cout<<"You have not given a valid input video filepath in command-line arguments\nPlease provide the video filepath below:\n";
			std::getline (std::cin,videolocation);//User input prompt for file path
			fs::path p(videolocation);//Boost filesystem object for path
			filestring = p.string();
		}
	}
	else{
		std::cout<<"The input file path cannot contain any empty spaces\nPlease enter input filepath in the following manner:";
		std::cout<<"Linux and Windows formats go here";
		std::getline (std::cin,videolocation);//User input prompt for file path
		fs::path p(videolocation);//Boost filesystem object for path
		filestring = p.string();
		while(!is_regular_file(p)){ //Boost filesystem function for checking if there is a real file in the mentioned path
			std::cout<<"You have not given a valid input video filepath in command-line arguments\nPlease provide the video filepath below:\n";
			std::getline (std::cin,videolocation);//User input prompt for file path
			fs::path p(videolocation);//Boost filesystem object for path
			filestring = p.string();
		}
	}

	//string filestring = videolocation.string();
	//Moving on to frame extraction from input video file which was obtained from path above
	VideoCapture videofile(filestring);
	std::cout<<filestring;

	if(!videofile.isOpened()){
	    cout << "Error opening input video file" << endl;
	    return -1;
	  }

	while(1){

	    Mat frame;
	    // Capture frame-by-frame
	    videofile >> frame;

	    // If the frame is empty, break immediately
	    if (frame.empty())
	      break;

	    //Undistortion of video frames
	    void undistort(cv::Mat frame, cv::Mat cleanimage, cv::Mat cameraParameters, cv::Mat distCoeffs);

	    // Display the resulting frame
	    imshow( "Frame", cleanimage );

	    // Press  ESC on keyboard to exit
	    char c=(char)waitKey(25);
	    if(c==27)
	      break;
	  }

	// When everything done, release the video capture object
	videofile.release();

	// Closes all the frames
	destroyAllWindows();

	return 0;
}

