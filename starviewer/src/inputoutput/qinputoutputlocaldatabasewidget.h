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


#ifndef UDGQINPUTOUTPUTLOCALDATABASEWIDGET_H
#define UDGQINPUTOUTPUTLOCALDATABASEWIDGET_H

#include "ui_qinputoutputlocaldatabasewidgetbase.h"

#include "localdatabasemanager.h"
#include "qdeleteoldstudiesthread.h"

#include <QMenu>

namespace udg {

class QCreateDicomdir;
class QWidgetSelectPacsToStoreDicomImage;
class StatsWatcher;
class StudyOperationResult;

/**
    Widget en el que controla les operacions d'entrada/sortida de la base de dades local
  */
class QInputOutputLocalDatabaseWidget : public QWidget, private Ui::QInputOutputLocalDatabaseWidgetBase {
Q_OBJECT

public:
    explicit QInputOutputLocalDatabaseWidget(QWidget *parent = nullptr);
    ~QInputOutputLocalDatabaseWidget();

    /// Consulta els estudis al dicomdir obert que compleixin la màscara de cerca
    void queryStudy(DicomMask queryMask);

    /// Li passem un punte a la interfície que crea dicomdir, per a que s'hi pugui comunicar-se per preguntar
    /// si l'estudi abans de ser esborrat està a llista d'estudis per crear un nou dicomdir, o per indicar-li
    /// que s'ha afegit demanat afegir un nou estudi a llista d'estudis per crear dicomdir
    /// TODO s'hauria buscar una manera més elegant de comunicar les dos classes, fer un singletton de QCreateDicomdir ?
    void setQCreateDicomdir(QCreateDicomdir *qcreateDicomdir);

    /// Neteja els resultats que es mostren de la cerca
    void clear();

public slots:

    /// Emet signal selectedPatients indicant que s'han seleccionat estudis per ser visualitzats
    /// Afegim un terer paràmetre "loadOnly" que ens indicarà si únicament volem carregar les dades sense necessitat de fer un "view"
    /// Així diferenciem els casos en que volem carregar dades del pacient "en background" (només fusionem dades del pacient i prou,
    /// sense aplicar canvis visibles sobre la interfície) útil pels casos de carregar estudis previs, per exemple.
    void view(QList<DicomMask> dicomMaskStudiesToView, bool loadOnly = false);
    void view(QString studyInstanceUID, bool loadOnly = false);

    /// Afegeix l'estudi amb l'Study Instance UID passat per paràmetre al Widget
    void addStudyToQStudyTreeWidget(QString studyInstanceUID);

    /// Treu l'estudi amb l'Study Instance UID passat per paràmetre del QStudyTreeWidget
    void removeStudyFromQStudyTreeWidget(QString studyInstanceUID);

signals:
    /// Signal que s'emet per indicar que es netegin els camps de cerca
    void clearSearchTexts();

    /// Signal que s'emet per indicar que s'ha demanat visualitzar un estudi
    /// Afegim un segon paràmetre per indicar si volem fer un "view" o únicament carregar en background les dades de pacient i prou
    void viewPatients(QList<Patient*> patientsToView, bool onlyLoad);

private:
    /// Crea les connexions entre signals i slots
    void createConnections();

    /// Genera el menú contextual que apareix quan clickem amb el botó dret a sobre d'un item del StudyTreeWidget
    void createContextMenuQStudyTreeWidget();

    /// Mostrar l'error que s'ha produït amb les operacions a la base de dades
    bool showDatabaseManagerError(LocalDatabaseManager::LastError error, const QString &doingWhat = "");

    /// Esborra els estudis vells
    // TODO Aquesta responsabilitat d'esborrar els estudis vells al iniciar-se l'aplicació s'hauria de
    // traslladar a un altre lloc, no és responsabilitat d'aquesta inferfície
    void deleteOldStudies();

    /// Retorna totes les imatges d'un pacient
    QList<Image*> getAllImagesFromPatient(Patient *patient);

private slots:
    /// Mostra les sèries d'un estudi, les consulta al dicomdir i les mostra al tree widget
    void requestedSeriesOfStudy(Study *studyRequestedSeries);

    /// Mostra al SeriesListWidget la previsualització de la sèrie seleccionada en aquell moment al QStudyTreeWidget
    void setSeriesToSeriesListWidget(Study *study);

    void currentSeriesOfQStudyTreeWidgetChanged(Series *series);

    void currentSeriesChangedOfQSeriesListWidget(const QString &studyInstanceUID, const QString &seriesInstanceUID);

    /// Esborra de la base de dades els estudis seleccionats en el QStudyTreeWidgetView
    void deleteSelectedItemsFromLocalDatabase();

    /// Slot que es dispara quan ha finalitzat el thread que esborrar els estudis vells, aquest slot comprova que no s'hagi produït cap error esborrant
    /// els estudis vells
    void deleteOldStudiesThreadFinished();

    /// Afegeix els estudis seleccionats a la llista d'estudis a convertir a dicomdir
    void addSelectedStudiesToCreateDicomdirList();

    /// Visualitza l'estudi que se li ha fet doble click QSeriesListWidget
    void viewFromQSeriesListWidget(QString studyInstanceUID, QString seriesInstanceUID);

    /// Visualitza els estudis seleccionats a la QStudyTreeWidget
    void viewFromQStudyTreeWidget();

    /// Fa signal indicant que els estudis seleccionats s'han de guardar al PACS
    void selectedStudiesStoreToPacs();

    /// Guarda la posició de l'splitter quan l'han mogut
    void qSplitterPositionChanged();

    /// Guarda els estudis seleccionats al PACS que l'usuari ha seleccionat
    void sendSelectedStudiesToSelectedPacs();

    /// Called when a store operation finishes with partial success.
    void onStorePartialSuccess(StudyOperationResult *result);
    /// Called when a store operation finishes with error.
    void onStoreError(StudyOperationResult *result);

private:
    QMenu m_contextMenuQStudyTreeWidget;
    QDeleteOldStudiesThread m_qdeleteOldStudiesThread;
    QCreateDicomdir *m_qcreateDicomdir;
    StatsWatcher *m_statsWatcher;
    QWidgetSelectPacsToStoreDicomImage *m_qwidgetSelectPacsToStoreDicomImage;

};

};// end namespace udg

#endif
