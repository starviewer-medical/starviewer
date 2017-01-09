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

#include "applicationstylehelper.h"
#include <QApplication>
#include <QFont>
#include "mathtools.h"

namespace udg {

double ApplicationStyleHelper::m_systemFontSize = 0;

ApplicationStyleHelper::ApplicationStyleHelper(bool applyScaleFactor)
{
    m_applyScaleFactor = applyScaleFactor;
}

int ApplicationStyleHelper::getScaledFontSize(double multiplier) const
{
    if (m_systemFontSize == 0) {
        m_systemFontSize = QApplication::font().pointSizeF();
    }

    double fontSize = m_systemFontSize;
    fontSize *= multiplier;
    if (m_applyScaleFactor)
    {
        fontSize *= ((QGuiApplication*)(QApplication::instance()))->devicePixelRatio();
    }
    return MathTools::roundToNearestInteger(fontSize);
}

int ApplicationStyleHelper::getToolsFontSize() const
{
    return getScaledFontSize(1.125);
}

int ApplicationStyleHelper::getCornerAnnotationFontSize() const
{
    return getScaledFontSize(1.125);
}

int ApplicationStyleHelper::getOrientationAnnotationFontSize() const
{
    return getScaledFontSize(1.25);
}

int ApplicationStyleHelper::getDefaultFontSize() const
{
    return getScaledFontSize(1);
}


} // End namespace udg
