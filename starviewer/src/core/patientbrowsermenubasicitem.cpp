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

#include "patientbrowsermenubasicitem.h"

namespace udg {

PatientBrowserMenuBasicItem::PatientBrowserMenuBasicItem(QObject *parent)
: QObject(parent)
{
}

void PatientBrowserMenuBasicItem::setText(const QString &text)
{
    m_text = text;
    emit textChanged();
}

QString PatientBrowserMenuBasicItem::getText()
{
    return m_text;
}

void PatientBrowserMenuBasicItem::setIdentifier(const QString &identifier)
{
    m_identifier = identifier;
    emit identifierChanged();
}

QString PatientBrowserMenuBasicItem::getIdentifier() const
{
    return m_identifier;
}

QString PatientBrowserMenuBasicItem::getType()
{
    return "BasicItem";
}

}
