/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGEXTRACTIMAGETOOL_H
#define UDGEXTRACTIMAGETOOL_H

#include "tool.h"

#include <QPointer>

namespace udg {

class Q2DViewer;
class DrawerPolyline;
class DrawerPrimitive;
class DrawerImage;
class PolylineROIOutliner;
class ExtractImageToolRepresentation;

/**
    Tool per calcular l'àrea i la mitjana de grisos d'un polígon

    @author Grup de Gràfics de Girona  (GGG) <vismed@ima.udg.es>
*/
class ExtractImageTool : public Tool {
Q_OBJECT
public:
    ExtractImageTool(QViewer *viewer, QObject *parent = 0);
    ~ExtractImageTool();

    void handleEvent(long unsigned eventID);

private:
    /// Retalla la imatge que s'ha de veure dins la polilínia
    void extractImage();

private slots:
    /// Indica que l'outliner ha acabat de dibuixar una primitiva
    void outlinerFinished(DrawerPrimitive *primitive);

private:
    /// Viewer 2D sobre el qual treballem
    Q2DViewer *m_2DViewer;

    /// Outliner que s'utilitza per a la línia
    PolylineROIOutliner *m_polylineROIOutliner;

    /// ToolRepresentation que s'utilitza
    ExtractImageToolRepresentation *m_extractImageToolRepresentation;

    /// Línia que es dibuixa
    QPointer<DrawerPolyline> m_polyline;

    /// Línia que es dibuixa
    QPointer<DrawerImage> m_image;
};

}

#endif
