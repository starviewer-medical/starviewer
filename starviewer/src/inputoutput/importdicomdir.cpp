/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "status.h"
#include "importdicomdir.h"
#include "imagelist.h"
#include "starviewersettings.h"
#include "cachestudydal.h"
#include "cacheimagedal.h"
#include "cacheseriesdal.h"
#include "studylist.h"
#include "serieslist.h"
#include "imagelist.h"
#include "dicomstudy.h"
#include "dicomseries.h"
#include "dicomimage.h"
#include "dicommask.h"
#include <QDir>
#include <QFile>
#include "starviewersettings.h"
#include "logging.h"
#include "scalestudy.h"

namespace udg
{


Status ImportDicomdir::import( QString dicomdirPath , QString studyUID , QString seriesUID , QString sopInstanceUID )
{
    Status state;
    ImageList imageList;
    QString studyPath;

    state = m_readDicomdir.open( dicomdirPath );

    if ( !state.good() ) return state;

    importarEstudi( studyUID , seriesUID , sopInstanceUID );

    return state;
}

Status ImportDicomdir::importarEstudi( QString studyUID , QString seriesUID , QString sopInstanceUID )
{
    Status state;
    CacheStudyDAL cacheStudyDAL;
    CacheSeriesDAL cacheSeriesDAL;
    DicomMask mask;
    StudyList studyList;
    SeriesList seriesList;
    QString studyPath;
    StarviewerSettings starviewerSettings;
    DICOMStudy study;
    DICOMSeries serie;
    ScaleStudy scaleDicomStudy;

    studyPath.insert( 0 , starviewerSettings.getCacheImagePath() );
    studyPath.append( "/" );
    studyPath.append( studyUID );
    studyPath.append( "/" );

    createPath( studyPath );

    mask.setStudyUID( studyUID );

    m_readDicomdir.readStudies( studyList , mask );
    studyList.firstStudy();

    study = studyList.getStudy();
    study.setAbsPath( studyPath );

    state = cacheStudyDAL.insertStudyDicomdir( &study );

    if ( state.code() == 2019 )
    {
        cacheStudyDAL.updateStudy( study );
    }
    else ERROR_LOG( state.text() );

    m_readDicomdir.readSeries( studyUID , seriesUID , seriesList );

    seriesList.firstSeries();

    if ( seriesList.end() ) ERROR_LOG ( "No s'han trobat series per l'estudi" );


    while ( !seriesList.end() )
    {
        serie = seriesList.getSeries();
        cacheSeriesDAL.insertSeries( &serie );
        importarSerie( studyUID , serie.getSeriesUID() , sopInstanceUID );

        seriesList.nextSeries();
    }

    scaleDicomStudy.scale( studyList.getStudy().getStudyUID() );

    cacheStudyDAL.setStudyRetrieved( studyList.getStudy().getStudyUID() );

    return state;
}

Status ImportDicomdir::importarSerie( QString studyUID , QString seriesUID , QString sopInstanceUID )
{
    Status state;
    ImageList imageList;

    QString seriesPath;
    StarviewerSettings starviewerSettings;

    seriesPath.insert( 0 , starviewerSettings.getCacheImagePath() );
    seriesPath.append( "/" );
    seriesPath.append( studyUID );
    seriesPath.append( "/" );
    seriesPath.append( seriesUID );
    createPath( seriesPath );

    m_readDicomdir.readImages( seriesUID , sopInstanceUID , imageList );

    imageList.firstImage();

    if ( imageList.end() ) ERROR_LOG ( "No s'han trobat imatges per la serie" );

    while ( !imageList.end() )
    {
        importarImatge( imageList.getImage() );
        imageList.nextImage();
    }

    return state;
}

Status ImportDicomdir::importarImatge( DICOMImage image )
{
    QString imagePath, imageFile;
    QFile copyFile;
    StarviewerSettings starviewerSettings;
    CacheImageDAL cacheImage;
    Status state;

    imagePath.insert( 0 , starviewerSettings.getCacheImagePath() );
    imagePath.append( "/" );
    imagePath.append( image.getStudyUID() );
    imagePath.append( "/" );
    imagePath.append( image.getSeriesUID() );
    imagePath.append( "/" );
    imagePath.append( image.getSOPInstanceUID() );

    if ( copyFile.copy( image.getImagePath() , imagePath ) )
    {
        image.setImageName ( image.getSOPInstanceUID() );
        state = cacheImage.insertImage( &image );
    }

    return state;

}

void ImportDicomdir::createPath( QString path )
{
    QDir studyDir;

    studyDir.mkdir( path );
}

}
