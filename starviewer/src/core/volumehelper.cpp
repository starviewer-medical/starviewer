#include "volumehelper.h"

#include "volume.h"
#include "image.h"

namespace udg {

VolumeHelper::VolumeHelper()
{
}

VolumeHelper::~VolumeHelper()
{
}

bool VolumeHelper::isPrimaryPET(Volume *volume)
{
    if (!volume)
    {
        return false;
    }

    Image *firstImage = volume->getImage(0);
    if (!firstImage)
    {
        return false;
    }
    
    return volume->getModality() == "PT" && firstImage->getImageType().contains("PRIMARY");
}

} // End namespace udg
