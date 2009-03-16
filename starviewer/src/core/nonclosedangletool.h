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
    enum { NONE, FIRST_LINE_FIXED, SECOND_LINE_FIXED };

    NonClosedAngleTool( QViewer *viewer, QObject *parent = 0 );

    ~NonClosedAngleTool();

    ///funcio manejadora dels events passats.
    void handleEvent( long unsigned eventID );

private:
    /// ens permet anotar els punts de les línies.
    void annotateLinePoints();

    /// ens simula la linia que estem dibuixant respecte el punt on està el mouse.
    void simulateLine(DrawerLine *line);

    /// Calcula l'angle de les dues línies dibuixades
    void computeAngle();

    ///calcula la correcta posició del caption de l'angle segons els punts de l'angle
    void textPosition( double *p1, double *p2, DrawerText *angleText );

private:
    ///Estats de la línia segons els punts
    enum { NO_POINTS, ONE_POINT };

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

    ///estat d'una línia
    int m_lineState;
};

}

#endif
