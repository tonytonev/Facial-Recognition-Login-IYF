#include "IYFMainWindow.h"

IYFMainWindow::IYFMainWindow(QOpenCVCamera * c, InYourFaceRec* frm, int w, int fps, QWidget *parent ):QWidget(parent)
{
    viewLog = stdout; //fopen("OutputLogs/viewLog.txt", "w");

    camSource = c;
    camDisplay = new QLabel();

    faceRecognitionModule = frm;

    // Create widgets
    layout = new QGridLayout();

    camDisplay = new QLabel(this);
    usernameText = new QLineEdit(QString("tony"));
    passwordText = new QLineEdit(QString("asdf"));
    loginButton = new QPushButton(QString("Login"));
    newPicButton = new QPushButton(QString("Retake Picture"));

    camTimer = new QTimer();
    camTimer->start(1000/32);
    connect(camTimer, SIGNAL(timeout()), this, SLOT(updateImage()));
    connect(loginButton, SIGNAL(clicked()), this, SLOT(loginClicked()));
    connect(newPicButton, SIGNAL(clicked()), this, SLOT(newPicClicked()));

    usernameText->setMinimumWidth(150);
    usernameText->setMaximumWidth(150);
    passwordText->setMinimumWidth(150);
    passwordText->setMaximumWidth(150);
    loginButton->setMinimumWidth(100);
    loginButton->setMaximumWidth(100);
    passwordText->setEchoMode(QLineEdit::Password);

    layout->addWidget(camDisplay,0,0,3,1, Qt::AlignCenter);
    layout->addWidget(newPicButton,3,0,1,1,Qt::AlignCenter);
    layout->addWidget(usernameText,0,1,1,1,Qt::AlignCenter);
    layout->addWidget(passwordText,1,1,1,1,Qt::AlignCenter);
    layout->addWidget(loginButton,2,1,1,1,Qt::AlignCenter);

    setLayout(layout);

    QImage img = camSource->toQImage(camSource->getImage());
    width = w;
    height = ((double)img.height())/((double)img.width()) * w;
    resize(0,0);

    this->setStyleSheet("background-color: white;");

    getNextPictures = 0;
    picturesTaken = 1;

    frameNum = 0;
    framesPerSample = fps;
    directory = "";
    strDirectory = "";

    foundFace(NULL);
}

IYFMainWindow::~IYFMainWindow()
{
    if(faceImage != NULL)
    {
        cvReleaseImage(&faceImage);
    }
}

void IYFMainWindow::updateImage()
{
    QImage display;
    IplImage * camFrame = camSource->getImage();
    IplImage * img = cvCreateImage(cvSize(width, height),IPL_DEPTH_8U, 3);
    cvResize(camFrame,img);

    frameNum++;
    if(!faceImage && frameNum >= framesPerSample)
    {
        frameNum = 0;

        // Detect faces and preprocess if found
        IplImage* processedImg = (IplImage*)faceRecognitionModule->preProcess((void*)img);

        // If a face was found
        if(processedImg != NULL)
        {
            // Copy image and show login control
            foundFace(processedImg);

            display = camSource->toQImage(faceImage);
            camDisplay->setPixmap(QPixmap::fromImage(display));
        }
        else
        {
            display = camSource->toQImage(img);
            camDisplay->setPixmap(QPixmap::fromImage(display));
        }

        cvReleaseImage(&processedImg);
    }

    cvReleaseImage(&img);

    resize(0,0);

    cout.flush();
}

void IYFMainWindow::pictureTaken(string str, int num)
{
    fileName = str;
    getNextPictures = num;
    picturesTaken = 0;
}

void IYFMainWindow::loginClicked()
{
    fprintf(viewLog, "Inside loginClicked()\n");
    cout.flush();

    //Need to check to make sure each field is not empty
    string loginId = usernameText->text().toStdString();
    string passId = passwordText->text().toStdString();

    loginResult result =  faceRecognitionModule->Login((char*)loginId.c_str(), (char*)passId.c_str(), (void*)faceImage) ;

    //insert result into some kind of result text box
    if(result == lrFailedLogin)
    {
        fprintf(viewLog, "Login failed\n");

        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setText("Login failed.");
        msgBox.exec();
    }
    else if(result == lrSuccess)
    {
       fprintf(viewLog, "Login success\n");

       QMessageBox msgBox;
       msgBox.setIcon(QMessageBox::Information);
       msgBox.setText("Login succeeded.");
       msgBox.exec();
    }

    fprintf(viewLog, "Exiting login\n");
}

void IYFMainWindow::newPicClicked()
{
    foundFace(NULL);
}

/*
    Preconditions: face is either NULL or a pre-processed image of the face.
    Postconditions: If face is non-null, update copy it to faceImage and show login controls.
                    Otherwise, delete any existing faceImage, and hide the login controls.
*/
void IYFMainWindow::foundFace(IplImage* face)
{
    if (face)
    {
        // Copy image
        faceImage = cvCloneImage(face);

        // Show login controls
        usernameText->show();
        passwordText->show();
        loginButton->show();
        newPicButton->show();
    }
    else
    {
        // Delete image
        if(faceImage != NULL)
            cvReleaseImage(&faceImage);
        faceImage = NULL;

        // Hide login controls
        usernameText->hide();
        passwordText->hide();
        loginButton->hide();
        newPicButton->hide();
    }
}
