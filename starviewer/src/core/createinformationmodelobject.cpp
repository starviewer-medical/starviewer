#include "createinformationmodelobject.h"

#include <QString>

#include "patient.h"
#include "study.h"
#include "series.h"
#include "image.h"
#include "dicomtagreader.h"
#include "dicomdictionary.h"

namespace udg{

Patient *CreateInformationModelObject::createPatient(DICOMTagReader *dicomTagReader)
{
    Patient *patient = new Patient;

    patient->setFullName(dicomTagReader->getAttributeByName(DICOMPatientsName));
    patient->setID(dicomTagReader->getAttributeByName(DICOMPatientID));
    patient->setBirthDate(dicomTagReader->getAttributeByName(DICOMPatientsBirthDate));
    patient->setSex(dicomTagReader->getAttributeByName(DICOMPatientsSex));

    return patient;
}

Study *CreateInformationModelObject::createStudy(DICOMTagReader *dicomTagReader)
{
    Study *study = new Study;
    QString studyModalities;

    study->setInstanceUID(dicomTagReader->getAttributeByName(DICOMStudyInstanceUID));
    study->setDate(dicomTagReader->getAttributeByName(DICOMStudyDate));
    study->setTime(dicomTagReader->getAttributeByName(DICOMStudyTime));
    study->setID(dicomTagReader->getAttributeByName(DICOMStudyID));
    study->setAccessionNumber(dicomTagReader->getAttributeByName(DICOMAccessionNumber));
    study->setDescription(dicomTagReader->getAttributeByName(DICOMStudyDescription));
    study->setPatientAge(dicomTagReader->getAttributeByName(DICOMPatientsAge));
    study->setHeight(dicomTagReader->getAttributeByName(DICOMPatientsSize).toDouble());
    study->setWeight(dicomTagReader->getAttributeByName(DICOMPatientsWeight).toDouble());
    study->setReferringPhysiciansName(dicomTagReader->getAttributeByName(DICOMReferringPhysiciansName));

    //Afegim la modalitat de l'estudi
    //Tenir en compte si aquest objecte s'utilitza per fer el dicomclassifierfillerstep que ells omplen la modalitat a partir de les series
    studyModalities = dicomTagReader->getAttributeByName(DICOMModalitiesInStudy);
    
    foreach(QString modality, studyModalities.split("\\"))
    {
        study->addModality(modality);
    }

    return study;
}

Series *CreateInformationModelObject::createSeries(DICOMTagReader *dicomTagReader)
{
    Series *series = new Series;

    series->setInstanceUID(dicomTagReader->getAttributeByName(DICOMSeriesInstanceUID));
    series->setModality(dicomTagReader->getAttributeByName(DICOMModality));
    series->setSeriesNumber(dicomTagReader->getAttributeByName(DICOMSeriesNumber));
    series->setDate(dicomTagReader->getAttributeByName(DICOMSeriesDate));
    series->setTime(dicomTagReader->getAttributeByName(DICOMSeriesTime));
    series->setInstitutionName(dicomTagReader->getAttributeByName(DICOMInstitutionName));
    series->setPatientPosition(dicomTagReader->getAttributeByName(DICOMPatientPosition));
    series->setProtocolName(dicomTagReader->getAttributeByName(DICOMProtocolName));
    series->setDescription(dicomTagReader->getAttributeByName(DICOMSeriesDescription));
    series->setFrameOfReferenceUID(dicomTagReader->getAttributeByName(DICOMFrameOfReferenceUID));
    series->setPositionReferenceIndicator(dicomTagReader->getAttributeByName(DICOMPositionReferenceIndicator));
    series->setManufacturer(dicomTagReader->getAttributeByName(DICOMManufacturer));
    series->setRequestedProcedureID(dicomTagReader->getAttributeByName(DICOMRequestedProcedureID));
    series->setScheduledProcedureStepID(dicomTagReader->getAttributeByName(DICOMScheduledProcedureStepID));
    series->setPerformedProcedureStepStartDate(dicomTagReader->getAttributeByName(DICOMPerformedProcedureStepStartDate));
    series->setPerformedProcedureStepStartTime(dicomTagReader->getAttributeByName(DICOMPerformedProcedureStepStartTime));

    /// Atributs necessaris pels hanging protocols
    series->setBodyPartExamined(dicomTagReader->getAttributeByName(DICOMBodyPartExamined));
    series->setViewPosition(dicomTagReader->getAttributeByName(DICOMViewPosition));

    return series;
}

Image *CreateInformationModelObject::createImage(DICOMTagReader *dicomTagReader)
{
    Image * image = new Image();

    image->setSOPInstanceUID(dicomTagReader->getAttributeByName(DICOMSOPInstanceUID));
    image->setInstanceNumber(dicomTagReader->getAttributeByName(DICOMInstanceNumber));

    return image;
}

}; //end udg namespace
