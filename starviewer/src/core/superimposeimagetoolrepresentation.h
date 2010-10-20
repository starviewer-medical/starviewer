/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGSUPERIMPOSEIMAGETOOLREPRESENTATION_H
#define UDGSUPERIMPOSEIMAGETOOLREPRESENTATION_H

#include "toolrepresentation.h"
#include "volume.h"

#include <QPointer>
#include <vtkImageData.h>

class vtkTransform;

namespace udg {

class DrawerImage;

/**
    TODO Documentar
*/
class SuperimposeImageToolRepresentation : public ToolRepresentation {
Q_OBJECT
public:
    SuperimposeImageToolRepresentation(Drawer *drawer, QObject *parent = 0);
    ~SuperimposeImageToolRepresentation();

    /// Se li passa la imatge extreta
    void setImageData(DrawerImage *image);

    /// Se li passen els parametres necessaris pel calcul
    void setParams(int view);

    /// Fa els calculs pertinents i escriu el text
    void calculate();

private:
    /// Crea els handlers
    void createHandlers();

    /// Mapeja els handlers amb els punts corresponents
    QList<double *> mapHandlerToPoint(ToolHandler *toolHandler);

private slots:
    /// Edita els vèrtexs de les DrawerPrimitive
    void moveAllPoints(double *movement);

private:
    /// Imatge delimitada per la polilínia
    QPointer<DrawerImage> m_image;

    /// Parametres necessaris per la representacio
    int m_view;
    double m_centroid[3];
    vtkTransform *m_transform;
};

}

#endif
