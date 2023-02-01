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
#include <QVector>

class vtkPolyData;
class vtkPoints;
class vtkCellArray;
class vtkActor2D;
class vtkPropAssembly;

namespace udg {

class StipplingVtkOpenGLPolyDataMapper2D;

/**
    Primitiva de polilínia pel Drawer
  */
class DrawerPolyline : public DrawerPrimitive {
Q_OBJECT
public:
    DrawerPolyline(QObject *parent = 0);
    ~DrawerPolyline();

    /// Afegim un punt a la polilínia.
    /// @param point[] Punt que defineix el punt
    void addPoint(double point[3]);

    /// Assigna el valor del punt i-éssim de la polilínia. En aquest cas equival a
    /// actualitzar el valor d'aquest punt indicat. Si 'i' està fora de rang,
    /// s'afegeix el punt donat al final, tal com si fessim addPoint(point)
    /// @param i índex del point que volem modificar
    /// @param point[] coordenades del point
    void setPoint(int i, double point[3]);

    /// Elimina el punt i-éssim de la polilínia. Si 'i' està fora de rang,
    /// no s'esborrarà cap punt de la polilínia.
    /// @param i índex del point que volem modificar
    void removePoint(int i);

    /// Retorna la polilínia com a objecte VTK de representació
    vtkProp* getAsVtkProp();

    /// Retorna el nombre de punts que te la polilinia
    int getNumberOfPoints();

    /// Retorna l'i-essim punt de la polilinia en cas de que existeixi.
    double* getPoint(int position);

    /// Esborra tots els punts de la polilinia
    void deleteAllPoints();

    /// Calcula la distància que té respecte al punt passat per paràmetre
    double getDistanceToPoint(double *point3D, double closestPoint[3]);

    void getBounds(double bounds[6]);

    /// Retorna la llista de punts de la polilínia
    QList<QVector<double> > getPointsList();

public slots:
    void update();

protected slots:
    void updateVtkProp();

private:
    /// Mètode intern per passar de la llista de punts a l'estructura vtk pertinent
    void buildVtkPoints();

    /// Mètode intern per transformar les propietats de la primitiva a propietats de vtk
    void updateVtkActorProperties();

private:
    /// Llista de punts de la polilínia
    QList<QVector<double> > m_pointsList;

    /// Estructures de vtk, per construir la polilínia
    vtkPolyData *m_vtkPolydata;
    vtkPoints *m_vtkPoints;
    vtkCellArray *m_vtkCellArray;
    vtkActor2D *m_vtkActor;
    vtkActor2D *m_vtkBackgroundActor;
    StipplingVtkOpenGLPolyDataMapper2D *m_vtkMapper;

    /// vtkProp per agrupar l'actor i el background per simular el contorn
    vtkPropAssembly *m_vtkPropAssembly;
};

}

#endif
