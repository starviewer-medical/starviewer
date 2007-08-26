/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "patientfiller.h"

#include <QApplication>
#include <QTime>

#include "patientfillerinput.h"
#include "logging.h"

// TODO Include's temporals mentre no tenim un registre:
#include "keyimagenotefillerstep.h"
#include "imagefillerstep.h"
#include "dicomfileclassifierfillerstep.h"
#include "ctfillerstep.h"
#include "presentationstatefillerstep.h"
#include "temporaldimensionfillerstep.h"
#include "volumegeneratorstep.h"
#include "referencelinesfillerstep.h"

namespace udg {

const QString UntilEndLabel = "UntilEndLabel1234|@#~";

PatientFiller::PatientFiller(QObject * parent) :QObject(parent)
{
    registerSteps();
}

PatientFiller::~PatientFiller()
{
    foreach (PatientFillerStep* fillerStep, m_registeredSteps)
    {
        delete fillerStep;
    }
}

void PatientFiller::fill(PatientFillerInput *input)
{
    fillUntil(input, UntilEndLabel);
}

void PatientFiller::fillUntil(PatientFillerInput *input, QString stopLabel)
{
    QList<PatientFillerStep*> processedFillerSteps;
    QList<PatientFillerStep*> candidatesFillerSteps = m_registeredSteps;
    bool continueIterating = true;

    DEBUG_LOG("Entrem a fillUntil amb stopLabel = " + stopLabel);

    int totalFillerSteps = m_registeredSteps.size();

    emit progress(0);
    qApp->processEvents();
    while (!input->getLabels().contains(stopLabel) && !candidatesFillerSteps.isEmpty() && continueIterating)
    {
        QList<PatientFillerStep*> fillerStepsToProcess;
        continueIterating = false;

        for (int i = 0; i < candidatesFillerSteps.size(); ++i)
        {
            if (input->hasAllLabels( candidatesFillerSteps.at(i)->getRequiredLabels() ))
            {
                fillerStepsToProcess.append( candidatesFillerSteps.at(i) );
                candidatesFillerSteps.removeAt(i);

                continueIterating = true;
            }
        }

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
    m_registeredSteps.append(new CTFillerStep() );
    m_registeredSteps.append(new PresentationStateFillerStep() );
    m_registeredSteps.append(new TemporalDimensionFillerStep() );
    m_registeredSteps.append(new VolumeGeneratorStep() );
    m_registeredSteps.append(new ReferenceLinesFillerStep() );
}

void PatientFiller::processPatientFillerStep(PatientFillerStep *patientFillerStep, PatientFillerInput *input)
{
    QTime time;
    patientFillerStep->setInput(input);
    time.start();
    patientFillerStep->fill();
    DEBUG_LOG( QString(patientFillerStep->name() + " ha trigat: %1 ").arg( time.elapsed() ));
}

}
