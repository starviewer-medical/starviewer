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
class DrawerLine;
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

private:
    /// ens permet anotar el punts de la primera línia.
    void annotateFirstLinePoints();

    /// ens permet anotar el punts de la segona línia.
    void annotateSecondLinePoints();

    /// ens simula la linia que estem dibuixant respecte el punt on està el mouse.
    void simulateLine();

    /// ens simula la 2a linia que estem dibuixant.
    void simulateSecondLine();

    /// Calcula l'angle de les dues línies dibuixades
    void computeAngle();

    ///calcula la correcta posició del caption de l'angle segons els punts de l'angle
    void textPosition( double *p1, double *p2, DrawerText *angleText );

    /**
    * retorna el punt d'interseccio de dues línies infinites
    * definides per dos segments
    * @param p1 primer punt de la primera recta
    * @param p2 segon punt de la primera recta
    * @param p3 primer punt de la segona recta
    * @param p4 segon punt de la segona recta
    */
    double *intersectionPoint(double *p1, double *p2, double *p3, double *p4, int &state);

private:
    ///estats de la línia segons la interseccio, no es distingeix entre PARALLEL i COINCIDENT
    enum { PARALLEL, INTERSECT };

    /// Viewer 2D sobre el qual treballem
    Q2DViewer *m_2DViewer;

    ///primera línia
    QPointer<DrawerLine> m_firstLine;

    ///segona línia
    QPointer<DrawerLine> m_secondLine;

    ///línia d'unió
    QPointer<DrawerLine> m_middleLine;

    ///estat de la tool
    int m_state;
    /// per controlar els punts de la línia que s'han dibuixat
    bool m_hasFirstPoint;
    bool m_hasSecondPoint;
};

}

#endif
