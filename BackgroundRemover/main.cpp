
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/video.hpp>
#include <opencv2/core/core.hpp>
#include <iostream>
#include <string>

using namespace cv;
using namespace std;

Mat removeBackground(Mat frame, Scalar lo)
{
    Mat im_bgr =frame ,im_hsv,input_bgra;

    //Scalar lo( 45, 100, 60, 0); // mean-var for low
    Scalar hi(82, 255, 255, 255); // mean + var for high

    if (im_bgr.empty()) return im_bgr;

    cvtColor(im_bgr, im_hsv, COLOR_BGR2HSV);
    blur(im_hsv, im_hsv, Size(1,1));

    Mat mask;
    inRange(im_hsv, lo, hi, mask);
    im_bgr.setTo(Scalar(255,255,255, 0), mask); // i used yellow, to make it more visible.

    cv::cvtColor(im_bgr, input_bgra, CV_BGR2BGRA);
    for (int y = 0; y < input_bgra.rows; ++y)
    for (int x = 0; x < input_bgra.cols; ++x)
    {
        cv::Vec4b & pixel = input_bgra.at<cv::Vec4b>(y, x);
        // if pixel is white
        if (pixel[0] == 255 && pixel[1] == 255 && pixel[2] == 255) {
            pixel[3] = 0;
        }
    }

    return input_bgra;
}

int main( int argc, char** argv )
{
    if (argc < 2) {
        cout<<"./BackgroundRemover image.jpg|video.mpeg\n"<<endl;
        return -1;
    }

    string file = argv[1];
    string extention = file.substr(file.find_last_of(".") + 1);

    if(extention == "jpg")
    {
        Scalar lo( 43, 64, 60, 0);
        Mat frame, outFrame;

        frame = imread(file);

        if (frame.empty())
        {
            std::cout << "file " << file << " not found or could not be opened" << std::endl;
            return -1;
        }
        outFrame = removeBackground(frame, lo);
        cv::imwrite("out.png", outFrame);
        imshow("Background Remover", outFrame);
        waitKey(0);
    }
    else if (extention == "png")
    {
        Scalar lo( 47, 110, 129, 0);
        Mat frame, outFrame;

        frame = imread(file);

        if (frame.empty())
        {
            std::cout << "file " << file << " not found or could not be opened" << std::endl;
            return -1;
        }
        outFrame = removeBackground(frame, lo);
        cv::imwrite("out.png", outFrame);
        imshow("Background Remover", outFrame);
        waitKey(0);
    }
    else if (extention == "mpeg")
    {
        cv::VideoCapture cap(file); // open a video file
        if(!cap.isOpened())  // check if succeeded
        {
            std::cout << "file " << file << " not found or could not be opened" << std::endl;
            return -1;
        }
        int frame_width=   cap.get(CV_CAP_PROP_FRAME_WIDTH);
        int frame_height=   cap.get(CV_CAP_PROP_FRAME_HEIGHT);
        VideoWriter video("out.avi",CV_FOURCC('M','J','P','G'),10, Size(frame_width,frame_height),true);
        Scalar lo( 50, 116, 114, 0);
        cv::namedWindow("Background Remover");

        unsigned long counter = 0;

        cv::Mat frame, outFrame;
        // read frames until end of video:
        while(cap.read(frame))
        {   // display frame
            outFrame = removeBackground(frame,lo);
            cv::imshow("Background Remover", outFrame); cv::waitKey(25);   // remove this line if you don't need the live output
            video.write(outFrame);
            std::cout << "processing " << counter++ << " frame..." << std::endl;
        }

    }
    else
    {
        cout<<"./BackgroundRemover image.jpg|video.mpeg\n"<<endl;
        return -1;
    }
    std::cout << "done."<< std::endl;
    return 0;
}
