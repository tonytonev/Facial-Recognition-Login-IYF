#ifndef MODEL_H
#define MODEL_H

struct Model
{
        // Function authUser verifies the username and password of the user in the database, and if they match
        //  it returns the UserID for that user, otherwise it returns 0
        virtual int authUser(char* username, char* password) = 0;
        virtual int insertUser(char* username, char* password) = 0;
        virtual char* getFaceModelXML(int userID) = 0;
        virtual bool setFaceModelXML(int userID, char* faceModelXML) = 0;
        virtual int getIdForUser(char* username) = 0;
};

#endif // MODEL_H
