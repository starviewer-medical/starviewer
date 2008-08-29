/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "scalestudy.h"
#include "dicommask.h"
#include "status.h"
#include "dicomimage.h"
#include "starviewersettings.h"
#include "cacheseriesdal.h"
#include "cacheimagedal.h"

#include "image.h" // per scalar l'imatge
#include "dicomseries.h"
#include "logging.h"
#include "series.h"

namespace udg {

ScaleStudy::ScaleStudy()
{
}

void ScaleStudy::scale( QString studyUID )
{
    Status state;
    QList<DICOMSeries> seriesList;
    DicomMask mask;
    int imageNumber;
    QString absPath , relPath, absPathScal, imageNumberString;
    StarviewerSettings settings;
    state = getSeriesOfStudy( studyUID,seriesList ); //busquem les sèries de l'estudi

    foreach( DICOMSeries series , seriesList ) //escalem una imatge per cada sèrie
    {
        //preparem la màscara per buscar la imatge del mig de l'estudi
        mask.setSeriesUID( series.getSeriesUID() );
        mask.setStudyUID( studyUID );
        state = countImageNumber(mask, imageNumber);//comptem el número d'imatges, per saber quina és la imatge del mig

        mask.setImageNumber( imageNumberString.setNum(imageNumber / 2 + 1) );//indiquem per quin número d' imatge hem de buscar el path

        relPath.clear();
        imageRelativePath( mask ,relPath );//busquem el path de la imatge del mig

        if (  relPath.length() == 0 )
        {/*hi ha algunes màquines per error tenen el número d'imatge 1 sempre, encara que l'estudi tingui 10 imatges
          *En aquest cas no funciona buscar la imatge del mig, i el que es fa és mostrar la primera imatge que trobem
          */
            mask.setImageNumber( "" );
            imageRelativePath( mask ,relPath );
        }
        absPath = settings.getCacheImagePath() + relPath; //creem el path absolut a la imatge a la imatge

        QImage thumbnail;
        QString modality = series.getSeriesModality();
        if( modality == "KO" )
            thumbnail.load(":/images/kinThumbnail.png");
        else if( modality == "PR" )
            thumbnail.load(":/images/presentationStateThumbnail.png");
        else if( modality == "SR" )
            thumbnail.load(":/images/structuredReportThumbnail.png");
        else
        {
            Image image;
            image.setPath(absPath);
            thumbnail = image.createThumbnail();
        }

        if (!thumbnail.save( this->getScaledImagePath( &series )))
        {
            ERROR_LOG("No s'ha pogut guardar el thumbnail a " + this->getScaledImagePath( &series ));
        }
    }
}

//TODO TREURE AQUEST MÈTODE QUAN NO S'UTILITZI EL DICOMSeries
QString ScaleStudy::getScaledImagePath(DICOMSeries* series)
{
    StarviewerSettings settings;
    return settings.getCacheImagePath() + series->getStudyUID() + "/" + series->getSeriesUID() + "/scaled.png";
}

QString ScaleStudy::getScaledImagePath(QString studyInstanceUID, Series *series)
{
    StarviewerSettings settings;
    return settings.getCacheImagePath() + studyInstanceUID + "/" + series->getInstanceUID() + "/scaled.png";
}

Status ScaleStudy::getSeriesOfStudy( QString studyUID , QList<DICOMSeries> &outResultsSeriesList )
{
    DicomMask mask;
    CacheSeriesDAL cacheSeriesDAL;

    mask.setStudyUID( studyUID );

    return cacheSeriesDAL.querySeries( mask , outResultsSeriesList );
}

Status ScaleStudy::countImageNumber( DicomMask mask, int &number )
{
    CacheImageDAL cacheImageDAL;

    return cacheImageDAL.countImageNumber( mask , number );
}

Status ScaleStudy::imageRelativePath( DicomMask mask , QString &relPath )
{
    QList<DICOMImage> imageList;
    DICOMImage image;
    Status state;

    CacheImageDAL cacheImageDAL;

    state = cacheImageDAL.queryImages( mask , imageList );

    if ( !imageList.isEmpty() )
    {
        image = imageList.value( 0 );

        relPath = QString("%1/%2/%3")
            .arg( image.getStudyUID() )
            .arg( image.getSeriesUID() )
            .arg( image.getImageName() );
    }
    return state;
}

ScaleStudy::~ScaleStudy()
{
}

};
