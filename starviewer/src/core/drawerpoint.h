/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGDRAWERPOINT_H
#define UDGDRAWERPOINT_H

#include "drawerprimitive.h"
// Qt's
#include <QVector>

class vtkSphereSource;
class vtkActor;
class vtkPolyDataMapper;

namespace udg {

/**
 *
 *  Primitiva de punt
 *
 */
class DrawerPoint : public DrawerPrimitive {
Q_OBJECT
public:
    DrawerPoint(QObject *parent = 0);
    ~DrawerPoint();

    /// Assignem el punt
    void setPosition(double point[3]);
    void setPosition(QVector<double> point);

    /// Li assignem el radi a la circumferència que defineix el punt
    void setRadius(double radius);

    vtkProp *getAsVtkProp();

    void getBounds(double bounds[6]);

    /// Calcula la distància que té respecte al punt passat per paràmetre
    double getDistanceToPoint(double *point3D);

public slots:

    void update();

protected slots:
    void updateVtkProp();

private:
    /// Mètode intern per transformar les propietats de la primitiva a propietats de vtk
    void updateVtkActorProperties();

private:
    /// Posició del punt
    double m_position[3];

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
