/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "output.h"
#include "logging.h"

// include's ITK
#include <itkNumericSeriesFileNames.h>
// qt
#include <QString>
#include <QDir>
#include <QFileInfo>

namespace udg {

Output::Output()
{
    m_writer = WriterType::New();
    m_volumeData = 0;

    m_seriesWriter = ImageSeriesWriterType::New();
}

Output::~Output()
{
}

bool Output::saveFile( const char* filename )
{
    bool ok = true;

    if( !m_volumeData )
    {
        WARN_LOG( "There's no dataset to write" );
        ok = false;
        return ok;
    }

    m_writer->SetFileName( filename );
    emit progress(0);
    try
    {
        m_writer->Update();
    }
    catch ( itk::ExceptionObject & e )
    {
        WARN_LOG( qPrintable( "Excepció escrivint l'arxiu [" + QString::fromLatin1(filename) + "]" ) );
        ok = false;
        emit progress(-1);
    }
    emit progress(100);

    return ok;

}

bool Output::saveSeries( const char* filename , int slice )
{
/*
TIFF i PNG només suporten unsigned chars i unsigned shorts, BMP nomes uchars, per tant per aquests 3 formats fem rescale a uchar ( 0-255 )

Sempre s'ha de fer un casting ( i un rescale image? )
*/
    bool ok = true;

    if( !m_volumeData )
    {
        WARN_LOG( "There's no dataset to write" );
        ok = false;
        return ok;
    }
    if( slice == -1 )
    {
        // escriure totes les llesques
    }
    else
    {
        // escriure la indicada, comprovar que no sobrepassa el límit, però això ja tindria que venir bé
        // segurament el que caldria fer és canviar l'input ( fer un "reslice" i extrure les llesques requerides)
    }

    RescaleFilterType::Pointer rescaler = RescaleFilterType::New();
    rescaler->SetInput( m_volumeData->getItkData() );
    rescaler->SetOutputMinimum( 0 );
    rescaler->SetOutputMaximum( 255 );
    m_seriesWriter->SetInput( rescaler->GetOutput() );

    // el format del fitxer serà NomDelFitxerXXXX.extensio on XXXX serà el nº de llesca. El nº de llesca serà de com a màxim 4 dígits
    QString seriesFormat;
    seriesFormat.sprintf( "%s/%s-%%04d.%s" , qPrintable( QFileInfo( filename ).dir().absolutePath() ) , qPrintable( QFileInfo( filename ).completeBaseName() ) , qPrintable( QFileInfo( filename ).suffix() ) );

    typedef itk::NumericSeriesFileNames    NameGeneratorType;
    NameGeneratorType::Pointer nameGenerator = NameGeneratorType::New();
    nameGenerator->SetSeriesFormat( seriesFormat.toLatin1() );
    nameGenerator->SetStartIndex( 1 );
    nameGenerator->SetEndIndex( m_volumeData->getVtkData()->GetDimensions()[2] ); // el nombre de llesques
    nameGenerator->SetIncrementIndex( 1 );
    m_seriesWriter->SetFileNames( nameGenerator->GetFileNames()  );

    emit progress(0);
    try
    {
        m_seriesWriter->Update();
    }
    catch ( itk::ExceptionObject & e )
    {
        WARN_LOG( qPrintable( "Excepció escrivint l'arxiu [" + QString::fromLatin1( filename ) +"]" ) );
        ok = false;
        emit progress(-1);
    }
    emit progress(100);
    return ok;
}

void Output::setInput(Volume* data)
{
    m_volumeData = data;
    // podríem decidir entre fer això abans o
    m_writer->SetInput( m_volumeData->getItkData() );

};

};  // end namespace udg
