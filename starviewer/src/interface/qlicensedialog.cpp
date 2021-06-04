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

#include "qlicensedialog.h"

#include <QFile>

namespace udg {

QLicenseDialog::QLicenseDialog(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);

    setAttribute(Qt::WA_DeleteOnClose);

    QFile copyrightFile(":/COPYRIGHT");

    if (copyrightFile.open(QFile::ReadOnly | QFile::Text))
    {
        m_copyrightTextBrowser->setPlainText(copyrightFile.readAll());
        copyrightFile.close();
    }

    QFile licenseFile(":/LICENSE");

    if (licenseFile.open(QFile::ReadOnly | QFile::Text))
    {
        m_licenseTextBrowser->setPlainText(licenseFile.readAll());
        licenseFile.close();
    }

    QFile noticeFile(":/NOTICE");

    if (noticeFile.open(QFile::ReadOnly | QFile::Text))
    {
        m_thirdPartyLicensesTextBrowser->setPlainText(noticeFile.readAll());
        noticeFile.close();
    }
}

} // namespace udg
