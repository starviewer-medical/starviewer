/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/



#include "transferfunctionio.h"

#include <QColor>
#include <QFile>
#include <QTextStream>

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
        qCritical( "No s'ha pogut obrir el fitxer %s!", qPrintable( file.fileName() ) );
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
        qCritical( "No s'ha pogut obrir el fitxer %s!", qPrintable( file.fileName() ) );
        return;
    }

    QTextStream out( &file );

    out << "[Color]\n";
//     QMapIterator< double, QColor > * itc = transferFunction.getColorPoints();
//     while ( itc->hasNext() )
//     {
//         itc->next();
//         out << itc->key() << " " << itc->value().redF() << " " << itc->value().greenF() << " " << itc->value().blueF() << "\n";
//     }
//     delete itc;
    QList< double > colorPoints = transferFunction.getColorPoints();
    foreach ( double x, colorPoints )
    {
        QColor color = transferFunction.getColor( x );
        out << x << " " << color.redF() << " " << color.greenF() << " " << color.blueF() << "\n";
    }

    out << "\n";

    out << "[Opacity]\n";
//     QMapIterator< double, double > * ito = transferFunction.getOpacityPoints();
//     while ( ito->hasNext() )
//     {
//         ito->next();
//         out << ito->key() << " " << ito->value() << "\n";
//     }
//     delete ito;
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
