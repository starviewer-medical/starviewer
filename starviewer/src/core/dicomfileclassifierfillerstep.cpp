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

#include "dicomfileclassifierfillerstep.h"

#include "createinformationmodelobject.h"
#include "dicomtagreader.h"
#include "patient.h"
#include "patientfillerinput.h"

namespace udg {

DICOMFileClassifierFillerStep::DICOMFileClassifierFillerStep()
{
}

DICOMFileClassifierFillerStep::~DICOMFileClassifierFillerStep()
{
}

bool DICOMFileClassifierFillerStep::fillIndividually()
{
    Q_ASSERT(m_input);

    const DICOMTagReader *dicomReader = m_input->getDICOMFile();

    Q_ASSERT(dicomReader);

    // Don't generate anything for invalid DICOM files
    if (!dicomReader->canReadFile())
    {
        return false;
    }

    QString patientID = dicomReader->getValueAttributeAsQString(DICOMPatientID);
    QString studyUID = dicomReader->getValueAttributeAsQString(DICOMStudyInstanceUID);
    QString seriesUID = dicomReader->getValueAttributeAsQString(DICOMSeriesInstanceUID);

    Patient *patient = m_input->getPatientByID(patientID);

    if (!patient)
    {
        patient = CreateInformationModelObject::createPatient(dicomReader);
        m_input->addPatient(patient);
    }

    Study *study = patient->getStudy(studyUID);

    if (!study)
    {
        study = CreateInformationModelObject::createStudy(dicomReader);
        patient->addStudy(study);
    }

    Series *series = study->getSeries(seriesUID);

    if (!series)
    {
        series = CreateInformationModelObject::createSeries(dicomReader);
        study->addSeries(series);
    }

    m_input->setCurrentSeries(series);

    return true;
}

}
