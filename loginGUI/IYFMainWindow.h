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

class IYFMainWindow : public QWidget
{
    Q_OBJECT
    public:
        IYFMainWindow(QOpenCVCamera * c, InYourFaceRec * frm = NULL, int w = 600, int fps = 1, QWidget *parent = 0);
        ~IYFMainWindow();

        FILE * viewLog;

    private:
        void foundFace(IplImage* face);

        QTimer * camTimer;
        QLabel * camDisplay;
        QOpenCVCamera * camSource;

        QLineEdit * usernameText;
        QLineEdit * passwordText;
        QPushButton * loginButton;
        QPushButton * newPicButton;

        QGridLayout *layout;
        InYourFaceRec* faceRecognitionModule;

        int getNextPictures;
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

    signals:

    public slots:
        void updateImage();
        void pictureTaken(string str, int num);
        void loginClicked();
        void newPicClicked();
};

#endif // IYFMAINWINDOW_H
