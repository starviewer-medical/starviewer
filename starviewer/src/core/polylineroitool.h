/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGPOLYLINEROITOOL_H
#define UDGPOLYLINEROITOOL_H

#include "tool.h"

#include <QPointer>

namespace udg {

class Q2DViewer;
class DrawerPolygon;
class DrawerPrimitive;
class PolylineROIOutliner;
class PolylineROIToolRepresentation;

/**
    Tool per crear ROIs amb una forma lliure determinada per un polígon irregular.
*/
class PolylineROITool : public Tool {
Q_OBJECT
public:
    PolylineROITool(QViewer *viewer, QObject *parent = 0);
    ~PolylineROITool();

    void handleEvent(long unsigned eventID);

private slots:
    /// Indica que l'outliner ha acabat de dibuixar una primitiva
    void outlinerFinished(DrawerPrimitive *primitive);

private:
    /// Viewer 2D sobre el qual treballem
    Q2DViewer *m_2DViewer;

    /// Outliner per dibuixar la ROI poligonal
    PolylineROIOutliner *m_polylineROIOutliner;

    /// ToolRepresentation que s'utilitza
    PolylineROIToolRepresentation *m_polylineROIToolRepresentation;

    /// Polígon que defineix la ROI
    QPointer<DrawerPolygon> m_roiPolygon;
};

}

#endif
