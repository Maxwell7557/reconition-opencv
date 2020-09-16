#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main()
{
    Mat src = imread("room.jpg", IMREAD_COLOR);
    Mat srcSQ;
    resize(src, srcSQ, Size(), 0.75, 0.75);
    Mat srcDRW = srcSQ.clone();
    Mat srcHSV, srcGRAY, srcBIN;

    cvtColor(srcSQ, srcHSV, COLOR_BGR2HSV);
    cvtColor(srcSQ, srcGRAY, COLOR_BGR2GRAY);
    threshold(srcGRAY,srcBIN,100,255,THRESH_BINARY);

    rectangle(srcDRW, Point(200,130), Point(275,230), Scalar(0,255,0), 3);
    rectangle(srcDRW, Point(100,310), Point(160,380), Scalar(0,0,255), 3);
    circle(srcDRW, Point(420,320), 45, Scalar(0,255,0), 3);

    imshow("Original image", srcSQ);
    imshow("HSV image", srcHSV);
    imshow("Grayscale image", srcGRAY);
    imshow("Binary image", srcBIN);
    imshow("Drawing", srcDRW);

    waitKey(0);
    return 0;
}
