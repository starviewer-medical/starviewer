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

#include <iostream>

namespace udg {

VolumeRepository::VolumeRepository()
{
}

Identifier VolumeRepository::addVolume( Volume* model)
{
    Identifier id;
    
    id = this->addItem( model );
    emit itemAdded( id );
    
    return id;
}

Volume* VolumeRepository::getVolume( Identifier id )
{
    return this->getItem( id );
    
}

void VolumeRepository::removeVolume( Identifier id )
{
    this->removeItem( id );
    emit itemRemoved( id );
}

int VolumeRepository::getNumberOfVolumes()
{
    return this->getNumberOfItems();
}

};

#endif
