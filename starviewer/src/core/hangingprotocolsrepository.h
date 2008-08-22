/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGHANGINGPROTOCOLSREPOSITORY_H
#define UDGHANGINGPROTOCOLSREPOSITORY_H

#include "repository.h"
#include "hangingprotocol.h"
#include "identifier.h"
#include <QObject>

namespace udg {

/**
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class HangingProtocolsRepository : public Repository<HangingProtocol>
{
Q_OBJECT

public:

     /// Ens retorna l'única instància del repositori
    static HangingProtocolsRepository* getRepository()
    {
        static HangingProtocolsRepository repository;
        return &repository;
    }

    ~HangingProtocolsRepository();

private:

    HangingProtocolsRepository();

};

}

#endif
