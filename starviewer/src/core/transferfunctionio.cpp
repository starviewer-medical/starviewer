/***************************************************************************
 *   Copyright (C) 2007 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.edu/GGG/index.html                                    *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/


#include "transferfunctionio.h"

#include <QColor>
#include <QDomDocument>
#include <QFile>
#include <QFileInfo>
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
    transferFunction->setName( QFileInfo( file ).fileName() );  // nom per defecte
    enum { NAME, COLOR, OPACITY } mode = COLOR;

    while ( !in.atEnd() )
    {
        QString line = in.readLine();
        QTextStream lineIn( &line, QIODevice::ReadOnly );
        QString first;
        bool ok;
        lineIn >> first;
        first = first.trimmed();

        if ( first.isEmpty() ) continue;
        else if ( first == "[Name]" ) mode = NAME;
        else if ( first == "[Color]" ) mode = COLOR;
        else if ( first == "[Opacity]" ) mode = OPACITY;
        else
        {
            if ( mode == NAME )
            {
                transferFunction->setName( line.trimmed() );
            }
            else
            {
                double x = first.toDouble( &ok );

                if ( ok )
                {
                    if ( mode == COLOR )
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
    }

    file.close();

    return transferFunction;
}


TransferFunction * TransferFunctionIO::fromFile( const QString & fileName )
{
    QFile file( fileName );
    return fromFile( file );
}


TransferFunction* TransferFunctionIO::fromXmlFile( QFile &file )    /// \todo afegir-hi comprovació d'errors
{
    QDomDocument xml;

    if ( !file.open( QFile::ReadOnly | QFile::Text ) )
    {
        DEBUG_LOG( QString( "No es pot llegir des del fitxer %1" ).arg( file.fileName() ) );
        return 0;
    }

    QString errorMsg;
    int errorLine, errorColumn;

    if ( !xml.setContent( &file, &errorMsg, &errorLine, &errorColumn ) )
    {
        file.close();
        DEBUG_LOG( errorMsg + QString( ". Línia: %1. Columna: %2." ).arg( errorLine ).arg( errorColumn ) );
        return 0;
    }

    file.close();

    TransferFunction *transferFunction = new TransferFunction();

    QDomElement transferFunctionElement = xml.documentElement();

    // nom
    QDomNode nameNode = transferFunctionElement.elementsByTagName( "name" ).item( 0 );
    if ( !nameNode.isNull() ) transferFunction->setName( nameNode.toElement().text() );
    else transferFunction->setName( QFileInfo( file ).fileName() ); // nom per defecte

    // color
    QDomNode colorNode = transferFunctionElement.elementsByTagName( "color" ).item( 0 );
    if ( !colorNode.isNull() )
    {
        QDomElement colorElement = colorNode.toElement();
        QDomNodeList colorPoints = colorElement.elementsByTagName( "point" );

        for ( uint i = 0; i < colorPoints.length(); i++ )
        {
            QDomElement colorPointElement = colorPoints.item( i ).toElement();
            transferFunction->addPointToColorRGB( colorPointElement.attribute( "value" ).toDouble(),
                                                  colorPointElement.attribute( "r" ).toDouble(),
                                                  colorPointElement.attribute( "g" ).toDouble(),
                                                  colorPointElement.attribute( "b" ).toDouble() );
        }
    }

    // opacitat
    QDomNode opacityNode = transferFunctionElement.elementsByTagName( "opacity" ).item( 0 );
    if ( !opacityNode.isNull() )
    {
        QDomElement opacityElement = opacityNode.toElement();
        QDomNodeList opacityPoints = opacityElement.elementsByTagName( "point" );

        for ( uint i = 0; i < opacityPoints.length(); i++ )
        {
            QDomElement opacityPointElement = opacityPoints.item( i ).toElement();
            transferFunction->addPointToOpacity( opacityPointElement.attribute( "value" ).toDouble(),
                                                 opacityPointElement.attribute( "a" ).toDouble() );
        }
    }

    return transferFunction;
}


TransferFunction* TransferFunctionIO::fromXmlFile( const QString &fileName )
{
    QFile file( fileName );
    return fromXmlFile( file );
}


void TransferFunctionIO::toFile( QFile & file, const TransferFunction & transferFunction )
{
    if ( !file.open( QFile::WriteOnly | QFile::Truncate | QFile::Text ) )
    {
        DEBUG_LOG( qPrintable( QString( "No es pot escriure al fitxer %1" ).arg( file.fileName() ) ) );
        return;
    }

    QTextStream out( &file );

    out << "[Name]\n";
    out << transferFunction.name() << "\n";

    out << "\n";

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


void TransferFunctionIO::toXmlFile( QFile &file, const TransferFunction &transferFunction )
{
    if ( !file.open( QFile::WriteOnly | QFile::Truncate | QFile::Text ) )
    {
        DEBUG_LOG( qPrintable( QString( "No es pot escriure al fitxer %1" ).arg( file.fileName() ) ) );
        return;
    }

    QDomDocument xml;

    QDomElement transferFunctionElement = xml.createElement( "transferfunction" );
    transferFunctionElement.setAttribute( "version", 0 );
    xml.appendChild( transferFunctionElement );

    // nom
    QDomElement nameElement = xml.createElement( "name" );
    transferFunctionElement.appendChild( nameElement );
    QDomText nameText = xml.createTextNode( transferFunction.name() );
    nameElement.appendChild( nameText );

    // color
    QDomElement colorElement = xml.createElement( "color" );
    transferFunctionElement.appendChild( colorElement );
    QList<double> colorPoints = transferFunction.getColorPoints();
    foreach ( double x, colorPoints )
    {
        QColor color = transferFunction.getColor( x );
        QDomElement colorPointElement = xml.createElement( "point" );
        colorPointElement.setAttribute( "value", x );
        colorPointElement.setAttribute( "r", color.redF() );
        colorPointElement.setAttribute( "g", color.greenF() );
        colorPointElement.setAttribute( "b", color.blueF() );
        colorElement.appendChild( colorPointElement );
    }

    // opacitat
    QDomElement opacityElement = xml.createElement( "opacity" );
    transferFunctionElement.appendChild( opacityElement );
    QList<double> opacityPoints = transferFunction.getOpacityPoints();
    foreach ( double x, opacityPoints )
    {
        QDomElement opacityPointElement = xml.createElement( "point" );
        opacityPointElement.setAttribute( "value", x );
        opacityPointElement.setAttribute( "a", transferFunction.getOpacity( x ) );
        opacityElement.appendChild( opacityPointElement );
    }

    QTextStream out( &file );
    out << xml.toString();
    out.flush();
    file.close();
}


void TransferFunctionIO::toXmlFile( const QString &fileName, const TransferFunction &transferFunction )
{
    QFile file( fileName );
    toXmlFile( file, transferFunction );
}


}
