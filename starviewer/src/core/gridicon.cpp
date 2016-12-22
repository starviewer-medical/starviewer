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

namespace udg {

GridIcon::GridIcon(QWidget *parent) : GridIcon(QString(), parent)
{
}

GridIcon::GridIcon(const QString &iconType, QWidget *parent) : QFrame(parent)
{
    setIconType(iconType);
    this->setFrameShape(QFrame::Shape::StyledPanel);
    this->setFrameShadow(QFrame::Shadow::Plain);
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
    this->setStyleSheet(QString("image: url(:/images/icons/%1.svg);").arg(iconType));
}

}
