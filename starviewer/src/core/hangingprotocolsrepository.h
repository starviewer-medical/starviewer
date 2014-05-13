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

#ifndef UDGHANGINGPROTOCOLSREPOSITORY_H
#define UDGHANGINGPROTOCOLSREPOSITORY_H

#include "repository.h"
#include "hangingprotocol.h"
#include "identifier.h"
#include <QObject>

namespace udg {

class HangingProtocolsRepository : public Repository<HangingProtocol> {
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
