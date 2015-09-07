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

GridIcon::GridIcon(QWidget *parent)
    : QFrame(parent)
{
    initialize();
}

GridIcon::GridIcon(const QString &iconType, QWidget *parent)
    : QFrame(parent)
{
    initialize(iconType);
}

GridIcon::~GridIcon()
{
}

void GridIcon::setIconType(QString iconType)
{
    if (iconType.isEmpty())
    {
        iconType = "axial";
    }

    setScaledPixmap(QPixmap(QString(":/images/%1.png").arg(iconType)));
}

void GridIcon::resizeEvent(QResizeEvent *event)
{
    QFrame::resizeEvent(event);

    setScaledPixmap(*m_label->pixmap());
}

void GridIcon::initialize(const QString &iconType)
{
    QGridLayout *gridLayout = new QGridLayout(this);
    gridLayout->setSpacing(0);
    gridLayout->setMargin(1);
    m_label = new QLabel(this);
    m_label->setStyleSheet("QLabel { border: 1px solid #909090; }");
    m_label->setAlignment(Qt::AlignCenter);
    setIconType(iconType);
    gridLayout->addWidget(m_label, 0, 0, 1, 1);
}

void GridIcon::setScaledPixmap(const QPixmap &pixmap)
{
    m_label->setPixmap(pixmap.scaled(this->width() - 2, this->height() - 2, Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

}
