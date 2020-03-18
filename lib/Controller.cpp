#include "Controller.h"

using namespace std;

Controller::Controller()
{
    confidenceThreshold = 0.9;

    // Load the model
    model = NULL;

    // Initalize the face detection module, used for pre-processing
    fdm = new FDMBasic("../data/Haars/haarcascade_frontalface_alt.xml", "../data/Haars/haarcascade_profileface.xml", "../data/Haars/ojoI.xml" ,"../data/Haars/ojoD.xml", "../data/Haars/Nariz.xml","../data/Haars/Mouth.xml");

    // Initalize log
    controllerLog = stdout; //fopen("OutputLogs/controllerLog.txt", "w");
}

Controller::~Controller()
{
    fclose(controllerLog);
    delete model;
}

void Controller::setFaceModel(void* fm)
{
    model = (Model *)fm;
}

/*
  Preconditions: Takes the username, the password, and an image of the user attempting to login. Image must be pre-processed.
  Postconditions: Returns an loginResult code.
*/
loginResult Controller::Login(char* username, char* password, IplImage* userImage)
{
    fprintf(controllerLog, "Entered Controller->Login\n");
    if(!hasModel())
        return lrFailedLogin;

    // Authenticate user and get user id
    int UserID = model->authUser(username, password);

    // If username/password authentification failed return
    if (UserID == 0)
    {
        fprintf(controllerLog, "Username/password are incorrect\n");
        return lrFailedLogin;
    }

    //checking to see if what UserID gets
    fprintf(controllerLog, "UserID = %d\n\n", UserID);

    // Get face model XML char*
    char* faceModelXML = model->getFaceModelXML(UserID);

    //test output the contents of the faceModel
    fprintf(controllerLog, "FaceModelXML:\n%.100s\n\n", faceModelXML);

    // Create face model object and load from XML
    FaceModel* faceModel = new EigenFaceModel(UserID, faceModelXML);

    // Compare face model with parameter 'userImage'
    double confidenceLevel = faceModel->compare(userImage);

    fprintf(controllerLog, "Confidence level: %f\n", confidenceLevel);

    // Return the result
    if (confidenceLevel < 0)
    {
        fprintf(controllerLog, "Return failed login\n\n");
        return lrFailedLogin;
    
    }
    //shouldn't happen till confidence level is implemeneted
    else if (confidenceLevel < confidenceThreshold)
    {
        fprintf(controllerLog, "Return bad image\n\n");
        return lrBadImage;
    }
    
    fprintf(controllerLog, "Return successful login\n\n");
    return lrSuccess;
}

/*
    Preconditions: Takes the username and password of a user, a face image array, and the number of images in the array.
                    Images must be pre-processed.
    Postconditions: If the user exists and the password is correct, replaces any existing model with a new model trained on the input images.
                    If the user doesn't exist, the user is created and trained on the images.
*/
trainResult Controller::Train(char* username, char* password, IplImage ** faceImgArr, int nFaces)
{
    fprintf(controllerLog, "Entered Controller->Train\n");
    if(!hasModel())
        return trFail;

    if( nFaces < 1)
    {
       //Must of passed at least 1 image to combine with test images
       return trFail;
    }

    int UserId;

    // If user already exists, find user id
    UserId = model->authUser(username, password);

    // If user not found or login filed, attempt to insert a new user
    if (UserId == 0)
        UserId = model->insertUser(username, password);

    // If unable to create new user, return
    if (UserId == 0)
    {
        fprintf(controllerLog, "Login for existing user failed or unable to insert new user.\n");
        return trFail;
    }

    //checking to see if what UserID gets
    fprintf(controllerLog, "UserId = %d\n", UserId);

    // Create and face model object and train on images
    FaceModel* faceModel = new EigenFaceModel(UserId, faceImgArr, nFaces);

    //get all the eigenface information into xml format
    char * faceModelXML = faceModel->exportToXML();

    /*fprintf(controllerLog, "BACK IN TRAIN\n");
    fprintf(controllerLog, "Face Model size %d\n",strlen(faceModelXML));
    fprintf(controllerLog, "Face Model %s\n", faceModelXML);*/

    //save the created face model xml char* in the db
    if(model->setFaceModelXML(UserId, (char*)faceModelXML))
    {
        fprintf(controllerLog, "Successfully set face model for user: %d\n", UserId);
        delete [] faceModelXML;
        return trSuccess;
    }
    else
    {
        fprintf(controllerLog, "Unable to set face model for user: %d\n", UserId);
        delete [] faceModelXML;
        return trFail;
    }
}

// Wrapper function for library API
loginResult Controller::Login(char* username, char* password, char* imgFilename)
{
    fprintf(controllerLog, "what is imgfilename: %s\n", imgFilename);
    // Load face image from file

    IplImage * faceImg;
    //allocate space for image
    faceImg = cvLoadImage(imgFilename, CV_LOAD_IMAGE_GRAYSCALE);

    if(!faceImg)
    {
          fprintf(controllerLog, "Could not load image file\n");
          return lrBadImage;
    }

    return Login(username, password, faceImg);
}

// Wrapper function for library API
loginResult Controller::Login(char* username, char* password, void* faceImg)
{
    return Login(username, password, (IplImage*)faceImg);
}

// Wrapper function for library API
trainResult Controller::Train(char* username, char* password, char** imgFilenames, int nFaces)
{
    IplImage* faceImg[nFaces];

    // Load face images from file
    for (int i = 0; i < nFaces; i++)
    {
        faceImg[i] = cvLoadImage(imgFilenames[i], CV_LOAD_IMAGE_GRAYSCALE);

        if(faceImg == NULL)
            fprintf(controllerLog, "Could not load image file\n");
    }

    return Train(username, password, faceImg, nFaces);
}

// Wrapper function for library API
trainResult Controller::Train(char* username, char* password, void** IplImages, int nFaces)
{
    return Train(username, password, (IplImage**)IplImages, nFaces);
}

// Gets the distance for a given username and a given image
double Controller::getDistance(char* username, char* imgFilename)
{
    if(!hasModel())
        return -1.0;

    // Get user id
    int UserID = model->getIdForUser(username);

    // Load face image from file
    IplImage* faceImg = cvLoadImage(imgFilename, CV_LOAD_IMAGE_GRAYSCALE);

    // Get face model XML string
    char* faceModelXML = model->getFaceModelXML(UserID);

    // Create face model object and load from XML
    EigenFaceModel* faceModel = new EigenFaceModel(UserID, faceModelXML);

    return faceModel->distanceToNN(faceImg);
}

int Controller::compareFaces(char * faceImgFilename1, char * faceImgFilename2)
{
    // Load face images from file
    IplImage* faceImg1 = cvLoadImage(faceImgFilename1, CV_LOAD_IMAGE_GRAYSCALE);
    IplImage* faceImg2 = cvLoadImage(faceImgFilename2, CV_LOAD_IMAGE_GRAYSCALE);

    IplImage* trainingImgs[] = {faceImg1, faceImg1};

    // Create face model object and load XML
    EigenFaceModel* faceModel = new EigenFaceModel(0, trainingImgs, 2 );

    if (faceModel->compare(faceImg2) > confidenceThreshold)
        return 1;
    else
        return 0;
}

/*
    Preconditions: inputFaceImg is an image of a face.
    Postconditions: Returns cropped, black-and-white version of the input image
*/
IplImage* Controller::preProcess(IplImage* inputFaceImg)
{
    IplImage * faceArray[1];

    // Detect, crop, and preprocess faces
    int nFaces = fdm->detectFaces(inputFaceImg, faceArray, 1);

    // If faces were detected, return processed face
    if (nFaces > 0)
    {
        return faceArray[0];
    }

    // If not faces were detected, return NULL
    return NULL;
}

// Wrapper for lib interface
void* Controller::preProcess(void* inputFaceImg)
{
    return (void*)preProcess((IplImage*)inputFaceImg);
}

/*
    Preconditions: Output filename must end in .pgm
    Postconditions: Takes an input file is read, processsed, and output stored in the location specified by outputImgFilename.
                    Returns true if successful, and false if unable to detect any faces.
*/
bool Controller::preProcess(char* inputImgFilename, char* outputImgFilename)
{
    // Load input image from file
    IplImage* inputFaceImg = cvLoadImage(inputImgFilename, CV_LOAD_IMAGE_ANYCOLOR);

    // Preprocess face
    IplImage* outputFaceImg = preProcess(inputFaceImg);

    cvReleaseImage(&inputFaceImg);

    // If faces were detected, save output to file
    if (outputFaceImg != NULL)
    {
        cvSaveImage(outputImgFilename, outputFaceImg);

        cvReleaseImage(&outputFaceImg);

        return true;
    }

    // If not faces were detected, return false
    return false;
}

bool Controller::hasModel()
{
    if(model == NULL)
    {
        fprintf(controllerLog, "Pass in a model first.\n");
        return false;
    }
    return true;
}
