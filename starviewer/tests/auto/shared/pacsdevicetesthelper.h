#ifndef PACSDEVICETESTHELPER_H
#define PACSDEVICETESTHELPER_H

#include <QString>
#include "pacsdevice.h"

namespace testing
{

/// Classe que retorna PACSDevice per utilitzar per testing
class PACSDeviceTestHelper {

public:
    /// Retorna un PacsDevice amb els camps mínims obligatoris. A AETitle, ID, Address i Institutiom se li assigna el valor de ID
    static udg::PacsDevice createPACSDeviceByID(QString ID);

    /// Retorna una PacsDevice amb el ID, AETitle, address i queryPort passat per paràmetre, la resta de camps obligatoris agafen el valor del ID
    static udg::PacsDevice createDimsePacsDevice(QString ID, QString AETitle, QString address, int queryPort);

    /// Creates and returns a WADO PacsDevice the given ID and base URI.
    static udg::PacsDevice createWadoPacsDevice(QString id, const QString &baseUri);

    /// Creates and returns a WADO-URI + DIMSE PacsDevice the given ID and other parameters.
    static udg::PacsDevice createWadoUriDimsePacsDevice(QString id, QString aeTitle, QString address, int queryPort, const QString &baseUri);
};
}

#endif // PACSDEVICETESTHELPER_H
