/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGRESOURCECOLLECTION_H
#define UDGRESOURCECOLLECTION_H

#include <QObject>
#include "identifier.h"

namespace udg {

// FWD declarations
class VolumeRepository;
/**
Agrupa tots els recursos de l'aplicació

@author Grup de Gràfics de Girona  ( GGG )
*/
class ResourceCollection : public QObject{
Q_OBJECT
public:
    ResourceCollection(QObject *parent = 0, const char *name = 0);

    ~ResourceCollection();

    /// Retorna el repositori de volums
    VolumeRepository *getVolumeRepository(){ return m_volumeRepository; }
private:
    /// Repositori de volums
    VolumeRepository *m_volumeRepository;
    /// Id del volum principal
    Identifier m_mainVolumeID;
};

};  //  end  namespace udg 

#endif
