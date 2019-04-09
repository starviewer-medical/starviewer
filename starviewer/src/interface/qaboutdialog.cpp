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

#include "qaboutdialog.h"

#include "qlicensedialog.h"
#include "starviewerapplication.h"
#ifdef STARVIEWER_CE
#include "qceaboutcontent.h"
#endif

#include <QPushButton>

namespace udg {

QAboutDialog::QAboutDialog(QWidget *parent)
 : QDialog(parent)
{
    setupUi(this);

    setWindowTitle(tr("About %1").arg(ApplicationNameString));

#ifdef STARVIEWER_LITE
    m_liteAboutTextLabel->setText(m_liteAboutTextLabel->text().arg(ApplicationNameString).arg(2019).arg(StarviewerVersionString).arg(OrganizationEmailString)
                                                              .arg(OrganizationWebURL));
    m_liteAboutTextLabel->setOpenExternalLinks(true);
    m_stackedWidget->setCurrentIndex(1);
#elif defined STARVIEWER_CE
    int index = m_stackedWidget->addWidget(new QCeAboutContent());
    m_stackedWidget->setCurrentIndex(index);
#else
    m_aboutTextLabel->setText(m_aboutTextLabel->text().arg(ApplicationNameString).arg(2019).arg(StarviewerVersionString).arg(OrganizationEmailString)
                                                      .arg(OrganizationWebURL));
    m_aboutTextLabel->setOpenExternalLinks(true);
    m_stackedWidget->setCurrentIndex(0);
#endif

    QPushButton *licenseButton = m_buttonBox->addButton(tr("License information"), QDialogButtonBox::ActionRole);
    connect(licenseButton, &QPushButton::clicked, this, &QAboutDialog::showLicenseInformation);
}

QAboutDialog::~QAboutDialog()
{
}

void QAboutDialog::showLicenseInformation()
{
    QLicenseDialog *licenseDialog = new QLicenseDialog(this);
    licenseDialog->open();
}

}
