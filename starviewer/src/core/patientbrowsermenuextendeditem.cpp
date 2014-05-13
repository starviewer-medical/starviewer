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

#include "patientbrowsermenuextendeditem.h"

#include "thumbnailcreator.h"

#include <QVBoxLayout>
#include <QPixmap>
#include <QPainter>

namespace udg {

const QString NoItemText(QObject::tr("No Item Selected"));

PatientBrowserMenuExtendedItem::PatientBrowserMenuExtendedItem(QWidget *parent)
: QFrame(parent)
{
    createInitialWidget();
}

PatientBrowserMenuExtendedItem::~PatientBrowserMenuExtendedItem()
{
}

void PatientBrowserMenuExtendedItem::createInitialWidget()
{
    QVBoxLayout *verticalLayout = new QVBoxLayout(this);
    m_icon = new QLabel(this);
    m_icon->setPixmap(QPixmap::fromImage(ThumbnailCreator::makeEmptyThumbnailWithCustomText(NoItemText)));
    m_icon->setAlignment(Qt::AlignCenter);
    verticalLayout->addWidget(m_icon);

    m_text = new QLabel(this);
    m_text->setText(NoItemText);
    m_text->setAlignment(Qt::AlignCenter);
    verticalLayout->addWidget(m_text);
}

void PatientBrowserMenuExtendedItem::setPixmap(const QPixmap &pixmap)
{
    m_icon->setPixmap(pixmap);
}

void PatientBrowserMenuExtendedItem::setText(const QString &text)
{
    m_text->setText(text);
}

}
