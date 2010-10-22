/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "drawer.h"
#include "q2dviewer.h"
#include "drawerprimitive.h"
#include "representationslayer.h"

// vtk
#include <vtkRenderer.h>

namespace udg {

Drawer::Drawer(Q2DViewer *viewer, QObject *parent)
 : QObject(parent)
{
    m_2DViewer = viewer;
}

Drawer::~Drawer()
{
}

void Drawer::draw(DrawerPrimitive *primitive)
{
    vtkProp *prop = primitive->getAsVtkProp();
    if(prop)
    {
        connect(primitive, SIGNAL(dying(DrawerPrimitive *)), SLOT(erasePrimitive(DrawerPrimitive *)));
        m_2DViewer->getRenderer()->AddViewProp(prop);
        updateRenderer();
    }
}

void Drawer::updateRenderer()
{
    m_2DViewer->render();
}

void Drawer::erasePrimitive(DrawerPrimitive *primitive)
{
    if (!primitive)
    {
        return;
    }
    // Elimina les referencies a la primitiva
    m_2DViewer->getRepresentationsLayer()->removePrimitive(primitive);
    m_2DViewer->getRenderer()->RemoveViewProp(primitive->getAsVtkProp());
    // TODO Fer-ho explícitament després o fer-ho dins del propi mètode removePrimitive()?
    m_2DViewer->render();
}

}
