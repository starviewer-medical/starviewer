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

#ifndef UDGDRAWERPOLYGON_H
#define UDGDRAWERPOLYGON_H

#include "drawerprimitive.h"
#include "q2dviewer.h"
#include "line3d.h"

class vtkPolyData;
class vtkPoints;
class vtkCellArray;
class vtkActor2D;
class vtkPolyDataMapper2D;
class vtkPropAssembly;

namespace udg {

/**
    Primitiva de polígon per al Drawer
  */
class DrawerPolygon : public DrawerPrimitive {
Q_OBJECT
public:
    DrawerPolygon(QObject *parent = 0);
    ~DrawerPolygon();

    /// Afegim un vèrtex al polígon.
    /// @param point[] Punt que defineix el vèrtex del polígon
    void addVertex(const Vector3 &point);

    /// Buida la llista de vèrtexs
    void removeVertices();

    /// Assigna el valor del vèrtex i-éssim del polígon. En aquest cas equival a
    /// actualitzar el valor d'aquest punt indicat. Si 'i' està fora de rang,
    /// s'afegeix el vèrtex donat al final, tal com si fessim addVertix(point)
    /// @param i índex del vèrtex que volem modificar
    /// @param point[] coordenades del vèrtex
    void setVertex(int i, const Vector3 &point);

    /// Ens retorna l'i-èssim vèrtex del polígon. Si l'índex està fora de rang ens retornarà un array sense inicialitzar
    Vector3 getVertex(int i) const;

    /// Returns the segments of the polygon
    QList<Line3D> getSegments();
    
    virtual vtkProp* getAsVtkProp() override;

    /// Ens retorna el nombre de punts que té el polígon
    int getNumberOfPoints() const;

    virtual double getDistanceToPointInDisplay(const Vector3 &displayPoint, Vector3 &closestDisplayPoint,
                                               std::function<Vector3(const Vector3&)> worldToDisplay) override;

    virtual std::array<double, 4> getDisplayBounds(std::function<Vector3(const Vector3&)> worldToDisplay) override;

    /// Returns the 2D coordinate indices corresponding to the plane where the polygon is lying on.
    /// If the plane could not be determined, -1, -1 will be returned
    void get2DPlaneIndices(int &x, int &y) const;

    /// Returns the vtkPolyData that represents this polygon.
    vtkPolyData* getVtkPolyData() const;

public slots:
    virtual void update() override;

protected slots:
    virtual void updateVtkProp() override;

private:

    /// Mètode intern per construir la pipeline de VTK.
    void buildVtkPipeline();
    /// Mètode intern per passar de la llista de punts a l'estructura vtk pertinent
    void buildVtkPoints();
    /// Mètode intern per transformar les propietats de la primitiva a propietats de vtk
    void updateVtkActorProperties();

private:
    /// Llista de punts del polígon
    QList<Vector3> m_pointsList;
    /// Indica si els punts han canviat des de l'última vegada que s'ha actualitzat la representació de VTK.
    bool m_pointsChanged;

    /// Estructures de vtk, per construir el polígon
    vtkPolyData *m_vtkPolyData;
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
