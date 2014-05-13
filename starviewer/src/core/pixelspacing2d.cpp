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

#include "pixelspacing2d.h"

#include <QString>

namespace udg {

PixelSpacing2D::PixelSpacing2D()
{
    m_x = 0.0;
    m_y = 0.0;
}

PixelSpacing2D::PixelSpacing2D(double x, double y)
{
    setX(x);
    setY(y);
}

PixelSpacing2D::~PixelSpacing2D()
{
}

void PixelSpacing2D::setX(double x)
{
    m_x = x;
}

void PixelSpacing2D::setY(double y)
{
    m_y = y;
}

double PixelSpacing2D::x() const
{
    return m_x;
}

double PixelSpacing2D::y() const
{
    return m_y;
}

bool PixelSpacing2D::isValid() const
{
    if (m_x <= 0.0 || m_y <= 0.0)
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool PixelSpacing2D::isEqual(const PixelSpacing2D &spacing, int decimalPrecision) const
{
    if (decimalPrecision < 0)
    {
        return m_x == spacing.x() && m_y == spacing.y();
    }
    else
    {
        QString thisXString = QString::number(m_x, 'f', decimalPrecision);
        QString thisYString = QString::number(m_y, 'f', decimalPrecision);
        QString paramXString = QString::number(spacing.x(), 'f', decimalPrecision);
        QString paramYString = QString::number(spacing.y(), 'f', decimalPrecision);
        
        return  thisXString == paramXString && thisYString == paramYString;
    }
}

} // namespace udg
