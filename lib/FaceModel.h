#ifndef FACERECOGNITIONMODULE_H
#define FACERECOGNITIONMODULE_H

#include <cvaux.h>

using namespace std;

class FaceModel
{
public:
    FaceModel(int userID) { UserID = userID; };
    virtual void train() { };
    virtual void loadFromXML(char* xml) { };
    virtual char * exportToXML() { };
    virtual double compare(IplImage* image) { };
protected:
    int UserID;
};

#endif // FACERECOGNITIONMODULE_H
