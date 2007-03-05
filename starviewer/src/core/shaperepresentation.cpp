/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "shaperepresentation.h"
#include <QString>
#include <vtkPolyDataMapper2D.h>
#include <vtkActor2D.h>
#include <vtkProperty2D.h>
#include <vtkCoordinate.h>
#include "shape.h"

namespace udg {

ShapeRepresentation::ShapeRepresentation()
{
    m_shapeActor = vtkActor2D::New();
    m_shapeMapper = vtkPolyDataMapper2D::New();
    m_coordinate = vtkCoordinate::New();
    m_coordinate->SetCoordinateSystemToWorld();/*
    m_shapeActor->GetPositionCoordinate()->SetCoordinateSystemToWorld();
    m_shapeActor->GetPosition2Coordinate()->SetCoordinateSystemToWorld();*/
    m_shapeMapper->SetTransformCoordinate( m_coordinate );
    
    //inicialment les shapes són invisibles
    visibilityOff();
}

ShapeRepresentation::~ShapeRepresentation()
{
    m_shapeActor->Delete();
    m_shapeMapper->Delete();
    m_coordinate->Delete();
}

void ShapeRepresentation::visibilityOff()
{
    m_shapeActor->VisibilityOff();
}

void ShapeRepresentation::visibilityOn()
{
    m_shapeActor->VisibilityOn();
}
///red, green, light_green, blue, light_blue, yellow, orange, purple, gray, black, white
void ShapeRepresentation::setColor( const char* color )
{
    if ( QString(color) == QString("red") )
        m_shapeActor->GetProperty()->SetColor( 1,0,0 );
    else if ( QString(color) == QString("green") )
        m_shapeActor->GetProperty()->SetColor( 0,0.5,0 );
    else if ( QString(color) == QString("blue") )
        m_shapeActor->GetProperty()->SetColor( 0,0,1 );
    else if ( QString(color) == QString("light_green") )
        m_shapeActor->GetProperty()->SetColor( 0,1,0 );
    else if ( QString(color) == QString("light_blue") )
        m_shapeActor->GetProperty()->SetColor( 0,1,1 );
    else if ( QString(color) == QString("yellow") )
        m_shapeActor->GetProperty()->SetColor( 1, 1, 0 );
    else if ( QString(color) == QString("orange") )
        m_shapeActor->GetProperty()->SetColor( 1.0,0.5,0.5 );
    else if ( QString(color) == QString("purple") )
        m_shapeActor->GetProperty()->SetColor( 0.5,0,0.5 );
    else if ( QString(color) == QString("gray") )
        m_shapeActor->GetProperty()->SetColor( 0.5,0.5,0.5 );
    else if ( QString(color) == QString("black") )
        m_shapeActor->GetProperty()->SetColor( 0,0,0 );
    else if ( QString(color) == QString("white") )
        m_shapeActor->GetProperty()->SetColor( 1,1,1 );
    else
        m_shapeActor->GetProperty()->SetColor( 1.0,0.5,0.5 );
}

void ShapeRepresentation::setColor( double r, double g, double b )
{
    m_shapeActor->GetProperty()->SetColor( r,g,b );
}

void ShapeRepresentation::setCoordinateSystemToWorld()
{
    m_coordinate->SetCoordinateSystemToWorld();
    m_shapeMapper->SetTransformCoordinate( m_coordinate );
}  

void ShapeRepresentation::setCoordinateSystemToDisplay()
{
    m_coordinate->SetCoordinateSystemToDisplay();
    m_shapeMapper->SetTransformCoordinate( m_coordinate );
}

}
