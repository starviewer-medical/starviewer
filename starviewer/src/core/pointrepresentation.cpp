/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "pointrepresentation.h"
#include "point.h"
#include <vtkDiskSource.h>
#include <vtkPolyDataMapper2D.h>
#include <vtkActor2D.h>

namespace udg {

PointRepresentation::PointRepresentation()
 : ShapeRepresentation()
{
    m_point = vtkDiskSource::New();
        
    //valors per defecte dels radis i resolució
    m_point->SetInnerRadius( 2 );
    m_point->SetOuterRadius ( 3 );
    m_point->SetCircumferentialResolution ( 20 );
    
    m_shapeMapper->SetInput( m_point->GetOutput() );
    m_shapeActor->SetMapper( m_shapeMapper );
    
    m_pointShape = new Point();
    
    setColor( "blue" );
}

PointRepresentation::PointRepresentation( double inner, double outer, int resolution, double position[2] )
    : ShapeRepresentation()
{
    m_point = vtkDiskSource::New();
        
    //valors per defecte dels radis i resolució
    m_point->SetInnerRadius( inner );
    m_point->SetOuterRadius ( outer );
    m_point->SetCircumferentialResolution ( resolution );
    
    m_shapeActor->SetPosition( position );
    
    m_shapeMapper->SetInput( m_point->GetOutput() );
    m_shapeActor->SetMapper( m_shapeMapper );
    
    m_pointShape = new Point( position );
    
    setColor( "blue" );
}

PointRepresentation::~PointRepresentation()
{
    m_point->Delete();
    delete m_pointShape;
}

void PointRepresentation::setInnerRadius( double r )
{
    m_point->SetInnerRadius( r );
}

void PointRepresentation::setOuterRadius ( double r )
{
    m_point->SetOuterRadius ( r );
}

void PointRepresentation::setCircumferentialResolution ( int r )
{
    m_point->SetCircumferentialResolution ( r );
}

void PointRepresentation::setPosition( double pos[2] )
{
    m_pointShape->setValues( pos );
    m_shapeActor->SetPosition( pos );
}

}
