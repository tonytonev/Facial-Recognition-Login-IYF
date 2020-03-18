# -------------------------------------------------
# Project created by QtCreator 2010-04-19T13:36:37
# -------------------------------------------------
QT -= gui
TARGET = SqliteModel
TEMPLATE = lib
DEFINES += SQLITEMODEL_LIBRARY
SOURCES += SqliteModel.cpp \
    sqlite3.c
HEADERS += SqliteModel.h \
    sqlite3.h \
    Model.h
