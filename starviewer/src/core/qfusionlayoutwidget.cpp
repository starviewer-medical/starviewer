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

#include "anatomicalplane.h"

namespace udg {

QFusionLayoutWidget::QFusionLayoutWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUi(this);

    m_gridIcon2x3_1_1->setIconType("axial");
    m_gridIcon2x3_1_2->setIconType("axial");
    m_gridIcon2x3_1_3->setIconType("coronal");
    m_gridIcon2x3_1_4->setIconType("coronal");
    m_gridIcon2x3_1_5->setIconType("sagital");
    m_gridIcon2x3_1_6->setIconType("sagital");

    m_gridIcon2x3_2_1->setIconType("axial");
    m_gridIcon2x3_2_2->setIconType("axial");
    m_gridIcon2x3_2_3->setIconType("coronal");
    m_gridIcon2x3_2_4->setIconType("coronal");
    m_gridIcon2x3_2_5->setIconType("sagital");
    m_gridIcon2x3_2_6->setIconType("sagital");

    m_gridIcon3x3_1->setIconType("axial");
    m_gridIcon3x3_2->setIconType("axial");
    m_gridIcon3x3_3->setIconType("axial");
    m_gridIcon3x3_4->setIconType("coronal");
    m_gridIcon3x3_5->setIconType("coronal");
    m_gridIcon3x3_6->setIconType("coronal");
    m_gridIcon3x3_7->setIconType("sagital");
    m_gridIcon3x3_8->setIconType("sagital");
    m_gridIcon3x3_9->setIconType("sagital");

    connect(m_item2x1First, &ItemMenu::isSelected, this, &QFusionLayoutWidget::layout2x1FirstRequested);
    connect(m_item2x1Second, &ItemMenu::isSelected, this, &QFusionLayoutWidget::layout2x1SecondRequested);
    connect(m_item3x1, &ItemMenu::isSelected, this, &QFusionLayoutWidget::layout3x1Requested);
    connect(m_item2x3First, &ItemMenu::isSelected, this, &QFusionLayoutWidget::layout2x3FirstRequested);
    connect(m_item2x3Second, &ItemMenu::isSelected, this, &QFusionLayoutWidget::layout2x3SecondRequested);
    connect(m_item3x3, &ItemMenu::isSelected, this, &QFusionLayoutWidget::layout3x3Requested);
}

QFusionLayoutWidget::~QFusionLayoutWidget()
{
}

void QFusionLayoutWidget::setModalities(const QString &modality1, const QString &modality2)
{
    m_label2x1First->setText(tr("2x1 %1").arg(modality1));
    m_label2x1Second->setText(tr("2x1 %1").arg(modality2));
    m_label2x3First->setText(tr("2x3 %1").arg(modality1));
    m_label2x3Second->setText(tr("2x3 %1").arg(modality2));
}

void QFusionLayoutWidget::setCurrentAnatomicalPlane(const AnatomicalPlane &anatomicalPlane)
{
    QString iconType;

    switch (anatomicalPlane)
    {
        case AnatomicalPlane::Axial:
            iconType = "axial";
            break;
        case AnatomicalPlane::Coronal:
            iconType = "coronal";
            break;
        case AnatomicalPlane::Sagittal:
            iconType = "sagital";
            break;
        default:
            // Unsupported plane: get out of here
            return;
    }

    m_gridIcon2x1_1_1->setIconType(iconType);
    m_gridIcon2x1_1_2->setIconType(iconType);

    m_gridIcon2x1_2_1->setIconType(iconType);
    m_gridIcon2x1_2_2->setIconType(iconType);

    m_gridIcon3x1_1->setIconType(iconType);
    m_gridIcon3x1_2->setIconType(iconType);
    m_gridIcon3x1_3->setIconType(iconType);
}

} // namespace udg
