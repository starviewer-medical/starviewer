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

#include "transdifferencetooldata.h"
#include "q2dviewer.h"
#include "volume.h"
#include "logging.h"

namespace udg {

TransDifferenceToolData::TransDifferenceToolData()
 : ToolData(), m_inputVolume(0), m_differenceVolume(0)
{
}

TransDifferenceToolData::~TransDifferenceToolData()
{
}

void TransDifferenceToolData::setInputVolume(Volume *input)
{
    m_inputVolume = input;
    // Quan canviem l'input cal invalidar el volum diferència
    m_differenceVolume = 0;
    // Quan canviem l'input posem totes les transicions a 0
    m_sliceTranslations = QVector<QPair<int, int> >(m_inputVolume->getDimensions()[2], QPair<int, int>(0, 0));
}

void TransDifferenceToolData::setDifferenceVolume(Volume *input)
{
    m_differenceVolume = input;
}
}
