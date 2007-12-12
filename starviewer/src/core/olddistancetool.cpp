/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "olddistancetool.h"
#include "q2dviewer.h"
#include "distancerepresentation.h"
#include "olddistancetooldata.h"
#include "distance.h"
#include "olddrawer.h"
#include "drawingprimitive.h"
#include "line.h"
#include "logging.h"

#include <vtkCommand.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <math.h>

namespace udg {

OldDistanceTool::OldDistanceTool( Q2DViewer *viewer , QObject * )
{
    m_nearestPoint = NOTHINGSELECTED;
    m_state = NONE;
    m_2DViewer = viewer;
    m_isCtrlPressed = false;

    m_annotationMode = "undefined";
    m_lastLeftButtonAction = "undefined";

    //de moment no hem recuperat dades correctes del Drawer
    m_correctData = false;

    //Creem aquests objectes per quan seleccionem una distància poder treballar amb les dades i la representació. Es creem amb paràmetres qualssevol perquè
    //ara no importa el que continguin, perquè no s'han d'utilitzar encara.
    m_selectedDistanceToolData = new OldDistanceToolData( m_distanceStartPosition, m_distanceStartPosition );
    m_selectedDistanceRepresentation = new DistanceRepresentation( m_selectedDistanceToolData );
}

OldDistanceTool::~OldDistanceTool()
{
    delete m_selectedDistanceToolData;
    delete m_selectedDistanceRepresentation;

    if ( m_distanceRepresentation )
        delete m_distanceRepresentation;

}

void OldDistanceTool::handleEvent( unsigned long eventID )
{
    switch( eventID )
    {
        //click amb el botó esquerre: anotació de distància
        case vtkCommand::LeftButtonPressEvent:
        m_lastLeftButtonAction = "pressed";
            if ( !m_2DViewer->getOldDrawer()->hasSelectedSet() ) //cas en que no hi ha cap conjunt de primitives (distància) seleccionat.
            {
                if ( m_isCtrlPressed )
                {
                    if ( m_2DViewer->getOldDrawer()->hasNearestSet() && m_2DViewer->getOldDrawer()->getNumberOfDrawedPrimitives() > 0 )
                    {
                        m_2DViewer->getOldDrawer()->selectNearestSet();
                        createSelectedDistanceData( m_2DViewer->getOldDrawer()->getSelectedSet() );
                    }
                 }
                else if ( m_state == NONE )
                {
                    this->startDistanceAnnotation();
                }
                else if ( m_state == ANNOTATING && m_annotationMode == "doubleClickAnnotation" )
                {
                    this->endDistanceAnnotation();
                    m_annotationMode = "undefined";
                }
            }
            else
            {
                if ( m_state == MOVINGPOINT )
                {
                    m_selectedDistanceRepresentation->getText()->visibilityOn();
                    m_selectedDistanceToolData->calculateDistance();
                    m_selectedDistanceRepresentation->refreshText( m_2DViewer->getView() );
//                     m_selectedDistanceRepresentation->refreshPolygon();

                    m_state = NONE;
                }
                else if (  m_isCtrlPressed )
                 {
                     m_2DViewer->getOldDrawer()->unselectSet();
                     //com que hem deseleccionat una distància, diem que no tenim dades correctes, per evitar la manipulació errònia de distàncies
                     m_correctData = false;
                 }
                else
                {
                    //determinem quin és el punt més proper si les dades han estat les esperades
                    if ( m_correctData )
                    {
                        this->getNearestPointOfSelectedDistance();
                        //fem invisible el text de la distància mentre movem algun punt
                        m_selectedDistanceRepresentation->getText()->visibilityOff();
                        m_selectedDistanceRepresentation->refreshText( m_2DViewer->getView() );

                        //canviem l'estat global de la tool: la deixem com que estem movent un punt.
                        m_state = MOVINGPOINT;
                    }
                    else
                        ERROR_LOG( "Recuperació errònia d'una distància esperada!!!!" );
                }
            }
            break;

        case vtkCommand::LeftButtonReleaseEvent:
            m_lastLeftButtonAction = "released";

             switch ( m_state )
            {
                case ANNOTATING:
                    if ( m_annotationMode == "releasedAnnotation" )
                    {
                         this->endDistanceAnnotation();
                         m_annotationMode = "undefined";
                    }
                break;
            }
        break;

        case vtkCommand::MouseMoveEvent:
            if ( m_lastLeftButtonAction == "pressed" )
                m_annotationMode = "releasedAnnotation";
            else
                m_annotationMode = "doubleClickAnnotation";

            switch ( m_state )
            {
                case NONE:
                    if ( ( m_2DViewer->getOldDrawer()->getNumberOfDrawedPrimitives() > 0 ) /*&& !m_2DViewer->getOldDrawer()->hasSelectedSet()*/ )
                    {
                        //només fem highlight si hi han primitives dibuixades i no n'hi ha cap de seleccionada perquè si és així, no ens interessa el highlighting
                        m_2DViewer->getOldDrawer()->highlightNearestPrimitives();
                    }
                    break;

                case ANNOTATING:
                    this->doDistanceSimulation();
                    break;

                case MOVINGPOINT:
                    if ( m_nearestPoint == FIRST )
                        this->moveFirstPoint();
                    else if ( m_nearestPoint == SECOND )
                        this->moveSecondPoint();
                break;
            }
            break;

        case vtkCommand::KeyPressEvent:
                this->answerToKeyEvent();
            break;

        case vtkCommand::KeyReleaseEvent:
            if ( ((int)( m_2DViewer->getInteractor()->GetKeyCode() ) ) == 0 ) // s'ha alliberat el Ctrl
                m_isCtrlPressed = false;
            break;

        default:
            break;
    }
}

void OldDistanceTool::createSelectedDistanceData( OldDistanceTool::PrimitivesSet *primitivesSet )
{
    //ens assegurem de que no és null
    if ( !primitivesSet )
    {
        return;
    }

    int numberOfPrimitives = 0;

    //obtenim una llista formada pel contingut del conjunt per tal de poder recòrrer-la amb un foreach.
    QList< DrawingPrimitive* > primitivesList = primitivesSet->values();

    //anem a mirar si és realment una distància: ha de contenir una línia, un text i un polígon, pertant el comptador, al final haurà de valer 3.
    foreach( DrawingPrimitive *primitive, primitivesList )
    {
        if ( primitive->getPrimitiveType() == "Line" )
        {
            Line *line = static_cast<Line*> ( primitive );
            m_selectedDistanceRepresentation->setLine( line );
            numberOfPrimitives++;
        }
        else if ( primitive->getPrimitiveType() == "Text" )
        {
            Text *text = static_cast<Text*> ( primitive );
            m_selectedDistanceRepresentation->setText( text );
            numberOfPrimitives++;
        }
//         else if ( primitive->getPrimitiveType() == "Polygon" )
//         {
//             Polygon *polygon = static_cast<Polygon*> ( primitive );
//             m_selectedDistanceRepresentation->setPolygon( polygon );
//             numberOfPrimitives++;
//         }
    }

    if ( numberOfPrimitives == 2/*3*/ )
        m_correctData = true;
}

void OldDistanceTool::startDistanceAnnotation()
{
    int xy[2];
    double position[4];

    m_state = ANNOTATING;

    m_2DViewer->getInteractor()->GetEventPosition( xy );
    m_2DViewer->computeDisplayToWorld( m_2DViewer->getRenderer() , xy[0], xy[1], 0, position );

    //només ens interessen els 3 primers valors de l'array de 4
    m_distanceStartPosition[0] = position[0];
    m_distanceStartPosition[1] = position[1];
    m_distanceStartPosition[2] = position[2];

    //creem les dades de la distància actual: li passem en aquest moment com a primer i segon punt el mateix perquè no té importància
    OldDistanceToolData *distanceData = new OldDistanceToolData( m_distanceStartPosition, m_distanceStartPosition );

    //creem la representació de la distància actual
    m_distanceRepresentation = new DistanceRepresentation( distanceData );

    //li diem a la representació que estableixi el text amb ombra
    m_distanceRepresentation->getText()->shadowOn();

    //li diem al drawer del nostre q2dviewer que dibuixi
    m_2DViewer->getOldDrawer()->drawLine( m_distanceRepresentation->getLine(), m_2DViewer->getCurrentSlice(), m_2DViewer->getView() );
}

void OldDistanceTool::doDistanceSimulation()
{
    int xy[2];
    double position[4];

    m_2DViewer->getInteractor()->GetEventPosition (xy);
    m_2DViewer->computeDisplayToWorld( m_2DViewer->getRenderer() , xy[0], xy[1], 0, position );

    //només ens interessen els 3 primers valors de l'array de 4
    m_distanceCurrentPosition[0] = position[0];
    m_distanceCurrentPosition[1] = position[1];
    m_distanceCurrentPosition[2] = position[2];

    //actualitzem l'objecte OldDistanceToolData de la representació actual i això farà que s'actualitzi la línia en el visor
    m_distanceRepresentation->getDistanceToolData()->setSecondPoint( m_distanceCurrentPosition );

    //li diem a la representació que actualitzi la línia
    m_distanceRepresentation->refreshLine();

    m_2DViewer->refresh();
}

void OldDistanceTool::endDistanceAnnotation()
{
    //per finalitzar la distància, calculem el text i la posició de la distància. També calculem les coordenades del fons del text
    m_distanceRepresentation->refreshText( m_2DViewer->getView() );

    //afegim al drawer un conjunt de primitives relacionades. A l'afegir dibuixarà el text i el fons perquè la línia ja esta dibuixada
    m_2DViewer->getOldDrawer()->addSetOfPrimitives( m_distanceRepresentation );

    //determinem que estem en l'estat NONE, és a dir, no estem anotant cap distància.
    m_state = NONE;
}

void OldDistanceTool::getNearestPointOfSelectedDistance()
{
    double *vertex1, *vertex2;
    int x, y;
    x = m_2DViewer->getInteractor()->GetEventPosition()[0];
    y = m_2DViewer->getInteractor()->GetEventPosition()[1];
    double toWorld[4];
    m_2DViewer->computeDisplayToWorld( m_2DViewer->getRenderer() , x, y , 0 , toWorld );
    double point[3] = { toWorld[0] , toWorld[1], toWorld[2] };

    //obtenim els dos punts de la distància recuperada
    vertex1 = m_selectedDistanceRepresentation->getLine()->getFirstPoint();
    vertex2 = m_selectedDistanceRepresentation->getLine()->getSecondPoint();

    //creem dues distàncies per mirar quina està més aprop
    Distance d1( vertex1, point );
    Distance d2( vertex2, point );

    if ( d1.getDistance2D() <= d2.getDistance2D() ) //vol dir que el punt1 de la distància està més aprop
    {
        m_nearestPoint = FIRST;
        m_selectedDistanceToolData->setFirstPoint( point );
    }
    else                                            //el punt2 és el més proper
    {
        m_nearestPoint = SECOND;
        m_selectedDistanceToolData->setSecondPoint( point );
    }

    m_selectedDistanceRepresentation->getLine()->refreshLine();
}

void OldDistanceTool::moveFirstPoint()
{
    int x, y;
    x = m_2DViewer->getInteractor()->GetEventPosition()[0];
    y = m_2DViewer->getInteractor()->GetEventPosition()[1];
    double toWorld[4];
    m_2DViewer->computeDisplayToWorld( m_2DViewer->getRenderer() , x, y , 0 , toWorld );
    double point[3] = { toWorld[0] , toWorld[1], toWorld[2] };
    m_selectedDistanceToolData->setFirstPoint( point );
    m_selectedDistanceRepresentation->getLine()->refreshLine();
}

void OldDistanceTool::moveSecondPoint()
{
    int x, y;
    x = m_2DViewer->getInteractor()->GetEventPosition()[0];
    y = m_2DViewer->getInteractor()->GetEventPosition()[1];
    double toWorld[4];
    m_2DViewer->computeDisplayToWorld( m_2DViewer->getRenderer() , x, y , 0 , toWorld );
    double point[3] = { toWorld[0] , toWorld[1], toWorld[2] };
    m_selectedDistanceToolData->setSecondPoint( point );
    m_selectedDistanceRepresentation->getLine()->refreshLine();
}

void OldDistanceTool::answerToKeyEvent()
{
    //responem a la intenció d'esborrar una distància, sempre que hi hagi una distància seleccionada i
    //s'hagi polsat la tecla adequada (tecla sup) o seleccionar una distància amb el Ctrl i un botó del mouse

    char keyChar = m_2DViewer->getInteractor()->GetKeyCode();
    int keyInt = (int)keyChar;

    if ( keyInt == 127 && m_2DViewer->getOldDrawer()->hasSelectedSet() ) //s'ha polsat el Sup
    {
        m_2DViewer->getOldDrawer()->removeSelectedSet();
        m_state = NONE;
    }
    else if ( keyInt == 0 ) // s'ha polsat el Ctrl
    {
        m_isCtrlPressed = true;
    }
}

}


