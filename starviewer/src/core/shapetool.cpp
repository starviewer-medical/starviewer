/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "shapetool.h"
#include "pointrepresentation.h"
#include <vtkRenderWindowInteractor.h>
#include <vtkPolyDataMapper2D.h>
#include <vtkPropPicker.h>
#include <vtkActor2D.h>
#include <vtkRenderer.h>
#include <vtkCommand.h>
#include <vtkMath.h>
#include <math.h>
#include <vtkTriangleFilter.h>
#include <vtkPolyLine.h>
#include <vtkPolyDataMapper.h>
#include <vtkCellData.h>
#include <vtkPoints.h>
#include <vtkDiskSource.h>
#include <vtkProperty.h>
#include <vtkProperty2D.h>
#include <vtkCoordinate.h>
#include <vtkCellArray.h>
#include "q2dviewer.h"


namespace udg {

ShapeTool::ShapeTool( Q2DViewer *viewer, QObject *parent, const char *name )
 : Tool()
{
    //la tool no té un comportament concret
    m_behavior = NONE;
    m_viewer   = viewer;
    m_nclicks = 0;
    
    //creació de tots els objectes de la classe
    m_picker   = vtkPropPicker::New();
        
    //fem les connexions necessàries
//     connect( m_viewer, SIGNAL( sliceChanged( int ) ), this , SLOT( drawShapesOfCurrentSlice( int ) ) );
}

ShapeTool::~ShapeTool()
{
    m_picker->Delete();
}

void ShapeTool::handleEvent( unsigned long eventID )
{
    int xy[2];
    double position[4]; 
    switch( eventID )
    {
        case vtkCommand::LeftButtonPressEvent:
            m_viewer->getInteractor()->GetEventPosition( xy );
            m_viewer->computeDisplayToWorld( m_viewer->getRenderer() , xy[0], xy[1], 0, position );
            switch ( m_behavior )
            {
                case POINT:
                    annotatePointRepresentation( position[0], position[1] );
                    break;
                case ELLIPSE:
                    m_nclicks++;
                    annotateEllipseRepresentation( position );
                    break;
                default:
                    break;    
            }
            break;
        
        case vtkCommand::MouseMoveEvent:
//             m_viewer->getInteractor()->GetEventPosition( xy );
//             m_viewer->computeDisplayToWorld( m_viewer->getRenderer() , xy[0], xy[1], 0, position );
//             doSimulation( position );
            break;
            
        case vtkCommand::RightButtonPressEvent:
            break;
            
        case vtkCommand::KeyPressEvent:
            break;
                
        default:
            break;
    }
}

// void ShapeTool::drawEndsOfRepresentationShape()
// {}
//     
// void ShapeTool::hideEndsOfRepresentationShape()
// {}
//     
// void ShapeTool::startAnnotation()
// {}
// 
void ShapeTool::doSimulation( double position[4] )
{
    switch ( m_behavior )
    {
        case ELLIPSE:
            if ( true ) //m_nclicks == 1 ) //ens indica que hem introduït el punt que marca el centre
            {
                double xradius = position[0];
                double yradius = position[1];
                double degInRad;
                
                double posx, posy;
        
                vtkPoints *pts = vtkPoints::New();
                vtkPolyData *pd = vtkPolyData::New();
                vtkCellArray *verts = vtkCellArray::New();
                vtkUnsignedCharArray *colors = vtkUnsignedCharArray::New();
                vtkTriangleFilter *tf = vtkTriangleFilter::New();
                vtkPolyDataMapper2D *mapper = vtkPolyDataMapper2D::New();
                vtkActor2D *actor = vtkActor2D::New();
                
                for (int i=0; i < 360; i++)
                {
                    degInRad = i*vtkMath::DegreesToRadians();
        
                    xradius = fabs(position[0] - m_startPosition[0]);
                    yradius = fabs(position[1] - m_startPosition[1]);
                    
                    posx = ((cos(degInRad)*xradius) + m_startPosition[0]);
                    posy = ((sin(degInRad)*yradius) + m_startPosition[1]);
                    
                    pts->InsertPoint(i, posx, posy, 0);
                    verts->InsertNextCell(i+1);
                    verts->InsertCellPoint(i);
                }
                
                pd->SetPoints(pts);
                pd->SetVerts(verts);
                        
                colors->SetNumberOfComponents(3);
                colors->SetNumberOfTuples(1);
                colors->SetTuple3(2,255, 0, 0);
                pd->GetCellData()->SetScalars(colors);

                tf->SetInput(pd);

                mapper->SetInput(tf->GetOutput());
                
                vtkCoordinate *m_coordinate = vtkCoordinate::New();
                m_coordinate->SetCoordinateSystemToWorld();
                actor->GetPositionCoordinate()->SetCoordinateSystemToWorld();
                actor->GetPosition2Coordinate()->SetCoordinateSystemToWorld();
                mapper->SetTransformCoordinate( m_coordinate );
                
                actor->SetMapper(mapper);
//                 actor->SetPosition(0,0);
                
                cout << "actor: " << *actor << endl;
                
                m_viewer->getRenderer()->AddActor(actor);
                m_viewer->getInteractor()->Render();
                
                pts->Delete();
                pd->Delete();
                verts->Delete();
                colors->Delete();
                tf->Delete();
                mapper->Delete();
                m_coordinate->Delete();
                actor->Delete();
            }
            break;
            
        default:
            break;
    }
}
        
// 
// void ShapeTool::endAnnotation()
// {}
//     
// void ShapeTool::selectRepresentationShape()
// {}
//     
// void ShapeTool::waitingForAction()
// {}

void ShapeTool::annotateEllipseRepresentation( double position[4] )
{
    if ( m_nclicks == 1 )//hem determinat la posició del centre 
    {
        //guardem la posició
        m_startPosition[0] = position[0];
        m_startPosition[1] = position[1];
        m_startPosition[2] = position[2];
        cout << "posicio inicial: " << m_startPosition[0] << " " << m_startPosition[1] << endl;
    }
    else if ( m_nclicks == 2 )
    {
        cout << "entro a click 2 " << endl;
        doSimulation( position );
        m_nclicks = 0;
    }
}
            
void ShapeTool::drawShapesOfCurrentSlice( int slice )
{
    QMultiMap<int, ShapeRepresentation*>::const_iterator i = m_representationShapesMap.constBegin();
    while (i != m_representationShapesMap.constEnd()) 
    {
        i.value()->getActor()->VisibilityOff();
        ++i;
    }
    m_viewer->getInteractor()->Render();
}

void ShapeTool::annotatePointRepresentation( double position[2] )
{
    m_point = new PointRepresentation( 1, 2, 20, position );
    
    //afegim al visor la representació que acabem de crear    
    m_viewer->getRenderer()->AddActor( m_point->getActor() );
    
    //el fem visible
    m_point->visibilityOn();
    
    //obliguem al visor a renderitzar el nou punt
    m_viewer->getInteractor()->Render();
    
    //afegim el PointRepresentation al multimap de la classe, per tal d'enregistrar tots
    //els objectes dibuixats.
    m_representationShapesMap.insert( m_viewer->getSlice(), m_point );    
}

void ShapeTool::annotatePointRepresentation( double x, double y )
{
    double position[2];
    position[0] = x;
    position[1] = y;
    annotatePointRepresentation( position );
}
}
