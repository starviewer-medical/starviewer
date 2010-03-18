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

    ///Copia el directori origen al directori destí
    static bool copyDirectory(const QString &sourceDirectory, const QString &sourceDestination);

};

};  //  end  namespace udg

#endif
