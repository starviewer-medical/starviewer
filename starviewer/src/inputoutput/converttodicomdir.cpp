/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "converttodicomdir.h"

#include <QString>
#include <QProgressDialog>
#include <QDir>
#include <QChar>
#include <QFile>
#include <QTextStream>
#include <QList>

#include "logging.h"
#include "status.h"
#include "cachestudydal.h"
#include "cacheseriesdal.h"
#include "cacheimagedal.h"
#include "dicommask.h"
#include "dicomstudy.h"
#include "dicomseries.h"
#include "dicomimage.h"
#include "convertdicomtolittleendian.h"
#include "starviewersettings.h"
#include "deletedirectory.h"
#include "starviewerapplication.h"
#include "localdatabasemanager.h"
#include "patient.h"
#include "study.h"
#include "image.h"

namespace udg {

ConvertToDicomdir::ConvertToDicomdir(QObject *parent) : QObject(parent)
{
    QDir dicomDir;

    m_study = 0;
    m_series = 0;
    m_image = 0;
    m_patient = 0;
}

void ConvertToDicomdir::addStudy( QString studyUID )
{
    /*Els estudis s'han d'agrupar per pacient, el que fem és afegir-los a llista d'estudis per convertir a
     dicomdir ja ordenats per pacient*/
    StudyToConvert studyToConvert;
    int index = 0;
    bool stop = false;

    LocalDatabaseManager localDatabaseManager;
    DicomMask studyMask;
    studyMask.setStudyUID(studyUID);
    QList<Patient*> patientList = localDatabaseManager.queryPatientStudy(studyMask);
    if(localDatabaseManager.getLastError() != LocalDatabaseManager::Ok)
    {
        ERROR_LOG(QString("Error al afegir un study per generar un DICOMDIR; Error: %1; StudyUID: %2")
                  .arg(localDatabaseManager.getLastError())
                  .arg(studyUID));
        return;
    }

    // \TODO Això s'ha de fer perquè queryPatientStudy retorna llista de Patients
    // Nosaltres, en realitat només en volem un.
    Patient *patient = patientList.first();

    studyToConvert.studyUID = studyUID;
    studyToConvert.patientId = patient->getID();

    delete patient;

    while ( index < m_studiesToConvert.count() && !stop )  //busquem la posició on s'ha d'inserir l'estudi a llista d'estudis per convertir a dicomdir, ordenant per id de pacient
    {
        if ( studyToConvert.patientId < m_studiesToConvert.at( index ).patientId ) //comparem amb els altres estudis de la llista, fins trobar el seu llloc corresponentm
        {
            stop = true;
        }
        else index++;
    }

    if ( stop ) //una vegada hem trobat la posició on ha d'anar l'inserim
    {
        m_studiesToConvert.insert( index , studyToConvert );
    }
    else m_studiesToConvert.push_back( studyToConvert );//en aquest cas val al final
}

Status ConvertToDicomdir::convert( QString dicomdirPath, CreateDicomdir::recordDeviceDicomDir selectedDevice )
{
    /* Primer copiem els estudis al directori desti, i posteriorment convertim el directori en un dicomdir*/
    Status state;
    int imageNumberTotal = 0;

    m_dicomDirPath = dicomdirPath;

    DicomMask studyMask;
    QList<Study*> studyList;

    LocalDatabaseManager localDatabaseManager;

    foreach(StudyToConvert studyToConvert, m_studiesToConvert)
    {
        studyMask.setStudyUID(studyToConvert.studyUID);
        Patient *patient = localDatabaseManager.retrieve(studyMask);
        if(localDatabaseManager.getLastError() != LocalDatabaseManager::Ok)
        {
            QString error = QString("Error al fer un retrieve study per generar un DICOMDIR; Error: %1; StudyUID: %2")
                    .arg(localDatabaseManager.getLastError())
                    .arg(studyToConvert.studyUID);
            state.setStatus(error, false, -1);
            break;
        }
        else
        {
            state.setStatus("", true, -1);
        }

        // \TODO Això de patient->getStudies().first s'ha de fer perquè queryPatientStudy retorna llista de Patients
        // Nosaltres, en realitat només en volem un sol study.
        Study *study = patient->getStudies().first();
        studyList.append(study);
        foreach(Series *series, study->getSeries())
        {
            imageNumberTotal += series->getNumberOfImages();
        }
    }

    if ( !state.good() )
    {
        deleteStudies();
        return state;
    }

    //sumem una imatge més per evitar que arribi el 100 % la progress bar, i així s'esperi a que es crei el dicomdir, que es fa quan s'invoca createDicomdir.Create()
    m_progress = new QProgressDialog( tr( "Creating Dicomdir..." ) , "" , 0 , imageNumberTotal + 1 );
    m_progress->setMinimumDuration( 0 );
    m_progress->setCancelButton( 0 );

    //copiem les imatges dels estudis seleccionats al directori desti
    state = copyStudiesToDicomdirPath(studyList);

    if ( !state.good() )
    {
        m_progress->close();
        deleteStudies();
        return state;
    }

    //una vegada copiada les imatges les creem
    state = createDicomdir( dicomdirPath , selectedDevice );

    m_progress->close();
    if ( !state.good() && state.code() != 4001 )// l'error 4001 és que les imatges no compleixen l'estàndard al 100, però el dicomdir es pot utilitzar
    {
        deleteStudies();
    }

    return state;
}

Status ConvertToDicomdir::createDicomdir( QString dicomdirPath, CreateDicomdir::recordDeviceDicomDir selectedDevice )
{
    CreateDicomdir createDicomdir;
    Status state, stateNotDicomConformance;

    createDicomdir.setDevice( selectedDevice );
    state = createDicomdir.create( dicomdirPath );//invoquem el mètode per convertir el directori destí Dicomdir on ja s'han copiat les imatges en un dicomdir
    if ( !state.good() )//ha fallat crear el dicomdir, ara intentem crear-lo en mode no estricte
    {
        createDicomdir.setStrictMode( false );
        state = createDicomdir.create( dicomdirPath );

        if ( state.good() )
        {
            return stateNotDicomConformance.setStatus("Alguna de les imatges no complia l'estàndard DICOM" , false , 4001 );
        }
    }

    return state;
}

Status ConvertToDicomdir::copyStudiesToDicomdirPath(QList<Study*> studyList)
{
    StudyToConvert studyToConvert;
    QString m_OldPatientId;
    QString patientNameDir;
    QDir patientDir;
    Status state;
    QChar fillChar = '0';
    Study *study;

    m_patient = 0;

    //agrupem estudis1 per pacient, com que tenim la llista ordenada per patientId
    while ( !m_studiesToConvert.isEmpty() )
    {
        studyToConvert = m_studiesToConvert.takeFirst();

        // \TODO: Es fa xapussa per anar ràpid. En realitat s'hauria de refer el mètode perquè funcionés amb una llista d'Study i no
        // mantenir dues llistes, una d'uids i l'altra d'study's. Ara, com que creem studyList a partir de m_studiesToConvert, ens aprofitem
        // de que hi tenim el mateix ordre.
        study = studyList.takeFirst();

        if(study->getInstanceUID() != studyToConvert.studyUID)
        {
            state.setStatus("La xapussa del copyStudiesToDicomdirPath no funciona, hi ha ordre diferent entre la llista studyList i m_studiesToConvert", false, -1);
            break;
        }

        //si el pacient es diferent creem un nou directori PAtient
        if ( m_OldPatientId != studyToConvert.patientId )
        {
            patientNameDir = QString( "/PAT%1" ).arg( m_patient , 5 , 10 , fillChar );
            m_dicomdirPatientPath = m_dicomDirPath + "/DICOM/" + patientNameDir;
            patientDir.mkpath( m_dicomdirPatientPath );
            m_patient++;
            m_study = 0;
            m_patientDirectories.push_back( m_dicomdirPatientPath );//creem una llista amb els directoris creats, per si es produeix algun error esborrar-los
        }

        state = copyStudyToDicomdirPath(study);

        delete study;
        if ( !state.good() ) break;
        m_OldPatientId = studyToConvert.patientId;
    }

    return state;
}


Status ConvertToDicomdir::copyStudyToDicomdirPath(Study *study)
{
    /*Creem el directori de l'estudi on es mourà un estudi seleccionat per convertir a dicomdir*/
    QDir studyDir;
    QChar fillChar = '0';
    QString studyName = QString( "/STU%1" ).arg( m_study , 5 , 10 , fillChar );
    QList<DICOMSeries> seriesList;
    DICOMSeries series;
    Status state;

    m_study++;
    m_series = 0;

    //Creem el directori on es guardar l'estudi en format DicomDir
    m_dicomDirStudyPath = m_dicomdirPatientPath + studyName;
    studyDir.mkdir( m_dicomDirStudyPath );

    foreach(Series *series, study->getSeries() ) //per cada sèrie de l'estudi, creem el directori de la sèrie
    {
        state = copySeriesToDicomdirPath( series );

        if ( !state.good() ) break;
    }

    return state;
}

Status ConvertToDicomdir::copySeriesToDicomdirPath(Series *series)
{
    QDir seriesDir;
    QChar fillChar = '0';
    //creem el nom del directori de la sèrie, el format és SERXXXXX, on XXXXX és el numero de sèrie dins l'estudi
    QString seriesName = QString( "/SER%1" ).arg( m_series , 5 , 10 , fillChar );
    DICOMImage image;
    Status state;

    m_series++;
    m_image = 0;
    //Creem el directori on es guardarà la sèrie en format DicomDir
    m_dicomDirSeriesPath = m_dicomDirStudyPath + seriesName;
    seriesDir.mkdir( m_dicomDirSeriesPath );

    foreach(Image *imageToCopy, series->getImages())
    {
        state = copyImageToDicomdirPath( imageToCopy );

        if ( !state.good() ) break;
    }

    return state;
}

Status ConvertToDicomdir::copyImageToDicomdirPath(Image *image)
{
    QChar fillChar = '0';
    //creem el nom del fitxer de l'imatge, el format és IMGXXXXX, on XXXXX és el numero d'imatge dins la sèrie
    QString  imageName = QString( "/IMG%1" ).arg( m_image , 5 , 10 , fillChar );
    QString imageInputPath, imageOutputPath;
    ConvertDicomToLittleEndian convertDicom;
    StarviewerSettings settings;
    DICOMSeries serie;
    Status state;

    m_image++;

    //Creem el path de la imatge
    imageInputPath = image->getPath();

    imageOutputPath = m_dicomDirSeriesPath + imageName;

    //convertim la imatge a littleEndian, demanat per la normativa DICOM i la guardem al directori desti
    state = convertDicom.convert(imageInputPath, imageOutputPath );

     m_progress->setValue( m_progress->value() + 1 ); // la barra de progrés avança
     m_progress->repaint();

    return state;
}

void ConvertToDicomdir::deleteStudies()
{
    DeleteDirectory deleteDirectory;

    while ( !m_patientDirectories.isEmpty() )
    {
        deleteDirectory.deleteDirectory( m_patientDirectories.takeFirst() , true );
    }
}

void ConvertToDicomdir::createReadmeTxt()
{
    QString readmeFilePath = m_dicomDirPath + "/README.TXT";
    QFile file( readmeFilePath );
    StarviewerSettings settings;

    if ( !file.open( QIODevice::WriteOnly | QIODevice::Text ) ) return;

    QTextStream out( &file );

    if ( settings.getInstitutionName().length() > 0)
    {
        out << "Dicomdir Generated by : " << settings.getInstitutionName() << "\n";

        if ( settings.getInstitutionAddress().length() > 0) out << settings.getInstitutionAddress() << "\n";

        if ( settings.getInstitutionTown().length() > 0 ) out << settings.getInstitutionTown() << " " << settings.getInstitutionZipCode() << "\n";

        if ( settings.getInstitutionCountry().length() > 0 ) out << settings.getInstitutionCountry() << "\n";

        if ( settings.getInstitutionPhoneNumber().length() > 0 ) out << "Phone number "  << settings.getInstitutionPhoneNumber() << "\n";

        if ( settings.getInstitutionEmail().length() > 0 ) out << "E-mail contact : " << settings.getInstitutionEmail() << "\n";

        out << "\n" << "\n";
    }

    out << "The dicomdir has been generated with Starviewer Version " << StarviewerVersionString << "\n";
    out << "E-mail contact : " << OrganizationEmailString << "\n";

    file.close();
}

ConvertToDicomdir::~ConvertToDicomdir()
{
}

}
