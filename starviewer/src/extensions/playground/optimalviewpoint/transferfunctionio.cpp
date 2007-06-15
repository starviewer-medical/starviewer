/***************************************************************************
 *   Copyright (C) 2007 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.edu/GGG/index.html                                    *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/


#include "transferfunctionio.h"

#include <QColor>
#include <QFile>
#include <QTextStream>

#include "logging.h"
#include "transferfunction.h"


namespace udg {


TransferFunctionIO::TransferFunctionIO()
{
}


TransferFunctionIO::~TransferFunctionIO()
{
}


TransferFunction * TransferFunctionIO::fromFile( QFile & file )
{
    if ( !file.open( QFile::ReadOnly | QFile::Text ) )
    {
        DEBUG_LOG( qPrintable( QString( "No es pot llegir des del fitxer %1" ).arg( file.fileName() ) ) );
        return 0;
    }

    QTextStream in( &file );
    TransferFunction * transferFunction = new TransferFunction();
    bool modeColor = true;

    while ( !in.atEnd() )
    {
        QString line = in.readLine();
        QTextStream lineIn( &line, QIODevice::ReadOnly );
        QString first;
        bool ok;
        lineIn >> first;

        if ( first.trimmed() == "[Color]" ) modeColor = true;
        else if ( first.trimmed() == "[Opacity]" ) modeColor = false;
        else
        {
            double x = first.toDouble( &ok );

            if ( ok )
            {
                if ( modeColor )
                {
                    double r, g, b;
                    lineIn >> r >> g >> b;
                    transferFunction->addPointToColorRGB( x, r, g, b );
                }
                else
                {
                    double opacity;
                    lineIn >> opacity;
                    transferFunction->addPointToOpacity( x, opacity );
                }
            }
            else continue;
        }
    }

    file.close();

    return transferFunction;
}


TransferFunction * TransferFunctionIO::fromFile( const QString & fileName )
{
    QFile file( fileName );
    return fromFile( file );
}


void TransferFunctionIO::toFile( QFile & file, const TransferFunction & transferFunction )
{
    if ( !file.open( QFile::WriteOnly | QFile::Truncate | QFile::Text ) )
    {
        DEBUG_LOG( qPrintable( QString( "No es pot escriure al fitxer %1" ).arg( file.fileName() ) ) );
        return;
    }

    QTextStream out( &file );

    out << "[Color]\n";
    QList< double > colorPoints = transferFunction.getColorPoints();
    foreach ( double x, colorPoints )
    {
        QColor color = transferFunction.getColor( x );
        out << x << " " << color.redF() << " " << color.greenF() << " " << color.blueF() << "\n";
    }

    out << "\n";

    out << "[Opacity]\n";
    QList< double > opacityPoints = transferFunction.getOpacityPoints();
    foreach ( double x, opacityPoints )
    {
        out << x << " " << transferFunction.getOpacity( x ) << "\n";
    }

    out.flush();
    file.close();
}


void TransferFunctionIO::toFile( const QString & fileName, const TransferFunction & transferFunction )
{
    QFile file( fileName );
    toFile( file, transferFunction );
}


}
