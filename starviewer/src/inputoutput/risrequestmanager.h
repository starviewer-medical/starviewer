
#ifndef UDGRISREQUESTMANAGER_H
#define UDGRISREQUESTMANAGER_H

#include <QQueue>
#include <QHash>
#include <QStringList>

#include "listenrisrequests.h"
#include "pacsdevice.h"

class QThread;

namespace udg {

class DicomMask;
class PacsManager;
class Patient;
class Study;
class QPopUpRISRequestsScreen;
class QueryPacsJob;
class PACSJob;
class RetrieveDICOMFilesFromPACSJob;

/**
    Classe manager que ens permet rebre peticions del RIS i processar-les
  */
class RISRequestManager : public QObject {
Q_OBJECT
public:
    RISRequestManager(PacsManager *pacsManager);
    /// Destructor de la classe
    ~RISRequestManager();

    /// Iniciem l'escolta de les peticions del RIS pel port especificat a la configuració
    void listen();

signals:
    /// Signal que s'emet per indicar que ja es pot començar a escoltar peticions a través de la classe ListenRISRequests que s'executa en un altre Thread
    void listenRISRequests();

    /// Signal que s'emet per indicar que es pari d'escoltar peticions a través de la classe ListenRISRequests que s'executa en un altre Thread
    void stopListenRISRequests();

    /// Signal que s'emet per indicar que s'ha de visualitzar l'estudi
    void viewStudyRetrievedFromRISRequest(QString studyInstanceUID);

    /// Signal que s'emet per indicar que s'ha de fer un load de l'estudi
    void loadStudyRetrievedFromRISRequest(QString studyInstanceUID);

private slots:
    /// Processa una petició del RIS per descarregar l'estudi que compleixi la màscara de cerca
    void processRISRequest(DicomMask mask);

    /// Slot que s'activa quan finalitza un job de consulta al PACS
    void queryPACSJobFinished(PACSJob *pacsJob);

    /// Slot que s'activa quan un job de consulta al PACS és cancel·lat
    void queryPACSJobCancelled(PACSJob *pacsJob);

    /// Slot que s'activa quan s'ha cancel·lat la descàrre d'una petició del RIS
    void retrieveDICOMFilesFromPACSJobCancelled(PACSJob *pacsJob);

    /// Slot que s'activa quan un job de descarrega d'una petició del RIS ha finalitzat
    void retrieveDICOMFilesFromPACSJobFinished(PACSJob *pacsJob);

    /// Mostrar un missatge indicant que s'ha produït un error escoltant peticions del RIS
    void showListenRISRequestsError(ListenRISRequests::ListenRISRequestsError error);

private:
    /// Inicialitza les variables globals per escoltar i executar peticions del RIS.
    /// No inicialitzem al construtor perquè si no ens indiquen que hem d'escoltar no cal, inicialitzar les variables i ocupar memòria
    void initialize();

    /// Crea les connexions entre Signals i Slots
    void createConnections();

    /// Cerca en els PACS marcats per defecte la màscara que ens ha indicat el RIS
    void queryPACSRISStudyRequest(DicomMask mask);

    /// Ens encua el QueryPACSJob al PACSManager i ens connecta amb els seus signals per poder processar els resultats. També afegeix el Job en una taula
    /// de hash on es guarden tots els QueryPACSJobs demanats per aquesta classe que estant pendents d'executar-se o s'estan executant
    void enqueueQueryPACSJobToPACSManagerAndConnectSignals(QueryPacsJob *queryPacsJob);

    /// S'activa quan ha finalitzat la consulta de la cerca del l'estudi sol·licitat pel RIS, comprova si s'han trobat estudis i si és així es descarrega
    void queryRequestRISFinished();

    /// Mostra un missatge indicant que s'ha produït un error al fer la consulta a un PACS
    void errorQueryingStudy(QueryPacsJob *queryPACSJob);

    /// Descarrega els estudis trobats a partir d'una queryPACSJob
    void retrieveFoundStudiesFromPACS(QueryPacsJob *queryPACSJob);

    /// Sol·licita descarregar l'estudi passat utilitzant el PACSManager
    RetrieveDICOMFilesFromPACSJob* retrieveStudy(QString pacsIDToRetrieve, Study *study);

private:
    /// No podem executar diverses peticions de RIS a la vegada, per això creem aquesta cua, que ens permetrà en el cas que se'ns
    /// demani una petició, quan ja n'hi hagi un altre executant, encuar la sol·licitud i esperar a llançar-la que l'actual hagi finalitzat.
    /// El motiu de que no podem executar més d'una sol·licitud a la vegada, és degut a la naturalesa assíncrona del PacsManager,
    /// quan retorna els resultats no sabem a quina sol·licitud del RIS pertany, no hi ha cap ordre establert, dificultant les coses.
    /// Necessitem saber si per un determinada sol·licitud hem trobat un estudi que compleixi el criteri de cerca, controls
    /// d'errors, etc.. si processim més d'una sol·licitud a la vegada, no sabríem de quina sol·licitud són els resultats o error,
    /// dificultant el control de les sol·licituds
    QQueue<DicomMask> m_queueRISRequests;
    ListenRISRequests *m_listenRISRequests;

    QPopUpRISRequestsScreen *m_qpopUpRISRequestsScreen;

    PacsManager *m_pacsManager;

    /// QThread que s'encarrega d'executar la classe escolta si arriben peticions del RIS
    QThread *m_listenRISRequestsQThread;

    /// Pot ser que diversos PACS continguin el mateix estudi amb un mateix accession number, per evitar descarregar-lo més d'una vegada ens guardem en una
    /// llista quins són els estudis descarregats.
    QStringList m_studiesInstancesUIDRequestedToRetrieve;
    // Llista de PACSJob pels quals una vegada l'estudi estigui descarregat s'ha de fer un view/load
    QList<int> m_pacsJobIDToViewWhenFinished;
    QList<int> m_pacsJobIDToLoadWhenFinished;

    /// Hash que ens guarda tots els QueryPACSJob pendent d'executar o que s'estan executant llançats des d'aquesta classe
    QHash<int, QueryPacsJob*> m_queryPACSJobPendingExecuteOrExecuting;

};

};  // end namespace udg

#endif
