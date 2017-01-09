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

#include "qlistenrisrequestsconfigurationscreen.h"

#include <QIntValidator>

#include "inputoutputsettings.h"

namespace udg {

QListenRisRequestsConfigurationScreen::QListenRisRequestsConfigurationScreen(QWidget *parent)
 : QWidget(parent)
{
    setupUi(this);
    m_RISRequestsPortWarningLabel->setVisible(false);
    m_RISRequestsPortWarningIcon->setVisible(false);
    loadRisConfiguration();

    this->setWindowIcon(QIcon(":images/icons/ris.svg"));
    createConnections();
    configureInputValidator();
}

QListenRisRequestsConfigurationScreen::~QListenRisRequestsConfigurationScreen()
{
}

void QListenRisRequestsConfigurationScreen::createConnections()
{
    connect(m_checkBoxListenRisRequests, SIGNAL(toggled(bool)), SLOT(updateListenRISRequestsSetting(bool)));
    connect(m_textPortListenRisRequests, SIGNAL(textEdited(QString)), SLOT(updateRISRequestsPortWarning()));
    connect(m_textPortListenRisRequests, SIGNAL(editingFinished()), SLOT(updateRISRequestsPortSetting()));
    connect(m_checkBoxViewAutomaticallyStudies, SIGNAL(toggled(bool)), SLOT(updateAutomaticallyViewStudiesSetting(bool)));
}

void QListenRisRequestsConfigurationScreen::configureInputValidator()
{
    m_textPortListenRisRequests->setValidator(new QIntValidator(0, 65535, m_textPortListenRisRequests));
}

void QListenRisRequestsConfigurationScreen::loadRisConfiguration()
{
    Settings settings;

    bool listenToRISRequests = settings.getValue(InputOutputSettings::ListenToRISRequests).toBool();

    m_checkBoxListenRisRequests->setChecked(listenToRISRequests);
    m_checkBoxViewAutomaticallyStudies->setEnabled(listenToRISRequests);
    m_textPortListenRisRequests->setEnabled(listenToRISRequests);

    m_checkBoxViewAutomaticallyStudies->setChecked(settings.getValue(InputOutputSettings::RISRequestViewOnceRetrieved).toBool());
    m_textPortListenRisRequests->setText(QString().setNum(settings.getValue(InputOutputSettings::RISRequestsPort).toInt()));
}

void QListenRisRequestsConfigurationScreen::updateRISRequestsPortWarning()
{
    if (!m_textPortListenRisRequests->text().isEmpty())
    {
        m_RISRequestsPortWarningLabel->setText(tr("The application has to be restarted to apply the changes."));
        m_RISRequestsPortWarningLabel->setVisible(true);
        m_RISRequestsPortWarningIcon->setVisible(true);
    }
    else
    {
        Settings settings;
        m_RISRequestsPortWarningLabel->setText(tr("A port number should be specified. Current configured port %1 will remain unchanged if none provided.")
            .arg(settings.getValue(InputOutputSettings::RISRequestsPort).toInt()));
        m_RISRequestsPortWarningLabel->setVisible(true);
        m_RISRequestsPortWarningIcon->setVisible(true);
    }
}

void QListenRisRequestsConfigurationScreen::updateListenRISRequestsSetting(bool enable)
{
    Settings settings;
    settings.setValue(InputOutputSettings::ListenToRISRequests, enable);

    m_RISRequestsPortWarningLabel->setEnabled(enable);
    m_RISRequestsPortWarningIcon->setEnabled(enable);
}

void QListenRisRequestsConfigurationScreen::updateRISRequestsPortSetting()
{
    if (m_textPortListenRisRequests->isModified() && !m_textPortListenRisRequests->text().isEmpty())
    {
        Settings settings;
        settings.setValue(InputOutputSettings::RISRequestsPort, m_textPortListenRisRequests->text().toInt());
    }
}

void QListenRisRequestsConfigurationScreen::updateAutomaticallyViewStudiesSetting(bool enable)
{
    Settings settings;
    settings.setValue(InputOutputSettings::RISRequestViewOnceRetrieved, enable);
}

};
