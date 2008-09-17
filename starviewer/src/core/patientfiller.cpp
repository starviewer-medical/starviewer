/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "patientfiller.h"

#include <QApplication>
#include <QTime>
#include <QtAlgorithms>

#include "patientfillerinput.h"
#include "logging.h"
#include "dicomtagreader.h"

// TODO Include's temporals mentre no tenim un registre:
#include "keyimagenotefillerstep.h"
#include "imagefillerstep.h"
#include "dicomfileclassifierfillerstep.h"
#include "presentationstatefillerstep.h"
#include "temporaldimensionfillerstep.h"
#include "mhdfileclassifierstep.h"
#include "orderimagesfillerstep.h"
// TODO de moment deixem fora el ReferenceLinesFillerStep perquè
// encara no fem ús de la informació que recopila
// #include "referencelinesfillerstep.h"

namespace udg {

PatientFiller::PatientFiller(QObject * parent) : QObject(parent)
{
    registerSteps();
    m_patientFillerInput = new PatientFillerInput();
    m_imageCounter = 0;
}

PatientFiller::~PatientFiller()
{
    foreach (PatientFillerStep* fillerStep, m_registeredSteps)
    {
        delete fillerStep;
    }

    delete m_patientFillerInput;
}

// Mètode intern per poder realitzar l'ordenació dels patientfiller
bool patientFillerMorePriorityFirst(const PatientFillerStep *s1, const PatientFillerStep *s2)
{
    return (*s1) < (*s2);
}

void PatientFiller::registerSteps()
{
    m_registeredSteps.append(new KeyImageNoteFillerStep() );
    m_registeredSteps.append(new ImageFillerStep() );
    m_registeredSteps.append(new DICOMFileClassifierFillerStep() );
    m_registeredSteps.append(new PresentationStateFillerStep() );
    m_registeredSteps.append(new TemporalDimensionFillerStep() );
    m_registeredSteps.append(new MHDFileClassifierStep() );
    m_registeredSteps.append(new OrderImagesFillerStep() );
    // TODO de moment deixem fora el ReferenceLinesFillerStep perquè
    // encara no fem ús de la informació que recopila
    //m_registeredSteps.append(new ReferenceLinesFillerStep() );
}

void PatientFiller::processPatientFillerStep(PatientFillerStep *patientFillerStep, PatientFillerInput *input)
{
    QTime time;
    patientFillerStep->setInput(input);
    time.start();
    patientFillerStep->fill();
    DEBUG_LOG( QString(patientFillerStep->name() + " ha trigat: %1 ").arg( time.elapsed() ));
}

void PatientFiller::processDICOMFile(DICOMTagReader *dicomTagReader)
{
    Q_ASSERT(dicomTagReader);

    m_patientFillerInput->setDICOMFile(dicomTagReader);

    QList<PatientFillerStep*> processedFillerSteps;
    QList<PatientFillerStep*> candidatesFillerSteps = m_registeredSteps;
    bool continueIterating = true;

    while (!candidatesFillerSteps.isEmpty() && continueIterating)
    {
        QList<PatientFillerStep*> fillerStepsToProcess;
        QList<PatientFillerStep*> newCandidatesFillerSteps;
        continueIterating = false;

        for (int i = 0; i < candidatesFillerSteps.size(); ++i)
        {
            if (m_patientFillerInput->hasAllLabels( candidatesFillerSteps.at(i)->getRequiredLabels() ))
            {
                fillerStepsToProcess.append( candidatesFillerSteps.at(i) );
                continueIterating = true;
            }
            else
            {
                newCandidatesFillerSteps.append( candidatesFillerSteps.at(i) );
            }
        }
        candidatesFillerSteps = newCandidatesFillerSteps;

        qSort(fillerStepsToProcess.begin(), fillerStepsToProcess.end(), patientFillerMorePriorityFirst); // Ordenem segons la seva prioritat

        foreach (PatientFillerStep *fillerStep, fillerStepsToProcess)
        {
            fillerStep->setInput(m_patientFillerInput);
            fillerStep->fillIndividually();
        }
    }

    m_patientFillerInput->initializeAllLabels();

    emit progress(m_imageCounter++);
}

void PatientFiller::finishDICOMFilesProcess()
{
    foreach (PatientFillerStep *fillerStep, m_registeredSteps)
    {
        fillerStep->postProcessing();
    }

    emit patientProcessed(m_patientFillerInput->getPatient());

    // Al acabar hem de reiniciar el comptador d'imatges
    m_imageCounter = 0;
}

void PatientFiller::abortDICOMFilesProcess()
{
    foreach (PatientFillerStep *fillerStep, m_registeredSteps)
    {
        fillerStep->abort();
    }
    
    m_patientFillerInput->initialize();

    // Reinicialitzem el comptador d'imatges
    m_imageCounter = 0;

    emit DICOMFilesProcessAborted();
}

QList<Patient*> PatientFiller::processDICOMFileList(QStringList dicomFiles)
{
    m_imageCounter = 0;

    foreach(QString dicomFile, dicomFiles)
    {
        DICOMTagReader *dicomTagReader = new DICOMTagReader(dicomFile);

        this->processDICOMFile( dicomTagReader );

        emit progress(++m_imageCounter);
    }

    foreach(PatientFillerStep *fillerStep, m_registeredSteps)
    {
        fillerStep->postProcessing();
    }

    return m_patientFillerInput->getPatientsList();
}
}
