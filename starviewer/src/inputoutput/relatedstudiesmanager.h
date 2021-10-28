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

#ifndef UDGRELATEDSTUDIESMANAGER_H
#define UDGRELATEDSTUDIESMANAGER_H

#include <QObject>

#include "pacsjob.h"

#include <unordered_set>

#include <QDate>

namespace udg {

class DicomMask;
class PacsManager;
class Patient;
class Study;
class StudyOperationResult;

/**
    Aquesta classe donat un Study demana els estudis relacionats o previs en els PACS configurats per defecte, degut a que
    ara actualment en el PACS podem tenir pacients que són el mateix però amb PatientID diferents, també a part de cercar estudis
    que coincideixin amb el PatientID també es farà una altre cerca per Patient Name.
  */
/* TODO: En teoria amb la implantació del SAP els problemes de que un Pacient té diversos Patient ID o que té el nom
   escrit de maneres diferents haurien de desapareixer, per tant d'aquí un temps quan la majoria d'estudis del PACS
   ja s'hagin fet a través del SAP i constatem que el Patient ID pel mateix pacient sempre és el mateix,
   la cerca per nom de pacient podria desapareixer
 */
class RelatedStudiesManager : public QObject {
Q_OBJECT
public:
    RelatedStudiesManager();
    ~RelatedStudiesManager() override;

    /// Enum to know if loadStudy method was able to load the study from database, it's being retrieved or failed.
    enum LoadStatus { Loaded, Retrieving, Failed };

    /// Fa una consulta d'estudis previs assíncrona als PACS que estiguin marcats per defecte, si dos del PACS retornen el mateix estudi només es tindrà en compte
    /// el del primer PACS que ha respós
    /// Si ja s'estigués executant una consulta la cancel·laria i faria la nova consulta
    void queryMergedPreviousStudies(Study *study);

    /// Fa una consulta d'estudis del pacient assíncrona als PACS que estiguin marcats per defecte, si dos del PACS retornen el mateix estudi només es tindrà en compte
    /// el del primer PACS que ha respós
    /// Si ja s'estigués executant una consulta la cancel·laria i faria la nova consulta
    void queryMergedStudies(Patient *patient);

    /// Cancel·la les consultes actuals que s'estan executant, i cancel·la les consultes encuades per executar
    void cancelCurrentQuery();

    /// Indica si s'executen queries en aquest moment
    bool isExecutingQueries();

    /// Retrieves and loads the given study from the specified PACS
    void retrieveAndLoad(Study *study, const PacsDevice &pacsDevice);

    /// Load study from the database. If it is not in the database it is retrieved from PACS before being loaded.
    RelatedStudiesManager::LoadStatus loadStudy(Study *study);

    /// Return the list of studies of the given patient stored in the database
    QList<Study*> getStudiesFromDatabase(Patient *patient);

signals:
    /// Signal que s'emet quan ha finalitzat la consulta d'estudis. La llista amb els resultats s'esborrarà quan es demani una altra cerca.
    void queryStudiesFinished(QList<Study*>);

    /// Signal que s'emet per indicar que s'ha produït un error durant la descarrega d'un estudi (pot ser previ o no)
    void errorDownloadingStudy(QString studyUID);

    /// Emitted when a requested study retrieve starts.
    void studyRetrieveStarted(QString studyInstanceUid);
    /// Emitted when a requested study retrieve finishes successfully.
    void studyRetrieveFinished(QString studyInstanceUid);
    /// Emitted when a requested study retrieve finishes with error.
    void studyRetrieveFailed(QString studyInstanceUid);
    /// Emitted when a requested study retrieve is cancelled.
    void studyRetrieveCancelled(QString studyInstanceUid);

    /// Emitted when loadStudy finishes to load a study successfully.
    void studyLoaded(Study *study);

private:
    /// Realitza una consulta dels estudis del pacient "patient" als PACS marcats per defecte.
    /// Si s'especifica una data "until" només cercarà els estudis fins la data especificada (aquesta inclosa).
    /// Si no es passa cap data per paràmetre cercarà tots els estudis, independentment de la data.
    void makeAsynchronousStudiesQuery(Patient *patient, QDate untilDate = QDate());

    /// Retorna una màscara de cerca base a partir de les quals es generan les DicomMask per cercar estudis relacionats
    DicomMask getBasicDicomMask();

    /// Ens indica si aquell estudi està a la llista d'estudis ja rebuts, per evitar duplicats
    /// Hem de tenir en compte que com fem la cerca per ID i un altre per Patient Name per obtenir més resultats
    /// potser que en les dos consultes ens retornin el mateix estudi, per tant hem d'evitar duplicats.
    bool isStudyInMergedStudyList(Study *study);

    /// Ens indica si aquest estudi és el mateix pel qual ens han demanat els estudis relacionts, per evitar incloure'l a la llista
    bool isMainStudy(Study *study);

    /// Inicialitza les variables per realitzar una nova consulta
    void initializeQuery();

    /// Adds the given StudyOperationResult representing a query to the list of pending queries and creates the needed connections to it.
    void addPendingQuery(StudyOperationResult *result);

    /// Emet signal indicant la la consulta ha acabat
    void queryFinished();

    /// Esborra els resultats de la cerca
    void deleteQueryResults();

    /// Retorna una llista indicant de quins PACS s'han descarregat els estudis que conté el pacient, sempre que continguin aquesta informació al DICOMSource
    QList<PacsDevice> getPACSRetrievedStudiesOfPatient(Patient *patient);

    /// Return DICOM Masks to know what to query. It takes into accound
    /// the PatientID and PatientName properties of the patient and the value of m_searchRelatedStudiesByName
    QList<DicomMask> getDicomMasks(Patient *patient);

private slots:
    /// Adds the studies from the query to the list of related studies found, avoiding duplicates.
    void mergeStudiesFromQuery(StudyOperationResult *result);
    /// Called when the given query has finished with error.
    void onQueryError(StudyOperationResult *result);
    /// Called when the given query is cancelled.
    void onQueryCancelled(StudyOperationResult *result);

    /// Called when PacsManager successfully starts to retrieve a requested study.
    void onStudyRetrieveStarted(void *requester, PACSJobPointer pacsJob);
    /// Called when PacsManager successfully retrieves a requested study.
    void onStudyRetrieveFinished(void *requester, PACSJobPointer pacsJob);
    /// Called when PacsManager fails to retrieve a requested study.
    void onStudyRetrieveFailed(void *requester, PACSJobPointer pacsJob);
    /// Called when PacsManager cancels the request of a study.
    void onStudyRetrieveCancelled(void *requester, PACSJobPointer pacsJob);

private:
    PacsManager *m_pacsManager;
    QList<Study*> m_mergedStudyList;

    /// Study instance UID de l'estudi a partir del qual hem de trobar estudis relacionats
    QString m_studyInstanceUIDOfStudyToFindRelated;

    /// Contains StudyOperationResults that represent queries in progress.
    std::unordered_set<StudyOperationResult*> m_pendingQueryResults;
    /// Boolea per saber si s'ha de cercar estudis relacionats a partir del nom del pacient.
    bool m_searchRelatedStudiesByName;
};

}

#endif // UDGRELATEDSTUDIESMANAGER_H
