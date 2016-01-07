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

bool VolumeHelper::isPrimaryCT(Volume *volume)
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

    return volume->getModality() == "CT" && firstImage->getImageType().contains("PRIMARY");
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

bool VolumeHelper::isPrimaryNM(Volume *volume)
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

    return (volume->getModality() == "NM" && firstImage->getImageType().contains("PRIMARY"));
}

} // End namespace udg
