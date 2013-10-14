#include "petctvolumedisplayunithandler.h"

#include "transferfunctionmodel.h"
#include "volume.h"
#include "image.h"
#include "series.h"
#include "volumedisplayunit.h"
#include "imagepipeline.h"

namespace udg {

PETCTVolumeDisplayUnitHandler::PETCTVolumeDisplayUnitHandler()
 : PairedVolumeDisplayUnitHandler()
{
    getTransferFunctionModel()->loadDefault2DTransferFunctions();
}

PETCTVolumeDisplayUnitHandler::~PETCTVolumeDisplayUnitHandler()
{
}

void PETCTVolumeDisplayUnitHandler::setupDefaultTransferFunctions()
{
    // If we have two volumes, the PET will have the first default 2D transfer function
    if (getNumberOfInputs() == 2)
    {
        if (m_transferFunctionModel->rowCount() > 0)
        {
            VolumeDisplayUnit *petUnit = getPETDisplayUnit();

            if (petUnit)
            {
                petUnit->setTransferFunction(m_transferFunctionModel->getTransferFunction(0));
            }
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
