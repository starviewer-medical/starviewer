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

namespace udg {

VolumeRepository::VolumeRepository()
{
}

Identifier VolumeRepository::addVolume( Volume* model)
{
    Identifier id;

    id = this->addItem( model );
    emit itemAdded( id );
    INFO_LOG( "S'ha afegit al repositori el volum amb id: " + QString::number( id.getValue() ) );
    return id;
}

Volume* VolumeRepository::getVolume( Identifier id )
{
    return this->getItem( id );
}

void VolumeRepository::removeVolume( Identifier id )
{
    // l'obtenim
    Volume *dum = this->getVolume( id );
    // el treiem de la llista
    this->removeItem( id );
    // i l'eliminem
    delete dum;

    emit itemRemoved( id );
    INFO_LOG( "S'ha esborrat del repositori el volum amb id: " + QString::number( id.getValue() ) );
}

int VolumeRepository::getNumberOfVolumes()
{
    return this->getNumberOfItems();
}

};

#endif
