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
#include <vtkLine.h>
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
#include <vtkPropAssembly.h>
#include <vtkProperty.h>
#include <vtkProperty2D.h>
#include <vtkTextProperty.h>
#include <vtkPropPicker.h>
#include <math.h>
#include <QList>

namespace udg {

DistanceTool::DistanceTool( Q2DViewer *viewer , QObject *, const char * )
{
    m_state = NONE;
    m_2DViewer = viewer;
    m_firstPointLocated = false;
    m_selectedAssembly = NULL;
    m_somePropSelected = false;
    m_nearestProp = NULL;

    //creació de tots els objectes de la classe

    m_distanceLine = vtkLineSource::New();
    m_lineActor = vtkActor2D::New();

    m_lineMapper = vtkPolyDataMapper2D::New();

    m_axialPicker = vtkPropPicker::New();
    m_coronalPicker = vtkPropPicker::New();
    m_sagittalPicker = vtkPropPicker::New();

    //per defecte són el mateix
    m_currentPicker = m_axialPicker;

    //assignem propietats als objectes
    m_lineActor->GetProperty()->SetColor(1.0,0.5,0.5);
    m_lineActor->GetProperty()->SetLineWidth(2);
    m_distanceLine->SetResolution (2);

    vtkCoordinate *coordinate = vtkCoordinate::New();
    coordinate->SetCoordinateSystemToWorld();
    m_lineMapper->SetTransformCoordinate( coordinate );

    m_interactor = m_2DViewer->getInteractor();

    //fem les connexions necessàries
    connect( m_2DViewer, SIGNAL( sliceChanged( int ) ), this , SLOT( drawDistancesOfSlice( int ) ) );

    coordinate->Delete();
}

DistanceTool::~DistanceTool()
{
//     m_actorsAssembly->Delete();
    m_distanceLine->Delete();
    m_lineMapper->Delete();
    m_lineActor->Delete();
    m_axialPicker->Delete();
    m_sagittalPicker->Delete();
    m_coronalPicker->Delete();
}

void DistanceTool::choosePicker()
{
    switch ( m_2DViewer->getView() )
    {
        case Q2DViewer::Axial:
            m_currentPicker = m_axialPicker;
            break;
        case Q2DViewer::Sagittal:
            m_currentPicker = m_sagittalPicker;
            break;
        case Q2DViewer::Coronal:
            m_currentPicker = m_coronalPicker;
            break;
    }
}

vtkCaptionActor2D* DistanceTool::createACaption2D()
{
    vtkCaptionActor2D *m_captionActor = vtkCaptionActor2D::New();
    m_captionActor->GetAttachmentPointCoordinate()->SetCoordinateSystemToWorld();
    m_captionActor->BorderOff();
    m_captionActor->LeaderOff();
    m_captionActor->ThreeDimensionalLeaderOff();
    m_captionActor->GetCaptionTextProperty()->SetColor( 1.0, 0.0, 0.0 );
    m_captionActor->SetPadding( 5 );
    m_captionActor->SetPosition( -1.0 , -1.0 );
    m_captionActor->SetHeight( 0.01 );
    m_captionActor->SetWidth( 0.09 );
    m_captionActor->GetCaptionTextProperty()->ShadowOff();
    m_captionActor->GetCaptionTextProperty()->ItalicOff();

    return ( m_captionActor );
}

void DistanceTool::handleEvent( unsigned long eventID )
{
    switch( eventID )
    {
        //click amb el botó esquerre: anotació de distància
        case vtkCommand::LeftButtonPressEvent:

            choosePicker();


            //anotem distància si no hi ha cap distància seleccionada, altrament
            //el que volem és interactuar amb la distància
            if (!m_somePropSelected)
            {
                if ( !m_firstPointLocated )
                    this->startDistanceAnnotation();
                else
                    this->endDistanceAnnotation();
            }
//                 else
//                 {
//                     if ( m_state == MOVINGPOINT )
//                         this->updateSelectedPoint();
//                     else
//                         this->giveMeTheNearestPoint();
//                 }
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

            choosePicker();

            if( m_selectedAssembly == NULL ) //si no tenim cap element seleccionat i polsem el botó dret, seleccionem
                this->selectDistance();
            else    //si tenim un element seleccionat i polsem el botó dret, deseleccionem l'element.
                m_selectedAssembly = NULL;
            break;

        case vtkCommand::KeyPressEvent:
            answerToKeyEvent();
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
    vtkLineSource *m_distanceLineAux = vtkLineSource::New();
    vtkActor2D *m_lineActorAux = vtkActor2D::New();
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

    //calculem la distància que s'ha mesurat
    Distance d(m_distanceStartPosition, m_distanceCurrentPosition);

    m_textDistanceActor = createACaption2D();

    QString str = QString( "%1 mm" ).arg( d.getDistance2D(), 0, 'f',  2);

    //Assignem el text a l'etiqueta de la distància i la situem
    m_textDistanceActor->SetCaption( qPrintable ( str ) );
    m_textDistanceActor->SetAttachmentPoint( m_distanceCurrentPosition );

    m_lineMapperAux->SetInput(m_distanceLineAux->GetOutput());
    m_lineActorAux->SetMapper(m_lineMapperAux);

    //afegim la distància anotada al mapa de distàncies
    switch ( m_2DViewer->getView() )
    {
        case Q2DViewer::Axial:
            m_distancesOfAxialViewMap.insert( m_2DViewer->getSlice(), m_lineActorAux );
            break;
        case Q2DViewer::Sagittal:
            m_distancesOfSagittalViewMap.insert( m_2DViewer->getSlice(), m_lineActorAux );
            break;
        case Q2DViewer::Coronal:
            m_distancesOfCoronalViewMap.insert( m_2DViewer->getSlice(), m_lineActorAux );
            break;
    }

    vtkPropAssembly *m_actorsAssembly = vtkPropAssembly::New();

    m_actorsAssembly->AddPart( m_lineActorAux );
    m_actorsAssembly->AddPart( m_textDistanceActor );

    m_currentPicker->AddPickList( m_actorsAssembly );

    m_2DViewer->getRenderer()->AddActor( m_actorsAssembly );

    m_2DViewer->getRenderer()->RemoveActor( m_lineActor );

    m_2DViewer->getInteractor()->Render();

    m_textDistanceActor->Delete();
    m_distanceLineAux->Delete();
    m_lineActorAux->Delete();
    m_lineMapperAux->Delete();
    coordinate->Delete();
    m_actorsAssembly->Delete();

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
    //li diem al picker que miri quin actor hi ha a la posició on hi ha hagut l'event del mouse (botó dret)
    int state = m_currentPicker->PickProp( m_interactor->GetEventPosition()[0], m_interactor->GetEventPosition()[1], m_2DViewer->getRenderer() );

    vtkProp *pickedProp =  m_currentPicker->GetViewProp();

    vtkPropCollection *pickCollection = m_currentPicker->GetPickList();

    if ( pickCollection->IsItemPresent( pickedProp ) != 0 )
    {
        m_selectedAssembly = vtkPropAssembly::SafeDownCast ( pickedProp );

        vtkPropCollection *assemblyCollection = m_selectedAssembly->GetParts();

        vtkActor2D *line = vtkActor2D::SafeDownCast ( assemblyCollection->GetItemAsObject( 0 ) );
        vtkCaptionActor2D *caption = vtkCaptionActor2D::SafeDownCast ( assemblyCollection->GetItemAsObject( 1 ) );

        if ( line != NULL && caption != NULL )
        {
            line->GetProperty()->SetColor( 0,0,1 );
            caption->GetCaptionTextProperty()->SetColor( 0,0,1 );
        }
    }

    m_2DViewer->refresh();
}

void DistanceTool::waitingForAction()
{
    vtkPropAssembly* m_auxAssembly;
    vtkPropCollection *assemblyCollection;
    vtkActor2D *line;
    vtkCaptionActor2D *caption;

    //\TODO augmentar el marge de pick (que no ens haguem de col·locar exactament a sobre d'una distància)


    if ( m_selectedAssembly == NULL )
    {
        //li diem al picker que miri quin actor hi ha a la posició on hi ha hagut l'event del mouse (botó dret)
        m_currentPicker->PickProp( m_interactor->GetEventPosition()[0], m_interactor->GetEventPosition()[1], m_2DViewer->getRenderer() );
        vtkProp* pickedProp =  m_currentPicker->GetViewProp();

        vtkPropCollection *pickCollection = m_currentPicker->GetPickList();

        if ( pickCollection->IsItemPresent( pickedProp ) != 0 )
        {
            m_auxAssembly = vtkPropAssembly::SafeDownCast ( pickedProp );

            assemblyCollection = m_auxAssembly->GetParts();

            line = vtkActor2D::SafeDownCast ( assemblyCollection->GetItemAsObject( 0 ) );
            caption = vtkCaptionActor2D::SafeDownCast ( assemblyCollection->GetItemAsObject( 1 ) );

            if ( line != NULL && caption != NULL )
            {
                line->GetProperty()->SetColor( 0,0,1 );
                caption->GetCaptionTextProperty()->SetColor( 0,0,1 );
            }
        }
        else
        {
            for ( int i = 0; i < pickCollection->GetNumberOfItems(); i++ )
            {
                m_auxAssembly = vtkPropAssembly::SafeDownCast ( pickCollection->GetItemAsObject( i ) );

                assemblyCollection = m_auxAssembly->GetParts();

                line = vtkActor2D::SafeDownCast ( assemblyCollection->GetItemAsObject( 0 ) );
                caption = vtkCaptionActor2D::SafeDownCast ( assemblyCollection->GetItemAsObject( 1 ) );
                if ( line != NULL && caption != NULL )
                {
                    line->GetProperty()->SetColor( 0.93,0.53,0.0 );
                    caption->GetCaptionTextProperty()->SetColor( 1,0,0 );
                }
            }
        }
    }
}

bool DistanceTool::giveMeNearestDistance()
{
    vtkPropCollection *assemblyCollection;
    vtkPropAssembly *auxAssembly;
    vtkPolyDataMapper2D *auxMap;
    vtkProp *auxProp;
    vtkLineSource *ls;
    vtkActor2D *line;
    double *r1 , *r2, distance;
    int x, y;
    double toWorld[4];
    double minDistance = 100000;

    x = m_2DViewer->getInteractor()->GetEventPosition()[0];
    y = m_2DViewer->getInteractor()->GetEventPosition()[1];

    m_2DViewer->computeDisplayToWorld( m_2DViewer->getRenderer() , x, y , 0 , toWorld );

    // detectem quin és l'actor més proper, l'identifiquem i llavors el deixem com a seleccionat
    double point[3] = { toWorld[0] , toWorld[1] , 0.0 };

    //col·lecció d'actors del picker de la vista actual
    vtkPropCollection *pickCollection = m_currentPicker->GetPickList();

    //per a cadascun dels elements de la col·lecció del picker
    for ( int i = 0; i < pickCollection->GetNumberOfItems(); i++ )
    {
        auxProp = vtkProp::SafeDownCast( pickCollection->GetItemAsObject( i ) );

        auxAssembly = vtkPropAssembly::SafeDownCast( auxProp );

        assemblyCollection = auxAssembly->GetParts();

        //agafem la línia
        line = vtkActor2D::SafeDownCast ( assemblyCollection->GetItemAsObject( 0 ) );

        if ( line != NULL )
        {
            //Obtenim el seu vtkLineSource per obtenir els punts de la línia
//             vtkPolyDataMapper2D *auxMap = vtkPolyDataMapper2D::SafeDownCast( actor->GetMapper() );
//             vtkLineSource *ls = vtkLineSource::SafeDownCast( auxMap->GetInput()->GetProducerPort()->GetProducer() );
//
//             if ( ls != NULL )
//             {
            //obtenim els 2 extrems de la distància
            r1 = line->GetPosition();
            r2 = line->GetPosition2();

            //mirem la distància que ens separa el punt de la línia
            distance = vtkLine::DistanceToLine( point , r1 , r2 );

            if ( distance < 50.0 && distance < minDistance )
            {
                minDistance = distance;
                m_nearestProp = auxProp;
            }
        }
        else
            cout << "el vtkLineSource té valor null" << endl;
        }

        return ( minDistance < 100000 );
//         else
//             cout << "la línia té valor null" << endl;
//     }
}

void DistanceTool::giveMeTheNearestPoint()
{
    int state = m_currentPicker->PickProp( m_interactor->GetEventPosition()[0], m_interactor->GetEventPosition()[1], m_2DViewer->getRenderer() );
    if ( state != 0 )
    {
        double clickPosition[3];

        m_2DViewer->getCurrentCursorPosition(clickPosition);
    /*
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
        m_state = MOVINGPOINT;*/
    }
}

void DistanceTool::moveSelectedPoint()
{
//     m_2DViewer->getCurrentCursorPosition( m_distanceCurrentPosition );
//
//         if (m_pointOfDistance == 1)
//             m_selectedDistance->SetPoint1( m_distanceCurrentPosition );
//         else
//             m_selectedDistance->SetPoint2( m_distanceCurrentPosition );
//
// //         m_selectedDiskActor->SetPosition( m_distanceCurrentPosition );
//
//         m_selectedActor->VisibilityOn();
//
//         m_2DViewer->getInteractor()->Render();
}

void DistanceTool::updateSelectedPoint()
{
//         double clickPosition[3];
//
//         m_state = NONE;
//
//         m_2DViewer->getCurrentCursorPosition(clickPosition);
//
//         Distance d;
//
//         if (m_pointOfDistance == 1)
//             d.setPoints( m_selectedDistance->GetPoint2() , clickPosition );
//         else
//             d.setPoints( m_selectedDistance->GetPoint1() , clickPosition );
//
//         QString str = QString("%1 mm").arg(d.getDistance2D(), 0, 'f', 2);
//
//         //Assignem el text a l'etiqueta de la distància i la situem
//         m_textDistanceActor->SetCaption( qPrintable ( str ));
//         m_textDistanceActor->SetAttachmentPoint( clickPosition );
// //         m_2DViewer->getRenderer()->AddActor(m_selectedActor);
//
//         m_textDistanceActor->VisibilityOn();
//
//         m_2DViewer->getInteractor()->Render();
}

bool DistanceTool::distancesListContainsThisDistance( vtkProp *distance, int slice )
{
    bool contains = false;
    int i = 0;
    QList<vtkActor2D*> list;

    switch ( m_2DViewer->getView() )
    {
        case Q2DViewer::Axial:
            list = m_distancesOfAxialViewMap.values( slice );
            break;
        case Q2DViewer::Sagittal:
            list = m_distancesOfSagittalViewMap.values( slice );
            break;
        case Q2DViewer::Coronal:
            list = m_distancesOfCoronalViewMap.values( slice );
            break;
    }

    vtkPropCollection *assemblyCollection = ( vtkPropAssembly::SafeDownCast ( distance ) )->GetParts();
    vtkActor2D *distanceLine = vtkActor2D::SafeDownCast ( assemblyCollection->GetItemAsObject( 0 ) );

    if ( list.count() > 0 )
    {
        do
        {
            vtkActor2D *currentActor = list.value( i );

            if ( distanceLine != NULL )
                contains = distanceLine == currentActor;
            else
                cout << "l'actor distància és null!!" << endl;

            i++;
        }while (!contains && i < list.count() );
    }

    return ( contains );
}


void DistanceTool::drawDistancesOfSlice( int slice )
{
    int i;

    vtkPropCollection *pickCollection;
    vtkProp *auxProp;

    if ( m_2DViewer->getView() != Q2DViewer::Axial )
    {
        pickCollection = m_axialPicker->GetPickList();

        for ( i = 0; i < pickCollection->GetNumberOfItems(); i++ )
        {
            auxProp = vtkProp::SafeDownCast ( pickCollection->GetItemAsObject( i ) );

            auxProp->VisibilityOff();
        }
    }

    if ( m_2DViewer->getView() != Q2DViewer::Sagittal )
    {
        pickCollection = m_sagittalPicker->GetPickList();

        for ( i = 0; i < pickCollection->GetNumberOfItems(); i++ )
        {
            auxProp = vtkProp::SafeDownCast ( pickCollection->GetItemAsObject( i ) );

            auxProp->VisibilityOff();
        }
    }

    if ( m_2DViewer->getView() != Q2DViewer::Coronal )
    {
        pickCollection = m_coronalPicker->GetPickList();

        for ( i = 0; i < pickCollection->GetNumberOfItems(); i++ )
        {
            auxProp = vtkProp::SafeDownCast ( pickCollection->GetItemAsObject( i ) );

            auxProp->VisibilityOff();
        }
    }

    choosePicker();
    pickCollection = m_currentPicker->GetPickList();

    for ( i = 0; i < pickCollection->GetNumberOfItems(); i++ )
    {
        auxProp = vtkProp::SafeDownCast ( pickCollection->GetItemAsObject( i ) );

        setVisibilityToThisDistanceProp( auxProp, distancesListContainsThisDistance( auxProp, slice ) );
    }

    m_2DViewer->getInteractor()->Render();

    //deixem constància de que no tenim seleccionada cap distància
    m_selectedAssembly = NULL;
}

void DistanceTool::answerToKeyEvent()
{
    //responem a la intenció d'esborrar una distància, sempre que hi hagi una distància seleccionada i
    //s'hagi polsat la tecla adequada (tecla espai)

    char keyChar = m_2DViewer->getInteractor()->GetKeyCode();
    int keyInt = (int)keyChar;

    if ( m_selectedAssembly != NULL && keyInt == 127 )
    {
        vtkPropCollection *pickCollection = m_currentPicker->GetPickList();

        if ( pickCollection->IsItemPresent( m_selectedAssembly ) != 0 ) //si conté la distància seleccionada
        {
            pickCollection->RemoveItem ( m_selectedAssembly );
            m_2DViewer->getRenderer()->RemoveActor( m_selectedAssembly );
            m_selectedAssembly = NULL;
            m_2DViewer->refresh();
        }
    }
}

void DistanceTool::setVisibilityToThisDistanceProp( vtkProp *prop, bool visibility )
{
    int visible = 0;

    if ( visibility )
        visible = 1;

    prop->SetVisibility( visible );
}

}


