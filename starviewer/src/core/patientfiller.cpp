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

#include "patientfiller.h"

#include "dicomfileclassifierfillerstep.h"
#include "dicomtagreader.h"
#include "encapsulateddocumentfillerstep.h"
#include "imagefillerstep.h"
//#include "keyimagenotefillerstep.h"       // future use
#include "logging.h"
#include "mhdfileclassifierstep.h"
#include "orderimagesfillerstep.h"
#include "patient.h"
#include "patientfillerinput.h"
#include "patientfillerstep.h"
//#include "presentationstatefillerstep.h"  // future use
#include "temporaldimensionfillerstep.h"
#include "volumefillerstep.h"

namespace udg {

namespace {

// Returns true if the list contains MHD files and false otherwise. Only the first file is checked.
bool containsMHDFiles(const QStringList &files)
{
    return !files.isEmpty() && files.first().endsWith(".mhd", Qt::CaseInsensitive);
}

}

PatientFiller::PatientFiller(DICOMSource dicomSource, QObject *parent)
 : QObject(parent), m_numberOfProcessedFiles(0)
{
    createSteps();

    m_patientFillerInput = new PatientFillerInput();
    m_patientFillerInput->setDICOMSource(std::move(dicomSource));

    foreach (PatientFillerStep *fillerStep, m_firstStageSteps)
    {
        fillerStep->setInput(m_patientFillerInput);
    }
    foreach (PatientFillerStep *fillerStep, m_secondStageSteps)
    {
        fillerStep->setInput(m_patientFillerInput);
    }
}

PatientFiller::~PatientFiller()
{
    foreach (PatientFillerStep *fillerStep, m_firstStageSteps)
    {
        delete fillerStep;
    }
    foreach (PatientFillerStep *fillerStep, m_secondStageSteps)
    {
        delete fillerStep;
    }

    delete m_patientFillerInput;
}

void PatientFiller::processDICOMFile(const DICOMTagReader *dicomTagReader)
{
    Q_ASSERT(dicomTagReader);

    m_patientFillerInput->setDICOMFile(dicomTagReader);

    foreach (PatientFillerStep *fillerStep, m_firstStageSteps)
    {
        // If some step fails to fill we can skip the rest of steps.
        // This is done in order to skip further processing of non-DICOM files (e.g. thumbnails when opening files from a directory).
        // Note: this is done thinking that the first stage steps are DICOMFileClassifierFillerStep and ImageFillerStep;
        // the condition may have to change if this circumstance changes.
        if (!fillerStep->fillIndividually())
        {
            break;
        }
    }

    emit progress(++m_numberOfProcessedFiles);
}

void PatientFiller::finishDICOMFilesProcess()
{
    foreach (Patient *patient, m_patientFillerInput->getPatientList())
    {
        foreach (Series *series, patient->getStudies().first()->getSeries())
        {
            m_patientFillerInput->setCurrentSeries(series);

            foreach (const QList<Image*> &currentImages, m_patientFillerInput->getCurrentImagesHistory())
            {
                m_patientFillerInput->setCurrentImages(currentImages, false);

                foreach (PatientFillerStep *fillerStep, m_secondStageSteps)
                {
                    fillerStep->fillIndividually();
                }
            }
        }
    }

    foreach (PatientFillerStep *fillerStep, m_secondStageSteps)
    {
        fillerStep->postProcessing();
    }

    emit patientProcessed(m_patientFillerInput->getPatient());
}

QList<Patient*> PatientFiller::processFiles(const QStringList &files)
{
    if (containsMHDFiles(files))
    {
        return processMHDFiles(files);
    }
    else
    {
        return processDICOMFiles(files);
    }
}

void PatientFiller::createSteps()
{
    m_firstStageSteps << new DICOMFileClassifierFillerStep() << new ImageFillerStep() << new EncapsulatedDocumentFillerStep();
    m_secondStageSteps << new VolumeFillerStep() << new OrderImagesFillerStep() << new TemporalDimensionFillerStep();
}

QList<Patient*> PatientFiller::processMHDFiles(const QStringList &files)
{
    MHDFileClassifierStep mhdFileClassiferStep;
    mhdFileClassiferStep.setInput(m_patientFillerInput);

    foreach (const QString &file, files)
    {
        m_patientFillerInput->setFile(file);

        if (!mhdFileClassiferStep.fillIndividually())
        {
            ERROR_LOG("Can't process MHD file " + file);
        }

        emit progress(++m_numberOfProcessedFiles);
    }

    return m_patientFillerInput->getPatientList();
}

QList<Patient*> PatientFiller::processDICOMFiles(const QStringList &files)
{
    foreach (const QString &dicomFile, files)
    {
        // The DICOMTagReader is deleted by the PatientFillerInput
        DICOMTagReader *dicomTagReader = new DICOMTagReader(dicomFile);
        this->processDICOMFile(dicomTagReader);
    }

    this->finishDICOMFilesProcess();

    return m_patientFillerInput->getPatientList();
}

}
