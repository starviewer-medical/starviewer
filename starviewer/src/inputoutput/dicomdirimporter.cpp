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
#include "localdatabasemanagerthreaded.h"
#include "qthreadrunwithexec.h"

namespace udg
{

bool DICOMDIRImporter::import(QString dicomdirPath, QString studyUID, QString seriesUID, QString sopInstanceUID)
{
    Status state = m_readDicomdir.open( QDir::toNativeSeparators( dicomdirPath ) );

    if ( !state.good() ) return false;
    
    LocalDatabaseManagerThreaded localDatabaseManagerThreaded;
    PatientFiller patientFiller;
    QThreadRunWithExec fillersThread;
    patientFiller.moveToThread( &fillersThread );
    
    //Connexions entre la descarrega i el processat dels fitxers
    connect(this, SIGNAL( imageImportedToDisk(DICOMTagReader*) ), &patientFiller, SLOT( processDICOMFile(DICOMTagReader*) ));
    connect(this, SIGNAL( importFinished() ), &patientFiller, SLOT( finishDICOMFilesProcess() ));
    
    //Connexió entre el processat i l'insersió al a BD
    connect(&patientFiller, SIGNAL( patientProcessed(Patient*) ), &localDatabaseManagerThreaded, SLOT( save(Patient*) ), Qt::DirectConnection);
    
    //Connexions per finalitzar els threads
    connect(&patientFiller, SIGNAL( patientProcessed(Patient*) ), &fillersThread, SLOT( quit() ), Qt::DirectConnection);
    connect(&localDatabaseManagerThreaded, SIGNAL( operationFinished(LocalDatabaseManagerThreaded::OperationType) ), &localDatabaseManagerThreaded, SLOT( quit() ), Qt::DirectConnection);

    //Connexions d'abortament
    connect(this, SIGNAL( importAborted() ), &fillersThread, SLOT( quit() ), Qt::DirectConnection );
    connect(this, SIGNAL( importAborted() ), &localDatabaseManagerThreaded, SLOT( quit() ), Qt::DirectConnection );
    
    localDatabaseManagerThreaded.start();
    fillersThread.start();

    bool ok = importStudy( studyUID , seriesUID , sopInstanceUID );

    if ( ok )
    {
        emit importFinished();
    }
    else
    {
        emit importAborted();
    }

    //Esperem que el processat i l'insersió a la base de dades acabin
    fillersThread.wait();
    localDatabaseManagerThreaded.wait();

    //Comprovem que s'hagi inserit correctament el nou estudi a la base de dades
    if (ok && localDatabaseManagerThreaded.getLastError() == LocalDatabaseManager::Ok)
    {
        INFO_LOG( "Estudi " + studyUID + " importat" );
        return true;
    }
    else return false;
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

    if (QFile::exists(image.getImagePath()))//comprovem si la imatge a importar existeix
    {
        dicomdirImagePath = image.getImagePath();
    }
    else if (QFile::exists(image.getImagePath().toLower()))
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

    if(!copyDicomdirImageToLocal(dicomdirImagePath, cacheImagePath))
    {
        //No s'ha pogut copiar comprovem, si és que el fitxer ja existeix
        if (QFile::exists(cacheImagePath))
        {
            //El fitxer ja existeix l'itentem esborrar
            if (QFile::remove(cacheImagePath))
            {
                //Hem esborrar el fitxer que ja existia, ara l'intentem copiar
                if(!copyDicomdirImageToLocal(dicomdirImagePath, cacheImagePath))
                {
                    ERROR_LOG("El fitxer: <" + dicomdirImagePath + "> no s'ha pogut copiar a <" + cacheImagePath + ">, el fitxer ja existia al destí, s'ha esborrat amb èxit, però alhora de copiar-lo ha fallat l'operació");
                    return false;
                }
            }
            else 
            {
                ERROR_LOG("El fitxer: <" + dicomdirImagePath + "> no s'ha pogut copiar a <" + cacheImagePath + ">, ja que el fitxer ja existeix al destí, s'ha intentat esborrar el fitxer local però ha fallat, podria ser que no tinguis permisos d'escriptura al direcctori destí");
                return false;
            }
        }
        else
        {
            ERROR_LOG("El fitxer: <" + dicomdirImagePath + "> no s'ha pogut copiar a <" + cacheImagePath + ">, podria ser que no tinguis permisos en el directori destí");
            return false;
        }
    }
    return true;
}

bool DICOMDIRImporter::copyDicomdirImageToLocal(QString dicomdirImagePath, QString localImagePath)
{
    if(QFile::copy(dicomdirImagePath, localImagePath))
    {
        DICOMTagReader *dicomTagReader = new DICOMTagReader(localImagePath);
        emit imageImportedToDisk(dicomTagReader);
        return true;
    }
    else return false;
}

}
