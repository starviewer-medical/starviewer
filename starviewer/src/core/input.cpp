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
// std
#include <iostream> // pel cerr de les excepcions i printTag()
//ITK
#include <itkMetaDataDictionary.h>
#include <itkMetaDataObject.h>
// QT
#include <QStringList>
// VTK
#include <vtkMath.h> // pel cross

namespace udg {

Input::Input()
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

bool Input::openFile( const char * fileName )
{
    ProgressCommand::Pointer observer = ProgressCommand::New();
    m_reader->AddObserver( itk::ProgressEvent(), observer );

    bool ok = true;

    m_reader->SetFileName( fileName );
    emit progress(0);
    try
    {
        m_reader->Update();
    }
    catch ( itk::ExceptionObject & e )
    {
        WARN_LOG( qPrintable( "Excepció llegint l'arxiu [" + QString::fromLatin1(fileName) + QString("] -> ") + e.GetDescription() ) )
        std::cerr << e << std::endl;

        ok = false;
        emit progress( -1 ); // això podria indicar excepció
    }
    if ( ok )
    {
        ImageType::Pointer imageData;
        imageData = m_reader->GetOutput();
        m_volumeData->setData( imageData );
        imageData->Delete();

        emit progress( 100 );
    }
    return ok;
}

bool Input::readSeries( const char *dirPath )
{
//     SeriesProgressCommand::Pointer observer = SeriesProgressCommand::New();
//     m_seriesReader->AddObserver( itk::ProgressEvent(), observer );

    bool ok = true;

    m_namesGenerator->SetInputDirectory( dirPath );

    const SeriesReaderType::FileNamesContainer &filenames = m_namesGenerator->GetInputFileNames();
    // això és necessari per després poder demanar-li el diccionari de meta-dades i obtenir els tags del DICOM
    m_seriesReader->SetImageIO( m_gdcmIO );
    m_seriesReader->SetFileNames( filenames );

    emit progress( 0 );

    try
    {
        m_seriesReader->Update();
    }
    catch ( itk::ExceptionObject & e )
    {
        WARN_LOG( qPrintable( "Excepció llegint els arxius del directori [" + QString::fromLatin1(dirPath) + QString("] -> ") + e.GetDescription() ) )
        std::cerr << e << std::endl;
        ok = false;
        emit progress( -1 ); // això podria indicar excepció
    }
    if ( ok )
    {
        ImageType::Pointer imageData;
        imageData = m_seriesReader->GetOutput();
        imageData->SetMetaDataDictionary( m_gdcmIO->GetMetaDataDictionary() );
        m_volumeData->setData( imageData );
        imageData->Delete();

        emit progress( 100 );
        this->setVolumeInformation();
    }

    return ok;
}

void Input::printTag( std::string tag , std::string name )
{
    std::string tagValue;
    if( this->queryTagAsString( tag , tagValue ) )
    {
        std::cout << name << " ( " << tag <<  ") ";
        std::cout << " is: " << tagValue << std::endl;
    }
    else
    {
        std::ostringstream message;
        message << "Tag" << tag << "(" << name << ")" << " not found in the DICOM header";
        WARN_LOG( qPrintable( "Tag " + QString::fromStdString(tag) + "(" + QString::fromStdString(name) + ")" + " not found in the DICOM header" ) );
    }
}

bool Input::queryTagAsString( std::string tag , std::string &result )
{
    bool ok = true;
    typedef itk::MetaDataDictionary   DictionaryType;
    const  DictionaryType & dictionary = m_gdcmIO->GetMetaDataDictionary();

    typedef itk::MetaDataObject< std::string > MetaDataStringType;

    DictionaryType::ConstIterator itr = dictionary.Begin();
    DictionaryType::ConstIterator end = dictionary.End();

    DictionaryType::ConstIterator tagItr = dictionary.Find( tag );

    if( tagItr == end )
    {
        ok = false;
    }
    else
    {
        MetaDataStringType::ConstPointer entryvalue = dynamic_cast<const MetaDataStringType *>( tagItr->second.GetPointer() );

        if( entryvalue )
        {
            result = entryvalue->GetMetaDataObjectValue();
        }
    }
    return ok;
}

char *Input::getOrientation( double vector[3] )
{
        char *orientation=new char[4];
        char *optr = orientation;
        *optr='\0';

        char orientationX = vector[0] < 0 ? 'R' : 'L';
        char orientationY = vector[1] < 0 ? 'A' : 'P';
        char orientationZ = vector[2] < 0 ? 'I' : 'S';

        double absX = fabs( vector[0] );
        double absY = fabs( vector[1] );
        double absZ = fabs( vector[2] );

        int i;
        for ( i = 0; i < 3; ++i )
        {
            if ( absX > .0001 && absX > absY && absX > absZ )
            {
                *optr++= orientationX;
                absX = 0;
            }
            else if ( absY > .0001 && absY > absX && absY > absZ )
            {
                *optr++= orientationY;
                absY = 0;
            }
            else if ( absZ > .0001 && absZ > absX && absZ > absY )
            {
                *optr++= orientationZ;
                absZ = 0;
            }
            else break;
            *optr='\0';
        }
        return orientation;
}

void Input::setVolumeInformation()
{
    // creem l'string de l'orientació del pacient
    std::string value;
    if( queryTagAsString( "0020|0020" , value ) )
    {
        QString str = QString::fromStdString( value );
        str.replace( QString( "\\" ) , QString( "," ) );
        m_volumeData->getVolumeSourceInformation()->setPatientOrientationString( qPrintable( str ) );
    }
    else
    {
        // si no tenim la informació directament l'haurem de deduir a partir dels dir cosines
        if( queryTagAsString( "0020|0037", value ) )
        {
            // passem de l'string als valors double
            double dirCosinesValuesX[3] , dirCosinesValuesY[3] , dirCosinesValuesZ[3];
            QString dirCosines = QString::fromStdString( value );
            QStringList list = dirCosines.split( "\\" );
            if( list.size() == 6 )
            {
                for ( int i = 0; i < 3; i++ )
                {
                    dirCosinesValuesX[ i ] = list.at( i ).toDouble();
                    dirCosinesValuesY[ i ] = list.at( i+3 ).toDouble();
                }

                vtkMath::Cross( dirCosinesValuesX , dirCosinesValuesY , dirCosinesValuesZ );
                // I ara ens disposem a crear l'string amb l'orientació del pacient
                QString patientOrientationString;

                patientOrientationString = this->getOrientation( dirCosinesValuesX );
                patientOrientationString += ",";
                patientOrientationString += this->getOrientation( dirCosinesValuesY );
                patientOrientationString += ",";
                patientOrientationString += this->getOrientation( dirCosinesValuesZ );
                m_volumeData->getVolumeSourceInformation()->setPatientOrientationString( qPrintable( patientOrientationString ) );
                m_volumeData->getVolumeSourceInformation()->setXDirectionCosines( dirCosinesValuesX );
                m_volumeData->getVolumeSourceInformation()->setYDirectionCosines( dirCosinesValuesY );
                m_volumeData->getVolumeSourceInformation()->setZDirectionCosines( dirCosinesValuesZ );
            }
            else
            {
                // hi ha algun error en l'string ja que han de ser 2 parells de 3 valors
                DEBUG_LOG( qPrintable( "No s'ha pogut determinar l'orientació del pacient (Tags 0020|0020 , 0020|0037) : " + dirCosines ) );
            }
        }
        else
        {
            // no podem obtenir l'string d'orientació del pacient
        }
    }

    // nom de la institució on s'ha fet l'estudi
    if( queryTagAsString( "0008|0080" , value ) )
    {
        m_volumeData->getVolumeSourceInformation()->setInstitutionName( value.c_str() );
    }
    else
    {
        // no tenim aquesta informació \TODO cal posar res?
        m_volumeData->getVolumeSourceInformation()->setInstitutionName( tr( "Unknown" ).toAscii() );
    }

    // nom del pacient
    if( queryTagAsString( "0010|0010" , value ) )
    {
        // pre-tractament per treure caràcters estranys com ^ que en alguns casos fan de separadors en comptes dels espais
        QString name = QString::fromStdString( value );
        while( name.indexOf("^") >= 0 )
            name.replace( name.indexOf("^") , 1 , QString(" ") );
        m_volumeData->getVolumeSourceInformation()->setPatientName( qPrintable( name ) );
    }
    // ID del pacient
    if( queryTagAsString( "0010|0020" , value ) )
    {
        m_volumeData->getVolumeSourceInformation()->setPatientID( value.c_str() );
    }

    // data de l'estudi
    if( queryTagAsString( "0008|0020" , value ) )
    {
        // la data està en format YYYYMMDD
        m_volumeData->getVolumeSourceInformation()->setStudyDate( value.c_str() );
    }

    // hora de l'estudi
    if( queryTagAsString( "0008|0030" , value ) )
    {
        // l'hora està en format HHMMSS
        m_volumeData->getVolumeSourceInformation()->setStudyTime( value.c_str() );
    }

    // accession number
    if( queryTagAsString( "0008|0050" , value ) )
    {
        m_volumeData->getVolumeSourceInformation()->setAccessionNumber( value.c_str() );
    }

    // window level
    if( queryTagAsString( "0028|1051" , value ) )
    {
        QString str = QString::fromStdString( value );
        if( str.contains( "\\" ) )
        {
            str = str.left( str.indexOf( "\\" ) );
        }
        double window = str.toDouble();

        m_volumeData->getVolumeSourceInformation()->setWindow( window );

        queryTagAsString( "0028|1050" , value );
        str = QString::fromStdString( value );
        if( str.contains( "\\" ) )
        {
            str = str.left( str.indexOf( "\\" ) );
        }
        double level = str.toDouble();
        m_volumeData->getVolumeSourceInformation()->setLevel( level );

    }

    // Protocol name
    if( queryTagAsString( "0018|1030" , value ) )
    {
        m_volumeData->getVolumeSourceInformation()->setProtocolName( value.c_str() );
    }

}

}; // end namespace udg

#endif
