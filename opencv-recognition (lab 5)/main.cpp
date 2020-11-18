#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

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

void show_fps_time(Mat frame, VideoCapture video)
{
    char fps[20];
    char time[20];
    sprintf(fps, "FPS: %.0f", video.get(CAP_PROP_FPS));
    putText(frame, fps, Point(20, 30), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 255), 2);
    sprintf(time, "TIME: %.0f ms", video.get(CAP_PROP_POS_MSEC));
    putText(frame, time, Point(20, 60), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 255), 2);
}

vector<Point2f> recognition_of_marking(Mat frame)
{
    Rect rect(0, 0, 10, 10);
    vector<Point2f> not_zero, points, interest;

    for (rect.y = 0; rect.y < frame.rows; rect.y += rect.height)
    {
        for (rect.x = 0; rect.x < frame.cols; rect.x += rect.width)
        {
            findNonZero(frame(rect), interest);
            if (!interest.empty())
            {
                for (int i = 0; i < interest.size(); i++)
                {
                    interest[i].x += rect.x;
                }
                not_zero.insert(not_zero.end(), interest.begin(), interest.end());

            } else if (!not_zero.empty())
            {
                points.push_back(Point((not_zero.front().x + not_zero.back().x) / 2, rect.y + (not_zero.front().y + not_zero.back().y) / 2));
                not_zero.clear();
            }
        }
    }
    return points;
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

        Mat frame_with_marking;
        frame.copyTo(frame_with_marking);

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

        Mat frame_binary;
        vector<Point2f> points;
        cvtColor(transformed, frame_binary, COLOR_BGR2GRAY);
        blur(frame_binary, frame_binary, Size(9, 9));
        threshold(frame_binary, frame_binary, 180, 255, THRESH_BINARY);
        points = recognition_of_marking(frame_binary);

        Mat tmp_inv = getPerspectiveTransform(trn_vertices, src_vertices);
        perspectiveTransform(points, points, tmp_inv);

        for(int i = 0; i < points.size(); i++) {
            circle(frame_with_marking, points[i], 4, Scalar(200, 70, 20), FILLED);
        }

        show_fps_time(frame, video);
        show_fps_time(frame_with_marking, video);

        imshow("Original video", frame);
        imshow("TRNSFRM", transformed);
        imshow("Binary", frame_binary);
        imshow("Road marking recognition", frame_with_marking);
        char c=(char)waitKey(25);
        if(c==27)
            break;
    }

    video.release();
    destroyAllWindows();
    return 0;
}
