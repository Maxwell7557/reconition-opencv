#include <opencv2/opencv.hpp>
#include <iostream>
//#include <string.h>

using namespace std;
using namespace cv;

void change_area_size(Mat frame, Point2f* src_vertices)
{
    circle(frame, src_vertices[0], 4, Scalar(0, 255, 0), FILLED);
    circle(frame, src_vertices[1], 4, Scalar(0, 255, 0), FILLED);
    circle(frame, src_vertices[2], 4, Scalar(0, 255, 0), FILLED);
    circle(frame, src_vertices[3], 4, Scalar(0, 255, 0), FILLED);

    line(frame, src_vertices[0], src_vertices[1], Scalar(0, 255, 0), 2);
    line(frame, src_vertices[1], src_vertices[2], Scalar(0, 255, 0), 2);
    line(frame, src_vertices[2], src_vertices[3], Scalar(0, 255, 0), 2);
    line(frame, src_vertices[3], src_vertices[0], Scalar(0, 255, 0), 2);
}

int main()
{
    VideoCapture video("highway.mp4");
    if(!video.isOpened())
    {
        cout << "Error with openning file" << endl;
        return -1;
    }

    int top = 240;
    int bottom = 590;
    int height = 170;

    int top_max = 600;
    int bottom_max = 600;
    int height_max = 720;

    namedWindow("Original video");
    createTrackbar("Top", "Original video", &top, top_max);
    createTrackbar("Bottom", "Original video", &bottom, bottom_max);
    createTrackbar("Height", "Original video", &height, height_max);

    while(1)
    {
        Mat frame;
        video >> frame;
        if (frame.empty())
            break;

        Point2f src_vertices[4];
        src_vertices[0] = Point(640 - top, height_max - height);
        src_vertices[1] = Point(640 + top, height_max - height);
        src_vertices[2] = Point(690 + bottom, height_max);
        src_vertices[3] = Point(690 - bottom, height_max);

        Point2f trn_vertices[4];
        trn_vertices[0] = Point(0, 0);
        trn_vertices[1] = Point(640, 0);
        trn_vertices[2] = Point(640, 480);
        trn_vertices[3] = Point(0, 480);

        Mat tmp = getPerspectiveTransform(src_vertices, trn_vertices);
        Mat transformed(480, 640, CV_8UC3);
        warpPerspective(frame, transformed, tmp, transformed.size());
        change_area_size(frame,src_vertices);

        char fps[20];
        char time[20];
        sprintf(fps, "FPS: %.0f", video.get(CAP_PROP_FPS));
        putText(frame, fps, Point(20, 30), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 255), 2);
        sprintf(time, "TIME: %.0f ms", video.get(CAP_PROP_POS_MSEC));
        putText(frame, time, Point(20, 60), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 255), 2);

        imshow("Original video", frame);
        imshow("TRNSFRM", transformed);
        char c=(char)waitKey(25);
        if(c==27)
            break;
    }

    video.release();
    destroyAllWindows();
    return 0;
}
