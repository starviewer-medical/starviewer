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

#include "patientbrowsermenuextendedinfo.h"

#include "patientbrowsermenuextendeditem.h"

#include <QVBoxLayout>
#include <QEvent>

namespace udg {

PatientBrowserMenuExtendedInfo::PatientBrowserMenuExtendedInfo(QWidget *parent)
: QFrame(parent)
{
    m_layout = new QVBoxLayout(this);
    m_layout->addWidget(new PatientBrowserMenuExtendedItem(this));
    m_layout->setMargin(0);
}

PatientBrowserMenuExtendedInfo::~PatientBrowserMenuExtendedInfo()
{
}

void PatientBrowserMenuExtendedInfo::setItems(const QList<PatientBrowserMenuExtendedItem*> &items)
{
    clearLayout(m_layout);

    int maxWidth = 0;
    int accumulatedHeight = 0;
    foreach(PatientBrowserMenuExtendedItem *item, items)
    {
        item->adjustSize();
        m_layout->addWidget(item);
        maxWidth = qMax(maxWidth, item->width());
        accumulatedHeight += item->height();
    }

    // Size needs to be explicitly defined
    setFixedHeight(accumulatedHeight);
    setFixedWidth(maxWidth);
}


bool PatientBrowserMenuExtendedInfo::event(QEvent *event)
{
    // Si s'ha pulsat l'escape
    if (event->type() == QEvent::Close)
    {
        emit closed();
    }
    return QFrame::event(event);
}

void PatientBrowserMenuExtendedInfo::clearLayout(QLayout* layout, bool deleteWidgets)
{
    while (QLayoutItem *item = layout->takeAt(0))
    {
        if (deleteWidgets)
        {
            if (QWidget *widget = item->widget())
            {
                delete widget;
            }
        }
        if (QLayout *childLayout = item->layout())
        {
            clearLayout(childLayout, deleteWidgets);
        }
        delete item;
    }
}

}
