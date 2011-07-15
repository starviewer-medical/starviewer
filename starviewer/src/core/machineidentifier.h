#ifndef MACHINEIDENTIFIER_H
#define MACHINEIDENTIFIER_H

#include <QString>

namespace udg {

class MachineIdentifier {
public:
    MachineIdentifier();

    /// Obtenir l'identificador de la màquina
    QString getMachineID();
    /// Obtenir el grup de treball on es troba de la màquina
    QString getGroupID();

private:
    /// Codifica en base64 una QByteArray i a més el transforma per que sigui una part d'una url vàlida
    QString encryptBase64Url(const QString &url);
    /// Busca la interfície de xarxa i retorna la seva adreça MAC.
    virtual QString getMACAddress();
    /// Busca el domini de la màquina.
    virtual QString getDomain();

};

}

#endif // MACHINEIDENTIFIER_H
