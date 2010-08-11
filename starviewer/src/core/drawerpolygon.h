/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGDRAWERPOLYGON_H
#define UDGDRAWERPOLYGON_H

#include "drawerprimitive.h"

class vtkPolyData;
class vtkPoints;
class vtkCellArray;
class vtkActor2D;
class vtkPolyDataMapper2D;

namespace udg {

/**
 *
 *  Primitiva de polígon per al Drawer
 *
 */
class DrawerPolygon : public DrawerPrimitive {
Q_OBJECT
public:
    DrawerPolygon(QObject *parent = 0);
    ~DrawerPolygon();

    /**
     * Afegim un vèrtex al polígon.
     * @param point[] Punt que defineix el vèrtex del polígon
     */
    void addVertix(double point[3]);
    void addVertix(double x, double y, double z);

    /**
     * Assigna el valor del vèrtex i-éssim del polígon. En aquest cas equival a
     * actualitzar el valor d'aquest punt indicat. Si 'i' està fora de rang,
     * s'afegeix el vèrtex donat al final, tal com si fessim addVertix(point)
     * @param i índex del vèrtex que volem modificar
     * @param point[] coordenades del vèrtex
     */
    void setVertix(int i, double point[3]);
    void setVertix(int i, double x, double y, double z);

    /// Ens retorna l'i-èssim vèrtex del polígon. Si l'índex està fora de rang ens retornarà un array sense inicialitzar
    const double* getVertix(int i);
    
    vtkProp* getAsVtkProp();

    /// Ens retorna el nombre de punts que té el polígon
    int getNumberOfPoints() const;

    /// Calcula la distància que té respecte al punt passat per paràmetre
    double getDistanceToPoint(double *point3D);

    void getBounds(double [6]);

    /// Calcula l'àrea del polígon
    double computeArea(int view, double *spacing = NULL);

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
    /// Llista de punts del polígon
    QList<QVector<double>> m_pointsList;

    /// Estructures de vtk, per construir el polígon
    vtkPolyData *m_vtkPolydata;
    vtkPoints *m_vtkPoints;
    vtkCellArray *m_vtkCellArray;
    vtkActor2D *m_vtkActor;
    vtkPolyDataMapper2D *m_vtkMapper;
};

}

#endif
