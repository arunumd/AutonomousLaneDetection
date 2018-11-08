# Autonomous Vehicle Lane detection using OpenCV-3 and C++

## Overview

Simple starter C++ project with:

- cmake
- OpenCV
- boost cpp library

## Project purpose

Lane detection is an active area of research in autonomous driving field. The ability of an autonomous vehicle to detect the road and track it continuously is at the heart of autonomous driving. There are several approaches to lane detection like the usage of deep learning, computer vision, machine learning, etc.

Every method has its own advantages and disadvantages. Neural networks are supposed to be the most powerful technique for lane detection. This project makes use of an image segmentation based approach. The most distinguishing feature of lanes on roads are the white or the yellow colored lane markings on the road surface. There isn't much variation in the lane markings on different roads. Hence such lane markings can be considered as reliable approach to lane detection. This project makes use of the computer vision tools provided by the OpenCV library and does image based segmentation for detecting lanes.

Segmentation is the process of isolating colors of interest from the remaining details present in a scene (image). For any segmentation we apply some kind of thresholding operation based on our colors of interest. In this project, we apply thresholds for white and yellow colors in Lab color space. While there are several other color spaces like HSV, HSL, RGB, BGR, etc., Lab color space is quite unique in that it has a separate channel dedicated only for lightness and darkness of an image. The remaining two channels are for the colors of the image. Hence this color space gives an operational ease to the user and gives more control to the user during thresholding. 

**The image shown below is the result obtained from Lab color thresholding operation**
![LabThreshold](http://i63.tinypic.com/ogcuhw.png)

After segmentation of the white and yellow lanes, we obtain a binary image wherein the regions of the image with the lanes become ones and all other regions in the image become zeros. After this step, the process is quite easy. We apply a gradient based edge detector like the Canny edge detector and then we apply Hough transform to obtain Hough lines on top of the edges. 

**A sample screenshot of the canny edge detection for this project is shown below**
![CannyOutput](http://i64.tinypic.com/25gdzd3.png)


**The image shown below is a sample screenshot of HoughLines detection**
![HoughDetection](http://i63.tinypic.com/2qb7tki.png)

Later, we manipulate the slopes of the obtained Hough lines and mark the lanes. We also make announcements like the road conditions ahead (left turn, right turn, etc.).

**The following image is the final result of lane detection**
![Lanedetection](http://i65.tinypic.com/6t20ly.png)

## Dependencies

The project depends on boost::filesystem library for fetching input files from a location. The other dependencies are the OpenCV C++ Library, a C++ 11 compiler and Cmake (Version 2.8 minimum).

Boost Cpp Library can be obtained by following the installation steps provided in this link below :
[Click here to navigate to the official boost cpp libraries page](https://www.boost.org/doc/libs/1_66_0/more/getting_started/unix-variants.html)

OpenCV3 can be installed by following the detailed installation procedure given in this website :
[Click here to navigate to the official opencv installation page](https://www.learnopencv.com/install-opencv3-on-ubuntu/)

CMake build system can be installed by typing this command below in your Ubuntu terminal :
```
sudo apt-get install cmake
```

## Standard install via command-line
```
git clone --recursive https://github.com/arunumd/LaneDetection
cd <path to repository>
mkdir build
cd build
cmake ..
make
Run tests: ./test/cpp-test
Run program: ./app/shell-app
When prompted enter the full path of the input video files (challenge_video.mp4 or project_video.mp4 based on your preference)
```

## Doxygen documentation

If you don't have doxygen already installed on your computer, then please do this install step below :
```
sudo apt-get install doxygen
```

After ensuring doxygen is installed in your computer, you can now create doxygen style documentation using this syntax below:
```
cd <path to repository>
cd doxygen
doxygen doxygenSettings.conf
```

Now you should be able to find the doxygen generated documentation files in ../doxygen/html and ../doxygen/latex folders
