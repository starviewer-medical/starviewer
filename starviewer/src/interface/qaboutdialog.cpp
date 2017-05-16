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

#include <QPushButton>
#include <QMessageBox>
#include <QDateTime>

namespace udg {

QAboutDialog::QAboutDialog(QWidget *parent)
 : QDialog(parent)
{
    setupUi(this);

    setWindowTitle(tr("About %1").arg(ApplicationNameString));
    m_applicationNameLabel->setText("<h2>" + ApplicationNameString + "</h2>");

    QString aboutMessage = tr(
        "<p>Copyright &copy; 2005-%2 Graphics & Imaging Laboratory (GILab) and Institut de Diagnòstic per la Imatge (IDI), Girona."
        "<p align='justify'>%1 is a basic but fully featured image review software dedicated to DICOM images produced by medical equipment (MRI,"
        " CT, PET, PET-CT, CR, MG, ...) fully compliant with the DICOM standard for image communication and image file formats. It can also read"
        " many other file formats specified by the MetaIO standard (*.mhd files). ").arg(ApplicationNameString).arg(2014);
    // TODO repassar els textos de l'about
#ifdef STARVIEWER_LITE
    aboutMessage += tr("<p align='justify'>%1 has been specifically designed for navigation and visualization of multimodality and"
               " multidimensional images").arg(ApplicationNameString);
#else
    aboutMessage += tr("%1 is able to receive images "
               "transferred by DICOM communication protocol from any PACS or medical imaging modality (STORE SCP - Service Class Provider, "
               "STORE SCU - Service Class User, and Query/Retrieve)."
               "<p align='justify'>%1 enables navigation and visualization of multimodality and multidimensional images through"
               " a complete 2D Viewer which integrates advanced reconstruction techniques such as Thick Slab (including Maximum"
               " Intensity Projection (MIP), Minimum Intensity Projection (MinIP) and average projection), fast orthogonal reconstruction"
               " and 3D navigation tools such as 3D-Cursor. It also incorporates Multi-Planar Reconstruction (MPR) and 3D Viewer for volume rendering."
               "<p align='justify'>%1 is at the same time a DICOM workstation for medical imaging and an image processing"
               " software for medical research (radiology and nuclear imaging), functional imaging and 3D imaging.").arg(ApplicationNameString);
#endif

    aboutMessage += tr("<p align='justify'>%1 is the result of the close collaboration between IDI and GiLab and experience of both entities in the fields of "
                "radiology, medical imaging and image processing.").arg(ApplicationNameString);;
    aboutMessage += tr("<p>Version: %1 </p>").arg(StarviewerVersionString);
    aboutMessage += tr("<p>Support email: <a href=\"mailto:%1\">%1</a></p>").arg(OrganizationEmailString);
    aboutMessage += tr("<p>Web: <a href=\"%1\">%1</a></p>").arg(OrganizationWebURL);
    m_aboutTextLabel->setText(aboutMessage);
    m_aboutTextLabel->setOpenExternalLinks(true);

    QPushButton *licenseButton = m_buttonBox->addButton(tr("License information"), QDialogButtonBox::ActionRole);
    connect(licenseButton, &QPushButton::clicked, this, &QAboutDialog::showLicenseInformation);

    m_crashBtn = 0;

    m_okBtn = m_buttonBox->addButton(tr("Ok"), QDialogButtonBox::ActionRole);
    connect(m_okBtn, &QPushButton::clicked, this, &QAboutDialog::btnOkClicked);
    connect(m_okBtn, &QPushButton::pressed, this, &QAboutDialog::btnOkPressed);
    connect(m_okBtn, &QPushButton::released, this, &QAboutDialog::btnOkReleased);
}

QAboutDialog::~QAboutDialog()
{
}

void QAboutDialog::showLicenseInformation()
{
    QLicenseDialog *licenseDialog = new QLicenseDialog(this);
    licenseDialog->open();
}

void QAboutDialog::btnCrashClicked()
{
    QMessageBox confirmMsgbox(QMessageBox::Icon::Warning, tr("Crash test"), tr("Are you sure do you want to crash Starviewer on purpose?"));
    confirmMsgbox.addButton(QMessageBox::StandardButton::Yes);
    confirmMsgbox.addButton(QMessageBox::StandardButton::No);
    if (confirmMsgbox.exec() == QMessageBox::Yes) {
        int* nowhere;
        nowhere = 0x0;
        *nowhere = 1;
    }

}

void QAboutDialog::btnOkClicked()
{
    if (!m_crashBtn) { //Crash button not created
        close();
    }
}

void QAboutDialog::btnOkPressed()
{
    m_longClickStart = QDateTime::currentMSecsSinceEpoch();
}

void QAboutDialog::btnOkReleased()
{
    if ((QDateTime::currentMSecsSinceEpoch() - m_longClickStart) > msecsToShowCrash ) {
        m_crashBtn = m_buttonBox->addButton(tr("Crash test"), QDialogButtonBox::ActionRole);
        connect(m_crashBtn, &QPushButton::clicked, this, &QAboutDialog::btnCrashClicked);
    }

}




}
