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
    m_currentStudySignalMapper = new QSignalMapper(this);
    m_priorStudySignalMapper = new QSignalMapper(this);
    m_queryScreen = SingletonPointer<QueryScreen>::instance();
    m_numberOfDownloadingStudies = 0;

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
            StudyInfo *studyInfo = m_infomationPerStudy.value(study->getInstanceUID());

            if (studyInfo != NULL)
            {
                if (studyInfo->status == Downloading)
                {
                    this->decreaseNumberOfDownladingStudies();
                }
                studyInfo->status = Finished;
                studyInfo->statusIcon->setPixmap(QPixmap(":/images/icons/dialog-ok-apply.svg"));
                studyInfo->downloadButton->setEnabled(false);
            }
        }

        updateVisibleCurrentRadioButtons();
        updateVisiblePriorRadioButtons();
    }
}

void QRelatedStudiesWidget::searchStudiesOf(Patient *patient)
{
    Q_ASSERT(patient);

    initializeSearch();
    m_patient = patient;

    connect(m_patient, SIGNAL(studyAdded(Study*)), SLOT(onStudyAdded(Study*)));

    m_modalitiesOfStudiesToHighlight.append(patient->getModalities());

    // Insert already loaded studies
    insertStudiesToTree(patient->getStudies());

    // Initially check most recent study as the CurrentStudy and the second one as the PriorStudy
    if (m_patient->getStudies().size() > 0)
    {
        Study *study = m_patient->getStudies().first();
        m_studyInstanceUIDOfCurrentStudy = study->getInstanceUID();
        m_infomationPerStudy.value(m_studyInstanceUIDOfCurrentStudy)->currentRadioButton->setChecked(true);

        if (m_patient->getStudies().size() > 1)
        {
            Study *prior = m_patient->getStudies().at(1);
            m_studyInstanceUIDOfPriorStudy = prior->getInstanceUID();
            m_infomationPerStudy.value(m_studyInstanceUIDOfPriorStudy)->priorRadioButton->setChecked(true);
        }

        updateVisiblePriorRadioButtons();
    }

    //Insert studies from the database
    insertStudiesToTree(m_relatedStudiesManager->getStudiesFromDatabase(patient));

    m_relatedStudiesManager->queryMergedStudies(patient);
}

void QRelatedStudiesWidget::setCurrentStudy(const QString &studyUID)
{
    StudyInfo *studyInfo = m_infomationPerStudy.value(studyUID);

    if (studyInfo)
    {
        // If the necessary radio button is not visible, unselect the prior study
        if (!studyInfo->currentRadioButton->isVisible())
        {
            StudyInfo *priorStudyInfo = m_infomationPerStudy.value(m_studyInstanceUIDOfPriorStudy);

            if (priorStudyInfo)
            {
                priorStudyInfo->priorRadioButton->click();
            }
        }

        studyInfo->currentRadioButton->click();
    }
}

void QRelatedStudiesWidget::toggleComparativeMode()
{
    if (!m_studyInstanceUIDOfPriorStudy.isEmpty())
    {
        StudyInfo *studyInfo = m_infomationPerStudy.value(m_studyInstanceUIDOfPriorStudy);
        studyInfo->priorRadioButton->click();
    }
    else
    {
        StudyInfo *studyInfo = m_infomationPerStudy.value(m_studyInstanceUIDOfCurrentStudy);
        if (studyInfo)
        {
            Study *currentStudy = studyInfo->study;
            QList<QPair<QString, QString> > searchPriorExpressions;

            if (currentStudy->getDescription().contains(QRegularExpression("T[oòó]rax", QRegularExpression::CaseInsensitiveOption)))
            {
                if (currentStudy->getModalitiesAsSingleString().contains(QRegularExpression("CR|DX")))
                {
                    QString descriptionExpression = "T[oòó]rax";

                    searchPriorExpressions << QPair<QString, QString>("CR", descriptionExpression);
                    searchPriorExpressions << QPair<QString, QString>("DX", descriptionExpression);
                }
            }
            else if (currentStudy->getModalitiesAsSingleString().contains(QRegularExpression("MG|CR")))
            {
                bool match = false;

                if (currentStudy->getModalitiesAsSingleString().contains("MG"))
                {
                    match = true;
                }
                else if (currentStudy->getDescription().contains(QRegularExpression("Mamograf[ií]a", QRegularExpression::CaseInsensitiveOption)))
                {
                    match = true;
                }

                if (match)
                {
                    searchPriorExpressions << QPair<QString, QString>("CR", "Mamograf[ií]a");
                    searchPriorExpressions << QPair<QString, QString>("MG", "");
                }
            }

            if (!searchPriorExpressions.isEmpty())
            {
                QList<Study*> studies;
                foreach (StudyInfo *info, m_infomationPerStudy.values())
                {
                    if (*currentStudy > *(info->study))
                    {
                        studies << info->study;
                    }
                }
                studies = Study::sortStudies(studies, Study::RecentStudiesFirst);

                foreach (Study *study, studies)
                {
                    bool match = false;
                    int i = 0;
                    while (!match && i < searchPriorExpressions.length())
                    {
                        const QPair<QString, QString> ex = searchPriorExpressions.at(i);
                        QRegularExpression modalityRegEx = QRegularExpression(ex.first);
                        QRegularExpression descriptionRegEx = QRegularExpression(ex.second, QRegularExpression::CaseInsensitiveOption);
                        if (study->getModalitiesAsSingleString().contains(modalityRegEx) && study->getDescription().contains(descriptionRegEx))
                        {
                            match = true;
                        }
                        i++;
                    }
                    if (match)
                    {
                        StudyInfo *studyInfo = m_infomationPerStudy.value(study->getInstanceUID());
                        studyInfo->priorRadioButton->click();

                        break;
                    }
                }
            }
        }
    }
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
    connect(m_currentStudySignalMapper, SIGNAL(mapped(const QString&)), SLOT(currentStudyRadioButtonClicked(const QString&)));
    connect(m_priorStudySignalMapper, SIGNAL(mapped(const QString&)), SLOT(priorStudyRadioButtonClicked(const QString&)));
    connect(m_queryScreen, SIGNAL(studyRetrieveStarted(QString)), SLOT(studyRetrieveStarted(QString)));
    connect(m_queryScreen, SIGNAL(studyRetrieveFinished(QString)), SLOT(studyRetrieveFinished(QString)));
    connect(m_queryScreen, SIGNAL(studyRetrieveFailed(QString)), SLOT(studyRetrieveFailed(QString)));
    connect(m_queryScreen, SIGNAL(studyRetrieveCancelled(QString)), SLOT(studyRetrieveCancelled(QString)));
}

void QRelatedStudiesWidget::initializeTree()
{

    // Inicialitzem la capçalera
    QStringList labels;
    labels << tr("Current") << tr("Prior") << "" << "" << tr("Modality") << tr("Description") << tr("Date") << tr("Name");
    m_relatedStudiesTree->setHeaderLabels(labels);

    // Fem 8 columnes perquè la primera l'amagarem
    m_relatedStudiesTree->setColumnCount(8);
    m_relatedStudiesTree->setRootIsDecorated(false);
    m_relatedStudiesTree->setItemsExpandable(false);
    m_relatedStudiesTree->header()->setSectionResizeMode(DownloadingStatus, QHeaderView::Fixed);
    m_relatedStudiesTree->header()->setSectionResizeMode(DownloadButton, QHeaderView::Fixed);
    m_relatedStudiesTree->header()->setSectionResizeMode(CurrentStudy, QHeaderView::Fixed);
    m_relatedStudiesTree->header()->setSectionResizeMode(PriorStudy, QHeaderView::Fixed);
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
    operationAnimation->setFileName(":/images/animations/loader.gif");
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

    // Add Radio Button to select current study
    QWidget *currentStudyWidget = new QWidget(m_relatedStudiesTree);
    QRadioButton *currentRadioButton = new QRadioButton(m_relatedStudiesTree);
    QHBoxLayout *currentStudyLayout = new QHBoxLayout(currentStudyWidget);
    currentStudyLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding));
    currentStudyLayout->addWidget(currentRadioButton);
    currentStudyLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding));
    currentStudyLayout->setMargin(0);
    m_currentStudyRadioGroup.addButton(currentRadioButton);
    m_relatedStudiesTree->setItemWidget(item, CurrentStudy, currentStudyWidget);
    connect(currentRadioButton, SIGNAL(clicked()), m_currentStudySignalMapper, SLOT(map()));
    m_currentStudySignalMapper->setMapping(currentRadioButton, study->getInstanceUID());

    // Add Radio Button to select prior study
    QWidget *priorStudyWidget = new QWidget(m_relatedStudiesTree);
    QRadioButton *priorRadioButton = new QRadioButton(m_relatedStudiesTree);
    QHBoxLayout *priorStudyLayout = new QHBoxLayout(priorStudyWidget);
    priorStudyLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding));
    priorStudyLayout->addWidget(priorRadioButton);
    priorStudyLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding));
    priorStudyLayout->setMargin(0);
    m_priorStudyRadioGroup.addButton(priorRadioButton);
    m_relatedStudiesTree->setItemWidget(item, PriorStudy, priorStudyWidget);
    connect(priorRadioButton, SIGNAL(clicked()), m_priorStudySignalMapper, SLOT(map()));
    m_priorStudySignalMapper->setMapping(priorRadioButton, study->getInstanceUID());

    QWidget *statusWidget = new QWidget(m_relatedStudiesTree);
    QHBoxLayout *statusLayout = new QHBoxLayout(statusWidget);
    QLabel *status = new QLabel(statusWidget);
    statusLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding));
    statusLayout->addWidget(status);
    statusLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding));
    statusLayout->setMargin(0);

    m_relatedStudiesTree->setItemWidget(item, DownloadingStatus, statusWidget);

    // Posem el botó en un Layout amb Margin 2 per a que els Items del QTreeWidget no estiguin tant junts i el control sigui més llegible
    QIcon dowloadIcon(QString(":/images/icons/visibility.svg"));
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
    relatedStudyInfo->currentRadioButton = currentRadioButton;
    relatedStudyInfo->priorRadioButton = priorRadioButton;
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
            StudyInfo *studyInfo = m_infomationPerStudy.value(study->getInstanceUID());
            if (studyInfo == NULL)
            {
                //Sempre hauria de ser més gran de 0
                insertStudyToTree(study);
            }
            else
            {
                if (studyInfo->study->getDICOMSource().getRetrievePACS().count() == 0)
                {
                    studyInfo->study->setDICOMSource(study->getDICOMSource());
                }
            }
        }

        updateList();

        m_relatedStudiesTree->setVisible(true);

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
    StudyInfo *studyInfo = m_infomationPerStudy.value(studyInstanceUID);

    studyInfo->downloadButton->setEnabled(false);

    switch (m_relatedStudiesManager->loadStudy(studyInfo->study))
    {
        case RelatedStudiesManager::Loaded:
            studyInfo->status = Finished;
            studyInfo->statusIcon->setPixmap(QPixmap(":/images/icons/dialog-ok-apply.svg"));
            notifyWorkingStudiesChangedIfReady();
            break;

        case RelatedStudiesManager::Failed:
            studyInfo->status = Failed;
            studyInfo->statusIcon->setPixmap(QPixmap(":/images/icons/dialog-cancel.svg"));
            studyInfo->downloadButton->setEnabled(true);
            break;

        case RelatedStudiesManager::Retrieving:
        {
            studyInfo->status = Pending;

            QMovie *statusAnimation = new QMovie();
            studyInfo->statusIcon->setMovie(statusAnimation);
            statusAnimation->setFileName(":/images/animations/loader.gif");
            statusAnimation->start();

            this->increaseNumberOfDownladingStudies();
        }
    }
}

void QRelatedStudiesWidget::studyRetrieveStarted(QString studyInstanceUID)
{
    StudyInfo *studyInfo = m_infomationPerStudy.value(studyInstanceUID);

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
    StudyInfo *studyInfo = m_infomationPerStudy.value(studyInstanceUID);

    // Comprovem que el signal capturat de QueryScreen sigui nostre
    if (studyInfo != NULL)
    {
        if (studyInfo->status == Downloading)
        {
            studyInfo->status = Finished;
            studyInfo->statusIcon->setPixmap(QPixmap(":/images/icons/dialog-ok-apply.svg"));

            this->decreaseNumberOfDownladingStudies();
        }
    }

}

void QRelatedStudiesWidget::studyRetrieveFailed(QString studyInstanceUID)
{
    StudyInfo *studyInfo = m_infomationPerStudy.value(studyInstanceUID);

    // Comprovem que el signal capturat de QueryScreen sigui nostre
    if (studyInfo != NULL)
    {
        if (studyInfo->status == Downloading)
        {
            studyInfo->status = Failed;
            studyInfo->statusIcon->setPixmap(QPixmap(":/images/icons/dialog-cancel.svg"));
            studyInfo->downloadButton->setEnabled(true);

            this->decreaseNumberOfDownladingStudies();
        }
    }
}

void QRelatedStudiesWidget::studyRetrieveCancelled(QString studyInstanceUID)
{
    StudyInfo *studyInfo = m_infomationPerStudy.value(studyInstanceUID);

    // Comprovem que el signal capturat de QueryScreen sigui nostre
    if (studyInfo != NULL)
    {
        if (studyInfo->status == Downloading || studyInfo->status == Pending)
        {
            studyInfo->status = Cancelled;
            studyInfo->statusIcon->setPixmap(QPixmap(":/images/icons/dialog-cancel.svg"));
            studyInfo->downloadButton->setEnabled(true);

            this->decreaseNumberOfDownladingStudies();
        }
    }
}

void QRelatedStudiesWidget::currentStudyRadioButtonClicked(const QString &studyInstanceUID)
{
    if (m_studyInstanceUIDOfCurrentStudy == studyInstanceUID)
    {
        return;
    }

    m_studyInstanceUIDOfCurrentStudy = studyInstanceUID;
    updateVisiblePriorRadioButtons();

    StudyInfo *studyInfo = m_infomationPerStudy.value(studyInstanceUID);

    if (studyInfo->status == Initialized)
    {
        retrieveAndLoadStudy(studyInstanceUID);
    }
    else if (studyInfo->status == Finished)
    {
        notifyWorkingStudiesChangedIfReady();
    }
}

void QRelatedStudiesWidget::notifyWorkingStudiesChangedIfReady()
{
    StudyInfo *currentStudyInfo = m_infomationPerStudy.value(m_studyInstanceUIDOfCurrentStudy);

    if (currentStudyInfo && currentStudyInfo->status == Finished)
    {
        if (m_studyInstanceUIDOfPriorStudy.isEmpty() || m_infomationPerStudy.value(m_studyInstanceUIDOfPriorStudy)->status == Finished)
        {
            emit workingStudiesChanged(m_studyInstanceUIDOfCurrentStudy, m_studyInstanceUIDOfPriorStudy);
        }
    }
}

void QRelatedStudiesWidget::priorStudyRadioButtonClicked(const QString &studyInstanceUID)
{
    StudyInfo *studyInfo = m_infomationPerStudy.value(studyInstanceUID);

    if (m_studyInstanceUIDOfPriorStudy == studyInstanceUID)
    {
        m_studyInstanceUIDOfPriorStudy = "";

        QAbstractButton* checked = m_priorStudyRadioGroup.checkedButton();
        m_priorStudyRadioGroup.setExclusive(false);
        checked->setChecked(false);
        m_priorStudyRadioGroup.setExclusive(true);

        updateVisibleCurrentRadioButtons();

        if (studyInfo->status == Finished)
        {
            notifyWorkingStudiesChangedIfReady();
        }
    }
    else
    {
        m_studyInstanceUIDOfPriorStudy = studyInstanceUID;

        updateVisibleCurrentRadioButtons();

        if (studyInfo->status == Initialized)
        {
            retrieveAndLoadStudy(studyInstanceUID);
        }
        else if (studyInfo->status == Finished)
        {
            notifyWorkingStudiesChangedIfReady();
        }
    }

}

void QRelatedStudiesWidget::updateVisibleCurrentRadioButtons()
{
    if (m_studyInstanceUIDOfPriorStudy.isEmpty())
    {
        foreach (StudyInfo *studyInfo, m_infomationPerStudy.values())
        {
            studyInfo->currentRadioButton->setVisible(true);
        }
    }
    else
    {
        StudyInfo *studyInfo = m_infomationPerStudy.value(m_studyInstanceUIDOfPriorStudy);

        if (studyInfo != NULL)
        {
            QDateTime dateTime = studyInfo->study->getDateTime();

            foreach (StudyInfo *studyInfo, m_infomationPerStudy.values())
            {
                if (studyInfo->study->getDateTime() > dateTime)
                {
                    studyInfo->currentRadioButton->setVisible(true);
                }
                else
                {
                    studyInfo->currentRadioButton->setVisible(false);
                }
            }
        }
    }
    // Don't use processEvents() here because it can cause a crash if the application is closed while studies are being inserted:
    // the close event is processed and this object is deleted, but then the execution continues with an invalid 'this' pointer
}

void QRelatedStudiesWidget::updateVisiblePriorRadioButtons()
{
    if (m_studyInstanceUIDOfCurrentStudy.isEmpty())
    {
        return;
    }

    StudyInfo *studyInfo = m_infomationPerStudy.value(m_studyInstanceUIDOfCurrentStudy);

    if (studyInfo != NULL)
    {
        QDateTime dateTime = studyInfo->study->getDateTime();

        foreach (StudyInfo *studyInfo, m_infomationPerStudy.values())
        {
            if (studyInfo->study->getDateTime() < dateTime)
            {
                studyInfo->priorRadioButton->setVisible(true);
            }
            else
            {
                studyInfo->priorRadioButton->setVisible(false);
            }
        }
    }
    // Don't use processEvents() here because it can cause a crash if the application is closed while studies are being inserted:
    // the close event is processed and this object is deleted, but then the execution continues with an invalid 'this' pointer
}

void QRelatedStudiesWidget::onStudyAdded(Study *study)
{
    if (study->getInstanceUID() == m_studyInstanceUIDOfCurrentStudy || study->getInstanceUID() == m_studyInstanceUIDOfPriorStudy)
    {
        notifyWorkingStudiesChangedIfReady();
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
