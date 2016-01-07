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

#ifndef UDGQRELATEDSTUDIESWIDGET_H
#define UDGQRELATEDSTUDIESWIDGET_H

#include <QWidget>
#include <QSignalMapper>
#include <QPushButton>
#include <QLabel>
#include <QHash>
#include <QButtonGroup>
#include <QRadioButton>

class QTreeWidgetItem;

namespace udg {

class Study;
class RelatedStudiesManager;
class QueryScreen;
class Patient;
class QTreeWidgetWithSeparatorLine;

class QRelatedStudiesWidget : public QFrame {
Q_OBJECT
public:
    QRelatedStudiesWidget(RelatedStudiesManager *relatedStudiesManager, QWidget *parent = 0);
    ~QRelatedStudiesWidget();

    /// Mètode per buscar els estudis relacionat amb el pacient.
    void searchStudiesOf(Patient *patient);

    /// Sets the study with the given studyUID as the current study. If the selected prior study is newer than this one, it is deselected.
    void setCurrentStudy(const QString &studyUID);

public slots:
    virtual void setVisible(bool visible);

    /// Actualitza la llista marcant aquells estudis que ja estan en memòria perquè sigui fàcil identificar-los i no es puguin tornar a descarregar.
    void updateList();

    /// If no prior study is selected, it tries to select one taking into account the properties of the current study.
    /// It is currently only available for mammography and CR Thorax.
    void toggleComparativeMode();

signals:
    /// S'emet només quan no hi ha altres estudis ja descarregant-se.
    void downloadingStudies();

    /// S'emet quan s'han acabat de descarregar tots els estudis demanats.
    void studiesDownloaded();

    /// Emit the StudyInstanceUID of the studies selected as the current and prior study.
    void workingStudiesChanged(const QString&, const QString&);

private:
    /// Creació de connexions
    void createConnections();
    /// Inicialització del QTreeWidget on es visualitzaran els estudis relacionats
    void initializeTree();
    /// Inicialització del widget mostrat quan s'estan cercant estudis relacionats.
    void initializeLookingForStudiesWidget();
    /// Insereix un estudi a l'arbre i genera el contenidor associat a aquest estudi.
    void insertStudyToTree(Study *study);
    /// Actualitza l'amplada del QTreeWidget per aconseguir que l'scroll horitzontal no apareixi i tota la info sigui visible.
    void updateWidgetWidth();
    /// Actualitza l'alçada del QTreeWidget per aconseguir que es vegi el màxim de contingut possible.
    /// Nota: Perquè funcioni correctament el QRelatedStudiesWidget ha de ser visible.
    void updateWidgetHeight();

    /// Metodes encarregats de controlar quan s'han d'emetre els signals \sa downloadingStudies i \sa studiesDownloaded;
    void increaseNumberOfDownladingStudies();
    void decreaseNumberOfDownladingStudies();

    void initializeSearch();

    /// Ressalta el QTreeWidgetItem posant el text que conté a negreta
    void highlightQTreeWidgetItem(QTreeWidgetItem *item);

    /// Treu del llistat de modalitats les modalitats que no són d'imatge
    QStringList removeNonImageModalities(const QStringList &studiesModalities);
    
    /// Indica si s'ha de ressaltar l'estudi en el QTreeWidget
    bool hasToHighlightStudy(Study *study);

    /// Make visible only those radio buttons that can be clicked taking into account
    /// the studies selected as the current and the prior at this time
    void updateVisibleCurrentRadioButtons();
    void updateVisiblePriorRadioButtons();

    /// Evaluate if both current and prior studies are loaded to notify the user has changed the working studies
    void notifyWorkingStudiesChangedIfReady();

    /// Insereix els estudis a l'arbre.
    void insertStudiesToTree(const QList<Study*> &studiesList);

private slots:
    /// Slot executed when the query is finished
    void queryStudiesFinished(const QList<Study*> &studiesList);

    /// Invoca la descàrrega i càrrega de l'estudi identificat amb l'uid proporcionat.
    void retrieveAndLoadStudy(const QString &studyInstanceUID);

    /// Actualitzen l'estat de l'objecte i la interfície de l'estudi en qüestió.
    void studyRetrieveStarted(QString studyInstanceUID);
    void studyRetrieveFinished(QString studyInstanceUID);
    void studyRetrieveFailed(QString studyInstanceUID);
    void studyRetrieveCancelled(QString studyInstanceUID);

    /// Slot executed when a radio button is clicked.
    /// It will proceed to load the selected study an notify the working studies has changed.
    void currentStudyRadioButtonClicked(const QString &studyInstanceUID);
    void priorStudyRadioButtonClicked(const QString &studyInstanceUID);

    /// Slot executed when an study has added.
    void onStudyAdded(Study *study);

private:
    /// Enumeració creada per tal de saber si els signals que emet QueryScreen pertanyen a alguna de les peticions d'aquesta classe
    enum Status { Initialized = 0, Pending = 1, Downloading = 2, Finished = 3, Failed = 4, Cancelled = 5 };
    enum Columns { CurrentStudy = 0, PriorStudy = 1, DownloadingStatus = 2, DownloadButton = 3, Modality = 4, Description = 5, Date = 6, Name = 7 };

    /// Contenidor d'objectes associats a l'estudi que serveix per facilitar la intercomunicació
    struct StudyInfo
    {
        Study *study;
        QTreeWidgetItem *item;
        Status status;
        QLabel *statusIcon;
        QPushButton *downloadButton;
        QRadioButton *currentRadioButton;
        QRadioButton *priorRadioButton;
    };

    /// Estructura que s'encarrega de guardar els contenidors associats a cada Study
    QHash<QString, StudyInfo*> m_infomationPerStudy;
    /// Widget utilitzat per mostrar la llista dels estudis relacionats
    QTreeWidgetWithSeparatorLine *m_relatedStudiesTree;
    /// Widget que apareix quan s'està fent la consulta dels possibles estudis relacionats.
    QWidget *m_lookingForStudiesWidget;
    /// Objecte encarregat de cercar estudis relacionats
    RelatedStudiesManager *m_relatedStudiesManager;
    /// Mapper utilitzat per saber cada botó de descàrrega a quin estudi està associat.
    QSignalMapper *m_signalMapper;
    /// Mapper utilitzat per saber cada botó de descàrrega a quin estudi està associat.
    QSignalMapper *m_currentStudySignalMapper;
    /// Mapper utilitzat per saber cada botó de descàrrega a quin estudi està associat.
    QSignalMapper *m_priorStudySignalMapper;
    /// Objecte utilitzat per invocar la descàrrega d'estudis.
    QueryScreen *m_queryScreen;
    /// Ens permet saber els estudis que s'estan descarregant.
    int m_numberOfDownloadingStudies;
    /// Pacient associat a la última cerca feta.
    Patient *m_patient;

    /// Button groups for current and prior radio buttons
    QButtonGroup m_currentStudyRadioGroup;
    QButtonGroup m_priorStudyRadioGroup;

    /// Working studies selected at a time
    QString m_studyInstanceUIDOfCurrentStudy;
    QString m_studyInstanceUIDOfPriorStudy;

    ///Llistat amb les modalitats dels estudis que hem de ressalar
    QStringList m_modalitiesOfStudiesToHighlight;
};

}
#endif // UDGQRELATEDSTUDIESWIDGET_H
