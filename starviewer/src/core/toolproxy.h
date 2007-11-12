/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGTOOLPROXY_H
#define UDGTOOLPROXY_H

#include <QObject>

namespace udg {

/**
Classe encarregada de mantenir les tools actives per un visualitzador i de proporcionar-lis els events corresponents a aquestes

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class ToolProxy : public QObject
{
Q_OBJECT
public:
    ToolProxy(QObject *parent = 0);

    ~ToolProxy();

};

}

#endif
