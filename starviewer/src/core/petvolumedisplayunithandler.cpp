#include "petvolumedisplayunithandler.h"

#include "transferfunctionmodel.h"
#include "volumedisplayunit.h"

namespace udg {

const char * const DefaultTransferFunctionName = "Black & White Inverse";

PETVolumeDisplayUnitHandler::PETVolumeDisplayUnitHandler()
{
    getTransferFunctionModel()->loadDefault2DTransferFunctions();
}

PETVolumeDisplayUnitHandler::~PETVolumeDisplayUnitHandler()
{
}

void PETVolumeDisplayUnitHandler::setupDefaultTransferFunctions()
{
    TransferFunction transferFunction;
    transferFunction.setName(DefaultTransferFunctionName);
    int index = m_transferFunctionModel->getIndexOf(transferFunction, true);

    if (index >= 0)
    {
        this->getMainVolumeDisplayUnit()->setTransferFunction(m_transferFunctionModel->getTransferFunction(index));
    }
}

}
