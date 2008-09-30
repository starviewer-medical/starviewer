/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "qcrashreporter.h"

/* Programa principal per executar l'aplicació de crash report */

int main( int argc, char* argv[] )
{

    QApplication app( argc, argv );

    udg::QCrashReporter reporter( app.arguments() );
    reporter.show();

    return app.exec();
}
