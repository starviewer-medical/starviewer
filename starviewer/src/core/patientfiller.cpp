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
#include "nondicomfileclassifierfillerstep.h"
#include "orderimagesfillerstep.h"
#include "patient.h"
#include "patientfillerinput.h"
#include "patientfillerstep.h"
//#include "presentationstatefillerstep.h"  // future use
#include "temporaldimensionfillerstep.h"
#include "volumefillerstep.h"

namespace udg {

namespace {

// Returns true if the given file is a DICOM file.
bool isDicom(const QString& fileName)
{
    return DICOMTagReader(fileName).canReadFile();
}

}

PatientFiller::PatientFiller(DICOMSource dicomSource, QObject *parent)
 : QObject(parent), m_dicomMode(true), m_numberOfProcessedFiles(0)
{
    m_patientFillerInput = new PatientFillerInput();
    m_patientFillerInput->setDICOMSource(std::move(dicomSource));
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

    if (m_firstStageSteps.isEmpty())
    {
        createSteps();
    }

    m_patientFillerInput->setDICOMFile(dicomTagReader);

    processCurrentFile();
}

void PatientFiller::finishFilesProcessing()
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
    if (files.isEmpty())
    {
        return QList<Patient*>();
    }

    m_dicomMode = isDicom(files.first());

    createSteps();

    foreach (const QString &file, files)
    {
        if (m_dicomMode)
        {
            // The DICOMTagReader is deleted by the PatientFillerInput
            DICOMTagReader *dicomTagReader = new DICOMTagReader(file);
            m_patientFillerInput->setDICOMFile(dicomTagReader);
        }
        else
        {
            m_patientFillerInput->setFile(file);
        }

        processCurrentFile();
    }

    this->finishFilesProcessing();

    return m_patientFillerInput->getPatientList();
}

void PatientFiller::createSteps()
{
    if (m_dicomMode)
    {
        m_firstStageSteps << new DICOMFileClassifierFillerStep() << new ImageFillerStep() << new EncapsulatedDocumentFillerStep();
        m_secondStageSteps << new VolumeFillerStep() << new OrderImagesFillerStep() << new TemporalDimensionFillerStep();
    }
    else
    {
        m_firstStageSteps << new NonDicomFileClassifierFillerStep();
        m_secondStageSteps << new VolumeFillerStep(true);
    }

    foreach (PatientFillerStep *fillerStep, m_firstStageSteps)
    {
        fillerStep->setInput(m_patientFillerInput);
    }

    foreach (PatientFillerStep *fillerStep, m_secondStageSteps)
    {
        fillerStep->setInput(m_patientFillerInput);
    }
}

void PatientFiller::processCurrentFile()
{
    foreach (PatientFillerStep *fillerStep, m_firstStageSteps)
    {
        // If some step fails to fill we can skip the rest of steps.
        // This is done in order to skip further processing of files when some error is found
        // (e.g. trying to process thumbnails as DICOM when opening files from a directory or finding an unrecognized file type in non-DICOM mode).
        // Note: this works as long as the first step is used to determine the file type, as is now the case;
        // the condition may have to change if this circumstance changes.
        if (!fillerStep->fillIndividually())
        {
            WARN_LOG(QString("Can't process file %1 as %2")
                     .arg(m_dicomMode ? m_patientFillerInput->getDICOMFile()->getFileName() : m_patientFillerInput->getFile())
                     .arg(m_dicomMode ? "DICOM" : "non-DICOM"));
            break;
        }
    }

    emit progress(++m_numberOfProcessedFiles);
}

}
