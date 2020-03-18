#include "QOpenCVCamera.h"

QOpenCVCamera::QOpenCVCamera(CvCapture * c)
{
    cam = c;
}

IplImage * QOpenCVCamera::getImage()
{
    IplImage *img = cvQueryFrame(cam);
//  IplImage * img2 = cvCreateImage( cvSize(img->width, img->height ), IPL_DEPTH_8U, 3);
//  cvSmooth(img, img2);
//  cvReleaseImage(&img);
    return img;
}

QImage QOpenCVCamera::toQImage(IplImage * img)
{
    int camIndex, camStart;
    QImage retImage(img->width, img->height, QImage::Format_RGB32);


    if(img->depth == IPL_DEPTH_8U)
    {
        if(img->nChannels == 3)
        {
            camIndex = 0;
            camStart = 0;
            for (int y = 0; y < img->height; y++)
            {
                unsigned char red,green,blue;
                camIndex = camStart;
                for (int x = 0; x < img->width; x++)
                {
                    red = img->imageData[camIndex+2];
                    green = img->imageData[camIndex+1];
                    blue = img->imageData[camIndex];

                    retImage.setPixel(x,y,qRgb(red, green, blue));
                    camIndex += 3;
                }
                camStart += img->widthStep;
            }
            retImage = retImage.mirrored(true,false);
        }
        else if(img->nChannels == 1)
        {
            camIndex = 0;
            camStart = 0;
            for (int y = 0; y < img->height; y++)
            {
                unsigned char red,green,blue;
                camIndex = camStart;
                for (int x = 0; x < img->width; x++)
                {
                    red = img->imageData[camIndex+2];
                    green = img->imageData[camIndex+1];
                    blue = img->imageData[camIndex];

                    retImage.setPixel(x,y,qRgb(red, green, blue));
                    camIndex += 1;
                }
                camStart += img->widthStep;
            }
            retImage = retImage.mirrored(true,false);
        }
    }
    else
        cout << "This type of IplImage is not implemented in QOpenCVWidget\n";

    return retImage;
}
