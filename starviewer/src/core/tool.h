/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGTOOL_H
#define UDGTOOL_H

#include <QObject>

namespace udg {

/**
Classe base per a totes les tools

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class Tool : public QObject
{
Q_OBJECT
public:
    Tool(QObject *parent = 0);

    ~Tool();

};

}

#endif
