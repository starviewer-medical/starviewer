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
    enum { NONE, FIRST_LINE_FIXED };

    NonClosedAngleTool( QViewer *viewer, QObject *parent = 0 );

    ~NonClosedAngleTool();

    ///funcio manejadora dels events passats.
    void handleEvent( long unsigned eventID );

private slots:

private:
    /// ens permet anotar el punts de la primera línia.
    void annotateFirstLinePoints();

    /// ens permet anotar el punts de la segona línia.
    void annotateSecondLinePoints();

    /// ens simula la linia que estem dibuixant respecte el punt on està el mouse.
    void simulateLine();

    /// ens simula la 2a linia que estem dibuixant.
    void simulateSecondLine();

    /// TODO ens simula una 2a línia respecte el punt on està el mouse.
    void simulateMirrorLine();

    void drawCircumference();//dibuixem arc?

    void fixFirstSegment();//fixSecondSegment? un per cada linia

    void computeAngle();//canviar punts de referencia per calcular l'angle

    void findInitialDegreeArc();//dibuixem arc?

    ///calcula la correcta posició del caption de l'angle segons els punts de l'angle
    void textPosition( double *p1, double *p2, DrawerText *angleText );

    ///retorna el valor mínim dels paràmetres
    double minimum(double d1, double d2, double d3, double d4);

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

    bool m_hasFirstPoint;

    bool m_hasSecondPoint;
};

}

#endif
