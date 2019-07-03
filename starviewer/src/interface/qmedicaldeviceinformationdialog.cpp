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

#include "qmedicaldeviceinformationdialog.h"

#include "interfacesettings.h"
#include "starviewerapplication.h"

namespace udg {

QMedicalDeviceInformationDialog::QMedicalDeviceInformationDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUi(this);

    this->setAttribute(Qt::WA_DeleteOnClose);

    this->setWindowTitle(this->windowTitle().arg(ApplicationNameString));
    m_titleLabel->setText(m_titleLabel->text().arg(ApplicationNameString));
    m_textBrowser->setHtml(m_textBrowser->toHtml().arg(ApplicationNameString));

    Settings settings;
    bool dontShowAgain = settings.getValue(InterfaceSettings::DontShowMedicalDeviceInformationDialog).toBool();
    m_dontShowAgainCheckBox->setChecked(dontShowAgain);
}

QMedicalDeviceInformationDialog::~QMedicalDeviceInformationDialog()
{
    Settings settings;
    settings.setValue(InterfaceSettings::DontShowMedicalDeviceInformationDialog, m_dontShowAgainCheckBox->isChecked());
}

} // namespace udg
