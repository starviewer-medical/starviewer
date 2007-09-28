/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "status.h"
#include "dicomdirimporter.h"
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

Status DICOMDIRImporter::import( QString dicomdirPath , QString studyUID , QString seriesUID , QString sopInstanceUID )
{
    Status state;
    ImageList imageList;
    QString studyPath;

    state = m_readDicomdir.open( dicomdirPath );

    if ( !state.good() ) return state;

    importStudy( studyUID , seriesUID , sopInstanceUID );

    return state;
}

Status DICOMDIRImporter::importStudy( QString studyUID , QString seriesUID , QString sopInstanceUID )
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

    studyPath = starviewerSettings.getCacheImagePath() + studyUID + "/";
    QDir directoryCreator;
    directoryCreator.mkdir( studyPath );

    mask.setStudyUID( studyUID );

    m_readDicomdir.readStudies( studyList , mask );
    studyList.firstStudy();

    study = studyList.getStudy();
    study.setAbsPath( studyPath );

    state = cacheStudyDAL.insertStudy( &study, "DICOMDIR" );

    if ( state.code() == 2019 ) // si ja existeix l'estudi actualitzem la informació
    {
        cacheStudyDAL.updateStudy( study );
    }
    else
    {
        if ( !state.good() ) ERROR_LOG( state.text() );
    }

    m_readDicomdir.readSeries( studyUID , seriesUID , seriesList );

    seriesList.firstSeries();

    if ( seriesList.end() ) ERROR_LOG ( "No s'han trobat series per l'estudi" );


    while ( !seriesList.end() )
    {
        serie = seriesList.getSeries();
        cacheSeriesDAL.insertSeries( &serie );
        importSeries( studyUID , serie.getSeriesUID() , sopInstanceUID );

        seriesList.nextSeries();
    }

    scaleDicomStudy.scale( studyList.getStudy().getStudyUID() );

    cacheStudyDAL.setStudyRetrieved( studyList.getStudy().getStudyUID() );

    return state;
}

Status DICOMDIRImporter::importSeries( QString studyUID , QString seriesUID , QString sopInstanceUID )
{
    Status state;
    ImageList imageList;
    QString seriesPath;
    StarviewerSettings starviewerSettings;

    seriesPath = starviewerSettings.getCacheImagePath() + "/" + studyUID + "/" + seriesUID;
    QDir directoryCreator;
    directoryCreator.mkdir( seriesPath );

    m_readDicomdir.readImages( seriesUID , sopInstanceUID , imageList );

    imageList.firstImage();

    if ( imageList.end() ) ERROR_LOG ( "No s'han trobat imatges per la serie" );

    while ( !imageList.end() )
    {
        importImage( imageList.getImage() );
        imageList.nextImage();
    }

    return state;
}

Status DICOMDIRImporter::importImage(DICOMImage image)
{
    QString imagePath, imageFile;
    StarviewerSettings starviewerSettings;
    CacheImageDAL cacheImage;
    Status state;

    imagePath = starviewerSettings.getCacheImagePath() + "/" + image.getStudyUID() + "/" + image.getSeriesUID() + "/" + image.getSOPInstanceUID();

    if( QFile::copy( image.getImagePath() , imagePath ) )
    {
        image.setImageName ( image.getSOPInstanceUID() );
        QFileInfo imageInfo( imagePath );
        if( imageInfo.exists() )
        {
            image.setImageSize( imageInfo.size() );
            state = cacheImage.insertImage( &image ); // TODO no se li hauria de canviar el path, sinó ara conté el del DICOMDIR, no?
        }
        else
        {
            ERROR_LOG("La imatge [" + imagePath + "] que s'ha volgut copiar de [" + image.getImagePath()  + "] no existeix" );
        }
    }
    else
    {
        // TODO no s'hauria de forçar la sobre-escriptura????
        DEBUG_LOG("El fitxer: <" + image.getImagePath() + "> no s'ha pogut copiar a <" + imagePath + ">, ja que ja existeix amb aquest mateix nom");
    }

    return state;
}

}
