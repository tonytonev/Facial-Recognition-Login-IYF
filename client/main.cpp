#define IYF_LIB_DIR "../lib/InYourFaceRec"
#define THIS_APP "iyf"

#include <QtCore/QCoreApplication>
#include <QFile>
#include <QLibrary>
#include <iostream>
#include <cstdlib>
#include <assert.h>
#include <stdio.h>

#include "IYFWrapper.h"
#include "SqliteModel.h"

using namespace std;

int main(int argc, char *argv[])
{
    if (argc < 4 || !(strcmp(argv[1], "-l") == 0 || strcmp(argv[1], "-t") == 0 || strcmp(argv[1], "-d") == 0 || strcmp(argv[1], "-p") == 0))
    {
        printf("Usage: \n\t%-13s%s -l <username> <password> <imgFilename>\n\t%-13s%s -t <username> <password> <imgFilename1> <imgFilename2> ...\n\t%-13s%s -d <username> <imgFilename>\n\t%-13s%s -p <inputFilename> <outputFilename>\n\n", "Login:", THIS_APP, "Train:", THIS_APP, "Distance:", THIS_APP,  "Pre-process:", THIS_APP);
        return 0;
    }

    typedef InYourFaceRec* (*getIYFRFuncType)();

    // Load IFY Rec library
    QLibrary IYFRecLib(IYF_LIB_DIR);
    getIYFRFuncType getInYourFaceRec = (getIYFRFuncType) IYFRecLib.resolve("getInYourFaceRec");

    // Make sure the library was loaded
    if (!getInYourFaceRec)
    {
        fprintf(stderr, "Could not load library InYourFaceRec.\n");
        return -1;
    }

    fprintf(stdout, "Library InYourFaceRec successfully loaded.\n");

    // Initialize facial recognition object
    InYourFaceRec* faceRec = getInYourFaceRec();
    SqliteModel * m = new SqliteModel("../data/db/IYF-db.sqlite");
    faceRec->setFaceModel(m);
    assert(faceRec);

    if (strcmp(argv[1], "-l") == 0)
    {
        fprintf(stdout, "Result Code: %d\n", faceRec->Login(argv[2], argv[3], argv[4]));
    }
    else if (strcmp(argv[1], "-t") == 0)
    {
        int nImages = (int)argc - 4;
        char* filenames[nImages];

        for (int i = 0; i < nImages; i++)
        {
            filenames[i] = argv[4 + i];
            printf("image %d: %s\n", i, filenames[i]);
        }

        fprintf(stdout, "Result Code: %d\n", faceRec->Train(argv[2], argv[3], filenames, nImages));
    }
    else if (strcmp(argv[1], "-p") == 0)
    {
        // Make sure input file exists
        if (!QFile(argv[2]).exists())
        {
            fprintf(stdout, "Input file not found.\n");
            return -1;
        }

        fprintf(stdout, "Pre-processing result: %d\n", faceRec->preProcess(argv[2], argv[3]));
    }
    else if (strcmp(argv[1], "-d") == 0)
    {
        fprintf(stdout, "Distance: %f\n", faceRec->getDistance(argv[2], argv[3]));
    }

    return 0;
}
