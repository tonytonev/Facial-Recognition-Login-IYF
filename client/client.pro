# -------------------------------------------------
# Project created by QtCreator 2010-04-14T20:17:13
# -------------------------------------------------
QT -= gui
TARGET = iyf
CONFIG += console
CONFIG -= app_bundle
TEMPLATE = app
INCLUDEPATH += ../lib \
    ../model
SOURCES += main.cpp \
    ../model/SqliteModel.cpp \
    ../model/sqlite3.c
HEADERS += ../model/SqliteModel.h \
    ../model/sqlite3ext.h \
    ../model/sqlite3.h \
    ../model/Model.h \
    ../model/convert.h
