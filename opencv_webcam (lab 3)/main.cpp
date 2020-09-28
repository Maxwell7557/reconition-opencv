#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

Mat get_cenny_frame(Mat original_frame)
{
    Mat gray_frame, blured_frame, canny_frame;
    cvtColor(original_frame, gray_frame, COLOR_BGR2GRAY);
    GaussianBlur(gray_frame, blured_frame, cv::Size(5, 5), 1.5);
    Canny(blured_frame, canny_frame, 25, 100);
    return canny_frame;
}

Mat get_yellow_frame(Mat original_frame)
{
    Mat yellow_frame, hsv_frame;
    cvtColor(original_frame, hsv_frame, COLOR_BGR2HSV);
    inRange(hsv_frame, Scalar(20, 50, 50), Scalar(30, 255, 255), yellow_frame);
//    inRange(hsv_frame, Scalar(25, 20, 20), Scalar(32, 255, 255), yellow_frame);
    return yellow_frame;
}

int main()
{
    VideoCapture cap;
    if(!cap.open(0))
        return 0;

    for(;;)
    {
          Mat frame;
          Mat blur_frame;
          Mat yellow_frame;
          Mat canny_frame;

          cap >> frame;
          blur(frame, blur_frame, Size(7,7));
          canny_frame = get_cenny_frame(frame);
          yellow_frame = get_yellow_frame(frame);

          if( frame.empty() ) break;
          imshow("Original webcam image", frame);
          imshow("Blured webcam image", blur_frame);
          imshow("Canny webcam image", canny_frame);
          imshow("InRanged webcam image", yellow_frame);

          if( waitKey(10) == 27 ) break; // stop capturing by pressing ESC
    }

    return 0;
}
