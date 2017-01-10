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

#ifndef UDGDRAWERLINE_H
#define UDGDRAWERLINE_H

#include "drawerprimitive.h"

class vtkLineSource;
class vtkActor2D;
class vtkPolyDataMapper2D;
class vtkPropAssembly;

namespace udg {

/**
    Primitiva de línia per al Drawer
  */
class DrawerLine : public DrawerPrimitive {
Q_OBJECT
public:
    DrawerLine(QObject *parent = 0);
    ~DrawerLine();

    /// Afegim el primer punt de la línia
    void setFirstPoint(Vector3 point);

    /// Afegim el segon punt de la línia
    void setSecondPoint(Vector3 point);

    virtual vtkProp* getAsVtkProp() override;

    /// Retorna el primer punt de la línia
    const Vector3& getFirstPoint() const;

    /// Retorna el segon punt de la línia
    const Vector3& getSecondPoint() const;

    virtual double getDistanceToPointInDisplay(const Vector3 &displayPoint, Vector3 &closestDisplayPoint,
                                               std::function<Vector3(const Vector3&)> worldToDisplay) override;

    virtual std::array<double, 4> getDisplayBounds(std::function<Vector3(const Vector3&)> worldToDisplay) override;

public slots:
    virtual void update() override;

protected slots:
    virtual void updateVtkProp() override;

private:
    /// Mètode intern per transformar les propietats de la primitiva a propietats de vtk
    void updateVtkActorProperties();

private:
    /// Primer punt de la línia
    Vector3 m_firstPoint;

    /// Segon punt de la línia
    Vector3 m_secondPoint;

    /// Estructures de vtk, per construir la línia
    vtkLineSource *m_vtkLineSource;
    vtkActor2D *m_vtkActor;
    vtkActor2D *m_vtkBackgroundActor;
    vtkPolyDataMapper2D *m_vtkMapper;

    /// vtkProp per agrupar l'actor i el background per simular el contorn
    vtkPropAssembly *m_vtkPropAssembly;
};

}

#endif
