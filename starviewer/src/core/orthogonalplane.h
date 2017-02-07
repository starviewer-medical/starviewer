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

#ifndef ORTHOGONALPLANE_H
#define ORTHOGONALPLANE_H

namespace udg {

/**
    Class that defines an orthogonal plane.
 */
class OrthogonalPlane {

public:
    /// Definition of the 3 orthogonal planes and the exceptional case None.
    enum Plane { XYPlane = 2, YZPlane = 0, XZPlane = 1, None = -1 };

    OrthogonalPlane(Plane plane = XYPlane);

    /// Returns the indices of the coordinate axes corresponding to this orthogonal plane.
    void getXYZIndexes(int &x, int &y, int &z) const;
    int getXIndex() const;
    int getYIndex() const;
    int getZIndex() const;

    /// Conversion operator: allows to use an OrthogonalPlane in a switch.
    operator Plane() const;

private:
    Plane m_plane;

};

}

#endif // ORTHOGONALPLANE_H
