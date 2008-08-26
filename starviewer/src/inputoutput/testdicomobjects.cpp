/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "testdicomobjects.h"

#include "logging.h"
#include "image.h"
#include "series.h"
#include "study.h"
#include "patient.h"

namespace udg
{

void TestDicomObjects::printImage(Image *imageToPrint)
{
    DEBUG_LOG("SOPInstanceUID" + imageToPrint->getSOPInstanceUID());
    DEBUG_LOG("InstanceNumber" + imageToPrint->getInstanceNumber());
    DEBUG_LOG("ImageOrientationPatient: " + QString().setNum(imageToPrint->getImageOrientationPatient()[0], 'g', 10) + "," +
                                            QString().setNum(imageToPrint->getImageOrientationPatient()[1], 'g', 10) + "," +
                                            QString().setNum(imageToPrint->getImageOrientationPatient()[2], 'g', 10) + "," +
                                            QString().setNum(imageToPrint->getImageOrientationPatient()[3], 'g', 10) + "," +
                                            QString().setNum(imageToPrint->getImageOrientationPatient()[4], 'g', 10) + "," +
                                            QString().setNum(imageToPrint->getImageOrientationPatient()[5], 'g', 10) + ",");
    DEBUG_LOG("PatientOrientation" + imageToPrint->getPatientOrientation());
    DEBUG_LOG("PixelSpacing: " +            QString().setNum(imageToPrint->getPixelSpacing()[0], 'g', 10) + "," +
                                            QString().setNum(imageToPrint->getPixelSpacing()[1], 'g', 10) + ",");
    DEBUG_LOG("SlicThickness: " + QString().setNum(imageToPrint->getSliceThickness(), 'g', 10));
    DEBUG_LOG("PatientPosition: "  +        QString().setNum(imageToPrint->getImagePositionPatient()[0], 'g', 10) + "," +
                                            QString().setNum(imageToPrint->getImagePositionPatient()[1], 'g', 10) + "," +
                                            QString().setNum(imageToPrint->getImagePositionPatient()[2], 'g', 10) + ",");
    DEBUG_LOG("SamplesPerPixel: " + QString().setNum(imageToPrint->getSamplesPerPixel() , 10) );
    DEBUG_LOG("Rows: " + QString().setNum(imageToPrint->getRows() , 10) );
    DEBUG_LOG("Columns: " + QString().setNum(imageToPrint->getColumns() , 10) );
    DEBUG_LOG("BitsAllocated: " + QString().setNum(imageToPrint->getBitsAllocated() , 10) );
    DEBUG_LOG("BitsStored: " + QString().setNum(imageToPrint->getBitsStored() , 10) );
    DEBUG_LOG("PixelRepresentation: " + QString().setNum(imageToPrint->getPixelRepresentation() , 10));
    DEBUG_LOG("RescaleScope: "  + QString().setNum(imageToPrint->getRescaleSlope(), 'g', 10));
    DEBUG_LOG("WindowLevelWidth: " + QString().setNum(imageToPrint->getWindowLevel(0).first, 'g', 10));
    DEBUG_LOG("WindowLevelCenter: " + QString().setNum(imageToPrint->getWindowLevel(0).second, 'g', 10));
    DEBUG_LOG("WindowLevelExplanations: " + imageToPrint->getWindowLevelExplanation(0));
    DEBUG_LOG("SliceLocation: " + imageToPrint->getSliceLocation());
    DEBUG_LOG("RescaleIntercept: " + QString().setNum(imageToPrint->getRescaleIntercept(), 'g', 10));
    DEBUG_LOG("NumberOfFrames: " + QString().setNum(imageToPrint->getNumberOfFrames(), 10));
    DEBUG_LOG("PhotometricInterpretation: " + imageToPrint->getPhotometricInterpretation());
}

void TestDicomObjects::printSeries(Series *seriesToPrint)
{
    DEBUG_LOG("InstanceUID: " + seriesToPrint->getInstanceUID());
    DEBUG_LOG("Number: " + seriesToPrint->getSeriesNumber());
    DEBUG_LOG("Modality: " + seriesToPrint->getModality());
    DEBUG_LOG("Date: " + seriesToPrint->getDateAsString());
    DEBUG_LOG("Time: " + seriesToPrint->getTimeAsString());
    DEBUG_LOG("InstitutionName: " + seriesToPrint->getInstitutionName());
    DEBUG_LOG("PatientPosition: " + seriesToPrint->getPatientPosition());
    DEBUG_LOG("ProtocolName: " + seriesToPrint->getProtocolName());
    DEBUG_LOG("Description: " + seriesToPrint->getDescription());
    DEBUG_LOG("FrameOfReferenceUID " + seriesToPrint->getFrameOfReferenceUID());
    DEBUG_LOG("PositionReferenceIndicator: " + seriesToPrint->getPositionReferenceIndicator());
    DEBUG_LOG("NumberOfPhases: " + QString().setNum(seriesToPrint->getNumberOfPhases(), 10));
    DEBUG_LOG("NumberOfSlicesPerPhase: " + QString().setNum(seriesToPrint->getNumberOfSlicesPerPhase(), 10));
    DEBUG_LOG("BodyPartExaminated: " + seriesToPrint->getBodyPartExamined());
    DEBUG_LOG("ViewPosition: " + seriesToPrint->getViewPosition());
    DEBUG_LOG("Manufacturer: " + seriesToPrint->getManufacturer());
}

void TestDicomObjects::printStudy(Study *studyToPrint)
{
    DEBUG_LOG("Instance UID" + studyToPrint->getInstanceUID());
    DEBUG_LOG("ID" + studyToPrint->getID());
    DEBUG_LOG("Patient Age" + QString().setNum(studyToPrint->getPatientAge(), 10));
    DEBUG_LOG("Patien Weigth" + QString().setNum(studyToPrint->getWeight(), 'g', 10));
    DEBUG_LOG("Patient Height" + QString().setNum(studyToPrint->getHeight(), 'g', 10));
    DEBUG_LOG("Modalities" + studyToPrint->getModalitiesAsSingleString());
    DEBUG_LOG("Date" + studyToPrint->getDateAsString());
    DEBUG_LOG("Time" + studyToPrint->getTimeAsString());
    DEBUG_LOG("Accession Number" + studyToPrint->getAccessionNumber());
    DEBUG_LOG("Description" + studyToPrint->getDescription());
    DEBUG_LOG("Referring Phsysicians Name" + studyToPrint->getReferringPhysiciansName());
}

void TestDicomObjects::printPatient(Patient *patientToPrint)
{
    DEBUG_LOG("Id: " + patientToPrint->getID());
    DEBUG_LOG("Full name: " + patientToPrint->getFullName());
    DEBUG_LOG("Birth date : " + patientToPrint->getBirthDateAsString());
    DEBUG_LOG("Sex : " + patientToPrint->getSex());
}

}
