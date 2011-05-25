#ifndef COPYDIRECTORY_H
#define COPYDIRECTORY_H

#include <QString>

namespace udg {

/**
    Classe que ens permet copiar el contingut d'un directori
  */
class CopyDirectory {
public:
    /// Copia el directori origen al directori destí
    static bool copyDirectory(const QString &sourceDirectory, const QString &sourceDestination);

};

};  // End namespace udg

#endif
