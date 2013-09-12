#include "pairedvolumedisplayunithandler.h"

namespace udg {

PairedVolumeDisplayUnitHandler::PairedVolumeDisplayUnitHandler()
 : GenericVolumeDisplayUnitHandler()
{
}

PairedVolumeDisplayUnitHandler::~PairedVolumeDisplayUnitHandler()
{
}

int PairedVolumeDisplayUnitHandler::getMaximumNumberOfInputs() const
{
    return 2;
}

} // End namespace udg
