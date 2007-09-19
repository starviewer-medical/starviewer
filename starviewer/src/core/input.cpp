/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGINPUT_CPP
#define UDGINPUT_CPP

#include "input.h"

// QT
#include <QStringList>

namespace udg {

Input::Input( QObject *parent )
 : QObject( parent )
{
}

Input::~Input()
{
}

}; // end namespace udg

#endif
