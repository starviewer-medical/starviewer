/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "localdatabasemanager.h"

#include "patient.h"
#include "localdatabaseimagedal.h"
#include "databaseconnection.h"
#include "dicommask.h"
#include "testdicomobjects.h"

namespace udg
{

LocalDatabaseManager::LocalDatabaseManager()
{

}


Status LocalDatabaseManager::insert(Patient *newPatient)
{
    Status state;
    DicomMask imageMask;
    state.setStatus("Normal", true, 0);

    DatabaseConnection *dbConnect = DatabaseConnection::getDatabaseConnection();

    LocalDatabaseImageDAL localDatabaseImageDAL;

    imageMask.setStudyUID("1.2");
    imageMask.setSeriesUID("1.2.3");

    localDatabaseImageDAL.setDatabaseConnection(dbConnect);

    localDatabaseImageDAL.deleteImage(imageMask);

    //localDatabaseImageDAL.insert(getImage(),1);
    
    /*TestDicomObjects::printImage(getImage());
    TestDicomObjects::printImage(localDatabaseImageDAL.query(imageMask).at(0));
    TestDicomObjects::printImage(localDatabaseImageDAL.query(imageMask).at(1));
    */
    return state;
}

Image * LocalDatabaseManager::getImage()
{
    Image *newImage = new Image();
    double imagePatientPosition[6] = {6,7,8,9,10,11}, imageOrientationPatient[6] = {26,27,28,29,30,31};

    newImage->setSOPInstanceUID("1.2.3.4");
    newImage->setInstanceNumber("1");
    newImage->setImageOrientationPatient(imageOrientationPatient);
    newImage->setPatientOrientation("2");
    newImage->setPixelSpacing(3,4);
    newImage->setSliceThickness(5.4321);
    newImage->setImagePositionPatient(imagePatientPosition);
    newImage->setSamplesPerPixel(12);
    newImage->setRows(13);
    newImage->setColumns(14);
    newImage->setBitsAllocated(15);
    newImage->setBitsStored(16);
    newImage->setPixelRepresentation(17);
    newImage->setRescaleSlope(18.9);
    newImage->addWindowLevel(19.9, 20.1);
    newImage->addWindowLevelExplanation("21");
    newImage->setSliceLocation("22");
    newImage->setRescaleIntercept(23.21);
    newImage->setNumberOfFrames(24);
    newImage->setPhotometricInterpretation("25");


    return newImage;
}

Image * LocalDatabaseManager::getImage2()
{
    Image *newImage = new Image();
    double imagePatientPosition[6] = {7,8,9,10,11,12}, imageOrientationPatient[6] = {27,28,29,30,31,32};

    newImage->setSOPInstanceUID("1.2.3.4");
    newImage->setInstanceNumber("2");
    newImage->setImageOrientationPatient(imageOrientationPatient);
    newImage->setPatientOrientation("3");
    newImage->setPixelSpacing(4,5);
    newImage->setSliceThickness(6.54321);
    newImage->setImagePositionPatient(imagePatientPosition);
    newImage->setSamplesPerPixel(13);
    newImage->setRows(14);
    newImage->setColumns(15);
    newImage->setBitsAllocated(16);
    newImage->setBitsStored(17);
    newImage->setPixelRepresentation(18);
    newImage->setRescaleSlope(19.9);
    newImage->addWindowLevel(20.1, 21.0);
    newImage->addWindowLevelExplanation("22");
    newImage->setSliceLocation("23");
    newImage->setRescaleIntercept(24.321);
    newImage->setNumberOfFrames(25);
    newImage->setPhotometricInterpretation("26");

    return newImage;
}

}
