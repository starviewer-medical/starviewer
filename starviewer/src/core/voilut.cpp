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

#include "voilut.h"

#include <QColor>

namespace udg {

VoiLut::VoiLut()
{
}

VoiLut::VoiLut(const WindowLevel &windowLevel)
{
    setWindowLevel(windowLevel);
}

VoiLut::VoiLut(const TransferFunction &lut)
    : m_originalLutExplanation(lut.name())
{
    setLut(lut);
}

VoiLut::VoiLut(const TransferFunction &lut, const QString &originalExplanation)
    : m_originalLutExplanation(originalExplanation)
{
    setLut(lut);
}

const WindowLevel& VoiLut::getWindowLevel() const
{
    return m_windowLevel;
}

void VoiLut::setWindowLevel(const WindowLevel &windowLevel)
{
    m_windowLevel = windowLevel;
    m_lut.clear();
    m_lut.setName(windowLevel.getName());
}

const TransferFunction& VoiLut::getLut() const
{
    return m_lut;
}

void VoiLut::setLut(const TransferFunction &lut)
{
    m_lut = lut;
    m_windowLevel.setWidth(lut.keys().last() - lut.keys().first());
    m_windowLevel.setCenter(lut.keys().first() + m_windowLevel.getWidth() / 2.0);
    m_windowLevel.setName(lut.name());
}

const QString& VoiLut::getExplanation() const
{
    return m_lut.name();
}

void VoiLut::setExplanation(const QString &explanation)
{
    m_windowLevel.setName(explanation);
    m_lut.setName(explanation);
}

const QString& VoiLut::getOriginalLutExplanation() const
{
    return m_originalLutExplanation;
}

bool VoiLut::isWindowLevel() const
{
    return m_lut.isEmpty();
}

bool VoiLut::isLut() const
{
    return !m_lut.isEmpty();
}

bool VoiLut::operator ==(const VoiLut &that) const
{
    return this->m_windowLevel == that.m_windowLevel && this->m_lut == that.m_lut;
}

bool VoiLut::operator !=(const VoiLut &that) const
{
    return !(*this == that);
}

VoiLut VoiLut::inverse() const
{
    if (this->isWindowLevel())
    {
        return WindowLevel(-m_windowLevel.getWidth(), m_windowLevel.getCenter(), m_windowLevel.getName());
    }
    else
    {
        TransferFunction inverseLut(m_lut);
        inverseLut.clearColor();
        QList<double> colorKeys = m_lut.colorKeys();

        foreach (double key, colorKeys)
        {
            QColor color = m_lut.getColor(key);
            inverseLut.setColor(key, 1.0 - color.redF(), 1.0 - color.greenF(), 1.0 - color.blueF());
        }

        return VoiLut(inverseLut, m_originalLutExplanation);
    }
}

} // namespace udg
