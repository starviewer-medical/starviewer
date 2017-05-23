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

#include "nmctvolumedisplayunithandler.h"

#include "transferfunctionmodel.h"
#include "transferfunctionmodelfiller.h"
#include "volume.h"
#include "image.h"
#include "series.h"
#include "volumedisplayunit.h"
#include "imagepipeline.h"
#include "defaulttransferfunctionselector.h"


namespace udg {

NMCTVolumeDisplayUnitHandler::NMCTVolumeDisplayUnitHandler(QObject *parent)
    : PairedVolumeDisplayUnitHandler(parent)
{
    TransferFunctionModelFiller().add2DTransferFunctions(getTransferFunctionModel());
}

NMCTVolumeDisplayUnitHandler::~NMCTVolumeDisplayUnitHandler()
{
}

void NMCTVolumeDisplayUnitHandler::setupDefaultTransferFunctions()
{
    VolumeDisplayUnit *nmUnit = getNMDisplayUnit();

    if (nmUnit)
    {
        int index = DefaultTransferFunctionSelector().getDefaultTransferFunctionForNMCT(m_transferFunctionModel);

        if (index >= 0)
        {
            nmUnit->setTransferFunction(m_transferFunctionModel->getTransferFunction(index));
        }
    }
}

void NMCTVolumeDisplayUnitHandler::updateMainDisplayUnitIndex()
{
    if (getNumberOfInputs() == 2)
    {
        if (m_displayUnits.at(1)->getVolume()->getModality() == "CT")
        {
            m_displayUnits.move(0, 1);
        }
    }
}

VolumeDisplayUnit* NMCTVolumeDisplayUnitHandler::getNMDisplayUnit() const
{
    VolumeDisplayUnit *nmUnit = 0;

    foreach (VolumeDisplayUnit *unit, m_displayUnits)
    {
        if (unit->getVolume()->getModality() == "NM")
        {
            nmUnit = unit;
            break;
        }
    }

    return nmUnit;
}

} // End namespace udg
