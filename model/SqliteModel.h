#ifdef Q_WS_WIN
#define MY_EXPORT __declspec(dllexport)
#else
#define MY_EXPORT
#endif

#ifndef SQLITEMODEL_H
#define SQLITEMODEL_H

#include <cstdio>
#include <stdlib.h>
#include <string.h>

#include "Model.h"
#include "sqlite3.h""


class SqliteModel : public Model
{
public:
    SqliteModel(char* filename);
    ~SqliteModel();

    int authUser(char* username, char* password);
    char* getFaceModelXML(int UserID);
    int insertUser(char* username, char* password);
    void storeFaceModelXML(int userID, char* faceModelXML);
    bool setFaceModelXML(int userID, char* faceModelXML);
    int getIdForUser(char* username);

private:
    sqlite3* db;
    sqlite3* dbro;
    sqlite3_stmt* get_uid_stmt;
    sqlite3_stmt* get_xml_with_uid_stmt;
    sqlite3_stmt* get_uid_with_uname_stmt;
    sqlite3_stmt* insert_user_stmt;
    sqlite3_stmt* set_facemod_stmt;
};

//extern "C" MY_EXPORT Model* getModel();

#endif // SQLITEMODEL_H
