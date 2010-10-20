/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGEXTRACTIMAGETOOLREPRESENTATION_H
#define UDGEXTRACTIMAGETOOLREPRESENTATION_H

#include "toolrepresentation.h"
#include "volume.h"

#include <QPointer>
#include <vtkImageData.h>

class vtkTransform;

namespace udg {

class DrawerPolyline;
class DrawerImage;

/**
    TODO Documentar
*/
class ExtractImageToolRepresentation : public ToolRepresentation {
Q_OBJECT
public:
    ExtractImageToolRepresentation(Drawer *drawer, QObject *parent = 0);
    ~ExtractImageToolRepresentation();

    /// Se li passa la linia ja dibuixada per un outliner
    void setPolyline(DrawerPolyline *polyline);

    /// Se li passa la imatge extreta
    void setImageData(DrawerImage *image);

    /// Se li passen els parametres necessaris pel calcul
    void setParams(int view, double *origin);

    /// Fa els calculs pertinents i escriu el text
    void calculate();

private:
    /// Crea els handlers
    void createHandlers();

    /// Mapeja els handlers amb els punts corresponents
    QList<double *> mapHandlerToPoint(ToolHandler *toolHandler);

    /// Troba el centre del polígon
    void findPolygonCentroid();

    /// Calcula el nou punt després d'una rotació
    void rotatePoint(double *point);

private slots:
    /// Edita els vèrtexs de les DrawerPrimitive
    void moveAllPoints(double *movement);

    /// Edita la figura: rotació
    void rotateRepresentation(double angle);

private:
    /// Línia que es dibuixa
    QPointer<DrawerPolyline> m_polyline;

    /// Imatge delimitada per la polilínia
    QPointer<DrawerImage> m_image;

    ///Parametres necessaris per la representacio
    int m_view;
    double m_centroid[3];
    double *m_origin;
    vtkTransform *m_transform;
};

}

#endif


