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

#include "volumerepository.h"
#include "volume.h"
#include "logging.h"
#include "volumereaderjobfactory.h"

namespace udg {

VolumeRepository::VolumeRepository()
{
}

Identifier VolumeRepository::addVolume(Volume *model)
{
    Identifier id;

    id = this->addItem(model);
    emit itemAdded(id);
    INFO_LOG("S'ha afegit al repositori el volum amb id: " + QString::number(id.getValue()));
    return id;
}

Volume* VolumeRepository::getVolume(Identifier id)
{
    return this->getItem(id);
}

void VolumeRepository::deleteVolume(Identifier id)
{
    // L'obtenim
    Volume *volume = this->getVolume(id);
    if (!volume)
    {
        INFO_LOG(QString("No existeix cap volum al repositori amb l'id: %1. No esborrarem res del repositori.").arg(id.getValue()));
        return;
    }

    // El treiem de la llista
    this->removeItem(id);

    // I l'eliminem
    VolumeReaderJobFactory *volumeReader = VolumeReaderJobFactory::instance();
    volumeReader->cancelLoadingAndDeleteVolume(volume);

    emit itemRemoved(id);
    INFO_LOG("S'ha esborrat del repositori el volum amb id: " + QString::number(id.getValue()));
}

int VolumeRepository::getNumberOfVolumes()
{
    return this->getNumberOfItems();
}

}
