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

#include "petctvolumedisplayunithandler.h"

#include "transferfunctionmodel.h"
#include "transferfunctionmodelfiller.h"
#include "volume.h"
#include "image.h"
#include "series.h"
#include "volumedisplayunit.h"
#include "imagepipeline.h"
#include "defaulttransferfunctionselector.h"

namespace udg {

PETCTVolumeDisplayUnitHandler::PETCTVolumeDisplayUnitHandler(QObject *parent)
 : PairedVolumeDisplayUnitHandler(parent)
{
}

PETCTVolumeDisplayUnitHandler::~PETCTVolumeDisplayUnitHandler()
{
}

void PETCTVolumeDisplayUnitHandler::setupDefaultTransferFunctions()
{
    VolumeDisplayUnit *petUnit = getPETDisplayUnit();

    if (petUnit)
    {
        int index = DefaultTransferFunctionSelector().getDefaultTransferFunctionForPETCT(m_transferFunctionModel);

        if (index >= 0)
        {
            petUnit->setTransferFunction(m_transferFunctionModel->getTransferFunction(index));
        }
    }
}

void PETCTVolumeDisplayUnitHandler::updateMainDisplayUnitIndex()
{
    if (getNumberOfInputs() == 2)
    {
        if (m_displayUnits.at(1)->getVolume()->getModality() == "CT")
        {
            m_displayUnits.move(0, 1);
        }
    }
}

VolumeDisplayUnit* PETCTVolumeDisplayUnitHandler::getPETDisplayUnit() const
{
    VolumeDisplayUnit *petUnit = 0;

    foreach (VolumeDisplayUnit *unit, m_displayUnits)
    {
        if (unit->getVolume()->getModality() == "PT")
        {
            petUnit = unit;
            break;
        }
    }

    return petUnit;
}

} // End namespace udg
