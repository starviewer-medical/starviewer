/*************************************************************************************
  Copyright (C) 2014 Laboratori de Gràfics i Imatge, Universitat de Girona &
  Institut de Diagnòstic per la Imatge.
  Girona 2014. All rights reserved.
  http://starviewer.udg.edu

  This file is part of the Starviewer (Medical Imaging Software) open source project.
  It is subject to the license terms in the LICENSE file found in the top-level
  directory of this distribution and at http://starviewer.udg.edu/license. No part of
  the Starviewer (Medical Imaging Software) open source project, including this file,
  may be copied, modified, propagated, or distributed except according to the
  terms contained in the LICENSE file.
 *************************************************************************************/

#ifndef UDGRISREQUESTMANAGER_H
#define UDGRISREQUESTMANAGER_H

#include <QObject>
#include "singleton.h"

#include "listenrisrequests.h"

#include <unordered_set>

#include <QQueue>

namespace udg {

class DicomMask;
class QPopUpExternalStudyRequestsScreen;
class StudyOperationResult;

/**
 * @brief The ExternalStudyRequestManager class is meant to process external study requests, e.g. from command line, RIS, SAP, etc.
 */
class ExternalStudyRequestManager : public QObject, public Singleton<ExternalStudyRequestManager>
{
    Q_OBJECT

public:
    /// Starts listenin on the RIS port if enabled in settings.
    void listen();

public slots:
    /// Processa una petició del RIS per descarregar l'estudi que compleixi la màscara de cerca
    void processRISRequest(DicomMask mask);

signals:
    /// Signal que s'emet per indicar que ja es pot començar a escoltar peticions a través de la classe ListenRISRequests que s'executa en un altre Thread
    void listenRISRequests();

    /// Signal que s'emet per indicar que es pari d'escoltar peticions a través de la classe ListenRISRequests que s'executa en un altre Thread
    void stopListenRISRequests();

    /// Signal que s'emet per indicar que s'ha de visualitzar l'estudi
    void viewStudyRetrievedFromRISRequest(QString studyInstanceUID);

    /// Signal que s'emet per indicar que s'ha de fer un load de l'estudi
    void loadStudyRetrievedFromRISRequest(QString studyInstanceUID);

private:
    explicit ExternalStudyRequestManager(QObject *parent = nullptr);
    ~ExternalStudyRequestManager() override;
    friend Singleton<ExternalStudyRequestManager>;

private slots:
    /// Posa els estudis d'un QueryPacsJob a la cua d'estudis trobats per processa
    void addFoundStudiesToRetrieveQueue(StudyOperationResult *result);
    /// Called when the given query has finished with error.
    void onQueryError(StudyOperationResult *result);
    /// Called when the given query is cancelled.
    void onQueryCancelled(StudyOperationResult *result);

    /// Called when a study retrieve finished with success.
    void onStudyRetrieveSucceeded(StudyOperationResult *result);
    /// Called when a study retrieve finished with partial success.
    void onStudyRetrieveFinishedWithPartialSuccess(StudyOperationResult *result);
    /// Called when a study retrieve finished with error.
    void onStudyRetrieveFailed(StudyOperationResult *result);
    /// Called when a study retrieve is cancelled.
    void onStudyRetrieveCancelled(StudyOperationResult *result);

    /// Mostrar un missatge indicant que s'ha produït un error escoltant peticions del RIS
    void showListenRISRequestsError(ListenRISRequests::ListenRISRequestsError error);

private:
    enum DICOMSourcesFromRetrieveStudy { PACS = 0, Database = 1 };

    /// Inicialitza les variables globals per escoltar i executar peticions del RIS.
    /// No inicialitzem al construtor perquè si no ens indiquen que hem d'escoltar no cal, inicialitzar les variables i ocupar memòria
    void initializeListener();

    /// Crea les connexions entre Signals i Slots
    void createConnections();

    /// Cerca en els PACS marcats per defecte la màscara que ens ha indicat el RIS
    void queryPACSRISStudyRequest(DicomMask mask);

    /// Adds the given StudyOperationResult representing a query to the list of pending queries and creates the needed connections to it.
    void addPendingQuery(StudyOperationResult *result);

    /// S'activa quan ha finalitzat la consulta de la cerca del l'estudi sol·licitat pel RIS, comprova si s'han trobat estudis i si és així es descarrega
    void queryRequestRISFinished();

    /// Descarrega els estudis trobats a partir d'una queryPACSJob
    void retrieveFoundStudiesInQueue();

    /// Comprova si l'estudi existeix a la base dades, si existeix pregunta a l'usuari si vol tornar a descarregar l'estudi si diu que si obté l'estudi
    /// de la base de dades, sinó descarrega l'estudi del PACS
    void retrieveStudyFoundInQueryPACS(Study *study);

    /// Sol·licita descarregar l'estudi passat utilitzant el PACSManager
    void retrieveStudyFromPACS(Study *study);

    /// Obté l'estudi de la base de dades
    void retrieveStudyFromDatabase(Study *study);

    /// Una vegada s'ha descarregat fa les accions pertinents amb aquell estudi. Emet signal per visualitzar/load o no fa res
    void doActionsAfterRetrieve(const QString &studyInstanceUid);

    /// Indica de quina font hem d'obtenir l'estudi.
    /// Comprova si l'estudi existeix a la base de dades, si existeix es pregunta a l'usuari si s'ha de tornar a descarregar-lo del PACS, i es guarda la resposta
    /// perquè en el cas que es trobin més estudis d'una petició del RIS que existeixin a la base de dades, per tots ja fer automàticament fer el que ha indicat l'usuari.
    /// Si l'estudi no existeix a la base de dades retorna que es descarregui del PACS
    /// retorna la resposta que ha donat l'usuari la primera vegada. Així no hem de molestar l'usuari cada vegada que trobem que un estudi existeix a la base de dades
    DICOMSourcesFromRetrieveStudy getDICOMSouceFromRetrieveStudy(Study *study);

    /// Pregunta a l'usuari si vol tornar a descarregar l'estudi si aquest existeix a la base de dades
    bool askToUserIfRetrieveFromPACSStudyWhenExistsInDatabase(const QString &fullPatientName) const;

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

    QPopUpExternalStudyRequestsScreen *m_qpopUpRISRequestsScreen;

    /// QThread que s'encarrega d'executar la classe escolta si arriben peticions del RIS
    QThread *m_listenRISRequestsQThread;

    /// Pot ser que diversos PACS continguin el mateix estudi amb un mateix accession number, per evitar descarregar-lo més d'una vegada ens guardem en una
    /// llista quins són els estudis descarregats per una mateixa petició.
    QStringList m_studiesInstancesUIDRequestedToRetrieve;

    /// Contains StudyOperationResults that represent queries in progress.
    std::unordered_set<StudyOperationResult*> m_pendingQueryResults;

    int m_numberOfStudiesAddedToRetrieveForCurrentRisRequest;

    /// Guarda a l'usuari si per una petició del RIS se li ha preguntat si els estudis que ja existeixen descarregats a la base de dades s'han de tornar a desgarregar
    bool m_hasBeenAskedToUserIfExistingStudiesInDatabaseHaveToBeenRetrievedAgain;
    /// Guarda la resposta de l'usuari a la pregunta de si s'han de tornar a descarregar els estudis ja existents a la BD
    bool m_studiesInDatabaseHaveToBeenRetrievedAgain;

    /// Segons que ha de tardar a amagar-se el PopUp QPopUpRisRequestScreen quan han acabat totes les descàrregues i temps que ha de tadar el QMessageBox a tancar-se automàticament
    /// Tots dos han de tenir el mateix temps perquè imaginem el cas que descarreguem un primer estudi del PACS llavor el segon el trobem a la BD, si el primer estudi s'ha acabat
    /// de descarregar el PopUp es pot tancar abans que l'usuari hagi pogut prèmer un del botons del QMessageBox preguntant si s'ha de tornar a descarregar l'estudi del PACS,
    /// fent que llavors no es pugui seguir la descarrega dels següents estudis a través de QPopUpRISRequestScree
    static const int secondsTimeOutToHidePopUpAndAutoCloseQMessageBox;

    QQueue<Study*> m_studiesToRetrieveQueue;

    /// Indica si per l'útlima petició del RIS s'ha emés signal per visualitzar un estudi
    bool m_signalViewStudyEmittedForLastRISRequest;
};

}

#endif
