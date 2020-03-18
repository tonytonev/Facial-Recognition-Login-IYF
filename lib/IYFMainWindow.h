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

#include "Controller.h"
#include "QOpenCVCamera.h"
#include "QTakeAPicture.h"
#include "FaceDetectionModule.h"
using namespace std;

class IYFMainWindow : public QWidget
{
    Q_OBJECT
    public:
        IYFMainWindow(QOpenCVCamera * c, FaceDetectionModule * fdm = NULL, int fps = 1, QWidget *parent = 0 );
        ~IYFMainWindow();

        Controller* controller;

        FILE * viewLog;

    private:
        QTimer * camTimer;
        QLabel * camDisplay;
        QOpenCVCamera * camSource;
        QTakeAPicture * takeAPictureControl;

        QPushButton * loginButton;
        QLineEdit * usernameText;
        QLineEdit * passwordText;
        QPushButton * trainButton;
        QPushButton * directoryButton;

        QGridLayout *layout;
        FaceDetectionModule * faceDetectionModule;

        IplImage * faceArray[16];

        bool getNextPicture;
        int frameNum;
        int framesPerSample;
        string fileName;

        QString strDirectory;
        QString directory;
        QDir aDir;

    signals:

    public slots:
        void updateImage();
        void pictureTaken(string str);
        void loginClicked();
        void trainClicked();
        void setExistingDirectory();
};

#endif // IYFMAINWINDOW_H
