/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "dicomdirimporter.h"

#include <QDir>
#include <QFile>
#include <QString>

#include "status.h"
#include "starviewersettings.h"
#include "cachestudydal.h"
#include "cacheimagedal.h"
#include "cacheseriesdal.h"
#include "dicomstudy.h"
#include "dicomseries.h"
#include "dicomimage.h"
#include "dicommask.h"
#include "logging.h"
#include "scalestudy.h"
#include "errordcmtk.h"
#include "patientfiller.h"
#include "dicomtagreader.h"
#include "localdatabasemanager.h"

namespace udg
{

Status DICOMDIRImporter::import(QString dicomdirPath, QString studyUID, QString seriesUID, QString sopInstanceUID)
{
    Status state;
    QString studyPath;

    state = m_readDicomdir.open( QDir::toNativeSeparators( dicomdirPath ) );

    if ( !state.good() ) return state;
#ifdef NEW_PACS
    PatientFiller patientFiller;
    LocalDatabaseManager localDatabaseManager;

    connect(this, SIGNAL( imageImportedToDisk(DICOMTagReader*) ), &patientFiller, SLOT( processDICOMFile(DICOMTagReader*) ));
    connect(&patientFiller, SIGNAL( patientProcessed(Patient *) ), &localDatabaseManager, SLOT( insert(Patient *) ));
#endif

    state = importStudy( studyUID , seriesUID , sopInstanceUID );

#ifdef NEW_PACS
    if (state.good())
    {
        patientFiller.finishDICOMFilesProcess();
    }
#endif
    INFO_LOG( "Estudi " + studyUID + " importat" );

    return state;
}

Status DICOMDIRImporter::importStudy(QString studyUID, QString seriesUID, QString sopInstanceUID)
{
    Status state;
    CacheStudyDAL cacheStudyDAL;
    CacheSeriesDAL cacheSeriesDAL;
    DicomMask mask;
    QList<DICOMStudy> studyListToImport;
    QList<DICOMSeries> seriesListToImport;
    QString studyPath;
    StarviewerSettings starviewerSettings;
    DICOMStudy study;
    DICOMSeries serie;
    ScaleStudy scaleDicomStudy;

    studyPath = starviewerSettings.getCacheImagePath() + studyUID + "/";
    QDir directoryCreator;
    directoryCreator.mkdir( studyPath );

    mask.setStudyUID( studyUID );

    m_readDicomdir.readStudies( studyListToImport , mask );

    if (studyListToImport.isEmpty())//comprovem que s'hagin trobat estudis per importar
    {
        ERROR_LOG( "NO S'HAN TROBAT ESTUDIS PER IMPORTAR" );
        state.setStatus( "No s'han trobat estudis per importar" , false , 1310 );
        return state;
    }

    study = studyListToImport.value(0);
    study.setAbsPath( studyPath );

#ifndef NEW_PACS
    state = cacheStudyDAL.insertStudy( &study, "DICOMDIR" );

    if ( state.code() == 2019 ) // si ja existeix l'estudi actualitzem la informació
    {
        cacheStudyDAL.updateStudy( study );
    }
    else
    {
        if ( !state.good() ) ERROR_LOG( state.text() );
    }
#endif

    m_readDicomdir.readSeries( studyUID , seriesUID , seriesListToImport );

    if ( seriesListToImport.isEmpty() ) ERROR_LOG ( "No s'han trobat series per l'estudi" );


    foreach(DICOMSeries seriesToImport, seriesListToImport)
    {
#ifndef NEW_PACS
        cacheSeriesDAL.insertSeries( &seriesToImport );
#endif
        state = importSeries( studyUID , seriesToImport.getSeriesUID() , sopInstanceUID );

        if ( !state.good() ) break;
    }

    if ( state.good() )
    {
        scaleDicomStudy.scale( study.getStudyUID() );

#ifndef NEW_PACS
        state = cacheStudyDAL.setStudyRetrieved( study.getStudyUID() );
#endif
    }

    return state;
}

Status DICOMDIRImporter::importSeries(QString studyUID, QString seriesUID, QString sopInstanceUID)
{
    Status state;
    QList<DICOMImage> imageListToImport;
    QString seriesPath;
    StarviewerSettings starviewerSettings;

    seriesPath = starviewerSettings.getCacheImagePath() + "/" + studyUID + "/" + seriesUID;
    QDir directoryCreator;
    directoryCreator.mkdir( seriesPath );

    m_readDicomdir.readImages( seriesUID , sopInstanceUID , imageListToImport );

    if ( imageListToImport.isEmpty() ) ERROR_LOG ( "No s'han trobat imatges per la serie" );

    foreach(DICOMImage imageToImport, imageListToImport)
    {
        state = importImage( imageToImport );
        if ( !state.good() ) break;
    }
    return state;
}

Status DICOMDIRImporter::importImage(DICOMImage image)
{
    QString cacheImagePath, dicomdirImagePath;
    StarviewerSettings starviewerSettings;
    CacheImageDAL cacheImage;
    Status state;

    cacheImagePath = starviewerSettings.getCacheImagePath() + image.getStudyUID() + "/" + image.getSeriesUID() + "/" + image.getSOPInstanceUID();

    if ( QFile::exists( image.getImagePath() ) )//comprovem si la imatge a importar existeix
    {
        dicomdirImagePath = image.getImagePath();
    }
    else if ( QFile::exists( image.getImagePath().toLower() ) )
    {
        /* Linux per defecte en les unitats vfat, mostra els noms de fitxer que són shortname ( 8 o menys caràcters ) en minúscules
           com que en el fitxer de dicomdir les rutes del fitxer es guarden en majúscules, si fem un exist del nom del fitxer sobre 
           unitats vfat falla, per això el que fem es convertir el nom del fitxer a minúscules
         */
        dicomdirImagePath = image.getImagePath().toLower();
    }
    else
    {
        state.setStatus( "Inconsistent dicomdir, some files don't exist" , false , 1303 );
        ERROR_LOG("Dicomdir inconsistent: La imatge [" + image.getImagePath() + "] no existeix" );
        return state;
    }

    if( QFile::copy( dicomdirImagePath , cacheImagePath ) )
    {
#ifdef NEW_PACS
        DICOMTagReader *dicomTagReader = new DICOMTagReader(cacheImagePath);
        emit imageImportedToDisk(dicomTagReader);
        state.setStatus( DcmtkNoError );
#else
        image.setImageName ( image.getSOPInstanceUID() );
        QFileInfo imageInfo( cacheImagePath );
        if( imageInfo.exists() )
        {
            image.setImageSize( imageInfo.size() );
            state = cacheImage.insertImage( &image ); // TODO no se li hauria de canviar el path, sinó ara conté el del DICOMDIR, no?

            //la imatge ja existeix a la base de dades, en aquest cas l'ignorem l'error ja pot ser que alguna part de les imatges que s'importen les tinguessim en la la base de dades local
            if (state.code() == 2019) state.setStatus( DcmtkNoError );
        }
        else
        {
            //No s'hauria de produir mai aquest error
            ERROR_LOG("Error no s'ha copiat la imatge [" + dicomdirImagePath + "] no s'ha copiat a [" + cacheImagePath + "] " );
            state.setStatus( DcmtkUnknowError );
        }
#endif
    }
    else
    {
        // TODO s'hauria de forçar la sobreescriptura, ara de moment no ho tractem, però la imatge bona hauria de ser la del dicomdir no la de la cache
        //ERROR_LOG("El fitxer: <" + image.getImagePath() + "> no s'ha pogut copiar a <" + imagePath + ">, podria ser que ja existeix amb aquest mateix nom, o que no tinguis permisos en el directori destí");
        //state.setStatus( DcmtkUnknowError );
    }

    return state;
}

}
