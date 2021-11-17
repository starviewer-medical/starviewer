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

#ifndef UDGQOPERATIONSTATESCREEN_H
#define UDGQOPERATIONSTATESCREEN_H

#include "ui_qoperationstatescreenbase.h"

#include <QHash>

#include "pacsjob.h"

// Fordward declarations
class QString;

namespace udg {

// Fordward declarations
class Status;
class Operation;
class RetrieveDICOMFilesFromPACSJob;
class SendDICOMFilesToPACSJob;
class Study;

/// Interfície que implementa la llista d'operacions realitzades cap a un PACS
class QOperationStateScreen : public QDialog, private Ui::QOperationStateScreenBase {
Q_OBJECT
public:
    enum ColumnIndex { Status = 0, Direction = 1, FromTo = 2, PatientID = 3, PatientName = 4, Date = 5, Started = 6, Series = 7, Files = 8, PACSJobID = 9 };

    QOperationStateScreen(QWidget *parent = 0);
    ~QOperationStateScreen();

protected:
    /// Event que s'activa al tancar al rebren un event de tancament
    /// @param event de tancament
    void closeEvent(QCloseEvent *ce);

private slots:
    /// Slot que s'activa quan s'ha encuat un nou PACSJob insereix al QTreeWidget la informació del nou job i la posa com a Pedent de realitzar
    void newPACSJobEnqueued(PACSJobPointer);

    /// Slot que s'activa quan job comença, al QTreeWidget es marca aquell job com començat
    void PACSJobStarted(PACSJobPointer);

    /// Slot que s'activa quan el job ha acabat, es marca aquell Job al QTreeWidget amb l'estatus en el que ha finalitzat
    void PACSJobFinished(PACSJobPointer);

    /// Slot que s'activa quan el job ha estat cancel·lat, es marca aquell job com ha cancel·lat
    void PACSJobCancelled(PACSJobPointer pacsJob);

    /// Slot que s'activa quan job ha fet una acció amb una imatge, s'augmenta pel job al QTreeWidget el número de d'imatges
    void DICOMFileCommit(PACSJobPointer pacsJob, int numberOfImages);

    /// Slot que s'activa quan job ha fer una acció amb una sèrie completa, s'augmenta pel job al QTreeWidget el número de sèries
    void DICOMSeriesCommit(PACSJobPointer pacsJob, int numberOfSeries);

    /// Neteja la llista d'estudis excepte dels que s'estant descarregant en aquells moments
    void clearList();

    /// Cancel·la totes les peticions al PACS que s'estan executant o estant pendents
    void cancelAllRequests();

    /// Cancel·la les peticions seleccionades al PACS que s'estan executant o estant pendents
    void cancelSelectedRequests();

    /// Slot que s'activa quan rebem SIGNAL que s'ha demanat cancel·lar un Job
    void requestedCancelPACSJob(PACSJobPointer);

private:
    QString m_currentProcessingStudyUID;
    QHash<int, PACSJobPointer> m_PACSJobPendingToFinish;

private:
    /// Crea les connexions pels signals i slots
    void createConnections();

    /// Afegeix al QTreeWidget el nou job encuat, i el mostra
    void insertNewPACSJob(PACSJobPointer pacsJob);

    /// Retorna l'objecte Study relatiu a PACSJob, només funciona amb PACSJob de tipus RetrieveDICOMFilesFromPACSJob i SendDICOMFilesToPACSJob
    const Study* getStudyFromPACSJob(PACSJobPointer pacsJob);

    /// Retorna un QString per mostrar-lo per la QOperationStateScreen indicant com ha finalitzat un PACSJob
    /// Per RetrieveDICOMFilesFromPACS pot retornar : RETRIEVED, CANCELLED, ERROR
    /// Per SendDICOMFilesToPACS pot retornar : SENT, CANCELLED, ERROR
    QString getPACSJobStatusResume(PACSJobPointer pacsJob);

    /// Ens retorna un QTreeWidgetItem a partir del seu pacsJobID si no el trobem es retorna null;
    QTreeWidgetItem* getQTreeWidgetItemByPACSJobId(int pacsJobID);
};

};

#endif
