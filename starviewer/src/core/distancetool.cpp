/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "distancetool.h"

#include "qviewer.h"
#include <iostream>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkLineSource.h>
#include <vtkActorCollection.h>
#include <vtkSphereSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>

namespace udg {

DistanceTool::DistanceTool( QViewer *viewer, QObject *parent, const char *name)
 : Tool( viewer , parent, name )
{
    m_state = Begin;
    m_viewer = viewer;

    m_distanceActorsCollection = vtkActorCollection::New();
    m_firstPointActor = vtkActor::New();
    m_secondPointActor = vtkActor::New();
    m_distanceLineActor = vtkActor::New();
    
    m_pointMapper = vtkPolyDataMapper::New();
    m_lineMapper = vtkPolyDataMapper::New();
    
    vtkSphereSource *pointSource = vtkSphereSource::New();
    m_pointMapper->SetInputConnection( pointSource->GetOutputPort() );
    
    m_firstPointActor->SetMapper( m_pointMapper );
    m_firstPointActor->GetProperty()->SetColor( 0 , 1 , 0 );
    m_secondPointActor->SetMapper( m_pointMapper );
    m_secondPointActor->GetProperty()->SetColor( 1 , 1 , 0 );
    
    m_lineSource = vtkLineSource::New();
    m_lineMapper->SetInputConnection( m_lineSource->GetOutputPort() );
    
    m_distanceLineActor->SetMapper( m_lineMapper );
    m_distanceLineActor->GetProperty()->SetColor( 0 , 1 , 0 );
    
    m_distanceRenderer =  0;
}

DistanceTool::~DistanceTool()
{
}

void DistanceTool::dispatchEvent( EventIdType event )
{

// estats : Inici, 1punt agafat , 2 punts agafats
// estat inici
// si event leftButtonPressed -> punt candidat , mostrar en pantalla
// ""                       + MouseMove : moure punt candidat, mostrar en pantalla
// ""       ""Released -> 1r punt confirmat, passar 2n estat (1punt agafat)
// estat 1punt agafat
// si MouseMove -> pintar línia desde 1r punt i mostrar anotació distància
// si leftButtonPressed {&& Mouse move }  ídem anterior
// si leftButtonReleased -> 2n punt confirmat , anotar distància , passar estat inicial

    switch( m_state )
    {
    
    case Begin:
        switch( event )
        {
        case LeftButtonDown:
            m_state = FirstPointCandidate;
            m_distance.setFirstPoint( m_viewer->getModelPointFromCursor() );
        break;
        default:
        break;
        }
    break;
    
    case FirstPointCandidate:
        switch( event )
        {
        case LeftButtonUp:
            m_state = FirstPointChosen;
            m_distance.setFirstPoint( m_viewer->getModelPointFromCursor() );
            
        break;
        
        case MouseMove:
            m_distance.setFirstPoint( m_viewer->getModelPointFromCursor() );
        break;
        
        default:
        break;
        }
    break;
    
    case FirstPointChosen:
        switch( event )
        {
        case LeftButtonDown:
            // posem automàtcament el valor del següent que és allà on està el mouse
            m_distance.setSecondPoint( m_viewer->getModelPointFromCursor() );
            m_state = SecondPointCandidate;
            
        break;
        
        case MouseMove:
            m_distance.setSecondPoint( m_viewer->getModelPointFromCursor() );
        break;
        
        default:
        break;
        }
    break;
    
    case SecondPointCandidate:
        switch( event )
        {
        case LeftButtonUp:
            m_distance.setSecondPoint( m_viewer->getModelPointFromCursor() );
            m_state = Begin;
            
        break;
        
        case MouseMove:
            // mentre es mogui el mouse, el punt anirà variant
            m_distance.setSecondPoint( m_viewer->getModelPointFromCursor() );
        break;
        
        default:
        break;
        }
    break;
    
    }

     drawDistance();    
}

void DistanceTool::drawDistance()
{
// de moment només guardem una distància, però hauria d'estar preparat per totes les distàncies
// aquesta característica ara està en un mètode però podria quedar separat en una altre classe independent
    switch( m_state )
    {
    case Begin:
    // nothing to do
    break;
         
    case FirstPointCandidate:        
            m_firstPointActor->SetPosition( m_distance.getFirstPoint().getX() , m_distance.getFirstPoint().getY(), m_distance.getFirstPoint().getZ() );
    
            m_secondPointActor->SetPosition( m_distance.getFirstPoint().getX() , m_distance.getFirstPoint().getY(), m_distance.getFirstPoint().getZ() );
            
            m_lineSource->SetPoint1( m_distance.getFirstPoint().getX() , m_distance.getFirstPoint().getY(), m_distance.getFirstPoint().getZ() );
            
            m_lineSource->SetPoint2( m_distance.getFirstPoint().getX() , m_distance.getFirstPoint().getY(), m_distance.getFirstPoint().getZ() );
            
            m_distanceRenderer->AddActor( m_firstPointActor );
    break;
    
    case FirstPointChosen:
        m_firstPointActor->SetPosition( m_distance.getFirstPoint().getX() , m_distance.getFirstPoint().getY(), m_distance.getFirstPoint().getZ() );
        
        m_lineSource->SetPoint1( m_distance.getFirstPoint().getX() , m_distance.getFirstPoint().getY(), m_distance.getFirstPoint().getZ() );
        
        m_lineSource->SetPoint2( m_distance.getSecondPoint().getX() , m_distance.getSecondPoint().getY(), m_distance.getSecondPoint().getZ() );
        
        m_distanceRenderer->AddActor( m_distanceLineActor );
        
        m_distanceRenderer->AddActor( m_secondPointActor );
    break;
    
    case SecondPointCandidate:
            
            m_secondPointActor->SetPosition( m_distance.getSecondPoint().getX() , m_distance.getSecondPoint().getY(), m_distance.getSecondPoint().getZ() );
            
            m_lineSource->SetPoint2( m_distance.getSecondPoint().getX() , m_distance.getSecondPoint().getY(), m_distance.getSecondPoint().getZ() );
                    
    break;
    
    }
}

};  // end namespace udg 
