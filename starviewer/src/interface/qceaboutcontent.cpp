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

#include "qceaboutcontent.h"

#include "starviewerapplication.h"

namespace udg {

QCeAboutContent::QCeAboutContent(QWidget *parent)
    : QWidget(parent)
{
    setupUi(this);

    m_applicationNameLabel->setText(m_applicationNameLabel->text().arg(ApplicationNameString));
    m_versionLabel->setText(m_versionLabel->text().arg(StarviewerVersionString));
    m_ceAboutTextLabel->setText(m_ceAboutTextLabel->text().arg(ApplicationNameString).arg(2019).arg(OrganizationEmailString).arg(OrganizationWebURL));
    QString buildDate = StarviewerBuildID;
    buildDate.replace(QRegularExpression("(\\d{4})(\\d{2})(\\d{2})\\d{2}"), "\\1-\\2-\\3");
    m_buildDateLabel->setText(m_buildDateLabel->text().arg(buildDate));

    m_ceAboutTextLabel->setOpenExternalLinks(true);
    m_ceManufacturerLabel->setOpenExternalLinks(true);
}

} // namespace udg
