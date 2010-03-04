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

    patient->setFullName(dicomTagReader->getValueAttributeAsQString(DICOMPatientsName));
    patient->setID(dicomTagReader->getValueAttributeAsQString(DICOMPatientID));
    patient->setBirthDate(dicomTagReader->getValueAttributeAsQString(DICOMPatientsBirthDate));
    patient->setSex(dicomTagReader->getValueAttributeAsQString(DICOMPatientsSex));

    return patient;
}

Study *CreateInformationModelObject::createStudy(DICOMTagReader *dicomTagReader)
{
    Study *study = new Study;
    QString studyModalities;

    study->setInstanceUID(dicomTagReader->getValueAttributeAsQString(DICOMStudyInstanceUID));
    study->setDate(dicomTagReader->getValueAttributeAsQString(DICOMStudyDate));
    study->setTime(dicomTagReader->getValueAttributeAsQString(DICOMStudyTime));
    study->setID(dicomTagReader->getValueAttributeAsQString(DICOMStudyID));
    study->setAccessionNumber(dicomTagReader->getValueAttributeAsQString(DICOMAccessionNumber));
    study->setDescription(dicomTagReader->getValueAttributeAsQString(DICOMStudyDescription));
    study->setPatientAge(dicomTagReader->getValueAttributeAsQString(DICOMPatientsAge));
    study->setHeight(dicomTagReader->getValueAttributeAsQString(DICOMPatientsSize).toDouble());
    study->setWeight(dicomTagReader->getValueAttributeAsQString(DICOMPatientsWeight).toDouble());
    study->setReferringPhysiciansName(dicomTagReader->getValueAttributeAsQString(DICOMReferringPhysiciansName));

    //Afegim la modalitat de l'estudi
    //Tenir en compte si aquest objecte s'utilitza per fer el dicomclassifierfillerstep que ells omplen la modalitat a partir de les series
    studyModalities = dicomTagReader->getValueAttributeAsQString(DICOMModalitiesInStudy);
    
    foreach(QString modality, studyModalities.split("\\"))
    {
        study->addModality(modality);
    }

    return study;
}

Series *CreateInformationModelObject::createSeries(DICOMTagReader *dicomTagReader)
{
    Series *series = new Series;

    series->setInstanceUID(dicomTagReader->getValueAttributeAsQString(DICOMSeriesInstanceUID));
    series->setModality(dicomTagReader->getValueAttributeAsQString(DICOMModality));
    series->setSeriesNumber(dicomTagReader->getValueAttributeAsQString(DICOMSeriesNumber));
    series->setDate(dicomTagReader->getValueAttributeAsQString(DICOMSeriesDate));
    series->setTime(dicomTagReader->getValueAttributeAsQString(DICOMSeriesTime));
    series->setInstitutionName(dicomTagReader->getValueAttributeAsQString(DICOMInstitutionName));
    series->setPatientPosition(dicomTagReader->getValueAttributeAsQString(DICOMPatientPosition));
    series->setProtocolName(dicomTagReader->getValueAttributeAsQString(DICOMProtocolName));
    series->setDescription(dicomTagReader->getValueAttributeAsQString(DICOMSeriesDescription));
    series->setFrameOfReferenceUID(dicomTagReader->getValueAttributeAsQString(DICOMFrameOfReferenceUID));
    series->setPositionReferenceIndicator(dicomTagReader->getValueAttributeAsQString(DICOMPositionReferenceIndicator));
    series->setManufacturer(dicomTagReader->getValueAttributeAsQString(DICOMManufacturer));
    series->setRequestedProcedureID(dicomTagReader->getValueAttributeAsQString(DICOMRequestedProcedureID));
    series->setScheduledProcedureStepID(dicomTagReader->getValueAttributeAsQString(DICOMScheduledProcedureStepID));
    series->setPerformedProcedureStepStartDate(dicomTagReader->getValueAttributeAsQString(DICOMPerformedProcedureStepStartDate));
    series->setPerformedProcedureStepStartTime(dicomTagReader->getValueAttributeAsQString(DICOMPerformedProcedureStepStartTime));

    /// Atributs necessaris pels hanging protocols
    series->setBodyPartExamined(dicomTagReader->getValueAttributeAsQString(DICOMBodyPartExamined));
    series->setViewPosition(dicomTagReader->getValueAttributeAsQString(DICOMViewPosition));
    QString laterality = dicomTagReader->getValueAttributeAsQString(DICOMLaterality);
    if( !laterality.isEmpty() )
        series->setLaterality( laterality.at(0) );

    return series;
}

Image *CreateInformationModelObject::createImage(DICOMTagReader *dicomTagReader)
{
    Image * image = new Image();

    image->setSOPInstanceUID(dicomTagReader->getValueAttributeAsQString(DICOMSOPInstanceUID));
    image->setInstanceNumber(dicomTagReader->getValueAttributeAsQString(DICOMInstanceNumber));

    return image;
}

}; //end udg namespace
