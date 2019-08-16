QT += core
QT -= gui

CONFIG += c++11

TARGET = dlibWin2
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp

win32:INCLUDEPATH += "D:\dlib_build\include"
win32:INCLUDEPATH += "F:\OpenCV_3.4.1_build\include"
win32:LIBS += "D:\dlib_build\lib\dlib19.17.0_debug_64bit_msvc1900.lib"
win32:LIBS += "F:\OpenCV_3.4.1_build\x64\vc14\lib\opencv_world341d.lib"

#unix:INCLUDEPATH += "./dlib_build/include"
#unix:INCLUDEPATH += "/home/Dev/libs/AI/dlib/dlib"
#unix:LIBS += "/home/Dev/libs/AI/dlib/installer/usr/local/lib64/libdlib.a"
unix:LIBS += "/home/Dev/libs/AI/dlib/dlib/libdlib.a"
#unix:LIBS += "./opencv-3.4.7"



LIBS += -L/usr/lib/x86_64-linux-gnu/ -lopencv_calib3d
LIBS += -lopencv_core -lopencv_dnn -lopencv_features2d -lopencv_flann -lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc -lopencv_ml
LIBS += -lopencv_objdetect -lopencv_photo -lopencv_shape -lopencv_stitching -lopencv_superres -lopencv_video
LIBS += -lopencv_videoio -lopencv_videostab

unix:LIBS +=  -lopenblas_pthreads #-lcudnn -ldlib
