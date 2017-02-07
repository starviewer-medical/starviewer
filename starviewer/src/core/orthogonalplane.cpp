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

#include "orthogonalplane.h"

#include "logging.h"

namespace udg {

OrthogonalPlane::OrthogonalPlane(Plane plane)
 : m_plane(plane)
{
}

void OrthogonalPlane::getXYZIndexes(int &x, int &y, int &z) const
{
    x = getXIndex();
    y = getYIndex();
    z = getZIndex();
}

int OrthogonalPlane::getXIndex() const
{
    switch (m_plane)
    {
        case XYPlane:
            return 0;

        case YZPlane:
            return 1;

        case XZPlane:
            return 0;

        case None:
            throw std::domain_error("x index not defined for non-orthogonal plane");

        default:
            throw std::domain_error(qUtf8Printable(QString("Unexpected plane: %1").arg(m_plane)));
    }
}

int OrthogonalPlane::getYIndex() const
{
    switch (m_plane)
    {
        case XYPlane:
            return 1;

        case YZPlane:
            return 2;

        case XZPlane:
            return 2;

        case None:
            throw std::domain_error("y index not defined for non-orthogonal plane");

        default:
            throw std::domain_error(qUtf8Printable(QString("Unexpected plane: %1").arg(m_plane)));
    }
}

int OrthogonalPlane::getZIndex() const
{
    switch (m_plane)
    {
        case XYPlane:
            return 2;

        case YZPlane:
            return 0;

        case XZPlane:
            return 1;

        case None:
            throw std::domain_error("z index not defined for non-orthogonal plane");

        default:
            throw std::domain_error(qUtf8Printable(QString("Unexpected plane: %1").arg(m_plane)));
    }
}

OrthogonalPlane::operator Plane() const
{
    return m_plane;
}

}
