/*************************************************************************************
  Copyright (C) 2014 Laboratori de Gràfics i Imatge, Universitat de Girona &
  Institut de Diagnòstic per la Imatge.
  Girona 2014. All rights reserved.
  http://starviewer.udg.edu

  This file is part of the Starviewer (Medical Imaging Software) open source project.
  It is subject to the license terms in the LICENSE file found in the top-level
  directory of this distribution and at http://starviewer.udg.edu/license. No part of
  the Starviewer (Medical Imaging Software) open source project, including this file,
  may be copied, modified, propagated, or distributed except according to the
  terms contained in the LICENSE file.
 *************************************************************************************/

#include "createinformationmodelobject.h"

#include <QString>

#include "patient.h"
#include "study.h"
#include "series.h"
#include "image.h"
#include "dicomtagreader.h"

namespace udg {

Patient* CreateInformationModelObject::createPatient(const DICOMTagReader *dicomTagReader)
{
    Patient *patient = new Patient;

    patient->setFullName(dicomTagReader->getValueAttributeAsQString(DICOMPatientName));
    patient->setID(dicomTagReader->getValueAttributeAsQString(DICOMPatientID));
    patient->setBirthDate(dicomTagReader->getValueAttributeAsQString(DICOMPatientBirthDate));
    patient->setSex(dicomTagReader->getValueAttributeAsQString(DICOMPatientSex));

    return patient;
}

Study* CreateInformationModelObject::createStudy(const DICOMTagReader *dicomTagReader)
{
    Study *study = new Study;
    QString studyModalities;

    study->setInstanceUID(dicomTagReader->getValueAttributeAsQString(DICOMStudyInstanceUID));
    study->setDate(dicomTagReader->getValueAttributeAsQString(DICOMStudyDate));
    study->setTime(dicomTagReader->getValueAttributeAsQString(DICOMStudyTime));
    study->setID(dicomTagReader->getValueAttributeAsQString(DICOMStudyID));
    study->setAccessionNumber(dicomTagReader->getValueAttributeAsQString(DICOMAccessionNumber));
    study->setDescription(dicomTagReader->getValueAttributeAsQString(DICOMStudyDescription));
    study->setPatientAge(dicomTagReader->getValueAttributeAsQString(DICOMPatientAge));
    study->setHeight(dicomTagReader->getValueAttributeAsQString(DICOMPatientSize).toDouble());
    study->setWeight(dicomTagReader->getValueAttributeAsQString(DICOMPatientWeight).toDouble());
    study->setReferringPhysiciansName(dicomTagReader->getValueAttributeAsQString(DICOMReferringPhysicianName));

    // Afegim la modalitat de l'estudi
    // Tenir en compte si aquest objecte s'utilitza per fer el dicomclassifierfillerstep que ells omplen la modalitat a partir de les series
    studyModalities = dicomTagReader->getValueAttributeAsQString(DICOMModalitiesInStudy);

    foreach (const QString &modality, studyModalities.split("\\"))
    {
        study->addModality(modality);
    }

    return study;
}

Series* CreateInformationModelObject::createSeries(const DICOMTagReader *dicomTagReader)
{
    Series *series = new Series;

    series->setSOPClassUID(dicomTagReader->getValueAttributeAsQString(DICOMSOPClassUID));
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
    if (!laterality.isEmpty())
    {
        series->setLaterality(laterality.at(0));
    }

    return series;
}

Image* CreateInformationModelObject::createImage(const DICOMTagReader *dicomTagReader)
{
    Image *image = new Image();

    image->setSOPInstanceUID(dicomTagReader->getValueAttributeAsQString(DICOMSOPInstanceUID));
    image->setInstanceNumber(dicomTagReader->getValueAttributeAsQString(DICOMInstanceNumber));

    return image;
}

}; // End udg namespace
