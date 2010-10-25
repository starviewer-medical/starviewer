/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gr�fics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#ifndef UDGRISREQUESTMANAGER_H
#define UDGRISREQUESTMANAGER_H

#include <QQueue>
#include <QHash>

#include "listenrisrequests.h"
#include "pacsdevice.h"

class QThread;

namespace udg {

class DicomMask;
class PacsManager;
class Patient;
class Study;
class QPopUpRisRequestsScreen;
class QueryPacsJob;
class PACSJob;

/** Classe manager que ens permet rebre peticions del RIS i processar-les
*/
/*TODO Aquesta classe hauria de ser l'encarregada també de descarregar l'estudi que ens ha sol·licitat el ris, ara mateix 
      fa un signal i una classe externa el descarrega. Això no es podrà fer fins que s'hagi fet refactoring de la part de descarregar
      estudis. Tenir en compte que ara actualment es fa un delete de la instància PACSManager actual, quan se li passi la PACSManager des de fora
      aquest delete s'haurà de treure*/
class RISRequestManager: public QObject{
Q_OBJECT
public:

    ///Destructor de la classe
    ~RISRequestManager();

    ///Iniciem l'escolta de les peticions del RIS pel port especificat a la configuració
    void listen();

signals:

    ///Signal que indica que s'ha descarregar un estudi sol·licitat pel RIS
    void retrieveStudyFromRISRequest(QString pacsID, Study *study);

    ///Signal que s'emet per indicar que ja es pot començar a escoltar peticions a través de la classe ListenRISRequests
    void listenRISRequests();

private slots:

    ///Processa una petició del RIS per descarregar l'estudi que compleixi la màscara de cerca
    void processRISRequest(DicomMask mask);

    ///Slot que s'activa quan finalitza un job de consulta al PACS
    void queryPACSJobFinished(PACSJob *pacsJob);

    ///Slot que s'activa quan un job de consulta al PACS és cancel·lat
    void queryPACSJobCancelled(PACSJob *pacsJob);

    ///Mostrar un missatge indicant que s'ha produït un error escoltant peticions del RIS
    void showListenRISRequestsError(ListenRISRequests::ListenRISRequestsError error);

private:

    /**Inicialitza les variables globals per escoltar i executar peticions del RIS.
      *No inicialitzem al construtor perquè si no ens indiquen que hem d'escoltar no cal, inicialitzar les variables i ocupar memòria
      */
    void initialize();

    ///Crea les connexions entre Signals i Slots
    void createConnections();

    ///Cerca en els PACS marcats per defecte la màscara que ens ha indicat el RIS
    void queryPACSRISStudyRequest(DicomMask mask);

    ///Ens encua el QueryPACSJob al PACSManager i ens connecta amb els seus signals per poder processar els resultats. També afegeix el Job en una taula
    ///de hash on es guarden tots els QueryPACSJobs demanats per aquesta classe que estant pendents d'executar-se o s'estan executant
    void enqueueQueryPACSJobToPACSManagerAndConnectSignals(QueryPacsJob *queryPacsJob);

    ///S'activa quan ha finalitzat la consulta de la cerca del l'estudi sol·licitat pel RIS, comprova si s'han trobat estudis i si és així es descarrega
    void queryRequestRISFinished();

    ///Mostra un missatge indicant que s'ha produït un error al fer la consulta a un PACS
    void errorQueryingStudy(QueryPacsJob *queryPACSJob);

private:

    ///Boolea que indica si hem trobat algun estudi que compleixi els criteris de cerca ens ha demanat el RIS
    bool m_foundRISRequestStudy; 

    /*No podem executar diverses peticions de RIS a la vegada, per això creem aquesta cua, que ens permetrà en el cas que se'ns 
     *demani una petició, quan ja n'hi hagi un altre executant, encuar la sol·licitud i esperar a llançar-la que l'actual hagi finalitzat.
     El motiu de que no podem executar més d'una sol·licitud a la vegada, és degut a la naturalesa assíncrona del PacsManager,
      quan retorna els resultats no sabem a quina sol·licitud del RIS pertany, no hi ha cap ordre establert, dificultant les coses.
     Necessitem saber si per un determinada sol·licitud hem trobat un estudi que compleixi el criteri de cerca, controls 
     d'errors, etc.. si processim més d'una sol·licitud a la vegada, no sabríem de quina sol·licitud són els resultats o error, 
     dificultant el control de les sol·licituds*/
    QQueue<DicomMask> m_queueRISRequests;
    ListenRISRequests *m_listenRISRequests;

    QPopUpRisRequestsScreen *m_qpopUpRisRequestsScreen;

    PacsManager *m_pacsManager;

    ///QThread que s'encarrega d'executar la classe escolta si arriben peticions del RIS
    QThread *m_listenRISRequestsQThread;

    ///Hash que ens guarda tots els QueryPACSJob pendent d'executar o que s'estan executant llançats des d'aquesta classe
    QHash<int, QueryPacsJob*> m_queryPACSJobPendingExecuteOrExecuting;

};

};  //  end  namespace udg

#endif
