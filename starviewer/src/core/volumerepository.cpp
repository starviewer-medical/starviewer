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
