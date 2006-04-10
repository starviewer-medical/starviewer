/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGSYSTEMINFORMATION_H
#define UDGSYSTEMINFORMATION_H

#include <QObject>

namespace udg {

/**
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class SystemInformation : public QObject
{
Q_OBJECT
public:
    SystemInformation(QObject *parent = 0);

    /** Retorna l'espai lliure de la particio on es troba el directori passat per parametre
      *         @return espai lliure del volum on es troba la cache
      */
    unsigned long long getDiskFreeSpace(QString dir);

    ~SystemInformation();

};

}

#endif
