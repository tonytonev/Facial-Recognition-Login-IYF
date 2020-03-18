#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <cv.h>
#include <highgui.h>

#include "Model.h"
#include "FaceModel.h"
#include "EigenFaceModel.h"
#include "FDMBasic.h"
#include "IYFWrapper.h"

class Controller : public InYourFaceRec
{
public:
    Controller();
    ~Controller();
    loginResult Login(char* username, char* password, IplImage* image);
    trainResult Train(char* user, char* pass, IplImage ** faceImgArr, int nFaces);

    // Library API methods
    void setFaceModel(void* fm);
    loginResult Login(char* username, char* password, char* imageFilename);
    loginResult Login(char* username, char* password, void* faceImg);
    trainResult Train(char* username, char* password, char** imageFilenames, int nFaces);
    trainResult Train(char* username, char* password, void** IplImages, int nFaces);
    double getDistance(char* username, char* imgFilename);
    int compareFaces(char * faceImgFilename1, char * faceImgFilename2);
    IplImage* preProcess(IplImage* inputImgFilename);
    bool preProcess(char* inputImgFilename, char* outputImgFilename);
    void* preProcess(void* inputFaceImg);

private:
    bool hasModel();

    Model* model;
    FDMBasic * fdm;
    FILE * controllerLog;

    double confidenceThreshold;
};

#endif // CONTROLLER_H
