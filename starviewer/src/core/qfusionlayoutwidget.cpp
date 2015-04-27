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

#include "qfusionlayoutwidget.h"

namespace udg {

QFusionLayoutWidget::QFusionLayoutWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUi(this);

    connect(m_item3x1, &ItemMenu::isSelected, this, &QFusionLayoutWidget::layout3x1Requested);
    connect(m_item3x3, &ItemMenu::isSelected, this, &QFusionLayoutWidget::layout3x3Requested);
}

QFusionLayoutWidget::~QFusionLayoutWidget()
{
}

} // namespace udg
