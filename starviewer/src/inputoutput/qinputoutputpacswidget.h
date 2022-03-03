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

#ifndef UDGQINPUTOUTPUTPACSWIDGET_H
#define UDGQINPUTOUTPUTPACSWIDGET_H

#include "ui_qinputoutputpacswidgetbase.h"

#include <unordered_set>

#include <QList>
#include <QMenu>

namespace udg {

class StatsWatcher;
class StudyOperationResult;

/**
    Widget en el que controla les operacions d'entrada/sortida del PACS
  */
class QInputOutputPacsWidget : public QWidget, private Ui::QInputOutputPacsWidgetBase {
Q_OBJECT
public:
    /// Definició d'accions que podem fer d'haver descarregat estudis
    enum ActionsAfterRetrieve { None = 0, View = 1, Load = 2 };

    /// Constructor de la classe
    explicit QInputOutputPacsWidget(QWidget *parent = nullptr);
    ~QInputOutputPacsWidget() override;

    /// Consulta els estudis al dicomdir obert que compleixin la màscara de cerca
    void queryStudy(DicomMask queryMask, QList<PacsDevice> pacsToQuery);

    /// Neteja els resultats de la última cerca
    void clear();

    /// Descarrega una estudi del pacs
    void retrieve(const PacsDevice &pacsDevice, ActionsAfterRetrieve actionAfterRetrieve, Study *studyToRetrieve, const QString &seriesInstanceUIDToRetrieve = "", const QString &sopInstanceUIDToRetrieve = "");

signals:
    /// Signal que s'emet per indicar que es netegin els camps de cerca
    void clearSearchTexts();

    /// Signal que s'emet per indicar que s'ha demanat visualitzar un estudi
    void viewRetrievedStudy(QString studyInstanceUID);

    /// Signal que s'emet per indicar que un cop descarregat l'estudi s'ha de carregar únicament
    void loadRetrievedStudy(QString studyInstanceUID);

    /// Signal que s'emet per indica que un estudi ha estat descarregat
    void studyRetrieveFinished(QString studyInstanceUID);

private:
    /// Crea les connexions entre signals i slots
    void createConnections();

    /// Genera el menú contextual que apareix quan clickem amb el botó dret a sobre d'un item del StudyTreeWidget
    void createContextMenuQStudyTreeWidget();

    /// Comprova que els paràmetres per la cerca siguin correctes, que no es tractir d'un consulta pesada i que ens hagin seleccionat
    /// algun PACS per consultar
    bool areValidQueryParameters(DicomMask *maskToQuery, QList<PacsDevice> pacsToQuery);

    /// Adds the given StudyOperationResult representing a query to the list of pending queries and creates the needed connections to it.
    void addPendingQuery(StudyOperationResult *result);

    /// Amaga/mostra que hi ha una query en progress i habilitat/deshabilitat el botó de cancel·lar la query actual
    void setQueryInProgress(bool queryInProgress);

    /// Descarrega els estudis seleccionats dels QStudyTreeWidget, i una vegada descarregats por a terme l'acció passada per paràmetre
    void retrieveSelectedItemsFromQStudyTreeWidget(ActionsAfterRetrieve _actionsAfterRetrieve);

private slots:
    /// Updates the study tree widget with the results from the given query.
    void showQueryResult(StudyOperationResult *result);
    /// Updates the study tree widget with the results from the given query and showns the warning(s) produced in the query.
    void showQueryResultAndWarning(StudyOperationResult *result);
    /// Shows the error produced in the given query.
    void showQueryError(StudyOperationResult *result);
    /// Called when the given query is cancelled.
    void onQueryCancelled(StudyOperationResult *result);

    /// Mostra les sèries d'un estudi, les consulta al dicomdir i les mostra al tree widget
    void requestedSeriesOfStudy(Study *study);

    /// Mostra les imatges d'un estudi, les consulta al dicomdir i les mostra al tree widget
    void requestedImagesOfSeries(Series *series);

    /// Importa cap a la base de dades local els estudis seleccionats
    void retrieveSelectedItemsFromQStudyTreeWidget();

    /// Importa cap a la base de dades local els estudis seleccionats indicant
    /// que s'han de visualitzar immediatament un cop descarregats
    void retrieveAndViewSelectedItemsFromQStudyTreeWidget();

    /// Cancel·la els QueryPACSJob que s'han llançat des d'aquesta classe i que encara no han finalitzat (cancel·la els que s'estan executant i els pendents
    /// d'executar).
    /// La idea és donar mètode que es pugui invocar per cancel·lar les consultes actuals, per exemple s'ha llançat una consulta a 3 PACS
    /// per cercar tots els estudis amb ID 1, si l'usuari canvia la consulta i diu que ara vol tots els estudis amb ID 2, no cal seguir endavant amb
    /// l'anterior consulta doncs aquest mètode està pensat per aquests casos per poder cancel·lar les consultes actuals llançades des d'aquesta classe
    /// que s'estan realitzant.
    void cancelCurrentQueriesToPACS();

    /// Called when a retrieve operation finishes successfully.
    void onRetrieveSuccess(StudyOperationResult *result, ActionsAfterRetrieve action);
    /// Called when a retrieve operation finishes with partial success.
    void onRetrievePartialSuccess(StudyOperationResult *result, ActionsAfterRetrieve action);
    /// Called when a retrieve operation finishes with error.
    void onRetrieveError(StudyOperationResult *result);

private:
    QMenu m_contextMenuQStudyTreeWidget;

    /// Contains StudyOperationResults that represent queries in progress.
    std::unordered_set<StudyOperationResult*> m_pendingQueryResults;

    StatsWatcher *m_statsWatcher;
};

}

#endif
