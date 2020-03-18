#ifdef Q_WS_WIN
#define MY_EXPORT __declspec(dllexport)
#else
#define MY_EXPORT
#endif

#include "IYFWrapper.h"
#include "Controller.h"


extern "C" MY_EXPORT InYourFaceRec* getInYourFaceRec()
{
    return new Controller();
}
