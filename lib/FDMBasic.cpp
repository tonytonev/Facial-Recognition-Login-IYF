#include "FDMBasic.h"

FDMBasic::FDMBasic(char * frontalFaceParameterFile,char * profileFaceParameterFile, char * leftEyeParameterFile, char * rightEyeParameterFile, char * noseParameterFile,char * mouthParameterFile)
{
    /* load the face classifier */
    cascade_ff = (CvHaarClassifierCascade*)cvLoad(frontalFaceParameterFile, 0, 0, 0);
    cascade_fp = (CvHaarClassifierCascade*)cvLoad(profileFaceParameterFile, 0, 0, 0);

    /* load the eye classifier */
    cascade_le = (CvHaarClassifierCascade*)cvLoad(leftEyeParameterFile, 0, 0, 0);
    cascade_re = (CvHaarClassifierCascade*)cvLoad(rightEyeParameterFile, 0, 0, 0);
    cascade_n = (CvHaarClassifierCascade*)cvLoad(noseParameterFile, 0, 0, 0);
    cascade_m = (CvHaarClassifierCascade*)cvLoad(mouthParameterFile, 0, 0, 0);

    /* setup memory storage, needed by the object detector */
    storage = cvCreateMemStorage(0);
    storage2 = cvCreateMemStorage(0);
    storage3 = cvCreateMemStorage(0);

    assert(cascade_ff);
    assert(cascade_fp);
    assert(cascade_le);
    assert(cascade_re);
    assert(cascade_n);
    assert(cascade_m);
    //assert(storage);
    N = 5;
    cannyThreshold = 5;
    lastRIrisX = 0;
    lastRIrisY = 0;
    lastRIrisR = 0;
    lastLIrisX = 0;
    lastLIrisY = 0;
    lastLIrisR = 0;

    xSize = 92;
    ySize = 112;

    bins = 3;
    param1 = 100;
    param2 = 10;
}

/*
    Preconditions: Input image must be BGR
    Postconditions: Loads faceArray with up to max cropped face images.
*/
int FDMBasic::detectFaces(IplImage * img, IplImage ** faceArray, const int max)
{
    IplImage * gray = convert(img);

    //cvCopy(gray,gray2);
    //cvCanny(gray, out, cannyThreshold*N*N, .5*cannyThreshold*N*N, N);
    //img = out;
    //return img;

    //enhanceImage(gray2);
    //img = gray;

    //return gray2;

    CvSeq *faces = cvHaarDetectObjects(
            gray, cascade_ff, storage,
            1.1, 3, CV_HAAR_DO_CANNY_PRUNING, cvSize(img->width/5, img->height/5));

    if (faces->total == 0)
    {
        cvReleaseImage(&gray);
        //delete img;
        //img = gray;
        return 0;
    }

    int numFaces = 0;

    /* draw a rectangle */
    for(int i =0; i < faces->total; i++)
    {
        CvRect *r = (CvRect*)cvGetSeqElem(faces, i);
        /* reset buffer for the next object detection */

        double scale = r->height/10;
        if(detectLeftEyes(img, gray, r, scale) && detectRightEyes(img, gray, r, scale) && detectNose(img, gray, r, scale) && detectMouth(img, gray, r, scale))
        {
            cvRectangle(img,
                        cvPoint(r->x, r->y),
                        cvPoint(r->x + r->width, r->y + r->height),
                        CV_RGB(0, 255, 0), 1, 8, 0);

            int newHeight = r->height*(float)ySize/xSize;
            int newY = r->y - (newHeight - r->height)/2;

            cvSetImageROI(gray, cvRect(r->x, newY, r->width, newHeight));
            IplImage * img2 = cvCreateImage(cvSize(xSize,ySize),IPL_DEPTH_8U, 1);
            cvResize(gray,img2);
            cvResetImageROI(gray);

            if(faceArray != NULL && numFaces < max)
            {
                faceArray[numFaces] = cvCreateImage(cvSize(xSize,ySize),IPL_DEPTH_8U, 1);
                cvCopy(img2,faceArray[numFaces]);
            }

            //Tile Grey Pictures
            int camIndex;
            int camStart = (img->height-ySize)*img->widthStep + numFaces*3*xSize;
            for (int y = 0; y < ySize; y++)
            {
                camIndex = camStart;
                for (int x = 0; x < xSize; x++)
                {
                    img->imageData[camIndex+0] = img2->imageData[y*img2->widthStep+x];
                    img->imageData[camIndex+1] = img2->imageData[y*img2->widthStep+x];
                    img->imageData[camIndex+2] = img2->imageData[y*img2->widthStep+x];
                    camIndex += 3;
                }
                camStart += img->widthStep;
            }
            cvReleaseImage(&img2);
            numFaces++;
        }
    }
    cvClearMemStorage(storage);

    //return gray;
    cvReleaseImage(&gray);
    return numFaces;
}

IplImage * FDMBasic::convert(IplImage * img)
{
    IplImage * gray = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 1);
    if(img->nChannels == 1)
        cvCopy(img,gray);
    else
        cvCvtColor(img, gray, CV_BGR2GRAY);
    cvEqualizeHist(gray,gray);
    cvSmooth(gray,gray, CV_GAUSSIAN, N,N);
    return gray;
}

bool FDMBasic::detectLeftEyes(IplImage * img, IplImage * gray, CvRect * r, double scale)
{
    int rX = r->x;
    int rY = r->y+(r->height*.2);
    cvSetImageROI(gray, cvRect(rX, rY, r->width*.6, r->height*.3));

    /* detect eyes */
    CvSeq* eyes = cvHaarDetectObjects(
            gray, cascade_le, storage2,
            1.15, 3, CV_HAAR_DO_CANNY_PRUNING, cvSize(1.5*scale,1*scale));

    if(eyes->total == 0)
    {
        cvResetImageROI(gray);
        cvClearMemStorage(storage2);
        return false;
    }

    /* draw a rectangle for each eye found */
    for(int i = 0; i < eyes->total; i++)
    {
        CvRect *r2 = (CvRect*)cvGetSeqElem(eyes, i);
        cvResetImageROI(gray);
/*        int rX2 = r2->x + r2->width * .2;
        int rY2 = r2->y + r2->height * .2;
        CvRect eyeRect = cvRect(rX + rX2 , rY + rY2 , r2->width*.6, r2->height*.6);
        enhanceImage(gray, eyeRect);
        cvSetImageROI(gray, eyeRect);

        CvSeq * iris = cvHoughCircles(gray, storage3, CV_HOUGH_GRADIENT,2, bins, param1, param2,0,r2->height*.3);
        if(iris->total > 0)
        {
            int minDist = 32768;
            int newX = 0;
            int newY = 0;
            int newR = 0;

            //int j = 0;
            for(int j = 0; j < iris->total; j++)
            {
                float *  p = (float *)cvGetSeqElem(iris,j);
                int temp;
                if((temp = dist(newX,newY,newR,lastLIrisX,lastLIrisY,lastLIrisR) < minDist))
                {
                    newX = p[0];
                    newY = p[1];
                    newR = p[2];
                    minDist = temp;
                }
            }
            cvCircle(img, cvPoint(cvRound(rX + rX2 + newX),cvRound(rY + rY2 + newY)),3, CV_RGB(255,255,255), -1, 8, 0 );
            cvCircle(img, cvPoint(cvRound(rX + rX2 + newX),cvRound(rY + rY2 + newY)), newR, CV_RGB(255,255,255), 3, 8, 0 );
            lastLIrisX += newX;
            lastLIrisY += newY;
            lastLIrisY += newR;
            lastLIrisX /= 2;
            lastLIrisY /= 2;
            lastLIrisY /= 2;*/
            cvRectangle(img,
                        cvPoint(rX+r2->x, rY+r2->y),
                        cvPoint(rX+r2->x + r2->width, rY+r2->y + r2->height),
                        CV_RGB(0, 0, 255), 1, 8, 0);

        /*}
        cvResetImageROI(gray);
        cvClearMemStorage(storage3);*/
    }
    cvClearMemStorage(storage2);
    return true;
}

bool FDMBasic::detectRightEyes(IplImage * img, IplImage * gray, CvRect * r, double scale)
{
    int rX = r->x+(r->width*.4);
    int rY = r->y+(r->height*.2);
    cvSetImageROI(gray, cvRect(rX, rY, r->width*.6, r->height*.4));

    /* detect eyes */
    CvSeq* eyes = cvHaarDetectObjects(
            gray, cascade_re, storage2,
            1.15, 3, CV_HAAR_DO_CANNY_PRUNING, cvSize(1.5*scale,1*scale));

    if(eyes->total == 0)
    {
        cvResetImageROI(gray);
        cvClearMemStorage(storage2);
        return false;
    }

    /* draw a rectangle for each eye found */
    for(int i = 0; i < eyes->total; i++)
    {
        CvRect *r2 = (CvRect*)cvGetSeqElem( eyes, i);
        cvResetImageROI(gray);
        /*int rX2 = r2->x + r2->width * .2;
        int rY2 = r2->y + r2->height * .2;
        CvRect eyeRect = cvRect(rX + rX2 , rY + rY2 , r2->width*.6, r2->height*.6);
        enhanceImage(gray, eyeRect);
        cvSetImageROI(gray, eyeRect);

        CvSeq * iris = cvHoughCircles(gray, storage3, CV_HOUGH_GRADIENT,2, bins, param1, param2,0,r2->height*.3);
        if(iris->total > 0)
        {
            int minDist = 32768;
            int newX = 0;
            int newY = 0;
            int newR = 0;

            //int j = 0;
            for(int j = 0; j < iris->total; j++)
            {
                float *  p = (float *)cvGetSeqElem(iris,j);
                int temp;
                if((temp = dist(newX,newY,newR,lastRIrisX,lastRIrisY,lastRIrisR) < minDist))
                {
                    newX = p[0];
                    newY = p[1];
                    newR = p[2];
                    minDist = temp;
                }
            }
            cvCircle(img, cvPoint(cvRound(rX + rX2 + newX),cvRound(rY + rY2 + newY)),3, CV_RGB(255,255,255), -1, 8, 0 );
            cvCircle(img, cvPoint(cvRound(rX + rX2 + newX),cvRound(rY + rY2 + newY)), newR, CV_RGB(255,255,255), 3, 8, 0 );
            lastRIrisX += newX;
            lastRIrisY += newY;
            lastRIrisR += newR;
            lastRIrisX /= 2;
            lastRIrisY /= 2;
            lastRIrisR /= 2;*/
            cvRectangle(img,
                        cvPoint(rX+r2->x, rY+r2->y),
                        cvPoint(rX+r2->x + r2->width, rY+r2->y + r2->height),
                        CV_RGB(0, 0, 255), 1, 8, 0);
        /*}

        cvResetImageROI(gray);
        cvClearMemStorage(storage3);*/
    }
    cvClearMemStorage(storage2);
    return true;
}

bool FDMBasic::detectNose(IplImage * img, IplImage * gray, CvRect * r, double scale)
{
    int rX = r->x+(r->width*.2);
    int rY = r->y+(r->height*.4);
    cvSetImageROI(gray, cvRect(rX, rY, r->width*.6, r->height*.4));
    //cvSetImageROI(img, cvRect(r->x+(r->width*.2), r->y + (r->height*.4), r->width*.6, r->height*.4));

    /* detect nose */
    CvSeq* nose = cvHaarDetectObjects(
            gray, cascade_n, storage2,
            1.15, 4, CV_HAAR_DO_CANNY_PRUNING, cvSize(2/3*scale, 1*scale));

    if(nose->total == 0)
    {
        cvResetImageROI(gray);
        //cvResetImageROI(img);
        cvClearMemStorage(storage2);
        return false;
    }
     /* draw a rectangle for each eye found */
    for(int i = 0; i < nose->total; i++ )
    {
        CvRect *r2 = (CvRect*)cvGetSeqElem(nose, i);
        cvRectangle(img,
                    cvPoint(rX + r2->x, rY + r2->y),
                    cvPoint(rX + r2->x + r2->width, rY + r2->y + r2->height),
                    CV_RGB(0, 255, 255), 1, 8, 0);
    }

    cvResetImageROI(gray);
    //cvResetImageROI(img);
    cvClearMemStorage(storage2);
    return true;
}

bool FDMBasic::detectMouth(IplImage * img, IplImage * gray, CvRect * r, double scale)
{
    int rX = r->x+(r->width*.2);
    int rY = r->y+(r->height*.7);
    cvSetImageROI(gray, cvRect(rX, rY, r->width*.6, r->height*.3));
    //cvSetImageROI(img, cvRect(r->x+(r->width*.2), r->y + (r->height*.7), r->width*.6, r->height*.3));

    /* detect eyes */
    CvSeq* mouth = cvHaarDetectObjects(
            gray, cascade_m, storage2,
            1.15, 4, CV_HAAR_DO_CANNY_PRUNING, cvSize(2/3*scale, 1*scale));

    if(mouth->total == 0)
    {
        cvResetImageROI(gray);
        //cvResetImageROI(img);
        cvClearMemStorage(storage2);
        return false;
    }

    for(int i = 0; i < mouth->total; i++ )
    {
        /* draw a rectangle for each mouth found */
        CvRect *r2 = (CvRect*)cvGetSeqElem(mouth, i);
        cvRectangle(img,
                    cvPoint(rX+r2->x, rY+r2->y),
                    cvPoint(rX+r2->x + r2->width, rY+r2->y + r2->height),
                    CV_RGB(255, 0, 0), 1, 8, 0);
    }

    cvResetImageROI(gray);
    //cvResetImageROI(img);
    cvClearMemStorage(storage2);
    return true;
}

int FDMBasic::dist(int x, int y, int z, int x2, int y2, int z2)
{
    return (x-x2)*(x-x2)+(y-y2)*(y-y2)+(z-z2)*(z-z2);
}

IplImage * FDMBasic::enhanceImage(IplImage * img, const CvRect & rect)
{
    cvRectangle(img,
                cvPoint(rect.x,rect.y),
                cvPoint(rect.x+rect.width, rect.y + rect.height),
                CV_RGB(255, 255, 255), 1, 8, 0);
    int camIndex;
    int camStart = rect.y*img->widthStep;
    for (int y = rect.y; y < rect.y+rect.height; y++)
    {
        double val;
        camIndex = camStart+rect.x;
        for (int x = rect.x; x < rect.x+rect.width; x++)
        {
            val = img->imageData[camIndex];
            val = val * 1.5;
            if(val >255)
                img->imageData[camIndex] = 255;
            else if(val <0)
                img->imageData[camIndex] = 255;
            else
                img->imageData[camIndex] = ((int)val);
            camIndex += 1;
        }
        camStart += img->widthStep;
    }
    return img;
}
