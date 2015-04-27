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

#include "gridicon.h"
#include <QGridLayout>
#include <QLabel>
#include <QPixmap>
#include <QResizeEvent>

#include "logging.h"

namespace udg {

GridIcon::GridIcon(QWidget *parent, const QString &iconType)
 : QFrame(parent)
{
    QGridLayout *gridLayout = new QGridLayout(this);
    gridLayout->setSpacing(0);
    gridLayout->setMargin(1);
    m_label = new QLabel(this);
    m_label->setFrameShape(QFrame::StyledPanel);
    QPixmap pixmap;

    QString path;
    if (!iconType.isEmpty())
    {
        path = QString::fromUtf8(":/images/") + iconType + ".png";
    }
    else
    {
        path = QString::fromUtf8(":/images/axial.png");
    }
    
    pixmap.load(path);

    m_label->setPixmap(pixmap);
    gridLayout->addWidget(m_label, 0, 0, 1, 1);
}

GridIcon::~GridIcon()
{
}

void GridIcon::resizeEvent(QResizeEvent *event)
{
    QFrame::resizeEvent(event);

    m_label->setPixmap(m_label->pixmap()->scaled(event->size().width() - 2, event->size().height() - 2, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    m_label->setAlignment(Qt::AlignCenter);
}

}
