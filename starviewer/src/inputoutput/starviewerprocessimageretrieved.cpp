/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "starviewerprocessimageretrieved.h"

#include <QString>
#include <QDir>

#include "dicomtagreader.h"
#include "dicomseries.h"
#include "starviewersettings.h"
#include "status.h"
#include "dicomimage.h"
#include "cachestudydal.h"
#include "cacheseriesdal.h"
#include "cacheimagedal.h"
#include "logging.h"

namespace udg {

StarviewerProcessImageRetrieved::StarviewerProcessImageRetrieved() : ProcessImage()
{
    m_downloadedImages = 0;
    m_error = false;
    m_downloadedSeries = 0;
}

void StarviewerProcessImageRetrieved::process( DICOMImage *image )
{
    Status state;
    CacheStudyDAL cacheStudyDAL;
    CacheImageDAL cacheImageDAL;
    DICOMSeries serie;

    /*si es la primera imatge que es descarrega fem un signal indicant que comença la descarrega, inserim la primera serie, i ara que tenim la informació de la sèrie update la modalitat de l'estudi*/
    if ( m_downloadedImages == 0 )
    {
        DICOMSeries serie;

        //canviem l'estat de l'estudi de PENDING A RETRIEVING
        state = cacheStudyDAL.setStudyRetrieving( image->getStudyUID() );
        if ( !state.good() ) m_error = true;

        //inserim serie
        insertSerie( image );

        m_error = ! getSeriesInformation(createImagePath(image), serie);

        m_studyUID = image->getStudyUID();
    }

    //inserim la nova sèrie que es descarrega
    if ( !m_addedSeriesList.contains( image->getSeriesUID() ) )
    {
        if ( insertSerie(image) )
        {
            emit( seriesRetrieved( image->getStudyUID() ) );

            if ( m_downloadedSeries == 0 )
            {
                emit( seriesView( image->getStudyUID() ) ); //aquest signal s'emet cap a qexecoperationthread, indicant que hi ha apunt una serie per ser visualitzada
            }
            m_downloadedSeries++;
        }
    }

    DICOMTagReader dicomFile;
    dicomFile.setFile( createImagePath(image) );

    //inserim imatge
    state = cacheImageDAL.insertImage( image );
    //si es produeix error no podem cancel·lar la descarregar, tirem endavant, quant finalitzi la descarregar avisarem de l'error, en cas de l'error 2019 és que la imatge ja existia a la cache
    if ( !state.good() && state.code() != 2019 ) m_error = true;

    m_downloadedImages++;
    emit( imageRetrieved( image->getStudyUID(),m_downloadedImages ) );
}

bool StarviewerProcessImageRetrieved::insertSerie(DICOMImage *newImage)
{
    DICOMSeries serie;
    bool ok = false;

    //inserim serie
    if ( getSeriesInformation(createImagePath(newImage), serie) )
    {
        CacheSeriesDAL cacheSeriesDAL;
        Status status = cacheSeriesDAL.insertSeries( &serie );
        //Podria ser que la sèrie estigués parcialment baixada, per tant si ja existeix ignorem l'errorProcessImage() de que ja existeix la sèrie a la base de dades
        if ( status.good() || status.code() == 2019 )
        {
            m_addedSeriesList.push_back( serie.getSeriesUID() );
            ok = true;
        }
        else
        {
            m_error = true;
        }
    }

    return ok;
}

/* Ara per ara per la configuració de les dcmtk no he descober com cancel·lar la descarrega d'imatges despres de produir-se un error, l'únic solució possible ara mateix i que m'han aconsellat als forums es matar el thread però aquesta idea no m'agrada perquè si matem el thread no desconnectem del PACS, no destruim les senyals amb el QRetreiveScreen i no esborrem el thread de la llista retrieveThreads, per tant de moment el que es farà es donar el error quant hagi finalitzat la descarrega
  */
void StarviewerProcessImageRetrieved::setError()
{
    m_error = true;
    ERROR_LOG( "Error descarregant l'estudi" );
}

bool StarviewerProcessImageRetrieved::getError()
{
    if ( m_downloadedImages == 0)
    {
        ERROR_LOG( "Error s'han descarregat 0 imatges de l'estudi" );
    }
    return m_error || m_downloadedImages == 0;
}

bool StarviewerProcessImageRetrieved::getSeriesInformation( QString imagePath, DICOMSeries &serie )
{
    DICOMTagReader dicomFile;

    if ( dicomFile.setFile(imagePath) )
    {
        serie.setStudyUID( dicomFile.getAttributeByName(DCM_StudyInstanceUID) );
        serie.setSeriesUID( dicomFile.getAttributeByName(DCM_SeriesInstanceUID) );
        serie.setSeriesNumber( dicomFile.getAttributeByName(DCM_SeriesNumber) );
        serie.setSeriesModality( dicomFile.getAttributeByName(DCM_Modality) );
        serie.setSeriesDescription( dicomFile.getAttributeByName(DCM_SeriesDescription) );
        serie.setBodyPartExaminated( dicomFile.getAttributeByName(DCM_BodyPartExamined) );
        serie.setProtocolName( dicomFile.getAttributeByName(DCM_ProtocolName) );
        serie.setSeriesTime( dicomFile.getAttributeByName(DCM_SeriesTime) );
        serie.setSeriesDate( dicomFile.getAttributeByName(DCM_SeriesDate) );

        //calculem el path de la serie que es crea a partir de l'StudyInstanceUID+SeriesInstanceUID
        serie.setSeriesPath( QDir::toNativeSeparators( dicomFile.getAttributeByName(DCM_StudyInstanceUID) + "/" + dicomFile.getAttributeByName(DCM_SeriesInstanceUID) + "/" ) );

        return true;
    }
    else
    {
        return false;
    }
}

QString StarviewerProcessImageRetrieved::createImagePath( DICOMImage *image )
{
    StarviewerSettings settings;
    DICOMSeries serie;
    QString imagePath;

    imagePath.insert( 0 , settings.getCacheImagePath() );
    imagePath.append( image->getStudyUID() );
    imagePath.append( "/" );
    imagePath.append( image->getSeriesUID() );
    imagePath.append("/");
    imagePath.append( image->getImageName() );

    return QDir::toNativeSeparators( imagePath );
}

StarviewerProcessImageRetrieved::~StarviewerProcessImageRetrieved()
{
    //com no sabem quant s'acaba la descàrrega de l'última sèrie, fem que s'indiqui que ha finalitzat la seva descàrrega quan es destrueix l'objecte StarviewerProcessImageRetrieved, que és destruït just finalitzar la descarrega de tot l'estudi
	emit( seriesRetrieved( m_studyUID ) );

}

};

