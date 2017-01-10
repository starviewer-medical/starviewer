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

    void setPosition(Vector3 position);

    vtkPropAssembly* getAsVtkPropAssembly();

    virtual vtkProp* getAsVtkProp() override;

    virtual double getDistanceToPointInDisplay(const Vector3 &displayPoint, Vector3 &closestDisplayPoint,
                                               std::function<Vector3(const Vector3&)> worldToDisplay) override;

    virtual std::array<double, 4> getDisplayBounds(std::function<Vector3(const Vector3&)> worldToDisplay) override;

    virtual void setVisibility(bool visible) override;

public slots:
    virtual void update() override;

protected slots:
    virtual void updateVtkProp() override;

private:
    /// Mètode intern per transformar les propietats de la primitiva a propietats de vtk
    void updateVtkActorProperties();

private:
    /// Centre de la crosshair.
    Vector3 m_position;

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
