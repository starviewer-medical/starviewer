/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGTOOLDATA_H
#define UDGTOOLDATA_H

#include <QObject>

namespace udg {

/**
Classe genèrica per a les dades d'un tool

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class ToolData : public QObject
{
Q_OBJECT
public:
    ToolData(QObject *parent = 0);

    ~ToolData();

};

}

#endif
