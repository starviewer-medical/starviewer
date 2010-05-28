/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGDRAWERCROSSHAIR_H
#define UDGDRAWERCROSSHAIR_H

#include "drawerprimitive.h"
#include "drawerline.h"

// Forward declarations
class vtkPropAssembly;

namespace udg {

/**
 *
 *  Primitiva que dibuixa un "Crosshair", és a dir, una creueta per situar un punt.
 *
 */
class DrawerCrossHair : public DrawerPrimitive {
Q_OBJECT
public:
    DrawerCrossHair(QObject *parent = 0);
    ~DrawerCrossHair();

    /// Afegim el primer punt de la línia
    void setCentrePoint(double x, double y, double z);

    vtkPropAssembly *getAsVtkPropAssembly();

    vtkProp *getAsVtkProp();

    double getDistanceToPoint(double *point3D);

    void getBounds(double bounds[6]);

    void setVisibility(bool visible);

public slots:

    void update();

protected slots:
    void updateVtkProp();

private:
    /// Mètode intern per transformar les propietats de la primitiva a propietats de vtk
    void updateVtkActorProperties();

private:
    /// Centre de la crosshair.
    double m_centrePoint[3];

    /// Línies per construir el crosshair
    DrawerLine * m_lineUp;
    DrawerLine * m_lineDown;
    DrawerLine * m_lineLeft;
    DrawerLine * m_lineRight;
    DrawerLine * m_lineFront;
    DrawerLine * m_lineBack;

    vtkPropAssembly *m_vtkPropAssembly;
};

}

#endif
