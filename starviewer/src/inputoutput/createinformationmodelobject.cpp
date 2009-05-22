#include "createinformationmodelobject.h"

#include <QString>

#include "patient.h"
#include "study.h"
#include "series.h"
#include "image.h"
#include "dicomtagreader.h"
#include "logging.h"

namespace udg{

Patient* CreateInformationModelObject::createPatient(DICOMTagReader *dicomTagReader)
{
    Patient *patient = new Patient;

    patient->setFullName(dicomTagReader->getAttributeByName(DCM_PatientsName));
    patient->setID(dicomTagReader->getAttributeByName(DCM_PatientID));
    patient->setBirthDate(dicomTagReader->getAttributeByName(DCM_PatientsBirthDate));
    patient->setSex(dicomTagReader->getAttributeByName(DCM_PatientsSex));

    return patient;
}

Study* CreateInformationModelObject::createStudy(DICOMTagReader *dicomTagReader)
{
    Study *study = new Study;
    QString studyModalities;

    study->setInstanceUID(dicomTagReader->getAttributeByName(DCM_StudyInstanceUID));
    study->setDate(dicomTagReader->getAttributeByName(DCM_StudyDate));
    study->setTime(dicomTagReader->getAttributeByName(DCM_StudyTime));
    study->setID(dicomTagReader->getAttributeByName(DCM_StudyID));
    study->setAccessionNumber(dicomTagReader->getAttributeByName(DCM_AccessionNumber));
    study->setDescription(dicomTagReader->getAttributeByName(DCM_StudyDescription));
    study->setPatientAge(dicomTagReader->getAttributeByName(DCM_PatientsAge));
    study->setHeight(dicomTagReader->getAttributeByName(DCM_PatientsSize).toDouble());
    study->setWeight(dicomTagReader->getAttributeByName(DCM_PatientsWeight).toDouble());
    study->setReferringPhysiciansName(dicomTagReader->getAttributeByName(DCM_ReferringPhysiciansName));

    //Afegim la modalitat de l'estudi
    //Tenir en compte si aquest objecte s'utilitza per fer el dicomclassifierfillerstep que ells omplen la modalitat a partir de les series
    studyModalities = dicomTagReader->getAttributeByName(DCM_ModalitiesInStudy);
    
    foreach(QString modality, studyModalities.split("\\"))
    {
        study->addModality(modality);
    }

    return study;
}

Series* CreateInformationModelObject::createSeries(DICOMTagReader *dicomTagReader)
{
    Series *series = new Series;

    series->setInstanceUID(dicomTagReader->getAttributeByName(DCM_SeriesInstanceUID));
    series->setModality(dicomTagReader->getAttributeByName(DCM_Modality));
    series->setSeriesNumber(dicomTagReader->getAttributeByName(DCM_SeriesNumber));
    series->setDate(dicomTagReader->getAttributeByName(DCM_SeriesDate));
    series->setTime(dicomTagReader->getAttributeByName(DCM_SeriesTime));
    series->setInstitutionName(dicomTagReader->getAttributeByName(DCM_InstitutionName));
    series->setPatientPosition(dicomTagReader->getAttributeByName(DCM_PatientPosition));
    series->setProtocolName(dicomTagReader->getAttributeByName(DCM_ProtocolName));
    series->setDescription(dicomTagReader->getAttributeByName(DCM_SeriesDescription));
    series->setFrameOfReferenceUID(dicomTagReader->getAttributeByName(DCM_FrameOfReferenceUID));
    series->setPositionReferenceIndicator(dicomTagReader->getAttributeByName(DCM_PositionReferenceIndicator));
    series->setManufacturer(dicomTagReader->getAttributeByName(DCM_Manufacturer));

    /// Atributs necessaris pels hanging protocols
    series->setBodyPartExamined(dicomTagReader->getAttributeByName(DCM_BodyPartExamined));
    series->setViewPosition(dicomTagReader->getAttributeByName(DCM_ViewPosition));

    return series;
}

Image* CreateInformationModelObject::createImage(DICOMTagReader *dicomTagReader)
{
    Image * image = new Image();

    image->setSOPInstanceUID(dicomTagReader->getAttributeByName(DCM_SOPInstanceUID));
    image->setInstanceNumber(dicomTagReader->getAttributeByName(DCM_InstanceNumber));

    return image;
}

}; //end udg namespace
