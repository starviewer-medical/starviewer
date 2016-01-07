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

#include "qcolorspinbox.h"

namespace udg {

QColorSpinBox::QColorSpinBox(QWidget *parent)
 : QSpinBox(parent)
{
    m_settingColor = false;

    setMaximum(255);
    setColor(QColor(255, 255, 255, 0));

    connect(this, SIGNAL(valueChanged(int)), SLOT(setOpacity(int)));
}

QColorSpinBox::~QColorSpinBox()
{
}

const QColor& QColorSpinBox::getColor() const
{
    return m_color;
}

void QColorSpinBox::setColor(const QColor &color)
{
    m_settingColor = true;
    m_color = color;
    QString foreground = QString(";color:") + (color.value() < 128 ? "white" : "black");
    this->setStyleSheet(QString("background-color:") + color.name() + foreground);
    this->setValue(color.alpha());
    emit colorChanged(m_color);
    m_settingColor = false;
}

void QColorSpinBox::setOpacity(int opacity)
{
    m_color.setAlpha(opacity);
    if (!m_settingColor)
    {
        emit colorChanged(m_color);
    }
}

}
