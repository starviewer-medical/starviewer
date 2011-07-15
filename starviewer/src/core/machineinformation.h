#ifndef UDG_MACHINEINFORMATION_H
#define UDG_MACHINEINFORMATION_H

#include <QString>
#include <QProcessEnvironment>

namespace udg {

class MachineInformation
{
public:
    MachineInformation();
    /// Busca la interfície de xarxa i retorna la seva adreça MAC.
    QString getMACAddress();
    /// Busca el domini de la màquina.
    QString getDomain();
private:
    virtual QProcessEnvironment getSystemEnvironment();
};

} // namespace udg

#endif // UDG_MACHINEINFORMATION_H
