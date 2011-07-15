#ifndef PACSDEVICETESTUTILS_H
#define PACSDEVICETESTUTILS_H

#include <QString>
#include "pacsdevice.h"

namespace udg
{
/// Classe que retorna PACSDevice per utilitzar per testing
class PACSDeviceTestUtils {

public:

    /// Retorna un PacsDevice amb els camps mínims obligatoris. A AETitle, ID, Address i Institutiom se li assigna el valor de ID
    static PacsDevice getTestPACSDevice(QString ID);
};
}

#endif // PACSDEVICETESTUTILS_H
