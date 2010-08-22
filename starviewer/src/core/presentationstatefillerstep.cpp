/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "presentationstatefillerstep.h"
#include "logging.h"
#include "patientfillerinput.h"
#include "dicomtagreader.h"
#include "study.h"
#include "series.h"

#include <dvpstat.h>
#include <dcdatset.h>
#include <QDir>

namespace udg {

PresentationStateFillerStep::PresentationStateFillerStep(): PatientFillerStep()
{
    m_requiredLabelsList << "DICOMFileClassifierFillerStep";
}

PresentationStateFillerStep::~PresentationStateFillerStep()
{
}

bool PresentationStateFillerStep::fillIndividually()
{
    Q_ASSERT(m_input);

    if (isPresentationStateSeries(m_input->getCurrentSeries()))
    {
        processPresentationState();
    }
    
    return true;
}

void PresentationStateFillerStep::processPresentationState()
{
    DICOMTagReader *dicomReader = m_input->getDICOMFile();
    OFCondition status;
    
    m_presentationStateHandler = new DVPresentationState;
    status = m_presentationStateHandler->read(*dicomReader->getDcmDataset());

    if(status.good())
    {
        m_input->getCurrentSeries()->getParentStudy()->addPresentationState("PR: " + dicomReader->getValueAttributeAsQString(DICOMContentLabel) + "\\" + dicomReader->getValueAttributeAsQString(DICOMSOPInstanceUID), dicomReader->getFileName());
    }
    else
    {
        DEBUG_LOG("Error en la lectura del presentation state");
    }
}
}

