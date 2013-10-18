#include "petvolumedisplayunithandler.h"

#include "transferfunctionmodel.h"

namespace udg {

PETVolumeDisplayUnitHandler::PETVolumeDisplayUnitHandler()
{
    getTransferFunctionModel()->loadDefault2DTransferFunctions();
}

PETVolumeDisplayUnitHandler::~PETVolumeDisplayUnitHandler()
{
}

}
