#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

vector<double> approximation(vector<Point2f> points)
{
    const int size = points.size();
    double x[size];
    double y[size];
    for (int i = 0; i < size; ++i)
    {
        x[i] = points[i].y;
        y[i] = points[i].x;
    }

    int i, j, k;
    long double sum, tmp, tmp_val;
    for (i = 0; i < size; i++)
        for (int j = i; j >= 1; j--)
          if (x[j] < x[j - 1])
          {
              tmp = x[j - 1];
              x[j - 1] = x[j];
              x[j] = tmp;
              tmp = y[j - 1];
              y[j - 1] = y[j];
              y[j] = tmp;
          }

    int cnt = 2;
    double arrs[size][size], fr[size];
    for (i = 0; i < cnt + 1; i++)
        for (j = 0; j < cnt + 1; j++)
        {
            arrs[i][j] = 0;
            for (k = 0; k < size; k++)
                arrs[i][j] += pow(x[k], i + j);
        }

    for (i = 0; i < cnt + 1; i++)
    {
        fr[i] = 0;
        for (k = 0; k < size; k++)
            fr[i] += pow(x[k], i) * y[k];
    }

    for (k = 0; k < cnt + 1; k++)
        for (i = k + 1; i < cnt + 1; i++)
        {
            tmp_val = arrs[i][k] / arrs[k][k];
            for (j = k; j < cnt + 1; j++)
                arrs[i][j] -= tmp_val * arrs[k][j];
            fr[i] -= tmp_val * fr[k];
        }

    vector<double> coeff(size);
    for (i = cnt; i >= 0; i--)
    {
        sum = 0;
        for (j = i; j < cnt + 1; j++)
            sum += arrs[i][j] * coeff[j];
        coeff[i] = (fr[i] - sum) / arrs[i][i];
    }

    return coeff;
}

void draw_trapeze(Mat frame, Point2f* src_vertices)
{
    line(frame, src_vertices[0], src_vertices[1], Scalar(0, 255, 0), 1);
    line(frame, src_vertices[1], src_vertices[2], Scalar(0, 255, 0), 1);
    line(frame, src_vertices[2], src_vertices[3], Scalar(0, 255, 0), 1);
    line(frame, src_vertices[3], src_vertices[0], Scalar(0, 255, 0), 1);
}

void show_fps_time(Mat frame, VideoCapture video)
{
    char fps[20];
    char time[20];
    sprintf(fps, "FPS: %.0f", video.get(CAP_PROP_FPS));
    putText(frame, fps, Point(20, 30), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255), 1);
    sprintf(time, "Time (ms): %.0f", video.get(CAP_PROP_POS_MSEC));
    putText(frame, time, Point(20, 60), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255), 1);
}

void recognition_of_marking(Mat frame, vector<vector<Point2f>> &points)
{
    Rect rect(0, 0, 40, 40);
    vector<Point2f> re_zero, POI;
    for (rect.y = 0; rect.y + 10 < frame.rows; rect.y += rect.height) {
        for (rect.x = 0; rect.x + 10 < frame.cols; rect.x += rect.width) {
            findNonZero(frame(rect), POI);
            if (!POI.empty()) {
                for (int i = 0; i < POI.size(); i++)
                    POI[i].x += rect.x;
                re_zero.insert(re_zero.end(), POI.begin(), POI.end());
            } else if (!re_zero.empty()) {
                float x_tmp = re_zero.front().x + re_zero.back().x;
                float y_tmp = re_zero.front().y + re_zero.back().y;
                rect.x < frame.cols / 2 ? points[0].push_back(Point(x_tmp/2, rect.y + y_tmp / 2)) : points[1].push_back(Point(x_tmp/2, rect.y + y_tmp / 2)) ;
                re_zero.clear();
            }
        }
        if (!re_zero.empty())
        {
            float x_tmp = re_zero.front().x + re_zero.back().x;
            float y_tmp = re_zero.front().y + re_zero.back().y;
            points[1].push_back(Point(x_tmp/2,  rect.y + y_tmp/2));
            re_zero.clear();
        }
    }
}

void draw_marks(Mat frame_with_marking, vector<Point2f> points)
{
    if (points.size() > 0)
        for(int i = 0; i < points.size(); i++)
        {
            line(frame_with_marking,Point(points[i].x -2, points[i].y -2), Point(points[i].x +2, points[i].y +2), Scalar(0, 0, 200), 2);
            line(frame_with_marking,Point(points[i].x +2, points[i].y -2), Point(points[i].x -2, points[i].y +2), Scalar(0, 0, 200), 2);
        }
}

void get_mask(Mat &bin)
{
    Mat hsv, white, yellow;
    cvtColor(bin, hsv, COLOR_BGR2HSV);
    inRange(hsv, Scalar(0, 0, 230), Scalar(255, 255, 255), white);
    inRange(hsv, Scalar(20, 100, 100), Scalar(30, 255, 255), yellow);
    bitwise_or(white, yellow, bin);
}

void split(vector<vector<Point2f> > points, vector<Point> &left_border, vector<Point> &right_border)
{
    vector<double> poly_left = approximation(points[0]);
    vector<double> poly_right = approximation(points[1]);

    for (auto y = points[0][0].y; y < points[0].back().y; y += 0.1)
    {
        double x = 0;
        for (unsigned i = 0; i < poly_left.size(); ++i)
            x += poly_left[i] * pow(y, i);
        left_border.push_back(Point(x, y));

        x = 0;
        for (unsigned i = 0; i < poly_right.size(); ++i)
          x += poly_right[i] * pow(y, i);
        right_border.push_back(Point(x, y));
    }
}

void draw(vector<vector<Point2f> > points, Mat tmp_inv, Mat end_frame, Mat frame_with_marking)
{
    if(points[0].size() > 0 && points[1].size() > 0)
    {
        perspectiveTransform(points[0], points[0], tmp_inv);
        perspectiveTransform(points[1], points[1], tmp_inv);

        vector<Point> left_border, right_border;
        split(points, left_border, right_border);

        vector<vector<Point> > poly_points;
        poly_points.push_back(left_border);
        poly_points[0].insert(poly_points[0].end(), right_border.rbegin(), right_border.rend());
        if(poly_points[0].size() > 0 && poly_points[1].size() > 0)
            fillPoly(end_frame, poly_points, Scalar(50, 200, 50));

        draw_marks(frame_with_marking, points[0]);
        draw_marks(frame_with_marking, points[1]);
    }
}

int main()
{
    VideoCapture video("highway-1.mp4");
    if(!video.isOpened())
    {
        cout << "Error with openning file" << endl;
        return -1;
    }

    int top = 240;
    int bottom = 590;
    int height = 170;

    namedWindow("Source");
    createTrackbar("Top", "Source", &top, 600);
    createTrackbar("Bottom", "Source", &bottom, 600);
    createTrackbar("Height", "Source", &height, 700);

    while(1)
    {
        Mat frame;
        video >> frame;
        if (frame.empty())
            break;

        Mat frame_with_marking, end_frame;
        frame.copyTo(frame_with_marking);
        frame.copyTo(end_frame);

        Point2f src_vertices[4];
        src_vertices[0] = Point(640 - top, 700 - height);
        src_vertices[1] = Point(640 + top, 700 - height);
        src_vertices[2] = Point(690 + bottom, 700);
        src_vertices[3] = Point(690 - bottom, 700);

        Point2f trn_vertices[4];
        trn_vertices[0] = Point(0, 0);
        trn_vertices[1] = Point(400, 0);
        trn_vertices[2] = Point(400, 400);
        trn_vertices[3] = Point(0, 400);

        Mat tmp = getPerspectiveTransform(src_vertices, trn_vertices);
        Mat transformed(400, 400, CV_8UC3);
        warpPerspective(frame, transformed, tmp, transformed.size());
        draw_trapeze(frame,src_vertices);

        Mat tmp_inv;
        Mat frame_binary = transformed.clone();
        vector<vector<Point2f> > points(2);
        get_mask(frame_binary);
        recognition_of_marking(frame_binary, points);

        tmp_inv = getPerspectiveTransform(trn_vertices, src_vertices);
        draw(points, tmp_inv, end_frame, frame_with_marking);

        show_fps_time(frame, video);
        show_fps_time(frame_with_marking, video);
        show_fps_time(end_frame, video);

        imshow("Source", frame);
        imshow("BirdEye", transformed);
        imshow("Binary Image", frame_binary);
        imshow("Road marking recognition", frame_with_marking);
        imshow("Polygon", end_frame);

        char c=(char)waitKey(25);
        if(c==27)
            break;
    }

    video.release();
    destroyAllWindows();
    return 0;
}
