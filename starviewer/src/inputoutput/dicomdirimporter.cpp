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

void DICOMDIRImporter::import(QString dicomdirPath, QString studyUID, QString seriesUID, QString sopInstanceUID)
{
    m_lastError = Ok;
    Status state = m_readDicomdir.open( QDir::toNativeSeparators( dicomdirPath ) );

    if (!state.good()) 
    {
        m_lastError = ErrorOpeningDicomdir;
        return;
    }

    LocalDatabaseManagerThreaded localDatabaseManagerThreaded;
    PatientFiller patientFiller;
    QThreadRunWithExec fillersThread;
    patientFiller.moveToThread(&fillersThread);

    //Creem les connexions necessàries per importar dicomdirs
    createConnections(&patientFiller, &localDatabaseManagerThreaded, &fillersThread);

    localDatabaseManagerThreaded.start();
    fillersThread.start();

    importStudy(studyUID, seriesUID, sopInstanceUID);

    if (getLastError() == Ok)
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
    if (getLastError() == Ok)
    {
        if (localDatabaseManagerThreaded.getLastError() == LocalDatabaseManager::Ok)
        {
            INFO_LOG( "Estudi " + studyUID + " importat" );
            m_lastError = Ok;
        }
        else m_lastError = DatabaseError;
    }
}

void DICOMDIRImporter::importStudy(QString studyUID, QString seriesUID, QString sopInstanceUID)
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

    if (!studyListToImport.isEmpty())//comprovem que s'hagin trobat estudis per importar
    {
        study = studyListToImport.value(0);
        study.setAbsPath( studyPath );

        m_readDicomdir.readSeries( studyUID , seriesUID , seriesListToImport );

        if ( seriesListToImport.isEmpty() )
        {
            ERROR_LOG ( "No s'han trobat series per l'estudi" );
            m_lastError = DicomdirInconsistent;
            return;
        }

        foreach(DICOMSeries seriesToImport, seriesListToImport)
        {
            importSeries(studyUID, seriesToImport.getSeriesUID(), sopInstanceUID);
            if (getLastError() != Ok) break;
        }
    }
    else 
    {
        ERROR_LOG("No s'ha trobat estudi per importar");
        m_lastError = DicomdirInconsistent;
    }
}

void DICOMDIRImporter::importSeries(QString studyUID, QString seriesUID, QString sopInstanceUID)
{
    QList<DICOMImage> imageListToImport;
    QString seriesPath;
    StarviewerSettings starviewerSettings;

    seriesPath = starviewerSettings.getCacheImagePath() + "/" + studyUID + "/" + seriesUID;
    QDir directoryCreator;
    directoryCreator.mkdir( seriesPath );

    m_readDicomdir.readImages( seriesUID , sopInstanceUID , imageListToImport );

    if ( imageListToImport.isEmpty() )
    {
        ERROR_LOG ( "No s'han trobat imatges per la serie" );
        m_lastError = DicomdirInconsistent;
        return;
    }

    foreach(DICOMImage imageToImport, imageListToImport)
    {
        importImage(imageToImport);
        if (getLastError() != Ok) break;
    }
}

void DICOMDIRImporter::importImage(DICOMImage image)
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
        m_lastError = DicomdirInconsistent;
        return;
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
                    m_lastError = ErrorCopyingFiles;
                }
            }
            else 
            {
                ERROR_LOG("El fitxer: <" + dicomdirImagePath + "> no s'ha pogut copiar a <" + cacheImagePath + ">, ja que el fitxer ja existeix al destí, s'ha intentat esborrar el fitxer local però ha fallat, podria ser que no tinguis permisos d'escriptura al direcctori destí");
                m_lastError = ErrorCopyingFiles;
            }
        }
        else
        {
            ERROR_LOG("El fitxer: <" + dicomdirImagePath + "> no s'ha pogut copiar a <" + cacheImagePath + ">, podria ser que no tinguis permisos en el directori destí");
            m_lastError = ErrorCopyingFiles;
        }
    }
}

bool DICOMDIRImporter::copyDicomdirImageToLocal(QString dicomdirImagePath, QString localImagePath)
{
    if(QFile::copy(dicomdirImagePath, localImagePath))
    {
        // donem permisos per si l'arxiu encara és read only al provenir d'un CD
        if( !QFile::setPermissions( localImagePath, QFile::WriteOwner | QFile::ReadOwner | QFile::ReadGroup | QFile::ReadOther) )
        {
                WARN_LOG( "No hem pogut canviar els permisos de lectura/escriptura pel fitxer importat [" + localImagePath + "]" );
        }
        // TODO perquè cal fer aquest DICOMTagReader? Encara es fa servir la cache de dicom tag reader????
        DICOMTagReader *dicomTagReader = new DICOMTagReader(localImagePath);
        emit imageImportedToDisk(dicomTagReader);
        return true;
    }
    else return false;
}

void DICOMDIRImporter::createConnections(PatientFiller *patientFiller, LocalDatabaseManagerThreaded *localDatabaseManagerThreaded, QThreadRunWithExec *fillersThread)
{
    //Connexions entre la descarrega i el processat dels fitxers
    connect(this, SIGNAL(imageImportedToDisk(DICOMTagReader*)), patientFiller, SLOT(processDICOMFile(DICOMTagReader*)));
    connect(this, SIGNAL(importFinished()), patientFiller, SLOT(finishDICOMFilesProcess()));

    //Connexió entre el processat i l'insersió al a BD
    connect(patientFiller, SIGNAL(patientProcessed(Patient*)), localDatabaseManagerThreaded, SLOT(save(Patient*)), Qt::DirectConnection);

    //Connexions per finalitzar els threads
    connect(patientFiller, SIGNAL(patientProcessed(Patient*)), fillersThread, SLOT(quit()), Qt::DirectConnection);
    connect(localDatabaseManagerThreaded, SIGNAL(operationFinished(LocalDatabaseManagerThreaded::OperationType)), localDatabaseManagerThreaded, SLOT(quit()), Qt::DirectConnection);

    //Connexions d'abortament
    connect(this, SIGNAL(importAborted()), fillersThread, SLOT(quit()), Qt::DirectConnection);
    connect(this, SIGNAL(importAborted()), localDatabaseManagerThreaded, SLOT(quit()), Qt::DirectConnection);
}

DICOMDIRImporter::DICOMDIRImporterError DICOMDIRImporter::getLastError()
{
    return m_lastError;
}

}
