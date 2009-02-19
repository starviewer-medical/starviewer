/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGNONCLOSEDANGLETOOL_H
#define UDGNONCLOSEDANGLETOOL_H

#include "tool.h"
#include <QPointer>

namespace udg {

class Q2DViewer;
class Volume;
class ImagePlane;
class DrawerLine;
class DrawerPolyline;
class DrawerText;

/**
Tool per calcular angles

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class NonClosedAngleTool : public Tool
{
Q_OBJECT
public:
 ///possibles estats de la tool
    enum { FIRST_POINT_FIXED, SECOND_POINT_FIXED, THIRD_POINT_FIXED, NONE };

    NonClosedAngleTool( QViewer *viewer, QObject *parent = 0 );

    ~NonClosedAngleTool();

    ///funcio manejadora dels events passats.
    void handleEvent( long unsigned eventID );

private slots:

private:
    /*
    *   funcions per canviar d'estat: second point, third point... etc
    *   simular segments com a distancetool.
    */
    /// ens permet anotar el primer punt de la primera línia.
    void annotateFirstPoint();

    /// ens simula la linia que estem dibuixant respecte el punt on està el mouse.
    void simulateLine();

    /// TODO ens simula una 2a línia respecte el punt on està el mouse.
    void simulateMirrorLine();

    ///ens simula el primer segment de l'angle respecte el punt on està el mouse.
    void simulateFirstSegmentOfAngle();

    ///ens simula el segon segment de l'angle respecte el punt on està el mouse.
    void simulateSecondSegmentOfAngle();

    void computeSecondSegmentLimit( double point1[3], double point2[3] );//no s'implementa?

    void drawCircumference();//dibuixem arc?

    void fixFirstSegment();//fixSecondSegment? un per cada linia

    void computeAngle();//canviar punts de referencia per calcular l'angle

    void findInitialDegreeArc();//dibuixem arc?

    ///calcula la correcta posició del caption de l'angle segons els punts de l'angle
    void textPosition( double *p1, double *p2, double *p3, DrawerText *angleText );

private:
    /// Viewer 2D sobre el qual treballem
    Q2DViewer *m_2DViewer;

    ///polilinia principal: es la polilinia que ens marca la forma que hem anat editant.
//     QPointer<DrawerPolyline> m_mainPolyline;

    ///primera línia
    QPointer<DrawerLine> m_firstLine;

    ///segona línia
    QPointer<DrawerLine> m_secondLine;

    ///polilinia de la circumferencia de l'angle.
    QPointer<DrawerPolyline> m_circumferencePolyline;

    ///estat de la tool
    int m_state;

    double m_radius;

    int m_initialDegreeArc;
};

}

#endif
