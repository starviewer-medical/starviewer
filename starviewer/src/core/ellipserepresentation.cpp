/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "ellipserepresentation.h"
#include "ellipse.h"
#include "point.h"
#include <vtkDiskSource.h>
#include <vtkPolyDataMapper2D.h>
#include <vtkActor2D.h>

namespace udg {

    EllipseRepresentation::EllipseRepresentation()
    : ShapeRepresentation()
    {
//         m_point = vtkDiskSource::New();
//         
//     //valors per defecte dels radis i resolució
//         m_point->SetInnerRadius( 2 );
//         m_point->SetOuterRadius ( 3 );
//         m_point->SetCircumferentialResolution ( 20 );
//     
//         m_shapeMapper->SetInput( m_point->GetOutput() );
//         m_shapeActor->SetMapper( m_shapeMapper );
//     
//         m_ellipseShape = new Point();
//     
//         setColor( "blue" );
    }

//     EllipseRepresentation::EllipseRepresentation( double inner, double outer, int resolution, double position[2] )
//     : ShapeRepresentation()
//     {
//         m_point = vtkDiskSource::New();
//         
//     //valors per defecte dels radis i resolució
//         m_point->SetInnerRadius( inner );
//         m_point->SetOuterRadius ( outer );
//         m_point->SetCircumferentialResolution ( resolution );
//     
//         m_shapeActor->SetPosition( position );
//     
//         m_shapeMapper->SetInput( m_point->GetOutput() );
//         m_shapeActor->SetMapper( m_shapeMapper );
//     
//         m_ellipseShape = new Point( position );
//     
//         setColor( "blue" );
//     }

    EllipseRepresentation::~EllipseRepresentation()
    {
//         m_point->Delete();
//         delete m_ellipseShape;
    }

    void EllipseRepresentation::setMinorRadius( Point p )
    {
        m_ellipseShape->setMinorRadius( p );
    }

    void EllipseRepresentation::setMinorRadius( double pos[2] )
    {
        m_ellipseShape->setMinorRadius( Point( pos ) );
    }

    void EllipseRepresentation::setMajorRadius( Point p )
    {
        m_ellipseShape->setMajorRadius( p );
    }
    
    void EllipseRepresentation::setMajorRadius( double pos[2] )
    {
        m_ellipseShape->setMajorRadius( Point( pos ) );
    }

    void EllipseRepresentation::setCenter( double pos[2] )
    {
        m_ellipseShape->setCenter( Point( pos ) );
    }
    
    void EllipseRepresentation::setCenter( Point pos )
    {
        m_ellipseShape->setCenter( Point( pos ) );
    }

}
