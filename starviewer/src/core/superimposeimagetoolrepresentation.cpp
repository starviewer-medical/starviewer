/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "superimposeimagetoolrepresentation.h"

#include "logging.h"
#include "drawer.h"
#include "drawerimage.h"
#include "q2dviewer.h"
#include "toolhandlerwithoutrepresentation.h"

namespace udg {

SuperimposeImageToolRepresentation::SuperimposeImageToolRepresentation(Drawer *drawer, QObject *parent)
 : ToolRepresentation(drawer, parent)
{
}

SuperimposeImageToolRepresentation::~SuperimposeImageToolRepresentation()
{
    delete m_image;
}

void SuperimposeImageToolRepresentation::setImageData(DrawerImage *image)
{
    m_image = image;
    m_primitivesList << m_image;
}

void SuperimposeImageToolRepresentation::setParams(int view)
{
    m_view = view;
}

void SuperimposeImageToolRepresentation::calculate()
{
    if (m_toolHandlers.size() == 0) // Already created
    {
        this->createHandlers();
    }
}

void SuperimposeImageToolRepresentation::createHandlers()
{
    ToolHandlerWithoutRepresentation *toolHandler = new ToolHandlerWithoutRepresentation(m_primitivesList);

    connect(toolHandler, SIGNAL(moveAllPoints(double *)), this, SLOT(moveAllPoints(double *)));
    connect(toolHandler, SIGNAL(destroy()), this, SLOT(destroy()));

    // HACK de profunditat...
    toolHandler->setParams(-1, 0.0);

    m_toolHandlers << toolHandler;
}

QList<double *> SuperimposeImageToolRepresentation::mapHandlerToPoint(ToolHandler *toolHandler)
{
    QList<double *> list;
    return list;
}

void SuperimposeImageToolRepresentation::moveAllPoints(double *movement)
{
    m_image->move(movement);

    this->refresh();
}

}
