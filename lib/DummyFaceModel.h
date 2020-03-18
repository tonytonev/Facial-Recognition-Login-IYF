#ifndef DUMMYFACEMODEL_H
#define DUMMYFACEMODEL_H

#include <string>
#include "FaceModel.h"

class DummyFaceModel : public FaceModel
{
public:
    DummyFaceModel(int userID);

    void train(IplImage** faceImages, int num);
    void loadFromXML(string xml);
    string exportToXML();
    double compare(IplImage* image);
protected:
    int UserID;
};

#endif // DUMMYFACEMODEL_H
