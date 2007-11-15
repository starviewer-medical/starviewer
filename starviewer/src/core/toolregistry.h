/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGTOOLREGISTRY_H
#define UDGTOOLREGISTRY_H

#include <QObject>

namespace udg {

/**
Registre de Tools i elements associats

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class ToolRegistry : public QObject
{
Q_OBJECT
public:
    ToolRegistry(QObject *parent = 0);

    ~ToolRegistry();

};

}

#endif
