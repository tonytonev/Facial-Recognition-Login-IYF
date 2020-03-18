#ifndef FDMBASIC_H
#define FDMBASIC_H

#include <cv.h>
#include <iostream>
#include <QImage>
#include "FDMBasic.h"


using namespace std;

class FDMBasic
{
    public:
        FDMBasic(char * frontalFaceParameterFile,char * profileFaceParameterFile, char * leftEyeParameterFile, char * rightEyeParameterFile,char * noseParameterFile,char * mouthParameterFile);
        int detectFaces(IplImage * img, IplImage ** faceArray = NULL, const int max = 1);
        IplImage * convert(IplImage * img);
        bool detectLeftEyes(IplImage * img, IplImage * gray, CvRect * r, double scale = 1);
        bool detectRightEyes(IplImage * img, IplImage * gray, CvRect * r, double scale = 1);
        bool detectNose(IplImage * img, IplImage * gray, CvRect * r, double scale = 1);
        bool detectMouth(IplImage * img, IplImage * gray, CvRect * r, double scale = 1);

        IplImage * enhanceImage(IplImage * img, const CvRect & rect);

    private:
        int dist(int x, int y, int z, int x2, int y2, int z2);
        CvHaarClassifierCascade *cascade_ff;
        CvHaarClassifierCascade *cascade_fp;
        CvHaarClassifierCascade *cascade_le;
        CvHaarClassifierCascade *cascade_re;
        CvHaarClassifierCascade *cascade_n;
        CvHaarClassifierCascade *cascade_m;
        CvMemStorage *storage;
        CvMemStorage *storage2;
        CvMemStorage *storage3;
        int N; // Gaussian Kernel Size
        int lastRIrisX;
        int lastRIrisY;
        int lastRIrisR;
        int lastLIrisX;
        int lastLIrisY;
        int lastLIrisR;
        int bins;
        int param1;
        int param2;
        double cannyThreshold;

        int xSize;
        int ySize;
};

#endif // FDMBASIC_H
