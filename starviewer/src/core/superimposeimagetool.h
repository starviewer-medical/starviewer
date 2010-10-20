/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGSUPERIMPOSEIMAGETOOL_H
#define UDGSUPERIMPOSEIMAGETOOL_H

#include "tool.h"

#include <QPointer>

#include <vtkImageFlip.h>

namespace udg {

class Q2DViewer;
class DrawerImage;
class SuperimposeImageToolRepresentation;

/**

	@author Grup de Gràfics de Girona  (GGG) <vismed@ima.udg.es>
*/
class SuperimposeImageTool : public Tool {
Q_OBJECT
public:
    SuperimposeImageTool(QViewer *viewer, QObject *parent = 0);
    ~SuperimposeImageTool();

    void handleEvent(long unsigned eventID);

private:
    /// Crea la imatge
    void superimposeImage();

    /// Fa un flip a la imatge
    void flipImage();

private:
    /// Viewer 2D sobre el qual treballem
    Q2DViewer *m_2DViewer;

    /// SuperImposeImageToolRepresentation
    SuperimposeImageToolRepresentation *m_superimposeImageToolRepresentation;

    /// Primitiva
    QPointer<DrawerImage> m_image;

    /// Fa el Flip horitzontal de la imatge
    vtkImageFlip *m_vtkFlip;

    /// Indica si s'ha acabat la creacio
    bool m_done;
};

}

#endif
