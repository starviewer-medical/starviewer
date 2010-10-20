/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGANGLETOOL_H
#define UDGANGLETOOL_H

#include "tool.h"
#include <QPointer>

namespace udg {

class Q2DViewer;
class DrawerPolyline;
class DrawerPrimitive;
class PolylineAngleOutliner;
class AngleToolRepresentation;

/**
    Tool per calcular angles
*/
class AngleTool : public Tool {
Q_OBJECT
public:
    AngleTool(QViewer *viewer, QObject *parent = 0);
    ~AngleTool();

    void handleEvent(long unsigned eventID);

private slots:
    /// Indica que l'outliner ha acabat de dibuixar una primitiva
    void outlinerFinished(DrawerPrimitive *primitive);

private:
    /// Viewer 2D sobre el qual treballem
    Q2DViewer *m_2DViewer;

    /// Outliner que s'utilitza per a la línia
    PolylineAngleOutliner *m_polylineAngleOutliner;

    /// ToolRepresentation que s'utilitza
    AngleToolRepresentation *m_angleToolRepresentation;

    /// Línia que es dibuixa
    QPointer<DrawerPolyline> m_polyline;
};

}

#endif
