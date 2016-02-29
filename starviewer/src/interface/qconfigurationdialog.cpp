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

#include "qconfigurationdialog.h"

#ifndef STARVIEWER_LITE
#include "qconfigurationscreen.h"
#endif

#include "qlocaldatabaseconfigurationscreen.h"
#include "qlistenrisrequestsconfigurationscreen.h"
#include "qdicomdirconfigurationscreen.h"
#include "q2dviewerconfigurationscreen.h"
#include "q2dviewerlayoutconfigurationscreen.h"
#include "qexternalapplicationconfigurationscreen.h"
#include "starviewerapplication.h"

namespace udg {

QConfigurationDialog::QConfigurationDialog(QWidget *parent, Qt::WindowFlags f)
 : QDialog(parent, f)
{
    setupUi(this);
    setWindowTitle(tr("%1 Configuration").arg(ApplicationNameString));
    setWindowFlags((this->windowFlags() | Qt::WindowMaximizeButtonHint) ^ Qt::WindowContextHelpButtonHint);

    // Configuració del visor 2D
    Q2DViewerConfigurationScreen *q2dviewerScreen = new Q2DViewerConfigurationScreen(this);
    addConfigurationWidget(q2dviewerScreen, tr("2D Viewer"), BasicConfiguration);

    // Configuració del layout del visor 2D
    Q2DViewerLayoutConfigurationScreen *q2dviewerLayoutScreen = new Q2DViewerLayoutConfigurationScreen(this);
    addConfigurationWidget(q2dviewerLayoutScreen, tr("2D Viewer Layout"), BasicConfiguration);

#ifndef STARVIEWER_LITE
    // No mostrem configuració del PACS
    QConfigurationScreen *pacsConfigurationScreen = new QConfigurationScreen(this);
    this->addConfigurationWidget(pacsConfigurationScreen, tr("PACS"), AdvancedConfiguration);
#endif

    // Configuracions de la base de dades local
    QLocalDatabaseConfigurationScreen *localDatabaseScreen = new QLocalDatabaseConfigurationScreen(this);
    this->addConfigurationWidget(localDatabaseScreen, tr("Local Database"), AdvancedConfiguration);

#ifndef STARVIEWER_LITE
    // No mostrem configuració del servei que escolta les peticions del RIS
    QListenRisRequestsConfigurationScreen *qListenRisRequestsConfigurationScreen = new QListenRisRequestsConfigurationScreen(this);
    this->addConfigurationWidget(qListenRisRequestsConfigurationScreen, tr("RIS Listener"), AdvancedConfiguration);
#endif

    // Configuració del programa de gravació
    QDICOMDIRConfigurationScreen *dicomdirScreen = new QDICOMDIRConfigurationScreen(this);
    this->addConfigurationWidget(dicomdirScreen, tr("DICOMDIR"), AdvancedConfiguration);

    // External applications configuration
    QExternalApplicationConfigurationScreen *externalApplicationScreen = new QExternalApplicationConfigurationScreen(this);
    this->addConfigurationWidget(externalApplicationScreen, tr("External application"), AdvancedConfiguration);

    connect(m_viewAdvancedOptions, SIGNAL(stateChanged(int)), SLOT(setViewAdvancedConfiguration()));

    m_optionsList->setCurrentRow(0);
    m_viewAdvancedOptions->setCheckState(Qt::Checked);
}

QConfigurationDialog::~QConfigurationDialog()
{
}

void QConfigurationDialog::setViewAdvancedConfiguration()
{
    foreach (QListWidgetItem *item, m_configurationListItems.values(AdvancedConfiguration))
    {
        item->setHidden(!m_viewAdvancedOptions->isChecked());
    }
    m_optionsList->setCurrentRow(0);
}

void QConfigurationDialog::addConfigurationWidget(QWidget *widget, const QString &name, ConfigurationType type)
{
    QWidget *page = new QWidget();

    QVBoxLayout *verticalLayout = new QVBoxLayout(page);
    verticalLayout->setSpacing(6);
    verticalLayout->setMargin(9);

    QHBoxLayout *horizontalLayout = new QHBoxLayout();
    horizontalLayout->setSpacing(6);
    horizontalLayout->setMargin(0);

    QLabel *optionTitleIcon = new QLabel(page);
    optionTitleIcon->setPixmap(widget->windowIcon().pixmap(48, 48));

    horizontalLayout->addWidget(optionTitleIcon);

    QLabel *optionTitle = new QLabel(page);
    optionTitle->setAlignment(Qt::AlignVCenter);
    optionTitle->setWordWrap(false);
    optionTitle->setText(widget->windowTitle());

    horizontalLayout->addWidget(optionTitle);

    QSpacerItem *spacerItem = new QSpacerItem(101, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    horizontalLayout->addItem(spacerItem);

    verticalLayout->addLayout(horizontalLayout);

    QFrame *line = new QFrame(page);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);

    verticalLayout->addWidget(line);
    verticalLayout->addWidget(widget);

    m_optionsStack->addWidget(page);
    QListWidgetItem *item = new QListWidgetItem(m_optionsList);
    item->setText(name);
    item->setIcon(widget->windowIcon());

    m_configurationListItems.insert(type, item);
}

}
