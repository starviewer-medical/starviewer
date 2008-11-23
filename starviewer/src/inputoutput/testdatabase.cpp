/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "testdatabase.h"

namespace udg
{

Patient* TestDatabase::getPatientComplete()
{
    Patient *returnPatient = getPatient();
    Study *study = getStudy();
    Series *series;

    //Afegim Series
    series = getSeries();
    series->addImage(getImage());
    study->addSeries(series);

    series = getSeries2();
    series->addImage(getImage2());
    study->addSeries(series);

    returnPatient->addStudy(study);

    return returnPatient;
}

Image* TestDatabase::getImage()
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
    //newImage->setBitsAllocated(15);
    //newImage->setBitsStored(16);
    //newImage->setPixelRepresentation(17);
    newImage->setRescaleSlope(18.9);
    newImage->addWindowLevel(19.9, 20.1);
    //newImage->addWindowLevelExplanation("21");
    //newImage->setSliceLocation("22");
    newImage->setRescaleIntercept(23.21);
    newImage->setNumberOfFrames(24);
    newImage->setPhotometricInterpretation("25");


    return newImage;
}

Image * TestDatabase::getImage2()
{
    Image *newImage = new Image();
    double imagePatientPosition[6] = {7,8,9,10,11,12}, imageOrientationPatient[6] = {27,28,29,30,31,32};

    newImage->setSOPInstanceUID("1.2.3.5");
    newImage->setInstanceNumber("2");
    newImage->setImageOrientationPatient(imageOrientationPatient);
    //newImage->setPatientOrientation("3");
    newImage->setPixelSpacing(4,5);
    newImage->setSliceThickness(6.54321);
    newImage->setImagePositionPatient(imagePatientPosition);
    //newImage->setSamplesPerPixel(13);
    newImage->setRows(14);
    newImage->setColumns(15);
    //newImage->setBitsAllocated(16);
    newImage->setBitsStored(17);
    newImage->setPixelRepresentation(18);
    //newImage->setRescaleSlope(19.9);
    newImage->addWindowLevel(20.1, 21.0);
    newImage->addWindowLevelExplanation("22");
    newImage->setSliceLocation("23");
    newImage->setRescaleIntercept(24.321);
    newImage->setNumberOfFrames(25);
    newImage->setPhotometricInterpretation("26");

    return newImage;
}

Series* TestDatabase::getSeries()
{
    Series *newSeries = new Series();

    newSeries->setInstanceUID("1.2.3");
    newSeries->setSeriesNumber("1");
    newSeries->setModality("2");
    newSeries->setDate("20080102");
    //newSeries->setTime("123456");
    newSeries->setInstitutionName("5");
    newSeries->setPatientPosition("6");
    newSeries->setProtocolName("7");
    newSeries->setDescription("8");
    //newSeries->setFrameOfReferenceUID("1.2.3.4");
    newSeries->setPositionReferenceIndicator("9");
    //newSeries->setNumberOfPhases(10);
    //newSeries->setNumberOfSlicesPerPhase(11);
    newSeries->setBodyPartExamined("12");
    newSeries->setViewPosition("13");
    //newSeries->setManufacturer("14");

    return newSeries;
}

Series* TestDatabase::getSeries2()
{
    Series *newSeries = new Series();

    newSeries->setInstanceUID("1.2.4");
    newSeries->setSeriesNumber("2");
    newSeries->setModality("3");
    newSeries->setDate("20090203");
    //newSeries->setTime("023456");
    //newSeries->setInstitutionName("6");
    newSeries->setPatientPosition("7");
    newSeries->setProtocolName("8");
    newSeries->setDescription("9");
    newSeries->setFrameOfReferenceUID("1.2.3.5");
    //newSeries->setPositionReferenceIndicator("10");
    newSeries->setNumberOfPhases(11);
    newSeries->setNumberOfSlicesPerPhase(12);
    newSeries->setBodyPartExamined("13");
    //newSeries->setViewPosition("14");
    newSeries->setManufacturer("15");

    return newSeries;
}

Study* TestDatabase::getStudy()
{
    Study *newStudy = new Study();

    newStudy->setInstanceUID("1.2");
    newStudy->setID("1");
    //newStudy->setPatientAge("2");
    //newStudy->setWeight(3.21);
    //newStudy->setHeight(4.321);
    newStudy->setDate("20080706");
    newStudy->setTime("123456");
    newStudy->setAccessionNumber("5");
    newStudy->setDescription("6");
    //newStudy->setReferringPhysiciansName("7");

    return newStudy;
}

Study* TestDatabase::getStudy2()
{
    Study *newStudy = new Study();

    newStudy->setInstanceUID("1.3");
    newStudy->setID("2");
    newStudy->setPatientAge("3");
    newStudy->setWeight(4.321);
    newStudy->setHeight(5.4321);
    newStudy->setDate("20080807");
    newStudy->setTime("023456");
    newStudy->setAccessionNumber("6");
    newStudy->setDescription("7");
    newStudy->setReferringPhysiciansName("8");

    return newStudy;
}

Patient* TestDatabase::getPatient()
{
    Patient *newPatient = new Patient();

    newPatient->setID("0");
    newPatient->setFullName("1");
    newPatient->setBirthDate("20080201");
    newPatient->setSex("2");

    return newPatient;
}

Patient* TestDatabase::getPatient2()
{
    Patient *newPatient = new Patient();

    newPatient->setID("1");
    newPatient->setFullName("3");
    newPatient->setBirthDate("20080303");
    newPatient->setSex("4");

    return newPatient;
}

}
