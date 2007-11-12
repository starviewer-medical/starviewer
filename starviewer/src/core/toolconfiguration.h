/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGTOOLCONFIGURATION_H
#define UDGTOOLCONFIGURATION_H

#include <QObject>

namespace udg {

/**
Classe encarregada de definir els atributs que configuren una tool

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class ToolConfiguration : public QObject
{
Q_OBJECT
public:
    ToolConfiguration(QObject *parent = 0);

    ~ToolConfiguration();

};

}

#endif
