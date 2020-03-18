#include "DummyFaceModel.h"


DummyFaceModel::DummyFaceModel(int userID) : FaceModel(userID)
{

}

/*
    Preconditions: Takes a array of images of the user's face.
    Postconditions: The face model is trained on the input images.
*/
void DummyFaceModel::train(IplImage** faceImages, int num)
{

}

/*
    Preconditions: Takes a XML std::string representation of the face model.
    Postconditions: The face model will be trained.
*/
void DummyFaceModel::loadFromXML(std::string xml)
{

}

/*
    Preconditions: The face model must be trained.
    Postconditions: Returns an XML std::string representation of the face model.
*/
std::string DummyFaceModel::exportToXML()
{
    return std::string("<DummyFaceModel/>");
}

/*
    Preconditions: Takes an image to compare to the current model.
    Postconditions: Returns a confidence level (probability) between 0 and 1 that the image is of the same user as this face model was trained on.
                    If the image bad (e.g. no face detected, face obscured), return -1.
*/
double DummyFaceModel::compare(IplImage* image)
{
    return 1.0;
}
