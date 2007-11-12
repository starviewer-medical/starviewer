/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGTOOLFACTORY_H
#define UDGTOOLFACTORY_H

#include <QObject>

namespace udg {

/**
Factory de Tools i elements associats

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class ToolFactory : public QObject
{
Q_OBJECT
public:
    ToolFactory(QObject *parent = 0);

    ~ToolFactory();

};

}

#endif
