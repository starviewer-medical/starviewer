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

PETCTVolumeDisplayUnitHandler::PETCTVolumeDisplayUnitHandler()
 : PairedVolumeDisplayUnitHandler()
{
    TransferFunctionModelFiller().add2DTransferFunctions(getTransferFunctionModel());
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
        if (m_displayUnits.at(1)->getVolume()->getImage(0)->getParentSeries()->getModality() == "CT")
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
        if (unit->getVolume()->getImage(0)->getParentSeries()->getModality() == "PT")
        {
            petUnit = unit;
            break;
        }
    }

    return petUnit;
}

} // End namespace udg
