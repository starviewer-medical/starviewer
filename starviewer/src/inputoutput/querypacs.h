#ifndef QUERYPACS
#define QUERYPACS

#include <QList>
#include <QHash>
#include <assoc.h>
#include <dcdeftag.h>

#include "pacsdevice.h"
#include "pacsrequeststatus.h"
#include "dicommask.h"
/// This class helps to interactive with the pacs, allow us to find studies in the pacs setting a search mask. Very important for this class a connection
/// and a mask search must be setted befoer query Studies

class DcmDataset;

struct T_DIMSE_C_FindRQ;
struct T_DIMSE_C_FindRSP;

namespace udg {

class Patient;
class Study;
class Series;
class Image;
class DICOMTagReader;
class PACSConnection;

class QueryPacs {
public:
    /// Constructor de la classe
    QueryPacs(PacsDevice pacsDevice);

    /// Cerca els estudis que compleixin la màscara passada
    PACSRequestStatus::QueryRequestStatus query(DicomMask mask);

    /// Indiquem que la consulta actual s'ha de cancel·lar.
    /// La cancel·lació de la query no es fa immediatament quan s'invoca el mètode, aquest mètode actualitza un flag, que cada vegada
    /// que rebem un element DICOM que compleix la màscara es comprova, si el flag indica que s'ha demanat cancel·lar llavors es
    /// cancel·la la query
    void cancelQuery();

    QList<Patient*> getQueryResultsAsPatientStudyList();
    QList<Series*> getQueryResultsAsSeriesList();
    QList<Image*> getQueryResultsAsImageList();

    /// Retorna un Hashtable que indica per l'UID de l'estudi a quin PACS pertany l'estudi
    QHash<QString, QString> getHashTablePacsIDOfStudyInstanceUID();

private:
    /// Fa el query al pacs
    PACSRequestStatus::QueryRequestStatus query();

    /// Aquest és un mètode que és cridat en callback per les dcmtk, per cada objecte dicom que es trobi en el PACS que compleix la query dcmtk el crida.
    /// Aquest mètode ens insereix la llista d'estudis, sèries o imatges l'objecte dicom trobat en funció del nivell del que sigui l'objecte.
    static void foundMatchCallback(void *callbackData, T_DIMSE_C_FindRQ *request, int responseCount, T_DIMSE_C_FindRSP *rsp, DcmDataset *responseIdentifiers);

    /// Cancel·la la consulta actual
    void cancelQuery(T_DIMSE_C_FindRQ *request);

    /// Afegeix l'objecte a la llista d'estudis si no hi existeix
    void addPatientStudy(DICOMTagReader *dicomTagReader);
    /// Afegeix l'objecte dicom a la llista de sèries si no hi existeix
    void addSeries(DICOMTagReader *dicomTagReader);
    /// Afegeix l'objecte dicom a la llista d'imatges si no hi existeix
    void addImage(DICOMTagReader *dicomTagReader);

    ///Ens converteix un DICOMMask a un DcmDataset per poder fer la query al PACS
    DcmDataset* ConvertDICOMMaskToDcmDataset(DicomMask dicomMask);

    ///Ens afegeix el tag amb el seu valor al DcmDataset passat per paràmetre, sempre i quan tagValue no sigui null
    void AddTagToDcmDatsetAsString(DcmDataset *dcmDataset, DcmTagKey dcmTagKey, QString tagValue);

    ///Quan consultem al PACS li hem d'indicar a quin nivell es vol fer la cerca, en funció dels tags que demanem al PACS, per exemple si al PACS demanem tags de 
    ///sèrie i estudi li hem d'indicar que la cerca és a nivell de sèrie, si li demanem només tags d'estudi se li ha d'indicar que és a nivell d'estudi. Aquest mètode
    ///a través de la dicomMask ens indica a quin nivell es fa la cerca
    QString getQueryLevelFromDICOMMask(DicomMask dicomMask);

    /// Converteix la respota rebuda per partl del PACS a QueryRequestStatus i  en cas d'error processa la resposta i grava l'error al log
    PACSRequestStatus::QueryRequestStatus processResponseStatusFromFindUser(T_DIMSE_C_FindRSP *findResponse, DcmDataset *statusDetail);

private:
    T_ASC_PresentationContextID m_presId;
    DicomMask m_dicomMask;
    PacsDevice m_pacsDevice;
    PACSConnection *m_pacsConnection;

    QList<Patient*> m_patientStudyList;
    QList<Series*> m_seriesList;
    QList<Image*> m_imageList;
    // TODO m_hashPacsIDOfStudyInstanceUID ara mateix no té gaire sentit perquè per defecte se li posa la ID del PACS el que fem la
    // cerca, però podem tenir el cas que les consultes es facin a un PACS i que aquest ens indiqui que l'estudi es troba guardat en
    // un altre PACS, tenir en compte per aquest cas que passa si tenim dos PACS amb el mateix nom
    /// Fa un relació d'StudyInstanceUID amb el pacs al qual pertany
    QHash<QString, QString> m_hashPacsIDOfStudyInstanceUID;

    // Flag que indica si s'ha de cancel·lar la query actual
    bool m_cancelQuery;
    // Indica si hem demanat la cancel·lació de la consulta actual
    bool m_cancelRequestSent;
};
};
#endif
