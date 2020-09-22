#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

Mat src, srcSQ, srcPRC;
Mat srcGRY;
Mat srcBLR;

void api_blur()
{
    GaussianBlur(srcSQ, srcBLR, Size(3, 3), 0);
    imshow( "Gaussian blur image", srcBLR );
}

Mat mat_mul(Mat first, Mat second)
{
    Mat result(3,3,first.type());
    for (int i=0; i < first.cols; i++)
    {
        for (int j=0; j < first.cols; j++)
        {
            if (first.type() == 16)
                result.at<Vec3b>(i,j) = first.at<Vec3b>(i,j) * second.at<float>(i,j);
            else if (first.type() == 0)
                result.at<uchar>(i,j) = first.at<uchar>(i,j) * second.at<float>(i,j);
        }
    }
    return result;
}

void own_blur()
{
    float data[9] = {1,2,1,2,4,2,1,2,1};
    vector<Vec3b> to_mat;
    Mat gauss_kernel = Mat(3,3,CV_32F,data);
    Mat blured_image;
    gauss_kernel = gauss_kernel/16;

    for (int i =1; i < srcSQ.rows-1; i++)
    {
        for (int j =1; j<srcSQ.cols-1; j++)
        {
            Rect roi(j-1,i-1,3,3);
            Mat sub_mat = srcSQ(roi).clone();
            Mat tt = mat_mul(sub_mat,gauss_kernel);
            Vec3b tmp(sum(tt)[0], sum(tt)[1], sum(tt)[2]);
            to_mat.push_back(tmp);
        }
    }

    blured_image = Mat(srcSQ.rows-2,srcSQ.cols-2,srcSQ.type(),&to_mat[0]);
    imshow( "Own blur image", blured_image );
}

void own_gradient()
{
    float data[9] = {-1,0,1,-2,0,2,-1,0,1};
    vector<uchar> to_mat;
    Mat grad = Mat(3,3,CV_32F,data);
    Mat grad_image;
    grad = grad/9;

    for (int i =1; i < srcGRY.rows-1; i++)
    {
        for (int j =1; j<srcGRY.cols-1; j++)
        {
            Rect roi(j-1,i-1,3,3);
            Mat sub_mat = srcGRY(roi).clone();
            Mat tt = mat_mul(sub_mat,grad);
            uchar tmp = sum(tt)[0];
            to_mat.push_back(tmp);
        }
    }

    grad_image = Mat(srcGRY.rows-2,srcGRY.cols-2,srcGRY.type(),&to_mat[0]);
    imshow( "Own grad (west) image", grad_image );
}

void add_borders(Mat &matrix)
{
    Mat rows = Mat::zeros(1,matrix.cols, matrix.type());
    vconcat(rows, matrix, matrix);
    vconcat(matrix, rows, matrix);
    Mat cols = Mat::zeros(matrix.rows,1,matrix.type());
    hconcat(cols, matrix, matrix);
    hconcat(matrix, cols, matrix);
}

int main()
{
    src = imread("room.jpg", IMREAD_COLOR);
    resize(src, srcSQ, Size(), 0.75, 0.75);
    cvtColor(srcSQ, srcGRY, COLOR_BGR2GRAY);

    imshow( "Original image", srcSQ );
    add_borders(srcSQ);
    add_borders(srcGRY);

    api_blur();
    own_blur();
    own_gradient();

    waitKey(0);
    return 0;
}
