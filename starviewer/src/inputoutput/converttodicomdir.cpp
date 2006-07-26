/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "converttodicomdir.h"

#include <QString>
#include <QProgressDialog>
#include <QStringList>
#include <QDir>
#include <QChar>

#include "logging.h"
#include "status.h"
#include "cacheseriesdal.h"
#include "cacheimagedal.h"
#include "seriesmask.h"
#include "imagemask.h"
#include "serieslist.h"
#include "imagelist.h"
#include "study.h"
#include "series.h"
#include "image.h"
#include "convertdicomtolittleendian.h"
#include "starviewersettings.h"
#include "createdicomdir.h"
#include "deletedirectory.h"

namespace udg {

ConvertToDicomdir::ConvertToDicomdir()
{
    QDir dicomDir;

    m_study = 0;
    m_series = 0;
    m_image = 0;
}

void ConvertToDicomdir::addStudy( QString studyUID )
{
    m_studiesToConvert.push_back( studyUID );
}

Status ConvertToDicomdir::convert( QString dicomdirPath )
{
    CacheImageDAL cacheImageDAL;
    //creem el nom del directori de l'estudi el format és STUXXXXX, on XXXXX és el numero d'estudi dins el dicomdir
    ImageMask imageMask;
    Status state;
    int imageNumberStudy , imageNumberTotal = 0 , i = 0 ;
    CreateDicomdir createDicomdir;
    QString studyUID;
    DeleteDirectory deleteDirectory;
    
    m_dicomDirPath = dicomdirPath;

    //comptem el numero d'imatges pel progress de la barra
    while ( i < m_studiesToConvert.count() )
    {   
        imageMask.setStudyUID( m_studiesToConvert.value(i).toAscii().constData() );
        state = cacheImageDAL.countImageNumber( imageMask , imageNumberStudy );
        if ( !state.good() ) break;
            
        imageNumberTotal = imageNumberStudy + imageNumberTotal;
        i++;
    }

    if ( !state.good() )
    {
        deleteDirectory.deleteDirectory( dicomdirPath );
        return state;
    }

    //sumem una imatge més per evitar que arribi el 100 % la progress bar, i així s'esperi a que es crei el dicomdir, que es fa quan s'invoca createDicomdir.Create()
    m_progress = new QProgressDialog( tr( "Creating Dicomdir..." ) , "" , 0 , imageNumberTotal + 1 );
    m_progress->setMinimumDuration( 0 );
    m_progress->setCancelButton( 0 );

    while ( !m_studiesToConvert.isEmpty() ) //per cada estudi a convertir
    {   
        studyUID = m_studiesToConvert.takeFirst();
        state = convertStudy( studyUID );        

        if ( !state.good() ) break; 
    }
 
    if ( !state.good() )
    {
        deleteDirectory.deleteDirectory( dicomdirPath );
        retunr state;
    }
   
    state = createDicomdir.create ( m_dicomDirPath.toAscii().constData() );//invoquem el mètode per convertir el directori destí Dicomdir on ja s'han copiat les imatges en un dicomdir
    
    m_progress->close();
    
    if ( !state.good() )
    {
        deleteDirectory.deleteDirectory( dicomdirPath );
    }

    return state;
}

Status ConvertToDicomdir::convertStudy( QString studyUID )
{
    CacheSeriesDAL cacheSeriesDAL;
    QDir studyDir;
    QChar fillChar = '0';    
    QString studyName = QString( "/STU%1" ).arg( m_study , 5 , 10 , fillChar );
    SeriesList seriesList;
    SeriesMask seriesMask;
    Series series;
    m_study++;
    m_series = 0;
    Status state;
    
    //Creem el directori on es guardar l'estudi en format DicomDir
    m_dicomDirStudyPath = m_dicomDirPath + studyName;
    studyDir.mkdir( m_dicomDirStudyPath );

    seriesMask.setStudyUID( studyUID.toAscii().constData() );
    
    state = cacheSeriesDAL.querySeries( seriesMask , seriesList ); //cerquem sèries de l'estudi

    if ( !state.good() ) return state;
    
    seriesList.firstSeries();

    while ( !seriesList.end() ) //per cada sèrie de l'estudi
    {
        state = convertSeries( seriesList.getSeries() );
        
        if ( !state.good() ) 
        {
            break;
        }
        else seriesList.nextSeries();
    }

    return state;
}

Status ConvertToDicomdir::convertSeries( Series series )
{
    QDir seriesDir;
    QChar fillChar = '0';    
    CacheImageDAL cacheImageDAL;
    //creem el nom del directori de la sèrie, el format és SERXXXXX, on XXXXX és el numero de sèrie dins l'estudi
    QString seriesName = QString( "/SER%1" ).arg( m_series , 5 , 10 , fillChar ); 
    Image image;
    ImageMask imageMask;
    ImageList imageList;
    Status state;

    m_series++;
    m_image = 0;
    //Creem el directori on es guardarà la sèrie en format DicomDir
    m_dicomDirSeriesPath = m_dicomDirStudyPath + seriesName;
    seriesDir.mkdir( m_dicomDirSeriesPath );

    imageMask.setSeriesUID( series.getSeriesUID() );
    imageMask.setStudyUID( series.getStudyUID() );

    state = cacheImageDAL.queryImages( imageMask , imageList ); // cerquem imatges de la sèrie

    if ( !state.good() ) return state;
    
    imageList.firstImage();

    while ( !imageList.end() ) //per cada imatge de la sèrie
    {
        state = convertImage( imageList.getImage() );
        
        if ( !state.good() )
        {
            break;
        }
        else imageList.nextImage();
    }
    
    return state;
}

Status ConvertToDicomdir::convertImage( Image image )
{
    QChar fillChar = '0';    
    //creem el nom del fitxer de l'imatge, el format és IMGXXXXX, on XXXXX és el numero d'imatge dins la sèrie
    QString  imageName = QString( "/IMG%1" ).arg( m_image , 5 , 10 , fillChar ) , imageInputPath , imageOutputPath; 
    ConvertDicomToLittleEndian convertDicom;
    StarviewerSettings settings;
    Series serie;
    Status state;    
    
    m_image++;

    //Creem el path de la imatge
    imageInputPath.insert( 0 , settings.getCacheImagePath() );
    imageInputPath.append( image.getStudyUID().c_str() );
    imageInputPath.append( "/" );
    imageInputPath.append( image.getSeriesUID().c_str() );
    imageInputPath.append( "/" );
    imageInputPath.append( image.getImageName().c_str() );

    imageOutputPath = m_dicomDirSeriesPath + imageName;

    //convertim la imatge a littleEndian, demanat per la normativa DICOM
    state = convertDicom.convert( imageInputPath.toAscii().constData() , imageOutputPath.toAscii().constData() );

     m_progress->setValue( m_progress->value() + 1 ); // la barra de progrés avança
     m_progress->repaint();
    
    return state;
}

ConvertToDicomdir::~ConvertToDicomdir()
{
}

}
