#include "singlevolumedisplayunithandler.h"

namespace udg {

SingleVolumeDisplayUnitHandler::SingleVolumeDisplayUnitHandler()
 : GenericVolumeDisplayUnitHandler()
{
}

SingleVolumeDisplayUnitHandler::~SingleVolumeDisplayUnitHandler()
{
}

int SingleVolumeDisplayUnitHandler::getMaximumNumberOfInputs() const
{
    return 1;
}

} // End namespace udg
