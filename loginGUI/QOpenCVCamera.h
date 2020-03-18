#ifndef QOPENCVCAMERA_H
#define QOPENCVCAMERA_H

#include <cv.h>
#include <highgui.h>
#include <iostream>
#include <QImage>

using namespace std;

class QOpenCVCamera
{
    public:
        QOpenCVCamera(CvCapture *c);
        IplImage * getImage();
        static QImage toQImage(IplImage *);

    private:
        CvCapture *cam;
};

#endif // QOPENCVCAMERAWIDGET_H
