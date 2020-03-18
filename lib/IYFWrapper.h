#ifdef Q_WS_WIN
#define MY_EXPORT __declspec(dllexport)
#else
#define MY_EXPORT
#endif

#ifndef LIBWRAPPER_H
#define LIBWRAPPER_H

enum loginResult
{
       lrSuccess,
       lrFailedLogin,
       lrBadImage
};

enum trainResult
{
       trSuccess,
       trFail
};

// The abstract interface for IYFaceRec object
struct InYourFaceRec
{
    virtual void setFaceModel(void* fm) = 0;
    virtual loginResult Login(char* username, char* password, char* imageFilename) = 0;
    virtual loginResult Login(char* username, char* password, void* IplImage) = 0;
    virtual trainResult Train(char* username, char* password, char** imageFilenames, int nFaces) = 0;
    virtual trainResult Train(char* username, char* password, void** IplImages, int nFaces) = 0;
    virtual double getDistance(char* username, char* imageFilename) = 0;
    virtual int compareFaces(char * faceImgFilename1, char * faceImgFilename2) = 0;
    virtual bool preProcess(char* inputImgFilename, char* outputImgFilename) = 0;
    virtual void* preProcess(void* inputFaceImg) = 0;
};

// Analogous to a constructor but in C style for the lib
extern "C" MY_EXPORT InYourFaceRec* getInYourFaceRec();

#endif // LIBWRAPPER_H
