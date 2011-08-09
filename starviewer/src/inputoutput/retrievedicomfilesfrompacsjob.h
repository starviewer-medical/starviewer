#ifndef RETRIEVEDICOMFILESFROMPACSJOB_H
#define RETRIEVEDICOMFILESFROMPACSJOB_H

#include <QObject>

#include "pacsjob.h"
#include "pacsrequeststatus.h"
#include "dicommask.h"

namespace udg {

class Patient;
class Study;
class RetrieveDICOMFilesFromPACS;
class PacsDevice;
class DICOMTagReader;

/**
    Job que s'encarrega de descarregar fitxers del PACS.
  */
class RetrieveDICOMFilesFromPACSJob : public PACSJob {
Q_OBJECT
public:
    /// Definim la prioritat del Job, aquesta prioritat ens definir l'ordre d'execuci en la cua
    enum RetrievePriorityJob { Low = 0, Medium = 2, High = 3 };

    /// Constructor/Destructor de la classe. Si seriesInstanceUID est buit descarregar tot l'estudi, si t valor noms aquella srie, i si tamb especifiquem el SOPInstanceUID
    /// noms descarregar la imatge amb el SOPInstanceUID de la srie especificada, de l'estudi especificat. L'Objecte Study que se li passa se'n fa copia, per evitar que si a
    /// fora destrueixen l'Study quan aquesta classe encara el necessiti peti l'aplicacio
    RetrieveDICOMFilesFromPACSJob(PacsDevice pacsDevice, RetrievePriorityJob, Study *studyToRetrieveDICOMFiles, const QString &seriesInstanceUID = "" , 
        const QString &sopInstanceUID = "");
    ~RetrieveDICOMFilesFromPACSJob();

    /// Retorna el tipus de PACSJob que s l'objecte
    PACSJob::PACSJobType getPACSJobType();

    /// Retorna l'estudi del qual s'han de descarregar els fitxers. Aquest objecte s'esborra quan es destrueixi el Job
    Study* getStudyToRetrieveDICOMFiles();

    /// Codi que executar el job
    void run();

    /// Retorna l'Status del Job
    PACSRequestStatus::RetrieveRequestStatus getStatus();

    /// Retorna l'Status descrit en un QString , aquest QString est pensat per ser mostrat en QMessageBox per informar a l'usuari de l'estat que ha retornat
    /// el job en el mateixa descripci s'indica de quin s l'estudi afectat
    QString getStatusDescription();

signals:
    /// Signal que s'emet quan s'ha descarregat un fitxer
    void DICOMFileRetrieved(PACSJob *pacsJob, int numberOfImagesRetrieved);

    /// Signal que s'emet quan s'ha descarregat una srie
    void DICOMSeriesRetrieved(PACSJob *pacsJob, int numberOfSeriesRetrieved);

    /// Signal que s'emet quan el DICOMTagReader est a punt per ser processats. Aquest signal s d's intern
    void DICOMTagReaderReadyForProcess(DICOMTagReader *dicomTagReader);

    /// Signal que indica que la descrrega de fitxers DICOM ha finalitzat. Aquest signal es fa quan la descarrega de fitxers DICOM ha acabat per encara
    /// queda processar els fillers per obtenir l'objecte Patient a guardar a la base de dades. Aquest signal s d'us intern
    void DICOMFilesRetrieveFinished();

    /// Abans de descarregar un estudi es comprova si hi ha espaci suficient, si no n'hi ha s'itentan esborrar estuis de la cach local per alliberar
    /// espai, amb aquest signal s'indica que l'estudi amb instanceUID s'esborrar de la cach
    /// TODO:Aquest signal no s'hauria de fer des d'aquesta classe sin des d'una CacheManager, per com de moment encara no est implementada
    /// temporalment emetem el signal des d'aqu
    void studyFromCacheWillBeDeleted(const QString &studyInstanceUID);

private slots:
    /// Slot que s'activa quan s'ha descarregat una imatge, respn al signal DICOMFileRetrieved de RetrieveDICOMFilesFromPACS
    void DICOMFileRetrieved(DICOMTagReader *dicomTagReader, int numberOfImagesRetrieved);

private:
    /// Indica la prioritat del job
    // Sobreescribim el mtode priority de la classe ThreadWeaver::Job
    int priority() const;

    /// Mtode que ens indica si hi ha espai disponible per descarregar estudis
    PACSRequestStatus::RetrieveRequestStatus thereIsAvailableSpaceOnHardDisk();

    /// Indica si l'estudi amb l'UID passat permetre ja existeix a la base de dades
    bool existStudyInLocalDatabase(QString studyInstanceUID);

    /// Esborra els fitxers descarregats de la cach si l'estudi no existeix a la base de dades
    /// Aquest mtode est pensat en casos que la descrrega falla i volem esborrar els fitxers descarregats, noms s'esborran si l'estudi no est inserit
    /// a la bd, si l'estudi est inserit no l'esborrem, perqu part dels fitxers descarregats ja podien estar inserit a la base de dades per una anterior
    /// descrrega
    void deleteRetrievedDICOMFilesIfStudyNotExistInDatabase();

    /// Demana que es cancelli la descarrega del job
    void requestCancelJob();

    /// Copia la informacio basica de l'estudi en un nou objecte Study
    Study* copyBasicStudyInformation(Study *studyToCopy);

    /// Els fillers ens retornen l'estudi i ens el insereixen a la base de dades sense la informacio del DICOMSource, aquest metode ens actualitza l'estudi
    /// a nivell de base de dades afegint-t'hi la informacio de quin DICOMSource prove
    void updateRetrievedStudyToAddDICOMSource(QString retrievedStudyinstanceUID, PacsDevice pacsRetrieveStudy);

    /// Ens retorna l'estudi descarregat en aquest job de la base de dades. Si no el troba retorna un Study null
    Patient* getRetrievedPatientStudyFromDatabase(QString retrievedStudyInstanceUID);

private:
    RetrieveDICOMFilesFromPACS *m_retrieveDICOMFilesFromPACS;
    Study *m_studyToRetrieveDICOMFiles;
    QString m_seriesInstanceUIDToRetrieve;
    QString m_SOPInstanceUIDToRetrieve;
    PACSRequestStatus::RetrieveRequestStatus m_retrieveRequestStatus;
    RetrievePriorityJob m_retrievePriorityJob;
    QList<QString> m_retrievedSeriesInstanceUID;
    QString m_lastImageSeriesInstanceUID;
    int m_numberOfSeriesRetrieved;
};

};

#endif
