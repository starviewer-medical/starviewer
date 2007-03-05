/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "distancetool.h"
#include "q2dviewer.h"
#include <vtkCommand.h>
#include <distance.h>
#include <vtkRenderer.h>
#include <vtkAlgorithmOutput.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkActor2DCollection.h>
#include <vtkDiskSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataMapper2D.h>
#include <vtkLineSource.h>
#include <vtkActor2D.h>
#include <vtkCaptionActor2D.h>
#include <vtkProperty.h>
#include <vtkProperty2D.h>
#include <vtkTextProperty.h>
#include <vtkPropPicker.h>
#include <math.h>
#include <QList>

namespace udg {

    DistanceTool::DistanceTool( Q2DViewer *viewer , QObject *, const char * )
    {
        m_state             = NONE;
        m_2DViewer          = viewer;
        m_firstPointLocated = false;  
        m_somePropFocused   = false;
        m_somePropSelected  = false; 
    
        //creació de tots els objectes de la classe
        m_distanceLine      = vtkLineSource::New();
        m_lineActor         = vtkActor2D::New();
        m_textDistanceActor = vtkCaptionActor2D::New();
        m_lineMapper        = vtkPolyDataMapper2D::New();
        m_picker            = vtkPropPicker::New();
        m_disk1             = vtkDiskSource::New();
        m_disk2             = vtkDiskSource::New();
        m_diskMapper1       = vtkPolyDataMapper2D::New();
        m_diskActor1        = vtkActor2D::New();
        m_diskMapper2       = vtkPolyDataMapper2D::New();
        m_diskActor2        = vtkActor2D::New();
        
        //assignem propietats als objectes
        m_lineActor->GetProperty()->SetColor(1.0,0.5,0.5);
        m_lineActor->GetProperty()->SetLineWidth(2);
        m_distanceLine->SetResolution (2);
        
        m_textDistanceActor->GetAttachmentPointCoordinate()->SetCoordinateSystemToWorld();
        m_textDistanceActor->BorderOff();
        m_textDistanceActor->LeaderOff();
        m_textDistanceActor->ThreeDimensionalLeaderOff();
        m_textDistanceActor->GetCaptionTextProperty()->SetColor( 1.0, 0.0, 0.0 );
        m_textDistanceActor->SetPadding( 5 );
        m_textDistanceActor->SetPosition( -1.0 , -1.0 );
        m_textDistanceActor->SetHeight( 0.01 );
        m_textDistanceActor->SetWidth( 0.09 );
        m_textDistanceActor->GetCaptionTextProperty()->ShadowOff();
        m_textDistanceActor->GetCaptionTextProperty()->ItalicOff();
        
        vtkCoordinate *coordinate = vtkCoordinate::New();
        coordinate->SetCoordinateSystemToWorld();
        m_lineMapper->SetTransformCoordinate( coordinate );
        m_diskMapper1->SetTransformCoordinate( coordinate );
        m_diskMapper2->SetTransformCoordinate( coordinate );
        
        m_interactor = m_2DViewer->getInteractor();
        
        //creem els extrems de la distància
        m_disk1->SetInnerRadius( 1 );
        m_disk1->SetOuterRadius ( 2 );
        m_disk1->SetCircumferentialResolution ( 20 );
        m_disk2->SetInnerRadius( 1 );
        m_disk2->SetOuterRadius ( 2 );
        m_disk2->SetCircumferentialResolution ( 20 );
        m_diskMapper1->SetInput( m_disk1->GetOutput() );
        m_diskActor1->SetMapper( m_diskMapper1 );
        m_diskActor1->VisibilityOff();
        m_diskMapper2->SetInput( m_disk2->GetOutput() );
        m_diskActor2->SetMapper( m_diskMapper2 );
        m_diskActor2->VisibilityOff();
    
        //afegim els punts a l'escena. No es veuran ja que estan sense visibilitat  
        m_2DViewer->getRenderer()-> AddActor( m_diskActor1 );
        m_2DViewer->getRenderer()-> AddActor( m_diskActor2 );
        
        //fem les connexions necessàries
        connect( m_2DViewer, SIGNAL( sliceChanged( int ) ), this , SLOT( drawDistancesOfSlice( int ) ) );
        
        coordinate->Delete();
    }
    
    DistanceTool::~DistanceTool()
    {
        m_distanceLine     ->Delete();
        m_lineMapper       ->Delete();
        m_lineActor        ->Delete();  
        m_picker           ->Delete();
        m_disk1            ->Delete();
        m_diskMapper1      ->Delete();
        m_diskActor1       ->Delete();
        m_disk2            ->Delete();
        m_diskMapper2      ->Delete();
        m_diskActor2       ->Delete();
        m_textDistanceActor->Delete();
    }
    
    void DistanceTool::handleEvent( unsigned long eventID )
    {   
        switch( eventID )
        {
            //click amb el botó esquerre: anotació de distància
            case vtkCommand::LeftButtonPressEvent:
                //anotem distància si no hi ha cap distància seleccionada, altrament
                //el que volem és interactuar amb la distància
                if (!m_somePropSelected)
                {
                    if ( !m_firstPointLocated )
                        this->startDistanceAnnotation();
                    else 
                        this->endDistanceAnnotation();
                }
                else
                {
                    if ( m_state == MOVINGPOINT )
                        this->updateSelectedPoint();
                    else
                        this->giveMeTheNearestPoint();
                }
                break;
        
            case vtkCommand::MouseMoveEvent:
                //al moure el mouse, simulem la distància que s'anotarà, dibuixant una línia.
                if( m_state == ANNOTATING )
                    this->doDistanceSimulation();
                else if ( m_state == NONE )
                    this->waitingForAction();
                else if ( m_state == MOVINGPOINT )
                    this->moveSelectedPoint();
                break;
            
            //click amb el botó dret: volem que si l'element clickat és una distància, quedi seleccionat.
            case vtkCommand::RightButtonPressEvent:
                if( m_state == NONE )
                    this->selectDistance();
                break;
            
            case vtkCommand::KeyPressEvent:
//                cout << "he polsat una tecla " << m_2DViewer->getInteractor()->GetKeyCode () << endl;
                break;
                
            default:
                break;
        }
    }
    
    void DistanceTool::startDistanceAnnotation()
    {
        int xy[2];
        double position[4];  
        
        m_state = ANNOTATING;
        
        //ens assegurem de que la línia d'informació no és visible
        m_lineActor->VisibilityOff();
        
        m_2DViewer->getInteractor()->GetEventPosition( xy );
        m_2DViewer->computeDisplayToWorld( m_2DViewer->getRenderer() , xy[0], xy[1], 0, position );
        
        //només ens interessen els 3 primers valors de l'array de 4
        m_distanceStartPosition[0] = position[0];
        m_distanceStartPosition[1] = position[1];
        m_distanceStartPosition[2] = position[2];
           
        //assignem el primer extrem a la línia de distància
        m_distanceLine->SetPoint1( m_distanceStartPosition );
        
        m_lineMapper->SetInput(m_distanceLine->GetOutput());
        m_lineActor->SetMapper(m_lineMapper);
                
        //deixem constància de que ja hem anotat el primer punt
        m_firstPointLocated = true;
        
        m_2DViewer->getRenderer()-> AddActor(m_lineActor);
    }
    
    void DistanceTool::doDistanceSimulation()
    {
        int xy[2];
        double position[4];
        
        m_2DViewer->getInteractor()->GetEventPosition (xy);
        m_2DViewer->computeDisplayToWorld( m_2DViewer->getRenderer() , xy[0], xy[1], 0, position );
        
        //només ens interessen els 3 primers valors de l'array de 4
        m_distanceCurrentPosition[0] = position[0];
        m_distanceCurrentPosition[1] = position[1];
        m_distanceCurrentPosition[2] = position[2];
        
        //assignem el segon extrem a la línia de distància
        m_distanceLine->SetPoint2( m_distanceCurrentPosition );
        
        m_lineActor->VisibilityOn();
        m_2DViewer->getInteractor()->Render();
    }
    
    void DistanceTool::endDistanceAnnotation()
    {
        //creem la línia final entre els dos punts
        vtkLineSource *m_distanceLineAux     = vtkLineSource::New();
        vtkActor2D *m_lineActorAux           = vtkActor2D::New();
        vtkPolyDataMapper2D *m_lineMapperAux = vtkPolyDataMapper2D::New(); 
        
        //assignem el sistema de coordenades al mapper
        vtkCoordinate *coordinate = vtkCoordinate::New();
        coordinate->SetCoordinateSystemToWorld();
        m_lineMapperAux->SetTransformCoordinate( coordinate );
        
        m_lineActorAux->GetProperty()->SetColor( 0.93,0.53,0.0 );
        m_lineActorAux->GetProperty()->SetLineWidth( 2 );
        m_distanceLineAux->SetResolution ( 2 );
        
        m_distanceLineAux->SetPoint1( m_distanceStartPosition );
        m_distanceLineAux->SetPoint2( m_distanceCurrentPosition );
        
//         cout << "punt 1 original: " << m_distanceLineAux->GetPoint1()[0] << " " << m_distanceLineAux->GetPoint1()[1] <<  " " << m_distanceLineAux->GetPoint1()[2] <<endl;
//         cout << "punt 2 original: " << m_distanceLineAux->GetPoint2()[0] << " " << m_distanceLineAux->GetPoint2()[1] << " " << m_distanceLineAux->GetPoint2()[2] <<endl;
//         
        //calculem la distància que s'ha mesurat    
        Distance d(m_distanceStartPosition, m_distanceCurrentPosition);
        
        QString str = QString( "%1 mm" ).arg( d.getDistance2D(), 0, 'f',  2);    
        
        //Assignem el text a l'etiqueta de la distància i la situem
        m_textDistanceActor ->SetCaption( qPrintable ( str ) );
        m_textDistanceActor->SetAttachmentPoint( m_distanceCurrentPosition );
        
        m_lineMapperAux->SetInput(m_distanceLineAux->GetOutput());
        m_lineActorAux->SetMapper(m_lineMapperAux);
        
        //afegim la distància anotada al mapa de distàncies
        m_distancesMap.insert( m_2DViewer->getSlice(), m_lineActorAux );
        
        m_2DViewer->getRenderer()->AddActor( m_lineActorAux );
        m_2DViewer->getRenderer()->AddActor( m_textDistanceActor );
        m_2DViewer->getInteractor()->Render();
        
        m_distanceLineAux  ->Delete();
        m_lineActorAux     ->Delete();
        m_lineMapperAux    ->Delete(); 
        coordinate         ->Delete();   
                
        //determinem que estem en l'estat NONE, és a dir, no estem anotant cap distància.            
        m_state = NONE;
        
        //deixem constància de que ja hem anotat la distància.
        m_firstPointLocated = false;
    }
    
    Distance* DistanceTool::getDistanceFromActor2D( vtkActor2D *actor )
    {
        vtkPolyDataMapper2D *auxMap = vtkPolyDataMapper2D::SafeDownCast( actor->GetMapper() );
        vtkLineSource *ls = vtkLineSource::SafeDownCast( auxMap->GetInput()->GetProducerPort()->GetProducer() );
        
        return ( new Distance( ls->GetPoint1(), ls->GetPoint2() ) );
    }
    
    void DistanceTool::selectDistance()
    {
        int state = m_picker->PickProp( m_interactor->GetEventPosition()[0], m_interactor->GetEventPosition()[1], m_2DViewer->getRenderer() );
        vtkProp* pickedProp =  m_picker->GetViewProp();
        if ( state != 0 && pickedProp->IsA("vtkActor2D"))
        {
            if ( !m_somePropSelected )
            {
                m_selectedActor = (vtkActor2D*)pickedProp;
                m_selectedActor->GetProperty()->SetColor(0,0,1);
                
                vtkPolyDataMapper2D *auxMap = vtkPolyDataMapper2D::SafeDownCast( m_selectedActor->GetMapper() );
                vtkLineSource *ls = vtkLineSource::SafeDownCast( auxMap->GetInput()->GetProducerPort()->GetProducer() );
                
                drawEndsOfSelectedDistance( ls->GetPoint1(), ls->GetPoint2() );
                //creem una còpia de la distància seleccionada, ja que no sabem perquè no ens deixa treballar directament amb la seleccionada.
                m_selectedDistance = vtkLineSource::New();
        
                //Donem les mateixes propietats a la còpia que les de l'original
                m_selectedDistance->SetPoint1( ls->GetPoint1() );
                m_selectedDistance->SetPoint2( ls->GetPoint2() );
        
                m_lineMapper->SetInput( m_selectedDistance->GetOutput() );
                m_selectedActor->SetMapper( m_lineMapper );
                
                /*m_selectedActor->GetMapper()->SetInputConnection(m_selectedDistance->GetOutputPort());
                m_selectedActor->GetMapper()->GetInput()->Update();
                */m_2DViewer->getInteractor()->Render();
                
                //esborrem l'original per no tenir dos objectes duplicats
//                  ls->Delete();
                
                m_somePropSelected = true;
            }
            else
            {
                //l'última distància seleccionada i que, pertant hem canviat de color, 
                //la tornem a deixar en l'estat inicial.
                m_selectedActor->GetProperty()->SetColor(0.93,0.53,0.0);
                hideEndsOfSelectedDistance();
                m_somePropSelected = false;
            }
        }
    }
    
    void DistanceTool::waitingForAction()
    {
        //li diem al picker que miri quin actor hi ha a la posició on hi ha hagut l'event del mouse (botó dret)
        int state = m_picker->PickProp( m_interactor->GetEventPosition()[0], m_interactor->GetEventPosition()[1], m_2DViewer->getRenderer() );
        vtkProp* pickedProp =  m_picker->GetViewProp();  
        if ( state != 0 )
        {
            if ( pickedProp->IsA( "vtkActor2D" ) && !m_somePropSelected && !m_somePropFocused )
            {
                m_focusedActor = vtkActor2D::SafeDownCast( pickedProp );
                m_focusedActor->GetProperty()->SetColor( 0,0,1 );
            
                vtkPolyDataMapper2D *auxMap = vtkPolyDataMapper2D::SafeDownCast( m_focusedActor->GetMapper() );
                
                vtkLineSource *ls = vtkLineSource::SafeDownCast( auxMap->GetInput()->GetProducerPort()->GetProducer() );
            
                drawEndsOfSelectedDistance( ls->GetPoint1(), ls->GetPoint2() );
                m_somePropFocused = true;
            }
            else
            {
                //cas en què ja no estem sobre de la distància que hem passat per sobre i no està seleccionada.
                if ( m_somePropFocused && !m_somePropSelected)
                {
                    int numberOfItems = m_2DViewer->getRenderer()->GetActors()->GetNumberOfItems();
                    
                    //l'última distància que ha rebut focus i que, pertant hem canviat de color, 
                    //la tornem a deixar en l'estat inicial, no té focus.
                    m_focusedActor->GetProperty()->SetColor( 0.93,0.53,0.0 );
                    hideEndsOfSelectedDistance();
                    m_somePropFocused = false;
                }
            }
        }
    }

    void DistanceTool::drawEndsOfSelectedDistance( double *point1, double *point2 )
    {
        //només dibuixem els extrems de la distància si aquesta no està seleccionada amb el botó dret,
        //ja que si està seleccionada ja es dibuixen sempre
        if (!m_somePropSelected)
        {
            cout << "punt 1disk: " << point1[0] << " " << point1[1] << " " << point1[2]<<endl;
            cout << "punt 2disk: " << point2[0] << " " << point2[1] << " " << point2[2] <<endl;
            
            //situem els extrems de la distància
            m_diskActor1->SetPosition( point1 );
            m_diskActor2->SetPosition( point2 );
            m_diskActor1->VisibilityOn();
            m_diskActor2->VisibilityOn();
            m_diskActor1->GetProperty()->SetColor( 0.93,0.53,0.0 );
            m_diskActor2->GetProperty()->SetColor( 0.93,0.53,0.0 );
            m_2DViewer->getInteractor()->Render();
        }
    }

    void DistanceTool::hideEndsOfSelectedDistance()
    {
        //només fem invisibles els extrems de la distància si aquesta no està seleccionada amb el botó dret,
        //ja que si està seleccionada ja es s'oculten sempre
        if (!m_somePropSelected)
        {
            m_diskActor1->VisibilityOff();
            m_diskActor2->VisibilityOff();
            m_2DViewer->getInteractor()->Render();
        }
    }
    
    void DistanceTool::giveMeTheNearestPoint()
    {
        int state = m_picker->PickProp( m_interactor->GetEventPosition()[0], m_interactor->GetEventPosition()[1], m_2DViewer->getRenderer() );
        if ( state != 0 )
        {
            double clickPosition[3];
            
            m_2DViewer->getCurrentCursorPosition(clickPosition);
            
            Distance d1(m_diskActor1->GetPosition(), clickPosition);
            Distance d2(m_diskActor2->GetPosition(), clickPosition);
            
            if (d1.getDistance2D() < d2.getDistance2D())
            {
                m_selectedDiskActor = m_diskActor1;
                m_pointOfDistance = 1;
            }
            else
            {
                m_selectedDiskActor = m_diskActor2;
                m_pointOfDistance = 2;
            }
            
            m_textDistanceActor->VisibilityOff();
            m_state = MOVINGPOINT;
        }
    }
    
    void DistanceTool::moveSelectedPoint()
    {   
       m_2DViewer->getCurrentCursorPosition( m_distanceCurrentPosition );
        
        if (m_pointOfDistance == 1)
            m_selectedDistance->SetPoint1( m_distanceCurrentPosition );
        else
            m_selectedDistance->SetPoint2( m_distanceCurrentPosition );
                    
        m_selectedDiskActor->SetPosition( m_distanceCurrentPosition );
                
        m_selectedActor->VisibilityOn();
        
        m_2DViewer->getInteractor()->Render();
    }
    
    void DistanceTool::updateSelectedPoint()
    {
        double clickPosition[3];
                
        m_state = NONE;
        
        m_2DViewer->getCurrentCursorPosition(clickPosition);
        
        Distance d;
        
        if (m_pointOfDistance == 1)
            d.setPoints( m_selectedDistance->GetPoint2() , clickPosition );
        else
            d.setPoints( m_selectedDistance->GetPoint1() , clickPosition );
        
        QString str = QString("%1 mm").arg(d.getDistance2D(), 0, 'f', 2);    
        
        //Assignem el text a l'etiqueta de la distància i la situem
        m_textDistanceActor ->SetCaption( qPrintable ( str ));
        m_textDistanceActor->SetAttachmentPoint( clickPosition );
//         m_2DViewer->getRenderer()->AddActor(m_selectedActor);
        
        m_textDistanceActor->VisibilityOn();
        
        m_2DViewer->getInteractor()->Render();
    }
    
    void DistanceTool::drawDistancesOfSlice( int slice )
    {
        int position;
        vtkActor2DCollection *col = m_2DViewer->getRenderer()->GetActors2D(); 
        
//         cout << "number of items: " << col->GetNumberOfItems()<<endl;
        
        //Al canviar de llesca, fem invisibles tots els actors de distàncies
        QMultiMap<int, vtkActor2D*>::const_iterator i = m_distancesMap.constBegin();
        while (i != m_distancesMap.constEnd()) {
//             cout << "tipus d'objecte: " << i.value()->GetClassName()<< endl;
            position = col->IsItemPresent( i.value() );
            if ( position != 0)
                i.value()->VisibilityOff();
//                 col->RemoveItem( position );
            ++i;
        }
        
//         cout << "number of items: " << col->GetNumberOfItems()<<endl;
         m_2DViewer->getInteractor()->Render();
// //         m_2DViewer->getRenderer()->RemoveAllProps();
//         
//         QList<vtkActor2D*> list = m_distancesMap.values( slice );
//         
//         cout << "a la llesca " << slice << " hi han " << list.count() << " actors de distancies" << endl; 
// //         if ( list.count() > 0 )
// //         {
// //             foreach ( vtkActor2D *actor, list )
// //             {
// // //                 m_distanceLine->SetPoint1( d->getFirstPoint() );
// // //                 m_distanceLine->SetPoint2( d->getSecondPoint() );
// // //                 m_lineMapper->SetInput( m_distanceLine->GetOutput() );
// // //                 m_lineActor->SetMapper( m_lineMapper );
// //                 m_2DViewer->getRenderer()->AddActor( actor );
// //             }
//             m_2DViewer->getInteractor()->Render();
//         }
    }
}


