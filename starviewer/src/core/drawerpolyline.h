/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGDRAWERPOLYLINE_H
#define UDGDRAWERPOLYLINE_H

#include "drawerprimitive.h"

class vtkPolyData;
class vtkPoints;
class vtkCellArray;
class vtkActor2D;
class vtkPolyDataMapper2D;

namespace udg {

/**
Primitiva de polilínia pel Drawer

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class DrawerPolyline : public DrawerPrimitive
{
Q_OBJECT
public:
    DrawerPolyline(QObject *parent = 0);

    ~DrawerPolyline();

    /**
     * Afegim un punt a la polilínia.
     * @param point[] Punt que defineix el punt
     */
    void addPoint( double point[3] );

    /**
     * Assigna el valor del punt i-éssim de la polilínia. En aquest cas equival a
     * actualitzar el valor d'aquest punt indicat. Si 'i' està fora de rang,
     * s'afegeix el punt donat al final, tal com si fessim addPoint(point)
     * @param i índex del point que volem modificar
     * @param point[] coordenades del point
     */
    void setPoint( int i, double point[3] );
    
    /**
     * Elimina el punt i-éssim de la polilínia. Si 'i' està fora de rang,
     * no s'esborrarà cap punt de la polilínia.
     * @param i índex del point que volem modificar
     */
    void removePoint( int i );

    ///ens retorna la polilínia com a objecte VTK de representació
    vtkProp *getAsVtkProp();

    ///ens retorna el nombre de punts que te la polilinia
    int getNumberOfPoints();

    ///ens retorna l'i-essim punt de la polilinia en cas de que existeixi.
    double* getPoint( int position );

    ///esborra tots els punts de la polilinia
    void deleteAllPoints();

    /// retorna els valors minims i maxims de les coordenades de la polilinia
    double* getPolylineBounds();

    /// Calcula l'area del poligon
    double computeArea( int view );
    
    ///calcula la distància que té respecte al punt passat per paràmetre
    double getDistanceToPoint( double *point3D );
    
    ///mira si està dins dels límits que marquen els punts
    bool isInsideOfBounds( double p1[3], double p2[3], int view );

public slots:
    void update( int representation );

protected slots:
    void updateVtkProp();

private:
    /// Mètode intern per passar de la llista de punts a l'estructura vtk pertinent
    void buildVtkPoints();

    /// Mètode intern per transformar les propietats de la primitiva a propietats de vtk
    void updateVtkActorProperties();

    /// Mètode que intercanvia els punts de sentit antihorari a horari
    void swap();
    
    ///ens diu  si point es correnspon amb algun dels dos punts que formen un determinat segment de la polilínia
    bool isPointIncludedInLineBounds( double point[3], double *p1, double *p2 );
    
private:
    /// Llista de punts de la polilínia
    QList< double* > m_pointsList;

    /// Estructures de vtk, per construir la polilínia
    vtkPolyData *m_vtkPolydata;
    vtkPoints *m_vtkPoints;
    vtkCellArray *m_vtkCellArray;
    vtkActor2D *m_vtkActor;
    vtkPolyDataMapper2D *m_vtkMapper;
};

}

#endif
