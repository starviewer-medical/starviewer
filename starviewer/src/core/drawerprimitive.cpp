/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "drawerprimitive.h"

namespace udg {

DrawerPrimitive::DrawerPrimitive(QObject *parent)
 : QObject(parent), m_isVisible(true), m_coordinateSystem(WorldCoordinateSystem), m_color(Qt::green)
{
}

DrawerPrimitive::~DrawerPrimitive()
{
}

void DrawerPrimitive::setVisibility( bool visible )
{
    m_isVisible = visible;
}

void DrawerPrimitive::visibilityOn()
{
    setVisibility( true );
}

void DrawerPrimitive::visibilityOff()
{
    setVisibility( false );
}

bool DrawerPrimitive::isVisible() const
{
    return m_isVisible;
}

void DrawerPrimitive::setCoordinateSystem( int system )
{
    m_coordinateSystem = system;
}

int DrawerPrimitive::getCoordinateSystem() const
{
    return m_coordinateSystem;
}

void DrawerPrimitive::setColor( QColor color )
{
    m_color = color;
}

QColor DrawerPrimitive::getColor() const
{
    return m_color;
}

vtkProp *DrawerPrimitive::getAsVtkProp()
{
}

}
