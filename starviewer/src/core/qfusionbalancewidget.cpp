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

#include "qfusionbalancewidget.h"

#include <QBoxLayout>
#include <QLabel>
#include <QSlider>

namespace udg {

// Length of the slider in pixels.
const int SliderLength = 200;

QFusionBalanceWidget::QFusionBalanceWidget(QWidget *parent)
 : QWidget(parent)
{
    m_slider = new QSlider(this);
    m_slider->setMinimum(0);
    m_slider->setMaximum(100);
    m_slider->setValue(50);
    m_slider->setTickInterval(50);
    m_slider->setTickPosition(QSlider::TicksBelow);

    m_firstVolumeLabel = new QLabel(this);
    m_firstVolumeLabel->setAlignment(Qt::AlignCenter);
    m_firstVolumePercentLabel = new QLabel(this);
    m_firstVolumePercentLabel->setAlignment(Qt::AlignCenter);
    m_firstVolumePercentLabel->setMinimumWidth(m_firstVolumePercentLabel->fontMetrics().width("100%"));
    QVBoxLayout *firstVolumeLayout = new QVBoxLayout();
    firstVolumeLayout->addWidget(m_firstVolumeLabel);
    firstVolumeLayout->addWidget(m_firstVolumePercentLabel);

    m_secondVolumeLabel = new QLabel(this);
    m_secondVolumeLabel->setAlignment(Qt::AlignCenter);
    m_secondVolumePercentLabel = new QLabel(this);
    m_secondVolumePercentLabel->setAlignment(Qt::AlignCenter);
    m_secondVolumePercentLabel->setMinimumWidth(m_secondVolumePercentLabel->fontMetrics().width("100%"));
    QVBoxLayout *secondVolumeLayout = new QVBoxLayout();
    secondVolumeLayout->addWidget(m_secondVolumeLabel);
    secondVolumeLayout->addWidget(m_secondVolumePercentLabel);

    m_layout = new QBoxLayout(QBoxLayout::BottomToTop, this);
    m_layout->addLayout(firstVolumeLayout);
    m_layout->addWidget(m_slider);
    m_layout->addLayout(secondVolumeLayout);
    m_layout->setMargin(5);

    this->setHorizontalOrientation();
    this->updatePercentLabels(50);

    connect(m_slider, SIGNAL(valueChanged(int)), SLOT(updatePercentLabels(int)));
    connect(m_slider, SIGNAL(valueChanged(int)), SIGNAL(balanceChanged(int)));
}

QFusionBalanceWidget::~QFusionBalanceWidget()
{
}

void QFusionBalanceWidget::setFirstVolumeModality(const QString &modality)
{
    m_firstVolumeLabel->setText(modality);
}

void QFusionBalanceWidget::setSecondVolumeModality(const QString &modality)
{
    m_secondVolumeLabel->setText(modality);
}

int QFusionBalanceWidget::getBalance() const
{
    return m_slider->value();
}

void QFusionBalanceWidget::setOrientation(Qt::Orientation orientation)
{
    if (orientation == Qt::Horizontal)
    {
        this->setHorizontalOrientation();
    }
    else
    {
        this->setVerticalOrientation();
    }
}

void QFusionBalanceWidget::setHorizontalOrientation()
{
    m_layout->setDirection(QBoxLayout::LeftToRight);
    m_slider->setOrientation(Qt::Horizontal);
    m_slider->setMinimumSize(SliderLength, 0);
}

void QFusionBalanceWidget::setVerticalOrientation()
{
    m_layout->setDirection(QBoxLayout::BottomToTop);
    m_slider->setOrientation(Qt::Vertical);
    m_slider->setMinimumSize(0, SliderLength);
}

void QFusionBalanceWidget::toggleOrientation()
{
    if (m_slider->orientation() == Qt::Horizontal)
    {
        this->setVerticalOrientation();
    }
    else
    {
        this->setHorizontalOrientation();
    }
}

void QFusionBalanceWidget::setBalance(int balance)
{
    m_slider->setValue(balance);
}

void QFusionBalanceWidget::updatePercentLabels(int balance)
{
    m_firstVolumePercentLabel->setText(QString("%1%").arg(100 - balance));
    m_secondVolumePercentLabel->setText(QString("%2%").arg(balance));
    this->repaint();
}

}
