#include <dlib/opencv.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/render_face_detections.h>
#include <dlib/image_processing.h>
#include <dlib/gui_widgets.h>
#include <dlib/image_transforms.h>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/opencv.hpp"
#include <opencv2/video/video.hpp>
#include <iostream>
#include <string>

#define FACE_DOWNSAMPLE_RATIO 2
#define SKIP_FRAMES 2

using namespace dlib;
using namespace std;
using namespace cv;

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

    cvtColor(im_bgr, input_bgra, CV_BGR2BGRA);
    for (int y = 0; y < input_bgra.rows; ++y)
    for (int x = 0; x < input_bgra.cols; ++x)
    {
        Vec4b & pixel = input_bgra.at<Vec4b>(y, x);
        // if pixel is white
        if (pixel[0] == 255 && pixel[1] == 255 && pixel[2] == 255) {
            pixel[3] = 0;
        }
    }

    return input_bgra;
}

void draw_polyline(Mat &img, const dlib::full_object_detection& d, const int start, const int end, Scalar color , bool isClosed = false)
{
    std::vector <Point> points;
    for (int i = start; i <= end; ++i)
    {
        points.push_back(Point(d.part(i).x(), d.part(i).y()));
    }
    polylines(img, points, isClosed, color, 2, 16);

}

Mat render_face (Mat &img, const dlib::full_object_detection& d)
{
    DLIB_CASSERT
    (
     d.num_parts() == 68,
     "\n\t Invalid inputs were given to this function. "
     << "\n\t d.num_parts():  " << d.num_parts()
     );

  //  draw_polyline(img, d, 0, 16, Scalar(255,0,0));           // Jaw line
    draw_polyline(img, d, 17, 21, Scalar(0,0,0));          // Left eyebrow
    draw_polyline(img, d, 22, 26, Scalar(0,0,0));          // Right eyebrow
    draw_polyline(img, d, 27, 30, Scalar(255,0,0));          // Nose bridge
    draw_polyline(img, d, 30, 35, Scalar(255,0,0), true);    // Lower nose
    draw_polyline(img, d, 36, 41, Scalar(255,255,255), true);    // Left eye
    draw_polyline(img, d, 42, 47, Scalar(255,255,255), true);    // Right Eye
    draw_polyline(img, d, 48, 59, Scalar(0,0,255), true);    // Outer lip
    draw_polyline(img, d, 60, 67, Scalar(0,0,255), true);    // Inner lip
    return img;
}

int main( int argc, char** argv )
{
  try
    {
       // VideoCapture cap;
                //cv::VideoCapture cap("G:/_PROJECTS/2_Inprogress/6-2-PhotoBooth/detect-face-parts/1.mpeg");
                cv::VideoCapture cap("./1.mpeg");

//        if (argc < 2) {
//            cap.open(0);
//        }else{
//            cap.open(argv[1]);
//        }

        if (!cap.isOpened())
        {
            cerr << "Unable to connect to camera or video" << endl;
            cerr << "Usage: PhotoBooth [file]" << endl;
            return 1;
        }

        int frame_width=   cap.get(CV_CAP_PROP_FRAME_WIDTH);
        int frame_height=   cap.get(CV_CAP_PROP_FRAME_HEIGHT);
        VideoWriter video("./out.avi",CV_FOURCC('M','J','P','G'),10, Size(frame_width,frame_height),true);

        // Load face detection and pose estimation models.
        frontal_face_detector detector = get_frontal_face_detector();
        shape_predictor pose_model;

        deserialize("./shape_predictor_68_face_landmarks.dat") >> pose_model;
        int count =0;
        Scalar lo( 50, 116, 114, 0);
        // Grab and process frames until the main window is closed by the user.

        while(true)
        {
            // Grab a frame
            Mat temp;
            cap >> temp;
            if (!temp.data) break;

            Mat im_small, greyMat;

            // Resize image for face detection
            resize(temp, im_small, Size(), 1.0/FACE_DOWNSAMPLE_RATIO, 1.0/FACE_DOWNSAMPLE_RATIO);

            cvtColor(im_small, greyMat, COLOR_BGR2GRAY);

            // Change to dlib's image format. No memory is copied.
            cv_image<bgr_pixel> cimg_small(im_small);

            // Turn OpenCV's Mat into something dlib can deal with.  Note that this just
            // wraps the Mat object, it doesn't copy anything.  So cimg is only valid as
            // long as temp is valid.  Also don't do anything to temp that would cause it
            // to reallocate the memory which stores the image as that will make cimg
            // contain dangling pointers.  This basically means you shouldn't modify temp
            // while using cimg.
            cv_image<bgr_pixel> cimg(temp);
            std::vector<dlib::rectangle> faces;
            // Detect faces

            // Detect faces on resize image
            if ( count++ % SKIP_FRAMES == 0 )
            {
                faces = detector(cimg_small,0);

                // Find the pose of each face.
                std::vector<full_object_detection> shapes;
                for (unsigned long i = 0; i < faces.size(); ++i){
                    // Resize obtained rectangle for full resolution image.
                     dlib::rectangle r(
                                   (long)(faces[i].left() * FACE_DOWNSAMPLE_RATIO),
                                   (long)(faces[i].top() * FACE_DOWNSAMPLE_RATIO),
                                   (long)(faces[i].right() * FACE_DOWNSAMPLE_RATIO),
                                   (long)(faces[i].bottom() * FACE_DOWNSAMPLE_RATIO)
                                );
                    shapes.push_back(pose_model(cimg, r));
                    render_face(temp, shapes[i]);
                    temp = render_face(temp, shapes[i]);
                }
                cout<<"shapes:"<<shapes.size()<<endl;
                temp = removeBackground(temp,lo);
                imshow("PhotoBooth",temp);
                video.write(temp);
                int key = waitKey(10);
                if (key == 27) break;
            }
        }
    }
    catch(serialization_error& e)
    {
        cout << "You need dlib's default face landmarking model file to run this example." << endl;
        cout << "You can get it from the following URL: " << endl;
        cout << "   http://dlib.net/files/shape_predictor_68_face_landmarks.dat.bz2" << endl;
        cout << endl << e.what() << endl;
    }
    catch(exception& e)
    {
        cout << e.what() << endl;
    }
}
