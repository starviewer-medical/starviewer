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

#include "patientfillerinput.h"
#include "logging.h"
#include "dicomtagreader.h"
#include "patient.h"

// TODO Include's temporals mentre no tenim un registre:
#include "imagefillerstep.h"
#include "dicomfileclassifierfillerstep.h"
#include "temporaldimensionfillerstep.h"
#include "mhdfileclassifierstep.h"
#include "orderimagesfillerstep.h"
#include "volumefillerstep.h"
// TODO encara no hi ha suport a KINs i Presentation States, per tant
// fins que no tinguem suport i implementem correctament els respectius
// filler steps no caldrà afegir-los dins del pipeline
//#include "keyimagenotefillerstep.h"
//#include "presentationstatefillerstep.h"

namespace udg {

PatientFiller::PatientFiller(DICOMSource dicomSource, QObject *parent)
 : QObject(parent)
{
    registerSteps();
    m_patientFillerInput = new PatientFillerInput();
    m_imageCounter = 0;

    m_patientFillerInput->setDICOMSource(dicomSource);

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

void PatientFiller::registerSteps()
{
    m_firstStageSteps << new DICOMFileClassifierFillerStep() << new ImageFillerStep();
    m_secondStageSteps << new VolumeFillerStep() << new OrderImagesFillerStep() << new TemporalDimensionFillerStep();

    // TODO encara no hi ha suport a KINs i Presentation States, per tant
    // fins que no tinguem suport i implementem correctament els respectius
    // filler steps no caldrà afegir-los dins del pipeline
    //m_registeredSteps.append(new KeyImageNoteFillerStep());
    //m_registeredSteps.append(new PresentationStateFillerStep());
}

void PatientFiller::processDICOMFile(const DICOMTagReader *dicomTagReader)
{
    Q_ASSERT(dicomTagReader);

    m_patientFillerInput->setDICOMFile(dicomTagReader);

    foreach (PatientFillerStep *fillerStep, m_firstStageSteps)
    {
        fillerStep->fillIndividually();
    }

    emit progress(++m_imageCounter);
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

    // Al acabar hem de reiniciar el comptador d'imatges
    m_imageCounter = 0;
}

QList<Patient*> PatientFiller::processFiles(const QStringList &files)
{
    // HACK per fer el cas especial dels mhd. Això està així perquè perquè el mètode
    // processDICOMFile s'espera un DICOMTagReader, que no podem crear a partir d'un mhd.
    // El filler d'mhd realment no s'està utilitzant a dintre del process de fillers com la resta.
    if (containsMHDFiles(files))
    {
        return processMHDFiles(files);
    }
    else
    {
        return processDICOMFiles(files);
    }
}

bool PatientFiller::containsMHDFiles(const QStringList &files)
{
    if (!files.isEmpty())
    {
        return files.first().endsWith(".mhd", Qt::CaseInsensitive);
    }
    else
    {
        return false;
    }
}

QList<Patient*> PatientFiller::processMHDFiles(const QStringList &files)
{
    PatientFillerInput patientFillerInput;
    m_imageCounter = 0;
    foreach (const QString &file, files)
    {
        patientFillerInput.setFile(file);

        MHDFileClassifierStep mhdFileClassiferStep;
        mhdFileClassiferStep.setInput(&patientFillerInput);
        if (!mhdFileClassiferStep.fillIndividually())
        {
            DEBUG_LOG("No s'ha pogut processar el fitxer MHD: " + file);
            ERROR_LOG("No s'ha pogut processar el fitxer MHD: " + file);
        }
        emit progress(++m_imageCounter);
    }

    return patientFillerInput.getPatientList();
}

QList<Patient*> PatientFiller::processDICOMFiles(const QStringList &files)
{
    m_imageCounter = 0;

    foreach (const QString &dicomFile, files)
    {
        DICOMTagReader *dicomTagReader = new DICOMTagReader(dicomFile);
        if (dicomTagReader->canReadFile())
        {
            this->processDICOMFile(dicomTagReader);
        }
    }

    this->finishDICOMFilesProcess();

    return m_patientFillerInput->getPatientList();
}

}
