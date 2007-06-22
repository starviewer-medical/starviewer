/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGINPUT_H
#define UDGINPUT_H

#include "volume.h"
#include "logging.h"
// qt
#include <QObject>
// itk
#include <itkImageFileReader.h>
#include <itkImageSeriesReader.h>
#include <itkGDCMImageIO.h>
#include <itkGDCMSeriesFileNames.h>
#include <itkCommand.h>
#include <itkSmartPointer.h>

#include "itkQtAdaptor.h"

namespace udg {
/**
    Classe auxiliar per monitorejar el progrés de la lectura del fitxer
*/
class ProgressCommand : public itk::Command
{
public:
    typedef  ProgressCommand   Self;
    typedef  itk::Command             Superclass;
    typedef  itk::SmartPointer<Self>  Pointer;
    itkNewMacro( Self );

protected:
    ProgressCommand() {};

public:
    typedef Volume::ItkImageType ImageType;
    typedef itk::ImageFileReader< ImageType >  ReaderType;
    typedef const ReaderType *ReaderTypePointer;

    void Execute(itk::Object *caller, const itk::EventObject & event)
    {
        Execute( (const itk::Object *)caller, event);
    }

    void Execute(const itk::Object * object, const itk::EventObject & event)
    {
        ReaderTypePointer m_reader = dynamic_cast< ReaderTypePointer >( object );
        if( typeid( event ) == typeid( itk::ProgressEvent ) )
        {
            DEBUG_LOG( QString("Progressant...%1").arg( m_reader->GetProgress() ) );
        }
        else
        {
            DEBUG_LOG( QString("No s'ha invocat ProgressEvent") );
        }
    }
};

/*!
    Classe auxiliar per monitorejar el progrés de la lectura d'una sèrie de fitxers
*/
class SeriesProgressCommand : public itk::Command
{
public:
    typedef  SeriesProgressCommand   Self;
    typedef  itk::Command             Superclass;
    typedef  itk::SmartPointer<Self>  Pointer;
    itkNewMacro( Self );

protected:
    SeriesProgressCommand() {};

public:
    typedef Volume::ItkImageType ImageType;
    typedef itk::ImageSeriesReader< ImageType >  SeriesReaderType;
    typedef const SeriesReaderType *SeriesReaderTypePointer;

    void Execute(itk::Object *caller, const itk::EventObject & event)
    {
        Execute( (const itk::Object *)caller, event);
    }

    void Execute(const itk::Object * object, const itk::EventObject & event)
    {
        SeriesReaderTypePointer m_seriesReader = dynamic_cast< SeriesReaderTypePointer >( object );
        if( typeid( event ) == typeid( itk::ProgressEvent ) )
        {
            DEBUG_LOG( QString("Progressant... %1").arg( m_seriesReader->GetProgress() ) );
        }
        else
        {
            DEBUG_LOG( QString("No s'ha invocat ProgressEvent") );
        }
    }
};

/**
    Aquesta classe s'ocupa de les tasques d'obrir tot tipus de fitxer

@author Grup de Gràfics de Girona ( GGG )
*/
class Input : public QObject
{
Q_OBJECT
public:

    /// Tipus d'error que podem tenir
    enum { NoError = 1, SizeMismatch, InvalidFileName, UnknownError };

    Input();
    ~Input();

    /**
     * Carrega un volum a partir del nom de fitxer que se li passi
     * @param fileName
     * @return noError en cas que tot hagi anat bé, el tipus d'error altrament
     */
    int openFile( QString fileName );

    /**
     * Donat un conjunt de fitxers els carrega en una única sèrie/volum
     * @param filenames
     * @return noError en cas que tot hagi anat bé, el tipus d'error altrament
     */
    int readFiles( QStringList filenames );

    /**
     * Lector de sèries dicom donat un directori que les conté
     * @param dirPath
     * @return noError en cas que tot hagi anat bé, el tipus d'error altrament
     */
    int readSeries( QString dirPath );

    /// Retorna un Volum
    Volume* getData() const { return m_volumeData; };

    // Això fa petar aplicació
    //itk::QtSignalAdaptor *m_progressSignalAdaptor;
public slots:
    /// emet el progrés de lectura d'una sèrie d'arxius
    void slotProgress()
    {
        emit progress( (int)( m_seriesReader->GetProgress() * 100 ) );
    }

signals:
    /// Indica el progrés en % de la lectura del fitxer
    void progress( int );

private:
    typedef Volume::ItkImageType ImageType;
    typedef itk::ImageFileReader< ImageType >  ReaderType;
    typedef ReaderType::Pointer    ReaderTypePointer;

    typedef itk::ImageSeriesReader< ImageType >     SeriesReaderType;
    typedef itk::GDCMImageIO                        ImageIOType;
    typedef itk::GDCMSeriesFileNames                NamesGeneratorType;

    /// El lector de sèries dicom
    SeriesReaderType::Pointer m_seriesReader;

    /// El lector estàndar de fitxers singulars, normalment servirà per llegir *.mhd's
    ReaderTypePointer    m_reader;

    /// Les dades llegides en format de volum
    Volume* m_volumeData;

    /// el lector de DICOM
    ImageIOType::Pointer m_gdcmIO;

    /// el generador dels noms dels fitxers DICOM d'un directori
    NamesGeneratorType::Pointer m_namesGenerator;

    /// Demana per un tag DICOM que serà retornat en format string. Retorna true si es troba el tag al header DICOM
    bool queryTagAsString( QString tag , QString &result );

    /// Es dedica a proporcionar al volum la informació que ens dóna el DICOM
    void setVolumeInformation();

    /// a partir dels direction cosines d'un eix ens dóna l'orientació referent al pacient en string
    QString getOrientation( double vector[3] );

    /// converteix un std::vector< std::string > en un QStringList i viceversa
    static QStringList stdVectorOfStdStringToQStringList( std::vector< std::string > vector );
    static std::vector< std::string > qstringListToStdVectorOfStdString( QStringList list );
};

};

#endif
