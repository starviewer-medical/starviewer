/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGINPUT_CPP
#define UDGINPUT_CPP

#include "input.h"
#include "volumesourceinformation.h"
#include "logging.h"
#include "dicomtagreader.h"

// QT
#include <QStringList>
#include <QFileInfo>
#include <QDir>

namespace udg {

Input::Input( QObject *parent )
 : QObject( parent )
{
    m_reader = ReaderType::New();
    m_seriesReader = SeriesReaderType::New();
    m_volumeData = new Volume;

    m_gdcmIO = ImageIOType::New();
    m_namesGenerator = NamesGeneratorType::New();

   /* typedef itk::QtSignalAdaptor SignalAdaptorType;
    SignalAdaptorType m_progressSignalAdaptor;
   */
     itk::QtSignalAdaptor *m_progressSignalAdaptor = new itk::QtSignalAdaptor;
//   Connect the adaptor as an observer of a Filter's event
    m_seriesReader->AddObserver( itk::ProgressEvent(),  m_progressSignalAdaptor->GetCommand() );
//
//  Connect the adaptor's Signal to the Qt Widget Slot
   connect( m_progressSignalAdaptor, SIGNAL( Signal() ), this, SLOT( slotProgress() ) );
}

Input::~Input()
{
    m_seriesReader->Delete();
    m_reader->Delete();
    m_gdcmIO->Delete();
    delete m_volumeData;
}

int Input::openFile( QString fileName )
{
    ProgressCommand::Pointer observer = ProgressCommand::New();
    m_reader->AddObserver( itk::ProgressEvent(), observer );

    int errorCode = NoError;

    m_reader->SetFileName( qPrintable(fileName) );
    emit progress(0);
    try
    {
        m_reader->Update();
    }
    catch ( itk::ExceptionObject & e )
    {
        ERROR_LOG( QString("Excepció llegint els arxius del directori [%1]\nDescripció: [%2]")
                .arg( QFileInfo( fileName ).dir().path() )
                .arg( e.GetDescription() )
                );
        // llegim el missatge d'error per esbrinar de quin error es tracta
        QString errorMessage( e.GetDescription() );
        if( errorMessage.contains("Size mismatch") )
        {
            errorCode = SizeMismatch;
        }
        emit progress( -1 ); // això podria indicar excepció
    }
    if ( errorCode == NoError )
    {
        ImageType::Pointer imageData;
        imageData = m_reader->GetOutput();
        m_volumeData->setData( imageData );
        imageData->Delete();

        m_volumeData->getVolumeSourceInformation()->setFilenames( fileName );
        emit progress( 100 );
    }
    return errorCode;
}

int Input::readFiles( QStringList filenames )
{
    int errorCode = NoError;
    if( filenames.isEmpty() )
    {
        WARN_LOG( "La llista de noms de fitxer per carregar és buida" );
        errorCode = InvalidFileName;
        return errorCode;
    }

    if( filenames.size() > 1 )
    {
        // això és necessari per després poder demanar-li el diccionari de meta-dades i obtenir els tags del DICOM
        m_seriesReader->SetImageIO( m_gdcmIO );

        // convertim la QStringList al format std::vector< std::string > que s'esperen les itk
        std::vector< std::string > stlFilenames = qstringListToStdVectorOfStdString( filenames );

        m_seriesReader->SetFileNames( stlFilenames );

        emit progress( 0 );

        try
        {
            m_seriesReader->Update();
        }
        catch ( itk::ExceptionObject & e )
        {
            ERROR_LOG( QString("Excepció llegint els arxius del directori [%1]\nDescripció: [%2]")
                .arg( QFileInfo( filenames.at(0) ).dir().path() )
                .arg( e.GetDescription() )
                );
            errorCode = SizeMismatch;
            emit progress( -1 ); // això podria indicar excepció
        }
        if ( errorCode == NoError )
        {
            ImageType::Pointer imageData;
            imageData = m_seriesReader->GetOutput();
            m_volumeData->setData( imageData );
            imageData->Delete();

            m_volumeData->getVolumeSourceInformation()->setFilenames( filenames );
            emit progress( 100 );
        }
    }
    else
    {
        this->openFile( filenames.at(0) );
    }
    return errorCode;
}

int Input::readSeries( QString dirPath )
{
//     SeriesProgressCommand::Pointer observer = SeriesProgressCommand::New();
//     m_seriesReader->AddObserver( itk::ProgressEvent(), observer );

    return readFiles( generateFilenames(dirPath) );
}

QStringList Input::generateFilenames( QString dirPath )
{
    m_namesGenerator->SetInputDirectory( qPrintable(dirPath) );
    const SeriesReaderType::FileNamesContainer &filenames = m_namesGenerator->GetInputFileNames();
    return stdVectorOfStdStringToQStringList( filenames );
}

QStringList Input::stdVectorOfStdStringToQStringList( std::vector< std::string > vector )
{
    QStringList list;
    for( unsigned int i = 0; i < vector.size(); i++ )
    {
        list += vector[i].c_str();
    }
    return list;
}

std::vector< std::string > Input::qstringListToStdVectorOfStdString( QStringList list )
{
    std::vector< std::string > stlVector;
    for( int i = 0; i < list.size(); i++ )
    {
        stlVector.push_back( list.at(i).toStdString() );
    }
    return stlVector;
}

}; // end namespace udg

#endif
