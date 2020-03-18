#include "EigenFaceModel.h"

//initializes all the variables that will be used for training
//essentially all the parameters to cvCalcEigenObjects
EigenFaceModel::EigenFaceModel(int userID, IplImage ** fImgArr, int nFaces ) : FaceModel(userID)
{
    char * trainingDir = "../data/TrainSet/";

    EigenLog = stdout; //fopen("OutputLogs/EigenLog.txt", "w");
    UserID = userID;

    fprintf(EigenLog, "Inside train constructor\n");

    /***** RETRIEVING THE TRAINING IMAGES *****/
    ifstream aDir;

    char trainingSet[strlen(trainingDir)];
    strcpy(trainingSet,trainingDir);
    strcat(trainingSet,"set.txt");
    aDir.open(trainingSet);
    fprintf(EigenLog, "Opening trainset directory%s\n",trainingSet);
    if(aDir.fail())
        fprintf(EigenLog, "Failed opening trainset directory\n");

    trainImgs = 0;
    while(!aDir.eof())
    {
        char dummy[64];
        aDir.getline(dummy,64);
        trainImgs++; //number of training images
    }
    aDir.close();

    //Creates a list of all image filenames in the directory selected

    aDir.open(trainingSet);
    char** images = new char*[trainImgs];
    for(int i =0; i < trainImgs; i++)
    {
        images[i] = new char[64];
        aDir.getline(images[i],64);
        fprintf(EigenLog, "%s\n",images[i]);
    }
    aDir.close();

    nTrainFaces = trainImgs + nFaces;

    //accounting for all the faces in training set, and sent to this constructor
    personNumTruthMat =  cvCreateMat(1, nTrainFaces, CV_32SC1); //matrix to hold id's

    fprintf(EigenLog, "How many total images to be trained %d\n", nTrainFaces);

    //allocating space in the faceImgArr for all trainfaces
    faceImgArr  =(IplImage **)cvAlloc(nTrainFaces * sizeof(IplImage *) );

    fprintf(EigenLog, "How many anonymouse images are there: %d\n", trainImgs);

    //fill faceImgArr with all the training set images first
    for (int iFace = 0; iFace < trainImgs; ++iFace)
    {
       char * anImage = images[iFace]; //image filename
       char imgName[strlen(trainingDir)]; //absolute path
       strcpy(imgName, trainingDir);
       strcat(imgName, anImage);

       fprintf(EigenLog, "Image path %d: %s\n", iFace, imgName);

       //load face image into faceImgArr
       faceImgArr[iFace] = cvLoadImage(imgName, CV_LOAD_IMAGE_GRAYSCALE);

       //throw error if any image could not be loaded
       if(!faceImgArr[iFace])
       {
           fprintf(EigenLog, "Could not load image file: %d\n", iFace);
           return;
       }
       //for every image added to arry, add to truth matrix
           //all anonymouse faces get integers from 0 to -n
       personNumTruthMat->data.i[iFace] = -iFace;
       delete [] anImage;
    }
    delete [] images;

    /***** TrainSet images are loaded *****/

    /***** Adding passed in images *****/
    //fill the rest of faceImgArr with the passed in faces
    //start at the index where left off
    for(int i = 0; i < nFaces; i++)
    {
        faceImgArr[i+trainImgs] = fImgArr[i];
        fprintf(EigenLog, "Image path added to faceImgArr %d of %d\n", i, nFaces);

        //for every image added to arry, add to truth matrix
        //each of these faces will be of the same train image
        //keep the id number the same
        personNumTruthMat->data.i[i+trainImgs] = UserID;
    }
    fprintf(EigenLog, "Image Uploads Done\n");

    nEigens = nTrainFaces - 1;

    //allocate space for the images and eigenVector
    faceImgSize.width = faceImgArr[0]->width;
    faceImgSize.height = faceImgArr[0]->height;

    fprintf(EigenLog, "Image Dimensions %d x %d\n", faceImgSize.width, faceImgSize.height);

    //when CalcEigenObjects returns each image in this array will
    //hold one eigenvector, (one "eigenface")
    //which will be floating point images with depth = IPL_DEPTH_32F

    fprintf(EigenLog, "Allocing %d bytes\n", sizeof(IplImage*) * nEigens);
    eigenVectArr = (IplImage **) cvAlloc(sizeof(IplImage*) * nEigens);
    fprintf(EigenLog, "Done Allocing %d bytes\n", sizeof(IplImage*) * nEigens);
    for(int i = 0; i < nEigens; i++)
    {
        fprintf(EigenLog, "Array Building %d\n", i);
        eigenVectArr[i] = cvCreateImage(faceImgSize, IPL_DEPTH_32F, 1);
        fprintf(EigenLog, "Array Built %d\n", i);
    }

    fprintf(EigenLog, "Image Arrays Built\n");

    //matrix to hold eigenvalues
    eigenValMat = cvCreateMat(1, nEigens, CV_32FC1);

    fprintf(EigenLog, "Eigen Array Built\n");

    //allocating space for the pAvgTraining image
    pAvgTrainImg = cvCreateImage(faceImgSize, IPL_DEPTH_32F, 1);

    fprintf(EigenLog, "Training Area Built\n");

    //Criteria that determines when to stop calculation of eigen objects.
    calcLimit = cvTermCriteria (CV_TERMCRIT_ITER, nEigens, 1);

    // Train the model
    train();
}

EigenFaceModel::EigenFaceModel(int userID, char * xml): FaceModel(userID)
{
    EigenLog = stdout; //fopen("OutputLogs/EigenLog.txt", "w");
    fprintf(EigenLog, "Inside recognize constructor\n");

    UserID              = userID;

    //all these variables are filled from load from xml
    nEigens                 = 0; //number of eigenvalues
    nTrainFaces             = 0; //number of training images
    faceImgArr              = 0; //array of pointers to face images
    trainPersonNumMat       = 0; //holds person numbers
    pAvgTrainImg            = 0; //Averaged object.
    eigenVectArr            = 0; //eigenvectors
    eigenValMat             = 0; //eigenvalues
    projectedTrainFaceMat   = 0; //projected training faces

    //variable filled in compare
    projectedTestFace       = 0;

    //Criteria that determines when to stop calculation of eigen objects.
    calcLimit = cvTermCriteria (CV_TERMCRIT_ITER, nEigens, 1);

    // Load data from xml
    loadFromXML(xml);
}

EigenFaceModel::~EigenFaceModel()
{
    printf("Inside EigenFaceModel Deconstructor");
   // fclose(EigenLog);
    cvReleaseImage(eigenVectArr);
    cvReleaseImage(faceImgArr);
}

/*
    Preconditions: Takes a array of images of the user's face.
    Postconditions: The face model is trained on the input images.
*/
void EigenFaceModel::train()
{
    fprintf(EigenLog, "Enter train\n");

    //compute average image, eigenvalues, and eigenvectors
    cvCalcEigenObjects(
            nTrainFaces,
            faceImgArr,
            (void*)eigenVectArr,
            CV_EIGOBJ_NO_CALLBACK,
            0, 0,
            &calcLimit,
            pAvgTrainImg,
            eigenValMat->data.fl);

    fprintf(EigenLog, "after calc eigen\n");
    //allocate space for the projected training face
    projectedTrainFaceMat = cvCreateMat(nTrainFaces, nEigens, CV_32FC1);

    for(int i = 0; i < nTrainFaces; i++)
    {
        cvEigenDecomposite(
                faceImgArr[i],
                nEigens,
                eigenVectArr,
                0, 0,
                pAvgTrainImg,
                projectedTrainFaceMat->data.fl + i *nEigens);
    }
    fprintf(EigenLog, "after decompose\n");
}

/*
    Preconditions: Takes a XML std::string representation of the face model.
    Postconditions: The face model will be trained. All private variables needed
                    will be loaded.
*/
void EigenFaceModel::loadFromXML(char * xml)
{
    fprintf(EigenLog, "Inside loadFromXML\n");

    //creating an xml file to dump the xml string into
    FILE * anXmlFile;
    char * tempfile = tmpnam(NULL);
    anXmlFile = fopen(tempfile, "w");
    fprintf(anXmlFile, xml);
    fprintf(anXmlFile, "\n");
    fclose(anXmlFile);

    //create a file storage interface
    CvFileStorage * fileStorage;

    fprintf(EigenLog, "Temp File %s\n", tempfile);
    try
    {
        fileStorage = cvOpenFileStorage(tempfile, 0, CV_STORAGE_READ);
    }
    catch (int e) {
        fprintf(EigenLog, "Invalid face model XML\n");
    }
    if(!fileStorage)
    {
            fprintf(EigenLog, "Can't open facedata.xml\n");
            return;
    }

    modelUserID = cvReadIntByName(fileStorage, 0, "modelUserID", 0);
    trainImgs = cvReadIntByName(fileStorage, 0, "trainImgs", 0);
    nEigens = cvReadIntByName(fileStorage, 0, "nEigens", 0);
    nTrainFaces = cvReadIntByName(fileStorage, 0, "nTrainFaces", 0);
    trainPersonNumMat = (CvMat *)cvReadByName(fileStorage, 0, "trainPersonNumMat", 0);
    eigenValMat = (CvMat *)cvReadByName(fileStorage, 0, "eigenValMat", 0);
    projectedTrainFaceMat =
            (CvMat *)cvReadByName(fileStorage, 0, "projectedTrainFaceMat", 0);
    pAvgTrainImg = (IplImage *)cvReadByName(fileStorage, 0, "avgTrainImg", 0);
    eigenVectArr = (IplImage **)cvAlloc(nTrainFaces * sizeof(IplImage *));
    for(int i = 0; i < nEigens; i++)
    {
            char varname[200];
            sprintf(varname, "eigenVect_%d", i);
            eigenVectArr[i] = (IplImage *)cvReadByName(fileStorage, 0, varname, 0);
    }

    //release file storage
    cvReleaseFileStorage(&fileStorage);
    return;
}

/*
    Preconditions: The face model must be trained.
    Postconditions: Returns an XML std::string representation of the face model.
*/
char * EigenFaceModel::exportToXML()
{
    fprintf(EigenLog, "Inside exportToXML %d %d %d\n", trainImgs, nTrainFaces, nEigens);

    CvFileStorage * fileStorage;
    int i;

    //create a file-storage interface
    char * tempfile = tmpnam(NULL);//"faceModelDebug.xml";//tmpnam(NULL);
    strcat(tempfile, ".xml");
    fprintf(EigenLog, "Temp File %s\n", tempfile);
    fileStorage = cvOpenFileStorage (tempfile, 0, CV_STORAGE_WRITE);
    if(!fileStorage)
    {
       fprintf(EigenLog, "Can't open faceExport.xml\n");
        return "<error>";
    }
    //fprintf(EigenLog, "HI\n");

    cvWriteInt (fileStorage, "modelUserId", UserID);
    cvWriteInt (fileStorage, "trainImgs", trainImgs);
    cvWriteInt (fileStorage, "nEigens",  nEigens);
    cvWriteInt (fileStorage, "nTrainFaces", nTrainFaces);
    cvWrite(fileStorage, "trainPersonNumMat", personNumTruthMat, cvAttrList(0,0));
    cvWrite(fileStorage, "eigenValMat", eigenValMat, cvAttrList(0,0));
    cvWrite(fileStorage, "projectedTrainFaceMat", projectedTrainFaceMat, cvAttrList(0,0));
    cvWrite(fileStorage, "avgTrainImg", pAvgTrainImg, cvAttrList(0,0));
    //fprintf(EigenLog, "PI %d\n", nEigens);
    for(i = 0; i < nEigens; i++)
    {
            char varname[200];
            sprintf(varname, "eigenVect_%d", i);
            cvWrite(fileStorage, varname, eigenVectArr[i], cvAttrList(0,0));
          //  fprintf(EigenLog, "MI\n");
    }
    //release the opencv file storage
    cvReleaseFileStorage(&fileStorage);
    //fprintf(EigenLog, "NI\n");

    //read from created file and place into a string
    string faceXmlData;
    ifstream inputFile;
    inputFile.open(tempfile);
    //fprintf(EigenLog, "LI\n");
    if(inputFile.fail())
    {
        fprintf(EigenLog, "failed to open faceExport.xml");
        return "<error>";
    }

    int count = 0;
    string line;
    getline(inputFile,line);
    while ( inputFile ) {
       //fprintf(EigenLog, "%d %d %d %s\n", count, faceXmlData.capacity(), faceXmlData.max_size(),line.c_str());
       faceXmlData += line + "\n";
       count ++;
       getline(inputFile, line);
    }
    inputFile.clear();
    inputFile.close();
    //fprintf(EigenLog, "JI\n");
    char * ret = new char[faceXmlData.length()];
    strcpy(ret, faceXmlData.c_str());
    return ret;
    //fprintf(EigenLog, "%s JI\n", ret);
}

/*
    Preconditions: Takes an image to compare to the current model.
    Postconditions: Returns a confidence level (probability) between 0 and 1 that the image is of the
                    same user as this face model was trained on.
                    If the image is bad (e.g. no face detected, face obscured), return -1.

                    Note: This should be statistifically significant: ( 1 - P ), where P is the probability of the null hypothesis.
*/
double EigenFaceModel::compare(IplImage* image)
{
    /* SKIPPING CONFIDENCE LEVEL FOR NOW */
    //return confidenceLevel(distanceToNN(image));

    int iNearest, nearest;
    iNearest = distanceToNN(image);
    nearest = trainPersonNumMat->data.i[iNearest];

    fprintf(EigenLog, "The id found closest to given image is %d\n", nearest);

    //trainImgs holds the id of the user
    if(nearest <= 0)
    {
        fprintf(EigenLog, "matched to an anonymous face\n");
        return -1.0;
    }
    else if(nearest == UserID) //matched correct user
    {
        fprintf(EigenLog, "matched right user\n");
        return 1.0;
    }
    //error
    return 0;
}

/*
    Preconditions: Takes a distance.
    Postconditions: Returns a confidence level from 0 to 1. Should be 1 - P, where P is the probability of the null hypothesis (no match).
*/
double EigenFaceModel::confidenceLevel(double distance)
{
    return distance;
}

/*
    Preconditions: Takes a face image. Model must be trained.
    Postconditions: Returns a distance from the given image to the nearest neighbor trained on.
*/
double EigenFaceModel::distanceToNN(IplImage* faceImage)
{
    double leastDistSq = DBL_MAX;
    int i, iTrain, iNearest = 0;

    if(faceImage == NULL)
        fprintf(EigenLog, "not getting face image\n");

    // Decomposite PCA data
    projectedTestFace = (float *)cvAlloc( nEigens*sizeof(float));
    cvEigenDecomposite(
            faceImage,
            nEigens,
            eigenVectArr,
            0,0,
            pAvgTrainImg,
            projectedTestFace);

    bool highDistance = false;

       for(iTrain = 0; iTrain < nTrainFaces; iTrain++)
       {
           double distSq = 0;

           for(i = 0; i < nEigens; i++)
           {
                   float d_i = projectedTestFace[i] -
                           projectedTrainFaceMat->data.fl[iTrain * nEigens + i];

                   if(d_i > 1200)
                       highDistance = true;
                   else if(d_i < -1200)
                       highDistance = true;

                   //  distSq += d_i * d_i;
                   //ALTERNATE METHOD
                   distSq += d_i*d_i/eigenValMat->data.fl[i]; //better accuracy

                  int nearest = trainPersonNumMat->data.i[iTrain];
                  fprintf(EigenLog, "The distance: %f was for user %d\n", d_i, nearest);

           }
            fprintf(EigenLog, "The total distance: %f was for user %d\n", distSq, trainPersonNumMat->data.i[iTrain]);
           if((distSq < leastDistSq) && (!highDistance))
           {

                   leastDistSq = distSq; //the least distance
                   iNearest = iTrain; // the user that was the closest
                   fprintf(EigenLog, "The least distance is now: %f\n", leastDistSq);
           }


           highDistance = false;
       }
    return iNearest;
}
