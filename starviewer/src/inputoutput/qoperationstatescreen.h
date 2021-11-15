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

namespace udg {

class StudyOperationResult;

/**
 * @brief The QOperationStateScreen class shows a list of all the study retrieve and store operations performed in Starviewer, past, current and pending.
 *
 * It allows to cancel current and pending operations.
 */
class QOperationStateScreen : public QDialog, private Ui::QOperationStateScreenBase {
Q_OBJECT
public:
    enum ColumnIndex { Status = 0, Direction = 1, FromTo = 2, PatientID = 3, PatientName = 4, Date = 5, Started = 6, Series = 7, Files = 8, Result = 9 };

    explicit QOperationStateScreen(QWidget *parent = nullptr);
    ~QOperationStateScreen() override;

protected:
    /// Event que s'activa al tancar al rebren un event de tancament
    /// @param event de tancament
    void closeEvent(QCloseEvent *ce) override;

private slots:
    /// Adds the operation to the list and creates the necessary connections.
    void registerOperation(StudyOperationResult *result);

    /// Called when an operation starts. Marks it as started in the widget.
    void onOperationStarted(StudyOperationResult *result);
    /// Called when an operation finishes. Marks it as finished in the widget and removes it from the map.
    void onOperationFinished(StudyOperationResult *result);
    /// Called when an operation is cancelled. Marks it as cancelled in the widget and removes it from the map.
    void onOperationCancelled(StudyOperationResult *result);

    /// Called when an instance is transferred. Updates the file counter.
    void onInstanceTransferred(StudyOperationResult *result, int totalInstancesTransferred);
    /// Called when a series is transferred. Updates the series counter.
    void onSeriesTransferred(StudyOperationResult *result, int totalSeriesTransferred);

    /// Neteja la llista d'estudis excepte dels que s'estant descarregant en aquells moments
    void clearList();

    /// Cancel·la totes les peticions al PACS que s'estan executant o estant pendents
    void cancelAllRequests();

    /// Cancel·la les peticions seleccionades al PACS que s'estan executant o estant pendents
    void cancelSelectedRequests();

private:
    /// Crea les connexions pels signals i slots
    void createConnections();

    /// Inserts an item into the tree for the given result.
    void insertIntoTree(StudyOperationResult *result);

private:
    /// Map from result to tree widget item for all current and pending operations.
    QHash<StudyOperationResult*, QTreeWidgetItem*> m_resultToItemMap;
};

}

#endif
