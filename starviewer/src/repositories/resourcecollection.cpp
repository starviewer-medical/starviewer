/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "resourcecollection.h"

#include "volumerepository.h"

namespace udg {

ResourceCollection::ResourceCollection(QObject *parent, const char *name)
 : QObject(parent, name)
{
    m_volumeRepository = VolumeRepository::getRepository();
}


ResourceCollection::~ResourceCollection()
{
}


};  // end namespace udg 
