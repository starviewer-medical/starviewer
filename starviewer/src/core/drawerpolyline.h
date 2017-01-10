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

#ifndef UDGDRAWERPOLYLINE_H
#define UDGDRAWERPOLYLINE_H

#include "drawerprimitive.h"

class vtkPolyData;
class vtkPoints;
class vtkCellArray;
class vtkActor2D;
class vtkPolyDataMapper2D;
class vtkPropAssembly;

namespace udg {

/**
    Primitiva de polilínia pel Drawer
  */
class DrawerPolyline : public DrawerPrimitive {
Q_OBJECT
public:
    DrawerPolyline(QObject *parent = 0);
    ~DrawerPolyline();

    /// Afegim un punt a la polilínia.
    void addPoint(const Vector3 &point);

    /// Assigna el valor del punt i-éssim de la polilínia. En aquest cas equival a
    /// actualitzar el valor d'aquest punt indicat. Si 'i' està fora de rang,
    /// s'afegeix el punt donat al final, tal com si fessim addPoint(point)
    /// @param i índex del point que volem modificar
    void setPoint(int i, const Vector3 &point);

    /// Elimina el punt i-éssim de la polilínia. Si 'i' està fora de rang,
    /// no s'esborrarà cap punt de la polilínia.
    /// @param i índex del point que volem modificar
    void removePoint(int i);

    /// Retorna la polilínia com a objecte VTK de representació
    virtual vtkProp* getAsVtkProp() override;

    /// Retorna el nombre de punts que te la polilinia
    int getNumberOfPoints();

    /// Retorna l'i-essim punt de la polilinia en cas de que existeixi.
    Vector3 getPoint(int position) const;

    /// Esborra tots els punts de la polilinia
    void deleteAllPoints();

    virtual double getDistanceToPointInDisplay(const Vector3 &displayPoint, Vector3 &closestDisplayPoint,
                                               std::function<Vector3(const Vector3&)> worldToDisplay) override;

    virtual std::array<double, 4> getDisplayBounds(std::function<Vector3(const Vector3&)> worldToDisplay) override;

    /// Retorna la llista de punts de la polilínia
    QList<Vector3> getPointsList() const;

public slots:
    virtual void update() override;

protected slots:
    virtual void updateVtkProp() override;

private:
    /// Mètode intern per passar de la llista de punts a l'estructura vtk pertinent
    void buildVtkPoints();

    /// Mètode intern per transformar les propietats de la primitiva a propietats de vtk
    void updateVtkActorProperties();

private:
    /// Llista de punts de la polilínia
    QList<Vector3> m_pointsList;

    /// Estructures de vtk, per construir la polilínia
    vtkPolyData *m_vtkPolydata;
    vtkPoints *m_vtkPoints;
    vtkCellArray *m_vtkCellArray;
    vtkActor2D *m_vtkActor;
    vtkActor2D *m_vtkBackgroundActor;
    vtkPolyDataMapper2D *m_vtkMapper;

    /// vtkProp per agrupar l'actor i el background per simular el contorn
    vtkPropAssembly *m_vtkPropAssembly;
};

}

#endif
