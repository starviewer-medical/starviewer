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

#include <QDateTime>
#include <QMessageBox>
#include <QPushButton>

namespace udg {

QAboutDialog::QAboutDialog(QWidget *parent)
 : QDialog(parent), m_dontClose(false)
{
    setupUi(this);

    this->setAttribute(Qt::WA_DeleteOnClose);

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

    m_crashButton = m_buttonBox->addButton(tr("Crash test"), QDialogButtonBox::ActionRole);
    m_crashButton->setAutoDefault(false);
    connect(m_crashButton, &QPushButton::clicked, this, &QAboutDialog::crash);

    QPushButton *licenseButton = m_buttonBox->addButton(tr("License information"), QDialogButtonBox::ActionRole);
    licenseButton->setAutoDefault(false);
    connect(licenseButton, &QPushButton::clicked, this, &QAboutDialog::showLicenseInformation);

    QPushButton *closeButton = m_buttonBox->button(QDialogButtonBox::Close);
    connect(closeButton, &QPushButton::pressed, this, &QAboutDialog::onCloseButtonPressed);
    connect(closeButton, &QPushButton::released, this, &QAboutDialog::onCloseButtonReleased);

    m_crashButton->hide();  // it has to be hidden at the end, otherwise it remains visible
}

QAboutDialog::~QAboutDialog()
{
}

void QAboutDialog::reject()
{
    if (m_dontClose)
    {
        // Don't close this time, but do it the next
        m_dontClose = false;
    }
    else
    {
        QDialog::reject();
    }
}

void QAboutDialog::showLicenseInformation()
{
    QLicenseDialog *licenseDialog = new QLicenseDialog(this);
    licenseDialog->open();
}

void QAboutDialog::crash()
{
    QMessageBox confirmCrash(QMessageBox::Warning, tr("Crash test"), tr("Are you sure you want to crash %1 on purpose?").arg(ApplicationNameString));
    confirmCrash.addButton(QMessageBox::Yes);
    confirmCrash.addButton(QMessageBox::No);

    if (confirmCrash.exec() == QMessageBox::Yes)
    {
        int *nowhere;
        nowhere = nullptr;
        *nowhere = 1;
    }
}

void QAboutDialog::onCloseButtonPressed()
{
    m_longClickStart = QDateTime::currentMSecsSinceEpoch();
}

void QAboutDialog::onCloseButtonReleased()
{
    static constexpr qint64 MillisecondsToShowCrash = 5000;

    if ((QDateTime::currentMSecsSinceEpoch() - m_longClickStart) > MillisecondsToShowCrash)
    {
        m_crashButton->show();
        m_dontClose = true;
    }
}

}
