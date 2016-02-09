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

#include "patientfillerstep.h"

#include "image.h"
#include "series.h"

namespace udg {

PatientFillerStep::PatientFillerStep()
    : m_input(nullptr)
{
}

PatientFillerStep::~PatientFillerStep()
{
}

void PatientFillerStep::setInput(PatientFillerInput *input)
{
    m_input = input;
}

bool PatientFillerStep::fillIndividually()
{
    return false;
}

void PatientFillerStep::postProcessing()
{
}

bool PatientFillerStep::isImageSeries(const Series *series)
{
    return Image::getSupportedModalities().contains(series->getModality());
}

bool PatientFillerStep::isKeyImageNoteSeries(const Series *series)
{
    return series->getModality() == "KO";
}

bool PatientFillerStep::isPresentationStateSeries(const Series *series)
{
    return series->getModality() == "PR";
}

}
