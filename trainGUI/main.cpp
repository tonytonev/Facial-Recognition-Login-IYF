#define SQLITEMODEL_PATH "../data/db/IYF-db.sqlite"
#define IYF_LIB_PATH "../lib/InYourFaceRec"

#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include <string.h>

//OpenCV files
#include <cvaux.h> //CalcEigenObject, cvEigenDecomposite
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

//QT
#include <QApplication>
#include <QWidget>
#include <QLibrary>

//Our Program
#include "QOpenCVCamera.h"
#include "IYFMainWindow.h"
#include "FDMBasic.h"
#include "IYFWrapper.h"
#include "SqliteModel.h"


typedef InYourFaceRec* (*getIYFRFuncType)();

int main(int argc, char **argv)
{
    // Load IFY Rec library
    QLibrary IYFRecLib(IYF_LIB_PATH);
    getIYFRFuncType getInYourFaceRec = (getIYFRFuncType) IYFRecLib.resolve("getInYourFaceRec");

    // Make sure the library was loaded
    if (!getInYourFaceRec)
    {
        fprintf(stderr, "Could not load library InYourFaceRec.\n");
        return -1;
    }

    fprintf(stdout, "Library InYourFaceRec successfully loaded.\n");

    // Using OpenCV to get the camera, and make sure that we can get an image from it
    CvCapture * camera = cvCreateCameraCapture(0);
    assert(camera);
    IplImage * image=cvQueryFrame(camera);
    assert(image);

    // Initialize facial recognition object
    InYourFaceRec* frm = getInYourFaceRec();
    SqliteModel * m = new SqliteModel(SQLITEMODEL_PATH);
    frm->setFaceModel(m);

    // Create the GUI
    QApplication app(argc, argv);
    QOpenCVCamera * mainCam = new QOpenCVCamera(camera);

    //passing nTrain faces to test whether the file is actually being read
    IYFMainWindow * mainWin = new IYFMainWindow(mainCam, frm);

    mainWin->setWindowTitle("InYourFace Train");
    mainWin->show();

    int retval = app.exec();
    cvReleaseCapture(&camera);

    return retval;
}
