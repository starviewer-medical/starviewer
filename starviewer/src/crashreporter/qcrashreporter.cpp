/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "qcrashreporter.h"

namespace udg {
    
QCrashReporter::QCrashReporter( const QStringList& args , QWidget *parent )
    : QWidget( parent )
{
    setupUi(this);
}

};