# #####################################################################
# Automatically generated by qmake (BetaMingw1a) Tue Jul 8 19:59:30 2008
# #####################################################################
TARGET = InYourFaceRec
TEMPLATE = lib
DEFINES += INYOURFACEREC_LIBRARY
INCLUDEPATH += /usr/local/include/opencv \
    ../model
LIBS += /usr/local/lib/libcv.dylib \
    /usr/local/lib/libcvaux.dylib \
    /usr/local/lib/libcxcore.dylib \
    /usr/local/lib/libhighgui.dylib
SOURCES += Controller.cpp \
    EigenFaceModel.cpp \
    IYFWrapper.cpp \
    FDMBasic.cpp
HEADERS += Controller.h \
    FaceModel.h \
    Model.h \
    EigenFaceModel.h \
    IYFWrapper.h \
    FDMBasic.h
FORMS += 
CONFIG += debug
