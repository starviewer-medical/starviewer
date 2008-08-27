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

// TODO Include's temporals mentre no tenim un registre:
#include "keyimagenotefillerstep.h"
#include "imagefillerstep.h"
#include "dicomfileclassifierfillerstep.h"
#include "presentationstatefillerstep.h"
#include "temporaldimensionfillerstep.h"
#include "volumegeneratorstep.h"
#include "mhdfileclassifierstep.h"
#include "orderimagesfillerstep.h"
// TODO de moment deixem fora el ReferenceLinesFillerStep perquè
// encara no fem ús de la informació que recopila
// #include "referencelinesfillerstep.h"

namespace udg {

const QString UntilEndLabel = "UntilEndLabel1234|@#~";

PatientFiller::PatientFiller(QObject * parent) :QObject(parent)
{
    registerSteps();
    m_patientFillerInput = new PatientFillerInput();
}

PatientFiller::~PatientFiller()
{
    foreach (PatientFillerStep* fillerStep, m_registeredSteps)
    {
        delete fillerStep;
    }

    delete m_patientFillerInput;
}

void PatientFiller::fill(PatientFillerInput *input)
{
    fillUntil(input, UntilEndLabel);
}

// Mètode intern per poder realitzar l'ordenació dels patientfiller
bool patientFillerMorePriorityFirst(const PatientFillerStep *s1, const PatientFillerStep *s2)
{
    return (*s1) < (*s2);
}

void PatientFiller::fillUntil(PatientFillerInput *input, QString stopLabel)
{
    QList<PatientFillerStep*> processedFillerSteps;
    QList<PatientFillerStep*> candidatesFillerSteps = m_registeredSteps;
    bool continueIterating = true;

    DEBUG_LOG("Entrem a fillUntil amb stopLabel = " + stopLabel);

    int totalFillerSteps = m_registeredSteps.size();

    emit progress(1);
    qApp->processEvents();

    while (!input->getLabels().contains(stopLabel) && !candidatesFillerSteps.isEmpty() && continueIterating)
    {
        QList<PatientFillerStep*> fillerStepsToProcess;
        QList<PatientFillerStep*> newCandidatesFillerSteps;
        continueIterating = false;

        for (int i = 0; i < candidatesFillerSteps.size(); ++i)
        {
            if (input->hasAllLabels( candidatesFillerSteps.at(i)->getRequiredLabels() ))
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
            processPatientFillerStep(fillerStep, input);
        }
        emit progress(totalFillerSteps - candidatesFillerSteps.size());
        qApp->processEvents();
    }
    emit progress(100);
}

void PatientFiller::registerSteps()
{
    m_registeredSteps.append(new KeyImageNoteFillerStep() );
    m_registeredSteps.append(new ImageFillerStep() );
    m_registeredSteps.append(new DICOMFileClassifierFillerStep() );
    m_registeredSteps.append(new PresentationStateFillerStep() );
    m_registeredSteps.append(new TemporalDimensionFillerStep() );
    m_registeredSteps.append(new VolumeGeneratorStep() );
    m_registeredSteps.append(new MHDFileClassifierStep() );
    m_registeredSteps.append(new OrderImagesFillerStep() );
// TODO de moment deixem fora el ReferenceLinesFillerStep perquè
// encara no fem ús de la informació que recopila
//     m_registeredSteps.append(new ReferenceLinesFillerStep() );
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

    while (!m_patientFillerInput->getLabels().contains(UntilEndLabel) && !candidatesFillerSteps.isEmpty() && continueIterating)
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

}

void PatientFiller::finishDICOMFilesProcess()
{
    DEBUG_LOG( "eeeeeeeeeeeeeeeeeeeee" );
        
    foreach (PatientFillerStep *fillerStep, m_registeredSteps)
    {
        fillerStep->postProcessing();
    }

    emit patientProcessed(m_patientFillerInput->getPatient());
}
}
