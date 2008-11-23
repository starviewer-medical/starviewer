/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGDRAWERPOINT_H
#define UDGDRAWERPOINT_H

#include "drawerprimitive.h"
//Qt's
#include <QVector>

class vtkSphereSource;
class vtkActor;
class vtkPolyDataMapper;

namespace udg {

/**
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class DrawerPoint : public DrawerPrimitive
{
Q_OBJECT
public:
    DrawerPoint(QObject *parent = 0);

    ~DrawerPoint();

    ///Afegim el primer punt de la línia
    void setPosition( double point[3] );
    void setPosition( QVector<double> point );

    vtkProp *getAsVtkProp();

    ///mira si està dins dels límits que marquen els punts
    bool isInsideOfBounds( double p1[3], double p2[3], int view );

    ///calcula la distància que té respecte al punt passat per paràmetre
    double getDistanceToPoint( double *point3D );

public slots:

    void update( int representation );

protected slots:
    void updateVtkProp();

private:
    /// Mètode intern per transformar les propietats de la primitiva a propietats de vtk
    void updateVtkActorProperties();

private:
    /// Estructures de vtk, per construir el punt
        /// Posició del punt
    double m_position[3];
    double m_sphereRadius;

        ///Actor que dibuixa la llavor
    vtkActor *m_pointActor;
        ///esfera que representa en punt de la llavor
    vtkSphereSource *m_pointSphere;
        ///mapejador del punt
    vtkPolyDataMapper *m_pointMapper;

};

}

#endif
