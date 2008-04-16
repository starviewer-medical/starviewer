/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGANGLETOOL_H
#define UDGANGLETOOL_H

#include "tool.h"

namespace udg {

class Q2DViewer;
class Volume;
class ImagePlane;
class DrawerPolyline;

/**
Tool per calcular angles

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class AngleTool : public Tool
{
Q_OBJECT
public:
 ///possibles estats de la tool
    enum { CENTER_FIXED, FIRST_POINT_FIXED, NONE };
    
    AngleTool( QViewer *viewer, QObject *parent = 0 );

    ~AngleTool();

    ///funcio manejadora dels events passats.
    void handleEvent( long unsigned eventID );

private slots:

private:
    /// ens permet anotar el primer vèrtex de l'angle.
    void annotateFirstPoint();

    ///ens simula el primer segment de l'angle respecte el punt on està el mouse.
    void simulateFirstSegmentOfAngle();
    
    ///ens simula el segon segment de l'angle respecte el punt on està el mouse.
    void simulateSecondSegmentOfAngle();
    
    void computeSecondSegmentLimit( double point1[3], double point2[3] );
    
    void drawCircumference();
    
    void fixFirstSegment();
    
    void computeAngle();
    
    void findInitialDegreeArc();

private:
    /// Viewer 2D sobre el qual treballem
    Q2DViewer *m_2DViewer;

    ///polilinia principal: es la polilinia que ens marca la forma que hem anat editant.
    DrawerPolyline *m_mainPolyline;

    ///polilinia de la circumferencia de l'angle.
    DrawerPolyline *m_circumferencePolyline;
    
    ///estat de la tool
    int m_state;
    
    double m_radius;
    
    int m_initialDegreeArc;
};

}

#endif
