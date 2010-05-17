/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "angletool.h"
#include "q2dviewer.h"
#include "logging.h"
#include "volume.h"
#include "drawer.h"
#include "drawerpolyline.h"
#include "drawertext.h"
#include "mathtools.h"
//vtk
#include <vtkRenderWindowInteractor.h>
#include <vtkCommand.h>

namespace udg {

AngleTool::AngleTool( QViewer *viewer, QObject *parent )
 : Tool(viewer, parent), m_mainPolyline(NULL), m_circlePolyline(NULL), m_state(None)
{
    m_toolName = "AngleTool";
    m_hasSharedData = false;

    m_2DViewer = qobject_cast<Q2DViewer *>( viewer );
    if( !m_2DViewer )
        DEBUG_LOG(QString("El casting no ha funcionat!!! És possible que viewer no sigui un Q2DViewer!!!-> ")+ viewer->metaObject()->className() );
    
    connect( m_2DViewer, SIGNAL( volumeChanged(Volume *) ), SLOT( initialize() ) );
}

AngleTool::~AngleTool()
{
    if ( m_state != None )
    {
        bool hasToRefresh = false;
        // Cal decrementar el reference count perquè 
        // l'annotació s'esborri si "matem" l'eina
        if ( m_mainPolyline )
        {
            m_mainPolyline->decreaseReferenceCount();
            delete m_mainPolyline;
            hasToRefresh = true;
        }
        
        if ( m_circlePolyline )
        {
            m_circlePolyline->decreaseReferenceCount();
            delete m_circlePolyline;
            hasToRefresh = true;
        }

        if( hasToRefresh )
            m_2DViewer->refresh();
    }
}

void AngleTool::handleEvent( long unsigned eventID )
{
    switch( eventID )
    {
        case vtkCommand::LeftButtonPressEvent:

            if( m_2DViewer->getInput() )
            {
                if ( m_2DViewer->getInteractor()->GetRepeatCount() == 0 )
                {
                    if ( m_state == None )
                        this->annotateFirstPoint();
                    else if ( m_state == FirstPointFixed )
                    {
                        this->fixFirstSegment();
                        this->findInitialDegreeArc();
                    }
                    else
                    {
                        //voldrem enregistrar l'últim punt, pertant posem l'estat a none
                        m_state = None;
                        finishDrawing();
                    }
                }
            }

        break;

        case vtkCommand::MouseMoveEvent:
            simulateCorrespondingSegmentOfAngle();
        break;
    }
}

void AngleTool::findInitialDegreeArc()
{
    //Per saber quin l'angle inicial, cal calcular l'angle que forma el primer segment anotat i un segment fictici totalment horitzontal.
    double horizontalP2[3], *vd1, *vd2, pv[3];
    double *p1 = m_mainPolyline->getPoint( 0 );
    double *p2 = m_mainPolyline->getPoint( 1 );

    int coord1, depthCoord;

    switch( m_2DViewer->getView() )
    {
        case QViewer::AxialPlane:
            coord1 = 0;
            depthCoord = 2;
            break;

        case QViewer::SagitalPlane:
            coord1 = 1;
            depthCoord = 0;
            break;

        case QViewer::CoronalPlane:
            coord1 = 2;
            depthCoord = 1;
            break;
    }

    for (int i = 0; i < 3; i++)
        horizontalP2[i] = p2[i];

    vd1 = MathTools::directorVector( p1, p2 );

    horizontalP2[coord1] += 10.0;
    vd2 = MathTools::directorVector( horizontalP2, p2 );
    MathTools::crossProduct(vd1, vd2, pv);

    if ( pv[depthCoord] > 0 )
    {
        m_initialDegreeArc =(int)MathTools::angleInDegrees( vd1, vd2 );
    }
    else
    {
        m_initialDegreeArc = -1 * (int)MathTools::angleInDegrees( vd1, vd2 );
    }
}

void AngleTool::annotateFirstPoint()
{
    m_mainPolyline = new DrawerPolyline;
    // Així evitem que durant l'edició la primitiva pugui ser esborrada per events externs
    m_mainPolyline->increaseReferenceCount();
    m_2DViewer->getDrawer()->draw( m_mainPolyline , m_2DViewer->getView(), m_2DViewer->getCurrentSlice() );

    double clickedWorldPoint[3];
    m_2DViewer->getEventWorldCoordinate( clickedWorldPoint );

    //afegim el punt
    m_mainPolyline->addPoint( clickedWorldPoint );
    m_mainPolyline->update( DrawerPrimitive::VTKRepresentation );

    //actualitzem l'estat de la tool
    m_state = FirstPointFixed;
}

void AngleTool::fixFirstSegment()
{
    m_mainPolyline->update( DrawerPrimitive::VTKRepresentation );

    //posem l'estat de la tool a CenterFixed, així haurà agafat l'últim valor.
    m_state = CenterFixed;

    //creem la polilínia per a dibuixar l'arc de circumferència i l'afegim al drawer
    m_circlePolyline = new DrawerPolyline;
    // Així evitem que durant l'edició la primitiva pugui ser esborrada per events externs
    m_circlePolyline->increaseReferenceCount();
    m_2DViewer->getDrawer()->draw( m_circlePolyline , m_2DViewer->getView(), m_2DViewer->getCurrentSlice() );
}

void AngleTool::drawCircle()
{
    double degreesIncrease, *newPoint, radius;
    int initialAngle, finalAngle, depthCoord;

    double *firstPoint = m_mainPolyline->getPoint(0);
    double *circleCentre = m_mainPolyline->getPoint(1);
    double *lastPoint = m_mainPolyline->getPoint(2);

    // calculem l'angle que formen els dos segments
    double *firstSegment = MathTools::directorVector( firstPoint, circleCentre );
    double *secondSegment = MathTools::directorVector( lastPoint, circleCentre );
    m_currentAngle = MathTools::angleInDegrees( firstSegment, secondSegment );
    
    // calculem el radi de l'arc de circumferència que mesurarà
    // un quart del segment més curt dels dos que formen l'angle
    double distance1 = MathTools::getDistance3D( firstPoint, circleCentre );
    double distance2 = MathTools::getDistance3D( circleCentre, lastPoint );
    radius = MathTools::minimum( distance1, distance2 ) / 4.0;

    int view = m_2DViewer->getView();
    switch( view )
    {
        case QViewer::AxialPlane:
            depthCoord = 2;
            break;

        case QViewer::SagitalPlane:
            depthCoord = 0;
            break;

        case QViewer::CoronalPlane:
            depthCoord = 1;
            break;
    }

    // calculem el rang de les iteracions per pintar l'angle correctament
    initialAngle = 360 - m_initialDegreeArc;
    finalAngle = int(360 - ( m_currentAngle+m_initialDegreeArc ) );
    
    double pv[3];
    MathTools::crossProduct(firstSegment, secondSegment, pv);
    if ( pv[depthCoord] > 0 )
    {
        finalAngle = int(m_currentAngle-m_initialDegreeArc);
    }
    if ( (initialAngle-finalAngle) > 180 )
    {
        initialAngle = int( m_currentAngle-m_initialDegreeArc );
        finalAngle = -m_initialDegreeArc;
    }

    // Reconstruim l'arc de circumferència
    m_circlePolyline->deleteAllPoints();
    for ( int i = initialAngle; i > finalAngle; i-- )
    {
        degreesIncrease = i*1.0*MathTools::DegreesToRadiansAsDouble;
        newPoint = new double[3];

        switch( view )
        {
            case QViewer::AxialPlane:
                newPoint[0] = cos( degreesIncrease )*radius + circleCentre[0];
                newPoint[1] = sin( degreesIncrease )*radius + circleCentre[1];
                newPoint[2] = 0.0;
                break;

            case QViewer::SagitalPlane:
                newPoint[0] = 0.0;
                newPoint[1] = cos( degreesIncrease )*radius + circleCentre[1];
                newPoint[2] = sin( degreesIncrease )*radius + circleCentre[2];
                break;

            case QViewer::CoronalPlane:
                newPoint[0] = sin( degreesIncrease )*radius + circleCentre[0];
                newPoint[1] = 0.0;
                newPoint[2] = cos( degreesIncrease )*radius + circleCentre[2];
                break;
        }
        m_circlePolyline->addPoint( newPoint );
    }

    m_circlePolyline->update( DrawerPrimitive::VTKRepresentation );
}

void AngleTool::simulateCorrespondingSegmentOfAngle()
{
    if( !m_mainPolyline )
        return;

    if( m_state != None )
    {
        // agafem la coordenada de pantalla
        double clickedWorldPoint[3];
        m_2DViewer->getEventWorldCoordinate( clickedWorldPoint );

        int pointIndex;
        if( m_state == FirstPointFixed )
            pointIndex = 1;
        else if( m_state == CenterFixed )
            pointIndex = 2;

        // assignem el segon o tercer punt de l'angle segons l'estat
        m_mainPolyline->setPoint( pointIndex, clickedWorldPoint );
        m_mainPolyline->update( DrawerPrimitive::VTKRepresentation );

        if( m_state == CenterFixed )
            drawCircle();
        
        m_2DViewer->refresh();
    }
}

void AngleTool::finishDrawing()
{
    // Així alliberem les primitives perquè puguin ser esborrades
    m_mainPolyline->decreaseReferenceCount();
    m_circlePolyline->decreaseReferenceCount();
    // eliminem l'arc de circumferència (s'esborra automàticament del drawer)
    delete m_circlePolyline;

    // afegim l'annotació textual
    DrawerText *text = new DrawerText;
    text->setText( tr("%1 degrees").arg( m_currentAngle,0,'f',1) );
    text->shadowOn();
    placeText( text );
    m_2DViewer->getDrawer()->draw( text , m_2DViewer->getView(), m_2DViewer->getCurrentSlice() );
}

void AngleTool::placeText( DrawerText *angleText )
{
    double *p1 = m_mainPolyline->getPoint(0);
    double *p2 = m_mainPolyline->getPoint(1);
    double *p3 = m_mainPolyline->getPoint(2);
    double position[3];
    int i, horizontalCoord, verticalCoord;

    switch( m_2DViewer->getView() )
    {
        case Q2DViewer::Axial:
            horizontalCoord = 0;
            verticalCoord = 1;
            break;

        case Q2DViewer::Sagital:
            horizontalCoord = 1;
            verticalCoord = 2;
            break;

        case Q2DViewer::Coronal:
            horizontalCoord = 0;
            verticalCoord = 2;
            break;
    }

    //mirem on estan horitzontalment els punts p1 i p3 respecte del p2
    if ( p1[0] <= p2[0] )
    {
        angleText->setHorizontalJustification( "Left" );

        if ( p3[horizontalCoord] <= p2[horizontalCoord] )
        {
            angleText->setAttachmentPoint( p2 );
        }
        else
        {
            for ( i = 0; i < 3; i++ )
                position[i] = p2[i];

            if ( p2[verticalCoord] <= p3[verticalCoord] )
            {
                position[verticalCoord] -= 2.;
            }
            else
            {
                position[verticalCoord] += 2.;
            }
            angleText->setAttachmentPoint( position );
        }
    }
    else
    {
        angleText->setHorizontalJustification( "Right" );

        if ( p3[horizontalCoord] <= p2[horizontalCoord] )
        {
            angleText->setAttachmentPoint( p2 );
        }
        else
        {
            for ( i = 0; i < 3; i++ )
                position[i] = p2[i];

            if ( p2[verticalCoord] <= p3[verticalCoord] )
            {
                position[verticalCoord] += 2.;
            }
            else
            {
                position[verticalCoord] -= 2.;
            }
            angleText->setAttachmentPoint( position );
        }
    }
}

void AngleTool::initialize()
{
    // Alliberem les primitives perquè puguin ser esborrades
    if( m_mainPolyline )
    {
        m_mainPolyline->decreaseReferenceCount();
        delete m_mainPolyline;
    }
    
    if( m_circlePolyline )
    {
        m_circlePolyline->decreaseReferenceCount();
        delete m_circlePolyline;
    }

    m_mainPolyline = NULL;
    m_circlePolyline = NULL;
    m_state = None;
}

}
