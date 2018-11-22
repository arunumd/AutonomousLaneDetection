/************************************************************************************************
* @file      : main file for Autonomous vehicle lane detection using OpenCV and C++
* @author    : Arun Kumar Devarajulu
* @brief     : The following lines of code iterate sequentially
*              between image frames in input video file and
*              perform the following lane detection pipeline :
*                 1.) Undistort the input image frames;
*                 2.) Smoothen the undistorted image frames;
*                 3.) Threshold the smoothened image frames with LAB color space;
*                 4.) Detect edges corresponding to road lanes;
*                 5.) Draw Hough Lines on the basis of the detected edges; and
*                 6.) Mark lanes based on the Hough Lines.
* @date      : October 8, 2018
* @copyright : 2018, Arun Kumar Devarajulu
* @license   : MIT License
*
*              Permission is hereby granted, free of charge, to any person obtaining a copy
*              of this software and associated documentation files (the "Software"), to deal
*              in the Software without restriction, including without limitation the rights
*              to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*              copies of the Software, and to permit persons to whom the Software is
*              furnished to do so, subject to the following conditions:
*
*              The above copyright notice and this permission notice shall be included in all
*              copies or substantial portions of the Software.
*
*              THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*              IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*              FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*              AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*              LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*              OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
*              SOFTWARE.
*************************************************************************************************/
#include <tuple>
#include <vector>
#include <string>
#include <utility>
#include <cstdlib>
#include <cmath>
#include "opencv2/core.hpp"
#include "opencv2/opencv.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/features2d.hpp"
#include "opencv2/xfeatures2d.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include "opencv2/imgproc/imgproc_c.h"
#include "opencv2/calib3d.hpp"
#include "opencv2/imgcodecs.hpp"
#include "Files.hpp"
#include "Cleaner.hpp"
#include "Thresholder.hpp"
#include "LanesMarker.hpp"
#include "RegionMaker.hpp"

namespace FS = boost::filesystem;    //! Short form for boost filesystem

int main(int argc, char *argv[]) {
    cv::Point p;
    std::vector<cv::Point> historicLane;
    cv::Point dummy;
    dummy.x = 0;
    dummy.y = 0;
    double oldSlopeLeft = 0;
    double oldSlopeRight = 0;
    int counter = 1;
    for (int i = 0; i < 4; i++) {
        historicLane.push_back(dummy);
    }
    //  Hardcoding certain parameters like screen area to search for, etc.
    std::vector<cv::Point> roiPoints;   // <First fillConvexPoly points
    roiPoints.push_back(cv::Point(527, 491));
    roiPoints.push_back(cv::Point(812, 491));
    roiPoints.push_back(cv::Point(1163, 704));
    roiPoints.push_back(cv::Point(281, 704));


    //  Dummy variable for temporary points storage in HoughLines
    std::pair <cv::Point2d, cv::Point2d> vertices;

    // Variable stores rho, theta from cv::HoughLines()
    std::vector<cv::Vec2f> lines;

    //  Initialize the Files class as an object
    Files location;

    std::string fileAddress;    //  <String variable for holding file name

    /****************************************************************
    *
    *  @Brief: The following lines of code perform sanity checks
    *          for valid filename containing valid input file
    *
    ****************************************************************/

    if (argc < 2) {
        std::cout << "Please enter directory location in command prompt\n";
        std::getline(std::cin, fileAddress);
        fileAddress = location.filePicker(fileAddress);
    } else if (argc == 2) {
        fileAddress = argv[1];
        fileAddress = location.filePicker(fileAddress);
    } else {
        std::cout << "The file path cannot contain empty spaces\n"
                  "please enter valid path without spaces.";
        std::getline(std::cin, fileAddress);
        fileAddress = location.filePicker(fileAddress);
    }

    cv::VideoCapture videofile(fileAddress);

    if (!videofile.isOpened()) {
        std::cout << "Error opening input video file" << std::endl;
        return -1;
    }

    int videoWidth = videofile.get(CV_CAP_PROP_FRAME_WIDTH);
    int videoHeight = videofile.get(CV_CAP_PROP_FRAME_HEIGHT);

    // Here we create a video writing object to write our output
    cv::VideoWriter video("../results/LanesDetection.avi",
                          CV_FOURCC('M', 'J', 'P', 'G'), 10,
                          cv::Size(videoWidth, videoHeight));

    while (1) {
        lines.clear();   // Emptying the container from previous iteration
        cv::Mat frame;

        videofile >> frame;  //  <Grab the image frame

        if (frame.empty())
            break;
        /*****************************************************************
        *
        *  To begin with, we grab the image frames and do pre-processing
        *
        ******************************************************************/

        Cleaner imgClean((cv::Mat_<double>(3, 3) << 1.15422732e+03, \
                          0.00000000e+00, 6.71627794e+02, 0.00000000e+00, \
                          1.14818221e+03, 3.86046312e+02, 0.00000000e+00, \
                          0.00000000e+00, 1.00000000e+00),  \
                         (cv::Mat_<double>(1, 8) << -2.42565104e-01, \
                          -4.77893070e-02, -1.31388084e-03, \
                          -8.79107779e-05, 2.20573263e-02, 0, 0, 0));

        imgClean.imgUndistort(frame);
        cv::Mat blurImg;
        blurImg = imgClean.imgSmoothen();

        /***************************************************************
        *
        *    After pre-processing we mask the white and yellow lanes
        *
        ****************************************************************/

        Thresholder lanethresh(cv::Scalar(137, 125, 125), \
                               cv::Scalar(255, 133, 137), \
                               cv::Scalar(135, 127, 146), \
                               cv::Scalar(223, 144, 212));

        cv::Mat labOutput;
        labOutput = lanethresh.convertToLab(blurImg);

        cv::Mat whiteOutput;
        whiteOutput = lanethresh.whiteMaskFunc();
        // Included 13 layers of white colors
        lanethresh.whiteStacker((233,  125,  129), (255,  130,  137));
        lanethresh.whiteStacker((196,  125,  128), (254,  131,  138));
        lanethresh.whiteStacker((221,  126,  131), (255,  129,  134));
        lanethresh.whiteStacker((236,  126,  134), (253,  131,  137));
        lanethresh.whiteStacker((213,  129,  130), (251,  133,  131));
        lanethresh.whiteStacker((206,  127,  126), (254,  132,  137));
        lanethresh.whiteStacker((240,  125,  135), (254,  131,  137));
        lanethresh.whiteStacker((137,  129,  125), (233,  132,  132));
        lanethresh.whiteStacker((221,  128,  127), (254,  130,  130));
        lanethresh.whiteStacker((196,  126,  128), (230,  129,  131));
        lanethresh.whiteStacker((202,  128,  127), (223,  130,  131));
        lanethresh.whiteStacker((233,  126,  128), (254,  129,  130));
        lanethresh.whiteStacker((202,  127,  130), (230,  128,  130));

        cv::Mat yellowOutput;
        yellowOutput = lanethresh.yellowMaskFunc();
        // Include 10 layers of yellow colors
        lanethresh.yellowStacker((203,  127,  178), (223,  136,  212));
        lanethresh.yellowStacker((190,  128,  184), (223,  137,  212));
        lanethresh.yellowStacker((190,  131,  184), (201,  143,  205));
        lanethresh.yellowStacker((160,  131,  176), (203,  143,  205));
        lanethresh.yellowStacker((192,  130,  180), (209,  144,  211));
        lanethresh.yellowStacker((141,  129,  169), (222,  143,  210));
        lanethresh.yellowStacker((147,  126,  159), (179,  140,  185));
        lanethresh.yellowStacker((139,  127,  148), (174,  136,  180));
        lanethresh.yellowStacker((135,  124,  150), (213,  136,  180));
        lanethresh.yellowStacker((140,  126,  146), (180,  137,  178));

        cv::Mat lanesMask;
        lanesMask = lanethresh.combineLanes();
        cv::imshow("Lanes Mask", lanesMask);

        /****************************************************************
        *
        *  After masking the lanes we get rid of the unnecessary details
        *  like horizon, trees, and other details on the sides of the
        *  roads which can likely interfere with proper detection of lanes
        *
        *****************************************************************/

        cv::Mat firstPolygonArea(lanesMask.rows, lanesMask.cols, \
                                 CV_8U, cv::Scalar(0));
        cv::Mat interestLanes = cv::Mat::zeros(lanesMask.size(), CV_8U);
        cv::fillConvexPoly(firstPolygonArea, roiPoints, cv::Scalar(1));
        lanesMask.copyTo(interestLanes, firstPolygonArea);

        /*****************************************************************
        *
        *   Later we employ a gradient based edge detector to detect
        *   sharp edges which will be our lanes
        *
        ******************************************************************/

        cv::Mat edges = cv::Mat::zeros(lanesMask.size(), CV_8U);
        cv::Canny(interestLanes, edges, 15, 45, 3);
        imshow("Canny Output", edges);

        /******************************************************************
        *
        *  Later we strengthen the detected edges by drawinng Hough Lines
        *  on top of their loci
        *
        *******************************************************************/

        cv::HoughLines(edges, lines, 1, CV_PI / 180, 15, 0, 0);
        LanesMarker lanesConsole;
        lanesConsole.lanesSegregator(lines);
        auto left = lanesConsole.leftLanesAverage();
        auto right = lanesConsole.rightLanesAverage();
        cv::Mat black_img = cv::Mat::zeros(labOutput.size(), \
                                           labOutput.type());
        cv::line(black_img, left.first, left.second, cv::Scalar(0, 0, 255), \
                 3, cv::LINE_AA);
        cv::line(black_img, right.first, right.second, cv::Scalar(0, 0, 255), \
                 3, cv::LINE_AA);
        imshow("Hough Output", lanesMask);

        /*********************************************************************
        *
        *  Later we draw polygonal region on the road which denotes a region
        *  within the bounds of two lanes in front of the vehicle
        *
        *********************************************************************/

        cv::Mat polygonLayer = cv::Mat::zeros(labOutput.size(), \
                                              labOutput.type());
        cv::Mat linesCanny = polygonLayer.clone();
        black_img.copyTo(polygonLayer, firstPolygonArea);
        cv::Canny(polygonLayer, linesCanny, 70, 210, 3);
        cv::Mat binaryRegions;
        cv::findNonZero(linesCanny, binaryRegions);

        RegionMaker polyMaker;
        auto polyRegionVertices = polyMaker.getPolygonVertices(binaryRegions);
        cv::Mat dummy = cv::Mat::zeros(labOutput.size(), labOutput.type());

        /***
        *@brief  : Initially we analyse the current slope value. If the slopes
        *          are zero then we are in first frame. Hence we assign old slope
        *          as current slope. Whereas, when the difference of old and current
        *          slopes are greater than 0.5 we assign current slope as old slope.
        *          This is because successive lanes can have only marginal changes of
        *          slopes. Very large changes are impossible in reality !
        *****/

        auto currentSlopeLeft = static_cast<float>(polyRegionVertices.at(0).y - \
                                polyRegionVertices.at(3).y) /
                                static_cast<float>(polyRegionVertices.at(0).x - \
                                        polyRegionVertices.at(3).x);

        auto currentSlopeRight = static_cast<float>(polyRegionVertices.at(1).y - \
                                 polyRegionVertices.at(2).y) /
                                 static_cast<float>(polyRegionVertices.at(1).x - \
                                         polyRegionVertices.at(2).x);

        if (oldSlopeLeft == 0 && oldSlopeRight == 0) {
            oldSlopeLeft = currentSlopeLeft;
            oldSlopeRight = currentSlopeRight;
        } else if (std::abs(oldSlopeLeft - currentSlopeLeft) > 0.5) {
            currentSlopeLeft = oldSlopeLeft;
        } else if (std::abs(oldSlopeRight - currentSlopeRight) > 0.5) {
            currentSlopeRight = oldSlopeRight;
        } else {}

        int index = 0;
        for (auto& vertex : polyRegionVertices) {
            if (vertex.y == 0 || vertex.x == 0) {
                polyRegionVertices[index].x = historicLane[index].x;
                polyRegionVertices[index].y = historicLane[index].y;
            } else {}
            index++;
        }

        if (counter > 1) {
            for (int i = 0; i < 4; i++) {
                if (historicLane.at(i).y == 0 && historicLane.at(i).x == 0) {
                    historicLane.at(i).x = polyRegionVertices.at(i).x;
                    historicLane.at(i).y = polyRegionVertices.at(i).y;
                }
                if ((std::abs(polyRegionVertices.at(i).x - \
                              historicLane.at(i).x) > 15) && (((std::abs(oldSlopeLeft \
                                      - currentSlopeLeft) > 0.3) || (std::abs(oldSlopeRight - \
                                              currentSlopeRight) > 0.3)))) {
                    polyRegionVertices.at(i).x = historicLane.at(i).x;
                    polyRegionVertices.at(i).y = historicLane.at(i).y;
                }
            }
        }

        historicLane = polyRegionVertices;

        /*********************************************************************
        *
        *  Now we extrapolate our polygon to fill a desired area on screen
        *
        *********************************************************************/
/*
        auto newSlopeLeft = static_cast<float>(polyRegionVertices.at(0).y - \
                                               polyRegionVertices.at(3).y) /
                            static_cast<float>(polyRegionVertices.at(0).x - \
                                               polyRegionVertices.at(3).x);

        auto newSlopeRight = static_cast<float>(polyRegionVertices.at(1).y - \
                                                polyRegionVertices.at(2).y) /
                             static_cast<float>(polyRegionVertices.at(1).x - \
                                                polyRegionVertices.at(2).x);*/

        auto leftIntercept = static_cast<float>(polyRegionVertices.at(0).y) - \
                             static_cast<float>(currentSlopeLeft) * \
                             static_cast<float>(polyRegionVertices.at(0).x);

        auto rightIntercept = static_cast<float>(polyRegionVertices.at(1).y) - \
                              (static_cast<float>(currentSlopeRight) * \
                               static_cast<float>(polyRegionVertices.at(1).x));

        polyRegionVertices.at(0).x = static_cast<double>((550 - \
                                     leftIntercept) / currentSlopeLeft);
        polyRegionVertices.at(0).y = 550.0;
        polyRegionVertices.at(1).x = static_cast<double>((550 - \
                                     rightIntercept) / currentSlopeRight);
        polyRegionVertices.at(1).y = 550.0;

        cv::fillConvexPoly(frame, polyRegionVertices, \
                           cv::Scalar(0, 255, 0), CV_AA, 0);

        double deviationLeft = std::abs(std::abs(currentSlopeLeft) - 1);

        double deviationRight = std::abs(std::abs(currentSlopeRight) - 1);

        double diffInDeviation = std::abs(deviationLeft - deviationRight);

        /********************************************************************
        *
        *                   At the end we make turn predictions
        *
        *********************************************************************/

        if ((deviationRight > deviationLeft) && diffInDeviation > 0.01) {
            cv::putText(frame, "Left turn ahead", cv::Point(30, 30),
                        cv::FONT_HERSHEY_COMPLEX_SMALL, 0.8, \
                        cv::Scalar(200, 200, 250), 1, CV_AA);
            /*std::cout << "\n\nLeft slope is : " << newSlopeLeft << \
                      " and right slope is : " << newSlopeRight << std::endl;
            std::cout << "\nDeviation left is : " << deviationLeft << \
                      " deviation right is : " << deviationRight << \
                      " difference in deviation is : " << diffInDeviation << " and the "
                      " turn signal is : Left turn ahead" << std:: endl;*/
        } else if (deviationRight < deviationLeft && diffInDeviation > 0.01) {
            cv::putText(frame, "Right turn ahead", cv::Point(30, 30),
                        cv::FONT_HERSHEY_COMPLEX_SMALL, 0.8, \
                        cv::Scalar(200, 200, 250), 1, CV_AA);
            /*std::cout << "\n\nLeft slope is : " << newSlopeLeft << \
                      " and right slope is : " << newSlopeRight << std::endl;
            std::cout << "\nDeviation left is : " << deviationLeft << \
                      " deviation right is : " << deviationRight << \
                      " difference in deviation is : " << diffInDeviation << " and the "
                      " turn signal is : Right turn ahead" << std:: endl;*/
        } else {}

        video.write(frame);

        cv::imshow("Final Lane Detection", frame);

        counter++;

        char c = static_cast<char> (cv::waitKey(20));
        if (c == 27)
            break;
        ///home/arun/Downloads/challenge_video.mp4
    }
    video.release();
    videofile.release();

    cv::destroyAllWindows();

    return 0;
}
