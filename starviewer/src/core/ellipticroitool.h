/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGELLIPTICROITOOL_H
#define UDGELLIPTICROITOOL_H

#include <QPointer>
#include "tool.h"

namespace udg {

class Q2DViewer;
class DrawerPrimitive;
class DrawerPolygon;
class EllipseOutliner;
class EllipticROIToolRepresentation;

/**
    Tool per calcular l'àrea i la mitjana de grisos d'una el·lipse
*/
class EllipticROITool : public Tool {
Q_OBJECT
public:
    EllipticROITool(QViewer *viewer, QObject *parent = 0);
    ~EllipticROITool();

    void handleEvent(long unsigned eventID);

private:
    /// Estats de la Tool
    enum { Ready, FirstPointFixed };

private slots:
    /// Es connecta al signal que emet l'outliner quan ha acabat
    void outlinerFinished(DrawerPrimitive *primitive);

private:
    /// Viewer 2D sobre el qual treballem
    Q2DViewer *m_2DViewer;
    
    /// Outliner per a dibuixar l'el·lipse
    EllipseOutliner *m_ellipseOutliner;

    /// ToolRepresentation que s'utilitza
    EllipticROIToolRepresentation *m_ellipticROIToolRepresentation;

    /// Primitiva que conté el dibuix de l'el·lipse
    QPointer<DrawerPolygon> m_ellipsePolygon;
};

}

#endif
