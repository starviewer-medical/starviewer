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

#ifndef UDGDRAWERPOINT_H
#define UDGDRAWERPOINT_H

#include "drawerprimitive.h"

class vtkSphereSource;
class vtkActor;
class vtkPolyDataMapper;

namespace udg {

/**
    Primitiva de punt
  */
class DrawerPoint : public DrawerPrimitive {
Q_OBJECT
public:
    DrawerPoint(QObject *parent = 0);
    ~DrawerPoint();

    /// Assignem el punt
    void setPosition(Vector3 point);

    /// Li assignem el radi a la circumferència que defineix el punt
    void setRadius(double radius);

    vtkProp* getAsVtkProp();

    void getBounds(double bounds[6]);

    /// Calcula la distància que té respecte al punt passat per paràmetre
    double getDistanceToPoint(const Vector3 &point3D, Vector3 &closestPoint);

public slots:
    void update();

protected slots:
    void updateVtkProp();

private:
    /// Mètode intern per transformar les propietats de la primitiva a propietats de vtk
    void updateVtkActorProperties();

private:
    /// Posició del punt
    Vector3 m_position;

    /// Radi de la circumferència amb la que dibuixem el punt
    double m_pointRadius;

    /// Actor que dibuixa la llavor
    vtkActor *m_pointActor;

    /// Esfera que representa en punt de la llavor
    vtkSphereSource *m_pointSphere;

    /// Mapejador del punt
    vtkPolyDataMapper *m_pointMapper;
};

}

#endif
