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

#include "qrelatedstudieswidget.h"

#include "logging.h"
#include "study.h"
#include "patient.h"
#include "relatedstudiesmanager.h"
#include "queryscreen.h"
#include "singleton.h"
#include "screenmanager.h"
#include "qtreewidgetwithseparatorline.h"

#include <QVBoxLayout>
#include <QMovie>
#include <QTreeWidgetItem>
#include <QScrollBar>
#include "applicationstylehelper.h"

namespace udg {

QRelatedStudiesWidget::QRelatedStudiesWidget(RelatedStudiesManager *relatedStudiesManager, QWidget *parent)
   : QFrame(parent)
{
    Q_ASSERT(relatedStudiesManager);
    
    m_relatedStudiesManager = relatedStudiesManager;
    
    setWindowFlags(Qt::Popup);
    QVBoxLayout *verticalLayout = new QVBoxLayout(this);

    m_lookingForStudiesWidget = new QWidget(this);
    m_relatedStudiesTree = new QTreeWidgetWithSeparatorLine(this);
    m_signalMapper = new QSignalMapper(this);
    m_queryScreen = SingletonPointer<QueryScreen>::instance();
    m_numberOfDownloadingStudies = 0;

    ApplicationStyleHelper style;
    style.setScaledFontSizeTo(this);

    initializeLookingForStudiesWidget();
    initializeTree();

    verticalLayout->addWidget(m_relatedStudiesTree);
    verticalLayout->addWidget(m_lookingForStudiesWidget);

    createConnections();

    m_lookingForStudiesWidget->setVisible(false);
}

QRelatedStudiesWidget::~QRelatedStudiesWidget()
{
    delete m_relatedStudiesTree;
    foreach (const QString &key, m_infomationPerStudy.keys())
    {
        delete m_infomationPerStudy.take(key);
    }
    delete m_lookingForStudiesWidget;
    delete m_signalMapper;
}

void QRelatedStudiesWidget::updateList()
{
    if (!m_infomationPerStudy.isEmpty())
    {
        foreach (Study *study, m_patient->getStudies())
        {
            StudyInfo *studyInfo = m_infomationPerStudy[study->getInstanceUID()];

            if (studyInfo != NULL)
            {
                if (studyInfo->status == Downloading)
                {
                    this->decreaseNumberOfDownladingStudies();
                }
                studyInfo->status = Finished;
                studyInfo->statusIcon->setPixmap(QPixmap(":/images/button_ok.png"));
                studyInfo->downloadButton->setEnabled(false);
            }
        }
    }
}

void QRelatedStudiesWidget::searchStudiesOf(Patient *patient)
{
    Q_ASSERT(patient);

    initializeSearch();
    m_patient = patient;
    m_modalitiesOfStudiesToHighlight.append(patient->getModalities());

    // Insert already loaded studies
    insertStudiesToTree(patient->getStudies());

    //Insert studies from the database
    insertStudiesToTree(m_relatedStudiesManager->getStudiesFromDatabase(patient));

    m_relatedStudiesManager->queryMergedStudies(patient);
}

void QRelatedStudiesWidget::initializeSearch()
{
    m_lookingForStudiesWidget->setVisible(true);

    int items = m_relatedStudiesTree->topLevelItemCount();
    for (int i = 0; i < items; i++)
    {
        delete m_relatedStudiesTree->takeTopLevelItem(0);
    }
    foreach (const QString &key, m_infomationPerStudy.keys())
    {
        delete m_infomationPerStudy.take(key);
    }
}

void QRelatedStudiesWidget::createConnections()
{
    connect(m_relatedStudiesManager, SIGNAL(queryStudiesFinished(QList<Study*>)), SLOT(queryStudiesFinished(QList<Study*>)));
    connect(m_signalMapper, SIGNAL(mapped(const QString&)), SLOT(retrieveAndLoadStudy(const QString&)));
    connect(m_queryScreen, SIGNAL(studyRetrieveStarted(QString)), SLOT(studyRetrieveStarted(QString)));
    connect(m_queryScreen, SIGNAL(studyRetrieveFinished(QString)), SLOT(studyRetrieveFinished(QString)));
    connect(m_queryScreen, SIGNAL(studyRetrieveFailed(QString)), SLOT(studyRetrieveFailed(QString)));
    connect(m_queryScreen, SIGNAL(studyRetrieveCancelled(QString)), SLOT(studyRetrieveCancelled(QString)));
}

void QRelatedStudiesWidget::initializeTree()
{

    // Inicialitzem la capçalera
    QStringList labels;
    labels << "" << "" << tr("Modality") << tr("Description") << tr("Date") << tr("Name");
    m_relatedStudiesTree->setHeaderLabels(labels);

    // Fem 8 columnes perquè la primera l'amagarem
    m_relatedStudiesTree->setColumnCount(6);
    m_relatedStudiesTree->setRootIsDecorated(false);
    m_relatedStudiesTree->setItemsExpandable(false);
    m_relatedStudiesTree->header()->setSectionResizeMode(DownloadingStatus, QHeaderView::Fixed);
    m_relatedStudiesTree->header()->setSectionResizeMode(DownloadButton, QHeaderView::Fixed);
    m_relatedStudiesTree->header()->setSectionsMovable(false);
    m_relatedStudiesTree->setUniformRowHeights(true);
    m_relatedStudiesTree->setSortingEnabled(true);

    // Ordenem els estudis per data i hora
    m_relatedStudiesTree->sortItems(Date, Qt::DescendingOrder);

    // El farem visible quan rebem la llista d'estudis previs
    m_relatedStudiesTree->setVisible(false);
}

void QRelatedStudiesWidget::initializeLookingForStudiesWidget()
{
    QHBoxLayout *horizontalLayout = new QHBoxLayout(m_lookingForStudiesWidget);

    QLabel *downloadigAnimation = new QLabel();
    QMovie *operationAnimation = new QMovie();
    operationAnimation->setFileName(":/images/loader.gif");
    downloadigAnimation->setMovie(operationAnimation);
    operationAnimation->start();

    horizontalLayout->addWidget(downloadigAnimation);
    horizontalLayout->addWidget(new QLabel(tr("Looking for related studies...")));
    horizontalLayout->addStretch();

}

void QRelatedStudiesWidget::insertStudyToTree(Study *study)
{
    QTreeWidgetItem *item = new QTreeWidgetItem();

    // Afegim l'item al widget
    m_relatedStudiesTree->addTopLevelItem(item);
    item->setFlags(Qt::ItemIsEnabled);

    item->setText(Name, study->getParentPatient()->getFullName());
    item->setText(Date, study->getDate().toString(Qt::ISODate) + "   " + study->getTimeAsString());
    item->setText(Modality, study->getModalitiesAsSingleString());
    item->setText(Description, study->getDescription());

    QWidget *statusWidget = new QWidget(m_relatedStudiesTree);
    QHBoxLayout *statusLayout = new QHBoxLayout(statusWidget);
    QLabel *status = new QLabel(statusWidget);
    statusLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding));
    statusLayout->addWidget(status);
    statusLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding));
    statusLayout->setMargin(0);

    m_relatedStudiesTree->setItemWidget(item, DownloadingStatus, statusWidget);

    // Posem el botó en un Layout amb Margin 2 per a que els Items del QTreeWidget no estiguin tant junts i el control sigui més llegible
    QIcon dowloadIcon(QString(":/images/view.png"));
    QPushButton *downloadButton = new QPushButton(dowloadIcon, QString(""));
    QWidget *downloadButtonWidget = new QWidget(m_relatedStudiesTree);
    QVBoxLayout *downloadButtonLayout = new QVBoxLayout(downloadButtonWidget);
    //Apliquem 1px més de layout per la línia separadora 
    downloadButtonLayout->setContentsMargins(0, 2, 0, 1);
    downloadButtonLayout->addWidget(downloadButton);

    connect(downloadButton, SIGNAL(clicked()), m_signalMapper, SLOT(map()));
    m_signalMapper->setMapping(downloadButton, study->getInstanceUID());

    if (hasToHighlightStudy(study))
    {
        highlightQTreeWidgetItem(item);
    }

    m_relatedStudiesTree->setItemWidget(item, DownloadButton, downloadButtonWidget);

    // Guardem informació relacionada amb l'estudi per facilitar la feina
    StudyInfo *relatedStudyInfo = new StudyInfo;
    relatedStudyInfo->item = item;
    relatedStudyInfo->study = study;
    relatedStudyInfo->downloadButton = downloadButton;
    relatedStudyInfo->statusIcon = status;
    relatedStudyInfo->status = Initialized;
    m_infomationPerStudy.insert(study->getInstanceUID(), relatedStudyInfo);
}

void QRelatedStudiesWidget::highlightQTreeWidgetItem(QTreeWidgetItem *item)
{
    for (int index = 0; index < item->columnCount(); index++)
    {
        item->setBackground(index, QColor(149, 206, 236, 255));
    }
}

void QRelatedStudiesWidget::updateWidgetWidth()
{
    int fixedSize = 0;
    for (int i = 0; i < m_relatedStudiesTree->columnCount(); i++)
    {
        m_relatedStudiesTree->resizeColumnToContents(i);
        fixedSize += m_relatedStudiesTree->columnWidth(i);
    }
    m_relatedStudiesTree->setFixedWidth(fixedSize + 20);
}

void QRelatedStudiesWidget::updateWidgetHeight()
{
    ScreenManager screen;
    int screenAvailableHeight = screen.getScreenLayout().getScreen(screen.getScreenID(this)).getAvailableGeometry().height();
    int topAndMargins = this->geometry().top() + m_relatedStudiesTree->geometry().top() * 2; // Es multiplica per 2 pel marge inferior.
    int maxHeight = screenAvailableHeight - topAndMargins;
    int minHeight = m_relatedStudiesTree->sizeHint().height();
    int contentHeight = minHeight;
    bool found = false;
    while (!found && contentHeight < maxHeight)
    {
        m_relatedStudiesTree->setFixedHeight(contentHeight);
        found = m_relatedStudiesTree->verticalScrollBar()->maximum() <= m_relatedStudiesTree->verticalScrollBar()->minimum();
        contentHeight += 5;
    }
}

void QRelatedStudiesWidget::insertStudiesToTree(const QList<Study*> &studiesList)
{
    if (studiesList.size() > 0)
    {
        foreach (Study *study, studiesList)
        {
            if (m_infomationPerStudy[study->getInstanceUID()] == NULL)
            {
                //Sempre hauria de ser més gran de 0
                insertStudyToTree(study);
            }
            else
            {
                StudyInfo *studyInfo = m_infomationPerStudy[study->getInstanceUID()];
                if (studyInfo->study->getDICOMSource().getRetrievePACS().count() == 0)
                {
                    studyInfo->study->setDICOMSource(study->getDICOMSource());
                }
            }
        }

        updateList();

        m_relatedStudiesTree->setVisible(true);

        ApplicationStyleHelper style;
        style.setScaledFontSizeTo(m_relatedStudiesTree);

        updateWidgetWidth();
        updateWidgetHeight();
    }

    this->adjustSize();
}

void QRelatedStudiesWidget::queryStudiesFinished(const QList<Study *> &studiesList)
{
    m_lookingForStudiesWidget->setVisible(false);
    insertStudiesToTree(studiesList);
}

void QRelatedStudiesWidget::retrieveAndLoadStudy(const QString &studyInstanceUID)
{
    StudyInfo *studyInfo = m_infomationPerStudy[studyInstanceUID];

    studyInfo->downloadButton->setEnabled(false);

    switch (m_relatedStudiesManager->loadStudy(studyInfo->study))
    {
        case RelatedStudiesManager::Loaded:
            studyInfo->status = Finished;
            studyInfo->statusIcon->setPixmap(QPixmap(":/images/button_ok.png"));
            break;

        case RelatedStudiesManager::Failed:
            studyInfo->statusIcon->setPixmap(QPixmap(":/images/cancel.png"));
            studyInfo->downloadButton->setEnabled(true);
            break;

        case RelatedStudiesManager::Retrieving:
        {
            studyInfo->status = Pending;

            QMovie *statusAnimation = new QMovie();
            studyInfo->statusIcon->setMovie(statusAnimation);
            statusAnimation->setFileName(":/images/loader.gif");
            statusAnimation->start();

            this->increaseNumberOfDownladingStudies();
        }
    }
}

void QRelatedStudiesWidget::studyRetrieveStarted(QString studyInstanceUID)
{
    StudyInfo *studyInfo = m_infomationPerStudy[studyInstanceUID];

    // Comprovem que el signal capturat de QueryScreen sigui nostre
    if (studyInfo != NULL)
    {
        if (studyInfo->status == Pending)
        {
            studyInfo->status = Downloading;
        }
    }
}

void QRelatedStudiesWidget::studyRetrieveFinished(QString studyInstanceUID)
{
    StudyInfo *studyInfo = m_infomationPerStudy[studyInstanceUID];

    // Comprovem que el signal capturat de QueryScreen sigui nostre
    if (studyInfo != NULL)
    {
        if (studyInfo->status == Downloading)
        {
            studyInfo->status = Finished;
            studyInfo->statusIcon->setPixmap(QPixmap(":/images/button_ok.png"));

            this->decreaseNumberOfDownladingStudies();
        }
    }

}

void QRelatedStudiesWidget::studyRetrieveFailed(QString studyInstanceUID)
{
    StudyInfo *studyInfo = m_infomationPerStudy[studyInstanceUID];

    // Comprovem que el signal capturat de QueryScreen sigui nostre
    if (studyInfo != NULL)
    {
        if (studyInfo->status == Downloading)
        {
            studyInfo->status = Failed;
            studyInfo->statusIcon->setPixmap(QPixmap(":/images/cancel.png"));
            studyInfo->downloadButton->setEnabled(true);

            this->decreaseNumberOfDownladingStudies();
        }
    }
}

void QRelatedStudiesWidget::studyRetrieveCancelled(QString studyInstanceUID)
{
    StudyInfo *studyInfo = m_infomationPerStudy[studyInstanceUID];

    // Comprovem que el signal capturat de QueryScreen sigui nostre
    if (studyInfo != NULL)
    {
        if (studyInfo->status == Downloading || studyInfo->status == Pending)
        {
            studyInfo->status = Cancelled;
            studyInfo->statusIcon->setPixmap(QPixmap(":/images/cancel.png"));
            studyInfo->downloadButton->setEnabled(true);

            this->decreaseNumberOfDownladingStudies();
        }
    }
}

void QRelatedStudiesWidget::increaseNumberOfDownladingStudies()
{
    m_numberOfDownloadingStudies++;
    if (m_numberOfDownloadingStudies == 1)
    {
        emit downloadingStudies();
    }
}

void QRelatedStudiesWidget::decreaseNumberOfDownladingStudies()
{
    m_numberOfDownloadingStudies--;
    if (m_numberOfDownloadingStudies == 0)
    {
        emit studiesDownloaded();
    }
}

void QRelatedStudiesWidget::setVisible(bool visible)
{
    QFrame::setVisible(visible);
    if (visible)
    {
        updateWidgetHeight();
        this->adjustSize();
    }
}

QStringList QRelatedStudiesWidget::removeNonImageModalities(const QStringList &studiesModalities)
{
    QStringList studiesModalitiesToReturn(studiesModalities);
    studiesModalitiesToReturn.removeAll("KO");
    studiesModalitiesToReturn.removeAll("PR");
    studiesModalitiesToReturn.removeAll("SR");

    return studiesModalities;
}

bool QRelatedStudiesWidget::hasToHighlightStudy(Study *study)
{
    foreach (const QString &modality, study->getModalities())
    {
        if (m_modalitiesOfStudiesToHighlight.contains(modality))
        {
            return true;
        }
    }

    return false;
}

}
