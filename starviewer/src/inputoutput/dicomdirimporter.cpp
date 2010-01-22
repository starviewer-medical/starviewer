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
#include "study.h"
#include "series.h"
#include "image.h"
#include "dicommask.h"
#include "logging.h"
#include "errordcmtk.h"
#include "patientfiller.h"
#include "dicomtagreader.h"
#include "localdatabasemanager.h"
#include "localdatabasemanagerthreaded.h"
#include "qthreadrunwithexec.h"
#include "localdatabasemanager.h"
#include "deletedirectory.h"

namespace udg
{

void DICOMDIRImporter::import(QString dicomdirPath, QString studyUID, QString seriesUID, QString sopInstanceUID)
{
    m_lastError = Ok;
    LocalDatabaseManager localDatabaseManager;
    LocalDatabaseManagerThreaded localDatabaseManagerThreaded;
    PatientFiller patientFiller;
    QThreadRunWithExec fillersThread;

    //Comprovem si hi ha suficient espai lliure per importar l'estudi
    if (!localDatabaseManager.thereIsAvailableSpaceOnHardDisk())
    {
        if (localDatabaseManager.getLastError() != LocalDatabaseManager::Ok)
        {
            m_lastError = ErrorFreeingSpace;
        }
        else m_lastError = NoEnoughSpace;

        return;
    }

    //obrim el dicomdir
    Status state = m_readDicomdir.open( QDir::toNativeSeparators( dicomdirPath ) );

    if (!state.good()) 
    {
        m_lastError = ErrorOpeningDicomdir;
        return;
    }

    m_qprogressDialog = new QProgressDialog("","", 0, 0);
    m_qprogressDialog->setModal(true);
    m_qprogressDialog->setCancelButton(0);
    m_qprogressDialog->setValue(1);
    m_qprogressDialog->setMinimumDuration(0);

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

    //Comprovem que s'hagi importat correctament el nou estudi 
    if (getLastError() != Ok)
    {
        deleteFailedImportedStudy(studyUID); // si hi hagut un error borrem els fitxers importats de l'estudi de la cache local
    }
    else
    {
        //Comprovem que s'hagi inserit correctament el nou estudi a la base de dades
        if (localDatabaseManagerThreaded.getLastError() == LocalDatabaseManager::Ok)
        {
            INFO_LOG( "Estudi " + studyUID + " importat" );
            m_lastError = Ok;
        }
        else 
        {
            if (localDatabaseManagerThreaded.getLastError() == LocalDatabaseManager::PatientInconsistent)
            {
                //No s'ha pogut inserir el patient, perquè patientfiller no ha pogut emplenar l'informació de patient correctament
                m_lastError = PatientInconsistent;
            }
            else m_lastError = DatabaseError;

            deleteFailedImportedStudy(studyUID); // si hi hagut un error borrem els fitxers importats de l'estudi de la cache local
        }
    }

    m_qprogressDialog->close();
}

void DICOMDIRImporter::importStudy(QString studyUID, QString seriesUID, QString sopInstanceUID)
{
    DicomMask mask;
    QList<Patient*> patientStudyListToImport;
    QString studyPath = LocalDatabaseManager::getCachePath() + studyUID + "/";;

    QDir().mkdir( studyPath );

    mask.setStudyUID( studyUID );

    m_readDicomdir.readStudies( patientStudyListToImport , mask );

    if (!patientStudyListToImport.isEmpty())//comprovem que s'hagin trobat estudis per importar
    {
        QList<Series*> seriesListToImport;

        m_qprogressDialog->setLabelText(tr("Importing study of ") + patientStudyListToImport.at(0)->getFullName());

        m_readDicomdir.readSeries( studyUID , seriesUID , seriesListToImport );

        if ( seriesListToImport.isEmpty() )
        {
            ERROR_LOG ( "No s'han trobat series per l'estudi" );
            m_lastError = DicomdirInconsistent;
            return;
        }

        foreach(Series *seriesToImport, seriesListToImport)
        {
            importSeries(studyUID, seriesToImport->getInstanceUID(), sopInstanceUID);
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
    QList<Image*> imageListToImport;
    QString seriesPath = LocalDatabaseManager::getCachePath() + studyUID + "/" + seriesUID;

    QDir().mkdir( seriesPath );

    m_readDicomdir.readImages( seriesUID , sopInstanceUID , imageListToImport );

    if ( imageListToImport.isEmpty() )
    {
        ERROR_LOG ( "No s'han trobat imatges per la serie" );
        m_lastError = DicomdirInconsistent;
        return;
    }

    foreach(Image *imageToImport, imageListToImport)
    {
        importImage(imageToImport, seriesPath);
        if (getLastError() != Ok) break;
    }
}

void DICOMDIRImporter::importImage(Image *image, QString pathToImportImage)
{
    QString cacheImagePath, dicomdirImagePath = getDicomdirImagePath(image);

    if (dicomdirImagePath.length() == 0)
    {
        m_lastError = DicomdirInconsistent;
        return;
    }

    cacheImagePath = pathToImportImage + "/" + image->getSOPInstanceUID();

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

        m_qprogressDialog->setValue(m_qprogressDialog->value() + 1);

        return true;
    }
    else return false;
}

QString DICOMDIRImporter::getDicomdirImagePath(Image *image)
{
    if (QFile::exists(image->getPath()))//comprovem si la imatge a importar existeix
    {
        return image->getPath();
    }
    else if (QFile::exists(image->getPath().toLower()))
    {
        /* Linux per defecte en les unitats vfat, mostra els noms de fitxer que són shortname ( 8 o menys caràcters ) en minúscules
           com que en el fitxer de dicomdir les rutes del fitxer es guarden en majúscules, si fem un exist del nom del fitxer sobre 
           unitats vfat falla, per això el que fem es convertir el nom del fitxer a minúscules
         */
        return image->getPath().toLower();
    }
    else
    {
        ERROR_LOG("Dicomdir inconsistent: La imatge [" + image->getPath() + "] no existeix" );
        return "";
    }

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

void DICOMDIRImporter::deleteFailedImportedStudy(QString studyInstanceUID)
{
    DeleteDirectory delDirectory;
    LocalDatabaseManager localDatabaseManager;

    INFO_LOG("S'esborrarà de la caché les imatges importades de l'estudi " + studyInstanceUID + " ja que la seva importació ha fallat");
    delDirectory.deleteDirectory(localDatabaseManager.getStudyPath(studyInstanceUID), true);
}

DICOMDIRImporter::DICOMDIRImporterError DICOMDIRImporter::getLastError()
{
    return m_lastError;
}

}
