/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#ifndef UDGVOLUMEREPOSITORY_CPP
#define UDGVOLUMEREPOSITORY_CPP

#include "volumerepository.h"
#include "volume.h"
#include "logging.h"
#include "asynchronousvolumereader.h"

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
    // l'obtenim
    Volume *volume = this->getVolume(id);
    // el treiem de la llista
    this->removeItem(id);

    // i l'eliminem
    AsynchronousVolumeReader volumeReader;
    volumeReader.cancelLoadingAndDeleteVolume(volume);

    emit itemRemoved(id);
    INFO_LOG("S'ha esborrat del repositori el volum amb id: " + QString::number(id.getValue()));
}

int VolumeRepository::getNumberOfVolumes()
{
    return this->getNumberOfItems();
}

}

#endif
