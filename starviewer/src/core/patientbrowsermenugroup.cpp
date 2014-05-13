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

#include "patientbrowsermenugroup.h"

namespace udg {

PatientBrowserMenuGroup::PatientBrowserMenuGroup(QObject *parent) :
    QObject(parent)
{
}

void PatientBrowserMenuGroup::setCaption(const QString &caption)
{
    m_caption = caption;
}

QString PatientBrowserMenuGroup::getCaption() const
{
    return m_caption;
}

void PatientBrowserMenuGroup::setElements(const QList<PatientBrowserMenuBasicItem*> &elements)
{
    m_elements = elements;
}

QList<PatientBrowserMenuBasicItem*> PatientBrowserMenuGroup::getElements() const
{
    return m_elements;
}

QList<QObject*> PatientBrowserMenuGroup::getElementsAsQObject() const
{
    QList<QObject*> output;
    foreach (PatientBrowserMenuBasicItem* item, m_elements)
    {
        output << item;
    }

    return output;
}

void PatientBrowserMenuGroup::setFusionElements(const QList<PatientBrowserMenuBasicItem*> &elements)
{
    m_fusionElements = elements;
}

QList<PatientBrowserMenuBasicItem*> PatientBrowserMenuGroup::getFusionElements() const
{
    return m_elements;
}

QList<QObject*> PatientBrowserMenuGroup::getFusionElementsAsQObject() const
{
    QList<QObject*> output;
    foreach (PatientBrowserMenuBasicItem* item, m_fusionElements)
    {
        output << item;
    }

    return output;
}

} // namespace udg
