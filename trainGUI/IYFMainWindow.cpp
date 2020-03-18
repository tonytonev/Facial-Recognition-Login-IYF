#include "IYFMainWindow.h"

IYFMainWindow::IYFMainWindow(QOpenCVCamera * c, InYourFaceRec* frm, int w, int fps, QWidget *parent ):QWidget(parent)
{
    viewLog = stdout; //fopen("OutputLogs/viewLog.txt", "w");

    camSource = c;
    camDisplay = new QLabel();

    faceRecognitionModule = frm;

    // Create widgets
    layout = new QGridLayout();

    instructionLabel = new QLabel(QString("Enter user information."));
    usernameLabel = new QLabel(QString("Username:"));
    password1Label = new QLabel(QString("Choose a password:"));
    password2Label = new QLabel(QString("Re-enter password:"));
    usernameText = new QLineEdit(QString(""));
    password1Text = new QLineEdit(QString(""));
    password2Text = new QLineEdit(QString(""));
    trainButton = new QPushButton(QString("Train"));

    camDisplay = new QLabel(this);

    newPicButton = new QPushButton(QString("Retake Picture"));
    usePicButton = new QPushButton(QString("Use Picture"));

    camTimer = new QTimer();
    camTimer->start(1000/32);
    connect(camTimer, SIGNAL(timeout()), this, SLOT(updateImage()));
    connect(trainButton, SIGNAL(clicked()), this, SLOT(trainClicked()));
    connect(newPicButton, SIGNAL(clicked()), this, SLOT(newPicClicked()));
    connect(usePicButton, SIGNAL(clicked()), this, SLOT(usePicClicked()));

    usernameText->setMinimumWidth(150);
    usernameText->setMaximumWidth(150);
    password1Text->setMinimumWidth(150);
    password1Text->setMaximumWidth(150);
    password2Text->setMinimumWidth(150);
    password2Text->setMaximumWidth(150);
    trainButton->setMinimumWidth(100);
    trainButton->setMaximumWidth(100);
    password1Text->setEchoMode(QLineEdit::Password);
    password2Text->setEchoMode(QLineEdit::Password);


    newPicButton->setMinimumWidth(100);
    newPicButton->setMaximumWidth(100);
    usePicButton->setMinimumWidth(100);
    usePicButton->setMaximumWidth(100);

    layout->addWidget(instructionLabel,0,0,1,2,Qt::AlignLeft);
    layout->addWidget(usernameLabel,1,0,1,1,Qt::AlignLeft);
    layout->addWidget(usernameText,1,1,1,1,Qt::AlignRight);
    layout->addWidget(password1Label,2,0,1,1,Qt::AlignLeft);
    layout->addWidget(password1Text,2,1,1,1,Qt::AlignRight);
    layout->addWidget(password2Label,3,0,1,1,Qt::AlignLeft);
    layout->addWidget(password2Text,3,1,1,1,Qt::AlignRight);
    layout->addWidget(trainButton,4,0,1,2,Qt::AlignCenter);

    layout->addWidget(camDisplay,1,0,1,2, Qt::AlignCenter);

    layout->addWidget(newPicButton,2,0,1,1,Qt::AlignCenter);
    layout->addWidget(usePicButton,2,1,1,1,Qt::AlignCenter);

    setLayout(layout);

    QImage img = camSource->toQImage(camSource->getImage());
    width = w;
    height = ((double)img.height())/((double)img.width()) * w;
    resize(0,0);

    this->setStyleSheet("background-color: white;");

    frameNum = 0;
    framesPerSample = fps;
    directory = "";
    strDirectory = "";

    numPicsToTake = 5;
    faceImgArray = new IplImage * [numPicsToTake];

    setGuiState(tgsEnterUserInfo);
}

IYFMainWindow::~IYFMainWindow()
{
    if(faceImage != NULL)
    {
        cvReleaseImage(&faceImage);
    }

    delete[] faceImgArray;
}

void IYFMainWindow::updateImage()
{
    if (guiState == tgsTakePic)
    {
        QImage display;
        IplImage * camFrame = camSource->getImage();
        IplImage * img = cvCreateImage(cvSize(width, height),IPL_DEPTH_8U, 3);
        cvResize(camFrame,img);

        display = camSource->toQImage(img);

        frameNum++;
        if (frameNum >= framesPerSample)
        {
            frameNum = 0;

            // Detect faces and preprocess if found
            IplImage* processedImg = (IplImage*)faceRecognitionModule->preProcess((void*)img);

            // If a face was found
            if(processedImg != NULL)
            {
                // Copy image
                faceImage = cvCloneImage(processedImg);

                // Display pic
                display = camSource->toQImage(faceImage);
                camDisplay->setPixmap(QPixmap::fromImage(display));

                // Ask user to approve pic
                setGuiState(tgsApprovePic);
            }
            else
            {
                camDisplay->setPixmap(QPixmap::fromImage(display));
            }

            cvReleaseImage(&processedImg);
        }

        cvReleaseImage(&img);
    }

    resize(0,0);

    cout.flush();
}

void IYFMainWindow::trainClicked()
{
    if (password1Text->text() != password2Text->text())
    {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setText("Passwords don't match. Please try again.");
        msgBox.exec();

        return;
    }

    setGuiState(tgsTakePic);
}

void IYFMainWindow::newPicClicked()
{
    setGuiState(tgsTakePic);
}

void IYFMainWindow::usePicClicked()
{
    if (picturesTaken < numPicsToTake)
    {
        // Store last pic taken in array
        faceImgArray[picturesTaken] = faceImage;

        picturesTaken++;

        // If this is the last pic
        if (picturesTaken == numPicsToTake)
        {

            string username = usernameText->text().toStdString();
            string password = password1Text->text().toStdString();

            // Train
            faceRecognitionModule->Train((char*)username.c_str(), (char*)password.c_str(), (void**)faceImgArray, numPicsToTake);

            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setText("User successfully trained.");
            msgBox.exec();

            // Clear text fields
            usernameText->setText(QString(""));
            password1Text->setText(QString(""));
            password2Text->setText(QString(""));

            setGuiState(tgsEnterUserInfo);
        }
        else
        {
            setGuiState(tgsTakePic);
        }
    }
}

/*
    Preconditions: face is either NULL or a pre-processed image of the face.
    Postconditions: If face is non-null, update copy it to faceImage and show login controls.
                    Otherwise, delete any existing faceImage, and hide the login controls.
*/
void IYFMainWindow::setGuiState(trainGuiState newState)
{
    if (newState == tgsEnterUserInfo)
    {
        instructionLabel->setText(QString("Enter user information."));
        
        usernameLabel->show();
        password1Label->show();
        password2Label->show();
        usernameText->show();
        password1Text->show();
        password2Text->show();
        trainButton->show();

        camDisplay->hide();

        newPicButton->hide();
        usePicButton->hide();

        picturesTaken = 0;
    }
    else if (newState == tgsTakePic)
    {
        instructionLabel->setText(QString("Look straight into the camera."));

        usernameLabel->hide();
        password1Label->hide();
        password2Label->hide();
        usernameText->hide();
        password1Text->hide();
        password2Text->hide();
        trainButton->hide();

        camDisplay->show();

        newPicButton->hide();
        usePicButton->hide();
    }
    else if (newState == tgsApprovePic)
    {
        instructionLabel->setText(QString("Is this a good picture?"));

        usernameLabel->hide();
        password1Label->hide();
        password2Label->hide();
        usernameText->hide();
        password1Text->hide();
        password2Text->hide();
        trainButton->hide();

        camDisplay->show();

        newPicButton->show();
        usePicButton->show();
    }

    guiState = newState;
}
