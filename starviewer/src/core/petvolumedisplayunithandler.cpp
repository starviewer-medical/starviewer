#include "petvolumedisplayunithandler.h"

#include "defaulttransferfunctionselector.h"
#include "transferfunctionmodel.h"
#include "transferfunctionmodelfiller.h"
#include "volumedisplayunit.h"

namespace udg {

PETVolumeDisplayUnitHandler::PETVolumeDisplayUnitHandler()
{
    TransferFunctionModelFiller().add2DTransferFunctions(getTransferFunctionModel());
}

PETVolumeDisplayUnitHandler::~PETVolumeDisplayUnitHandler()
{
}

void PETVolumeDisplayUnitHandler::setupDefaultTransferFunctions()
{
    int index = DefaultTransferFunctionSelector().getDefaultTransferFunctionForPET(m_transferFunctionModel);

    if (index >= 0)
    {
        this->getMainVolumeDisplayUnit()->setTransferFunction(m_transferFunctionModel->getTransferFunction(index));
    }
}

}
