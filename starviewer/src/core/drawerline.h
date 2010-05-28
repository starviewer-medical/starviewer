/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGDRAWERLINE_H
#define UDGDRAWERLINE_H

#include "drawerprimitive.h"

class vtkLineSource;
class vtkActor2D;
class vtkPolyDataMapper2D;

namespace udg {

/**
 *
 *  Primitiva de línia per al Drawer
 *
 */
class DrawerLine : public DrawerPrimitive {
Q_OBJECT
public:
    DrawerLine(QObject *parent = 0);
    ~DrawerLine();

    /// Afegim el primer punt de la línia
    void setFirstPoint(double point[3]);
    void setFirstPoint(double x, double y, double z);

    /// Afegim el segon punt de la línia
    void setSecondPoint(double point[3]);
    void setSecondPoint(double x, double y, double z);

    vtkProp *getAsVtkProp();

    /// Retorna el primer punt de la línia
    double *getFirstPoint(){return m_firstPoint;};

    /// Retorna el segon punt de la línia
    double *getSecondPoint(){return m_secondPoint;};

    /// Calcula la distancia entre punt i punt
    double computeDistance(double * spacing = NULL);

    /// Calcula la distància que té respecte al punt passat per paràmetre
    double getDistanceToPoint(double *point3D);

    void getBounds(double bounds[6]);

public slots:

    void update();

protected slots:
    void updateVtkProp();

private:
    /// Mètode intern per transformar les propietats de la primitiva a propietats de vtk
    void updateVtkActorProperties();

private:
    /// Primer punt de la línia
    double m_firstPoint[3];

    /// Segon punt de la línia
    double m_secondPoint[3];

    /// Estructures de vtk, per construir la línia
    vtkLineSource *m_vtkLineSource;
    vtkActor2D *m_vtkActor;
    vtkPolyDataMapper2D *m_vtkMapper;
};

}

#endif
