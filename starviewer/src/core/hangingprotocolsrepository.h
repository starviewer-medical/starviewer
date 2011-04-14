#ifndef UDGHANGINGPROTOCOLSREPOSITORY_H
#define UDGHANGINGPROTOCOLSREPOSITORY_H

#include "repository.h"
#include "hangingprotocol.h"
#include "identifier.h"
#include <QObject>

namespace udg {

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
