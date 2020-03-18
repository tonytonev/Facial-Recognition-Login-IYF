#ifndef IYFMAINWINDOW_H
#define IYFMAINWINDOW_H

#include <QWidget>
#include <QString>
#include <QTimer>
#include <QLabel>
#include <QGridLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QDialog>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <string>
#include <QLibrary>
#include <QMessageBox>

#include "QOpenCVCamera.h"
#include "FDMBasic.h"
#include "IYFWrapper.h"

typedef InYourFaceRec* (*getIYFRFuncType)();

using namespace std;

enum trainGuiState
{
    tgsEnterUserInfo,
    tgsTakePic,
    tgsApprovePic
};

class IYFMainWindow : public QWidget
{
    Q_OBJECT
    public:
        IYFMainWindow(QOpenCVCamera * c, InYourFaceRec * frm = NULL, int w = 600, int fps = 1, QWidget *parent = 0);
        ~IYFMainWindow();

        FILE * viewLog;

    private:
        QTimer * camTimer;
        QLabel * camDisplay;
        QOpenCVCamera * camSource;

        QLabel * instructionLabel;
        QLabel * usernameLabel;
        QLabel * password1Label;
        QLabel * password2Label;
        QLineEdit * usernameText;
        QLineEdit * password1Text;
        QLineEdit * password2Text;
        QPushButton * trainButton;
        QPushButton * newPicButton;
        QPushButton * usePicButton;

        QGridLayout *layout;
        InYourFaceRec* faceRecognitionModule;

        int numPicsToTake;
        int picturesTaken;
        int frameNum;
        int framesPerSample;
        int width;
        int height;
        string fileName;

        QString strDirectory;
        QString directory;
        QDir aDir;

        IplImage * faceImage;
        IplImage ** faceImgArray;

        trainGuiState guiState;
        void setGuiState(trainGuiState newState);

    signals:

    public slots:
        void updateImage();
        void trainClicked();
        void newPicClicked();
        void usePicClicked();
};

#endif // IYFMAINWINDOW_H
