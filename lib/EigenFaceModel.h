#ifndef EIGENFACEMODEL_H
#define EIGENFACEMODEL_H

#include "FaceModel.h"

#include <cvaux.h>
#include <highgui.h>
#include <fstream>
#include <string>

using namespace std;

class EigenFaceModel : public FaceModel
{
public:
    //constructor for training
    EigenFaceModel(int userID, IplImage ** fImgArr, int nFaces);

    //constructor for recognizing
    EigenFaceModel(int userID, char * xml);
    ~EigenFaceModel();

    char * exportToXML();
    double compare(IplImage* image);
    double distanceToNN(IplImage* faceImage);

    //testing purposes
    FILE * EigenLog;

protected:
    int                 UserID;
    IplImage        **  faceImgArr;      //array of images
    CvTermCriteria      calcLimit;
    CvSize              faceImgSize;
    float           *   projectedTestFace;
    //holds the number of trainingImgs
    //is also used as the id of trained user
    int                 trainImgs;
	//holds the trainedModel train person's User ID
	int					modelUserID;

    //needed for cvEigenCalcObject
    int                 nEigens;                //number of eigenvectors
    int                 nTrainFaces;            //number of train faces
    CvMat           *   trainPersonNumMat;      //person numbers from import
    CvMat           *   personNumTruthMat;      //person numbers to export
    CvMat           *   eigenValMat;            //eigenvalues
    CvMat           *   projectedTrainFaceMat;  //projected training faces
    IplImage        *   pAvgTrainImg;
    IplImage        **  eigenVectArr;           //eigenvectors

    void train();
    void loadFromXML(char * xml);
    double confidenceLevel(double distance);
};

#endif // EIGENFACEMODEL_H
