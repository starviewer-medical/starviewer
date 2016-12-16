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

#ifndef UDGDRAWERCROSSHAIR_H
#define UDGDRAWERCROSSHAIR_H

#include "drawerprimitive.h"
#include "drawerline.h"

// Forward declarations
class vtkPropAssembly;

namespace udg {

/**
    Primitiva que dibuixa un "Crosshair", és a dir, una creueta per situar un punt.
  */
class DrawerCrossHair : public DrawerPrimitive {
Q_OBJECT
public:
    DrawerCrossHair(QObject *parent = 0);
    ~DrawerCrossHair();

    /// Afegim el primer punt de la línia
    void setCentrePoint(double x, double y, double z);

    vtkPropAssembly* getAsVtkPropAssembly();

    vtkProp* getAsVtkProp();

    double getDistanceToPoint(const Vector3 &point3D, Vector3 &closestPoint);

    void getBounds(double bounds[6]);

    void setVisibility(bool visible);

public slots:
    void update();

protected slots:
    void updateVtkProp();

private:
    /// Mètode intern per transformar les propietats de la primitiva a propietats de vtk
    void updateVtkActorProperties();

private:
    /// Centre de la crosshair.
    Vector3 m_centrePoint;

    /// Línies per construir el crosshair
    DrawerLine *m_lineUp;
    DrawerLine *m_lineDown;
    DrawerLine *m_lineLeft;
    DrawerLine *m_lineRight;
    DrawerLine *m_lineFront;
    DrawerLine *m_lineBack;

    vtkPropAssembly *m_vtkPropAssembly;
};

}

#endif
