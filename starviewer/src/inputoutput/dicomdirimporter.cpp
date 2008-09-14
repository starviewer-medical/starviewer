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
#include "dicomstudy.h"
#include "dicomseries.h"
#include "dicomimage.h"
#include "dicommask.h"
#include "logging.h"
#include "errordcmtk.h"
#include "patientfiller.h"
#include "dicomtagreader.h"
#include "localdatabasemanager.h"

namespace udg
{

bool DICOMDIRImporter::import(QString dicomdirPath, QString studyUID, QString seriesUID, QString sopInstanceUID)
{
    Status state = m_readDicomdir.open( QDir::toNativeSeparators( dicomdirPath ) );

    if ( !state.good() ) return false;

    PatientFiller patientFiller;
    LocalDatabaseManager localDatabaseManager;

    connect(this, SIGNAL( imageImportedToDisk(DICOMTagReader*) ), &patientFiller, SLOT( processDICOMFile(DICOMTagReader*) ));
    connect(&patientFiller, SIGNAL( patientProcessed(Patient *) ), &localDatabaseManager, SLOT( insert(Patient *) ));

    bool ok = importStudy( studyUID , seriesUID , sopInstanceUID );

    if (ok)
    {
        patientFiller.finishDICOMFilesProcess();
    }

    INFO_LOG( "Estudi " + studyUID + " importat" );

    return ok;
}

bool DICOMDIRImporter::importStudy(QString studyUID, QString seriesUID, QString sopInstanceUID)
{
    DicomMask mask;
    QList<DICOMStudy> studyListToImport;
    QList<DICOMSeries> seriesListToImport;
    QString studyPath;
    StarviewerSettings starviewerSettings;
    DICOMStudy study;
    DICOMSeries serie;

    studyPath = starviewerSettings.getCacheImagePath() + studyUID + "/";
    QDir directoryCreator;
    directoryCreator.mkdir( studyPath );

    mask.setStudyUID( studyUID );

    m_readDicomdir.readStudies( studyListToImport , mask );

    if (studyListToImport.isEmpty())//comprovem que s'hagin trobat estudis per importar
    {
        ERROR_LOG( "No s'han trobat estudis per importar" );
        return false;
    }

    study = studyListToImport.value(0);
    study.setAbsPath( studyPath );

    m_readDicomdir.readSeries( studyUID , seriesUID , seriesListToImport );

    if ( seriesListToImport.isEmpty() ) ERROR_LOG ( "No s'han trobat series per l'estudi" );


    foreach(DICOMSeries seriesToImport, seriesListToImport)
    {
        if ( !importSeries(studyUID, seriesToImport.getSeriesUID(), sopInstanceUID) ) return false;
    }

    return true;
}

bool DICOMDIRImporter::importSeries(QString studyUID, QString seriesUID, QString sopInstanceUID)
{
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
        if ( !importImage(imageToImport) ) return false;
    }

    return true;
}

bool DICOMDIRImporter::importImage(DICOMImage image)
{
    QString cacheImagePath, dicomdirImagePath;
    StarviewerSettings starviewerSettings;

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
        ERROR_LOG("Dicomdir inconsistent: La imatge [" + image.getImagePath() + "] no existeix" );
        return false;
    }

    if( QFile::copy( dicomdirImagePath , cacheImagePath ) )
    {
        DICOMTagReader *dicomTagReader = new DICOMTagReader(cacheImagePath);
        emit imageImportedToDisk(dicomTagReader);
    }
    else
    {
        // TODO s'hauria de forçar la sobreescriptura, ara de moment no ho tractem, però la imatge bona hauria de ser la del dicomdir no la de la cache
        //ERROR_LOG("El fitxer: <" + image.getImagePath() + "> no s'ha pogut copiar a <" + imagePath + ">, podria ser que ja existeix amb aquest mateix nom, o que no tinguis permisos en el directori destí");
    }

    return true;
}

}
