/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGINPUT_H
#define UDGINPUT_H

#include <vector>
// qt
#include <QObject>

namespace udg {

/**
    Aquesta classe s'ocupa de les tasques d'obrir tot tipus de fitxer

@author Grup de Gràfics de Girona ( GGG )
*/
class Input : public QObject
{
Q_OBJECT
public:
    Input( QObject *parent = 0 );
    ~Input();

};

};

#endif
