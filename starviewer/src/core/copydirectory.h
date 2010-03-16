/***************************************************************************
 *   Copyright (C) 2010 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef COPYDIRECTORY_H
#define COPYDIRECTORY_H

#include <QString>

namespace udg {

/** Classe que ens permet copiar el contingut d'un directori

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class CopyDirectory{
public:

    /**Copia el directori origen al directori destí. Si la còpia falla al copiar algun fitxer o directori esborra del directori destí
       tot el que s'havia copiat fins el moment*/
    static bool copyDirectory(const QString &sourceDirectory, const QString &sourceDestination);

private:
    
    ///Mètode recursiu per copiar directoris
    static bool copyDirectoryRecursive(const QString &sourceDirectory, const QString &sourceDestination);

};

};  //  end  namespace udg

#endif
