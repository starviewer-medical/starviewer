/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGRETRIEVEDICOMFILESFROMPACS_H
#define UDGRETRIEVEDICOMFILESFROMPACS_H

#include <QObject>

#include "pacsjob.h"
#include "pacsrequeststatus.h"
#include "dicommask.h"

namespace udg {

/**
    Job que s'encarrega de descarregar fitxers del PACS.

   @author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class Study;
class RetrieveDICOMFilesFromPACS;
class PacsDevice;
class DICOMTagReader;

class RetrieveDICOMFilesFromPACSJob: public PACSJob
{
Q_OBJECT
public:
    ///Definim la prioritat del Job, aquesta prioritat ens definirà l'ordre d'execució en la cua
    enum RetrievePriorityJob { Low = 0, Medium = 2, High = 3 };
 
    ///Constructor/Destructor de la classe
    RetrieveDICOMFilesFromPACSJob(PacsDevice pacsDevice, Study *studyToRetrieveDICOMFiles, DicomMask dicomMaskToRetrieve, RetrievePriorityJob);
    ~RetrieveDICOMFilesFromPACSJob();

    ///Retorna el tipus de PACSJob que és l'objecte
    PACSJob::PACSJobType getPACSJobType();

    ///Retorna l'estudi del qual s'han de descarregar els fitxers
    Study* getStudyToRetrieveDICOMFiles();

    ///Codi que executarà el job
    void run();

    ///Retorna l'Status del Job
    PACSRequestStatus::RetrieveRequestStatus getStatus();

    /**Retorna l'Status descrit en un QString , aquest QString està pensat per ser mostrat en QMessageBox per informar a l'usuari de l'estat que ha retornat
        el job en el mateixa descripció s'indica de quin és l'estudi afectat*/
    QString getStatusDescription();

signals:

    ///Signal que s'emet quan s'ha descarregat un fitxer
    void DICOMFileRetrieved(PACSJob *pacsJob, int numberOfImagesRetrieved);

    ///Signal que s'emet quan s'ha descarregat una sèrie
    void DICOMSeriesRetrieved(PACSJob *pacsJob, int numberOfSeriesRetrieved);

    ///Signal que s'emet quan el DICOMTagReader està a punt per ser processats. Aquest signal és d'ús intern
    void DICOMTagReaderReadyForProcess(DICOMTagReader *dicomTagReader);

    /**Signal que indica que la descàrrega de fitxers DICOM ha finalitzat. Aquest signal es fa quan la descarrega de fitxers DICOM ha acabat però encara
       queda processar els fillers per obtenir l'objecte Patient a guardar a la base de dades. Aquest signal és d'us intern*/
    void DICOMFilesRetrieveFinished();

    /**Abans de descarregar un estudi es comprova si hi ha espaci suficient, si no n'hi ha s'itentan esborrar estuis de la caché local per alliberar 
       espai, amb aquest signal s'indica que l'estudi amb instanceUID s'esborrarà de la caché*/
    /*TODO:Aquest signal no s'hauria de fer des d'aquesta classe sinó des d'una CacheManager, però com de moment encara no està implementada 
      temporalment emetem el signal des d'aquí*/
    void studyFromCacheWillBeDeleted(const QString &studyInstanceUID);

private slots:

    ///Slot que s'activa quan s'ha descarregat una imatge, respón al signal DICOMFileRetrieved de RetrieveDICOMFilesFromPACS
    void DICOMFileRetrieved(DICOMTagReader *dicomTagReader, int numberOfImagesRetrieved);

private:

    ///Indica la prioritat del job
    //Sobreescribim el mètode priority de la classe ThreadWeaver::Job
    int priority() const;

    ///Mètode que ens indica si hi ha espai disponible per descarregar estudis
    PACSRequestStatus::RetrieveRequestStatus thereIsAvailableSpaceOnHardDisk();

    ///Indica si l'estudi amb l'UID passat peràmetre ja existeix a la base de dades
    bool existStudyInLocalDatabase(QString studyInstanceUID);

    ///Esborra els fitxers descarregats de la caché si l'estudi no existeix a la base de dades
    /*Aquest mètode està pensat en casos que la descàrrega falla i volem esborrar els fitxers descarregats, només s'esborran si l'estudi no està inserit
      a la bd, si l'estudi està inserit no l'esborrem, perquè part dels fitxers descarregats ja podien estar inserit a la base de dades per una anterior
      descàrrega*/
    void deleteRetrievedDICOMFilesIfStudyNotExistInDatabase();

    ///Demana que es cancel·li la descarrega del job
    void requestCancelJob();

private:

    bool m_abortRequested;
    RetrieveDICOMFilesFromPACS *m_retrieveDICOMFilesFromPACS;
    Study *m_studyToRetrieveDICOMFiles;
    DicomMask m_dicomMaskToRetrieve;
    PACSRequestStatus::RetrieveRequestStatus m_retrieveRequestStatus;
    RetrievePriorityJob m_retrievePriorityJob;
    QList<QString> m_retrievedSeriesInstanceUID;
    QString m_lastImageSeriesInstanceUID;
    int m_numberOfSeriesRetrieved;
};

};

#endif
