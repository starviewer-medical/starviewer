/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "nonclosedangletool.h"
#include "q2dviewer.h"
#include "logging.h"
#include "volume.h"
#include "drawer.h"
#include "drawerline.h"
#include "drawerpolyline.h"
#include "drawertext.h"
#include "mathtools.h"
//vtk
#include <vtkRenderWindowInteractor.h>
#include <vtkCommand.h>
#include <vtkProp.h>
#include <vtkLine.h>
#include <vtkPoints.h>

#include "mathtools.h"
#include <vtkMath.h>
//Qt
#include <QList>

namespace udg {

NonClosedAngleTool::NonClosedAngleTool( QViewer *viewer, QObject *parent )
 : Tool(viewer, parent)
{
    m_toolName = "NonClosedAngleTool";
    m_hasSharedData = false;

    m_2DViewer = qobject_cast<Q2DViewer *>( viewer );
    if( !m_2DViewer )
        DEBUG_LOG(QString("El casting no ha funcionat!!! És possible que viewer no sigui un Q2DViewer!!!-> ")+ viewer->metaObject()->className() );

    m_circumferencePolyline= NULL;
    m_firstLine = NULL;
    m_secondLine = NULL;
    m_state = NONE;
}

NonClosedAngleTool::~NonClosedAngleTool()
{
    if ( m_state != NONE )
    {
        if ( m_firstLine )
            delete m_firstLine;
        if ( m_secondLine )
            delete m_secondLine;
        if ( m_circumferencePolyline )
            delete m_circumferencePolyline;
    }
}

void NonClosedAngleTool::handleEvent( long unsigned eventID )
{
    switch( eventID )
    {
        case vtkCommand::LeftButtonPressEvent:

            if( m_2DViewer->getInput() )
            {
                if ( m_2DViewer->getInteractor()->GetRepeatCount() == 0 )
                {
                    if ( m_state == NONE )
                        this->annotateFirstLinePoints();
                    else if ( m_state == FIRST_LINE_FIXED )
                    {
                        this->annotateSecondLinePoints();
                        if ( m_state == NONE )
                        {
                            computeAngle();
                            //Acabem les línies
                            m_firstLine = NULL;
                            m_secondLine = NULL;

                        }
                    }
                    m_2DViewer->getDrawer()->refresh();
                }
            }
        break;

        case vtkCommand::MouseMoveEvent:

            if( m_firstLine && m_state == NONE )
                this->simulateLine();
            else if ( m_secondLine && m_state == FIRST_LINE_FIXED )
//                 this->simulateMirrorLine();
                this->simulateSecondLine();

            m_2DViewer->getDrawer()->refresh();

        break;
    }
}

void NonClosedAngleTool::findInitialDegreeArc()
{
/*    //Per saber quin l'angle inicial, cal calcular l'angle que forma el primer segment anotat i un segment fictici totalment horitzontal.
    double horizontalP2[3], *vd1, *vd2, *pv;
    double *p1 = m_mainPolyline->getPoint( 0 );
    double *p2 = m_mainPolyline->getPoint( 1 );

    int coord1, coord2;

    switch( m_2DViewer->getView() )
    {
        case QViewer::AxialPlane:
            coord1 = 0;
            coord2 = 2;
            break;

        case QViewer::SagitalPlane:
            coord1 = 1;
            coord2 = 0;
            break;

        case QViewer::CoronalPlane:
            coord1 = 2;
            coord2 = 1;
            break;
    }

    for (int i = 0; i < 3; i++)
        horizontalP2[i] = p2[i];

    vd1 = MathTools::directorVector( p1, p2 );

    horizontalP2[coord1] += 10.0;
    vd2 = MathTools::directorVector( horizontalP2, p2 );
    pv = MathTools::vectorialProduct(vd1, vd2);

    if ( pv[coord2] > 0 )
    {
        m_initialDegreeArc =(int)MathTools::angleInDegrees( vd1, vd2 );
    }
    else
    {
        m_initialDegreeArc = -1 * (int)MathTools::angleInDegrees( vd1, vd2 );
    }*/
}

void NonClosedAngleTool::annotateFirstLinePoints()
{
    if ( !m_firstLine )
    {
        m_firstLine = new DrawerLine;
        m_hasFirstPoint = false;
        m_hasSecondPoint = false;
    }

    double position[4];
    double computed[3];

    //capturem l'event de clic esquerre
    int *xy = m_2DViewer->getEventPosition();

    m_2DViewer->computeDisplayToWorld( m_2DViewer->getRenderer() , xy[0], xy[1], 0, position );
    computed[0] = position[0];
    computed[1] = position[1];
    computed[2] = position[2];

    //afegim el punt
    if( !m_hasFirstPoint ) {
        m_firstLine->setFirstPoint( computed );
        m_hasFirstPoint = true;
    }
    else
    {
        m_firstLine->setSecondPoint( computed );
        //actualitzem els atributs de la linia

        if( !m_hasSecondPoint )
        {
            m_2DViewer->getDrawer()->draw( m_firstLine , m_2DViewer->getView(), m_2DViewer->getCurrentSlice() );
        }
        else
            m_firstLine->update( DrawerPrimitive::VTKRepresentation );

        m_state = FIRST_LINE_FIXED;
    }

}

void NonClosedAngleTool::annotateSecondLinePoints()
{
    if ( !m_secondLine )
    {
        m_secondLine = new DrawerLine;
        m_hasFirstPoint = false;
        m_hasSecondPoint = false;
    }

    double position[4];
    double computed[3];

    //capturem l'event de clic esquerre
    int *xy = m_2DViewer->getEventPosition();

    m_2DViewer->computeDisplayToWorld( m_2DViewer->getRenderer() , xy[0], xy[1], 0, position );
    computed[0] = position[0];
    computed[1] = position[1];
    computed[2] = position[2];

    //afegim el punt
    if( !m_hasFirstPoint ) {
        m_secondLine->setFirstPoint( computed );
        m_hasFirstPoint = true;
    }
    else
    {
        m_secondLine->setSecondPoint( computed );
        //actualitzem els atributs de la linia

        if( !m_hasSecondPoint )
        {
            m_2DViewer->getDrawer()->draw( m_secondLine , m_2DViewer->getView(), m_2DViewer->getCurrentSlice() );
        }
        else
            m_secondLine->update( DrawerPrimitive::VTKRepresentation );

        m_state = NONE;

    }

}

void NonClosedAngleTool::simulateLine()
{
    double position[4];
    double computed[3];

    //capturem l'event de clic esquerre
    int *xy = m_2DViewer->getEventPosition();

    m_2DViewer->computeDisplayToWorld( m_2DViewer->getRenderer() , xy[0], xy[1], 0, position );
    computed[0] = position[0];
    computed[1] = position[1];
    computed[2] = position[2];

    m_firstLine->setSecondPoint( computed );
    //actualitzem els atributs de la linia

    if( !m_hasSecondPoint )
    {
        m_2DViewer->getDrawer()->draw( m_firstLine , m_2DViewer->getView(), m_2DViewer->getCurrentSlice() );
        m_hasSecondPoint = true;
    }
    else
        m_firstLine->update( DrawerPrimitive::VTKRepresentation );

}

void NonClosedAngleTool::simulateSecondLine()
{
    double position[4];
    double computed[3];

    //capturem l'event de clic esquerre
    int *xy = m_2DViewer->getEventPosition();

    m_2DViewer->computeDisplayToWorld( m_2DViewer->getRenderer() , xy[0], xy[1], 0, position );
    computed[0] = position[0];
    computed[1] = position[1];
    computed[2] = position[2];

    m_secondLine->setSecondPoint( computed );
    //actualitzem els atributs de la linia

    if( !m_hasSecondPoint )
    {
        m_2DViewer->getDrawer()->draw( m_secondLine , m_2DViewer->getView(), m_2DViewer->getCurrentSlice() );
        m_hasSecondPoint = true;
    }
    else
        m_secondLine->update( DrawerPrimitive::VTKRepresentation );

}

void NonClosedAngleTool::simulateMirrorLine()
{
    ///TODO
}

void NonClosedAngleTool::drawCircumference()
{/*
    double degrees, intersection[2], xAxis1[2], xAxis2[2], yAxis1[2], yAxis2[2], *newPoint, xRadius, yRadius;
    int initialI, finalI, viewCoord;

    int view = m_2DViewer->getView();

    double *p1 = m_mainPolyline->getPoint( 0 );
    double *p2 = m_mainPolyline->getPoint( 1 );
    double *p3 = m_mainPolyline->getPoint( 2 );

    double *vd1 = MathTools::directorVector( p1, p2 );
    double *vd2 = MathTools::directorVector( p3, p2 );

    double angle = MathTools::angleInDegrees( vd1, vd2 );

    switch( view )
    {
        case QViewer::AxialPlane:
            xAxis1[0] = p2[0];
            xAxis1[1] = p2[1];

            xAxis2[0] = p1[0];
            xAxis2[1] = p2[1];

            yAxis1[0] = p2[0];
            yAxis1[1] = p2[1];

            yAxis2[0] = p2[0];
            yAxis2[1] = p1[1];

            viewCoord = 2;
            break;

        case QViewer::SagitalPlane:
            xAxis1[0] = p2[2];
            xAxis1[1] = p2[1];

            xAxis2[0] = p1[2];
            xAxis2[1] = p2[1];

            yAxis1[0] = p2[2];
            yAxis1[1] = p2[1];

            yAxis2[0] = p2[2];
            yAxis2[1] = p1[1];

            viewCoord = 0;
            break;

        case QViewer::CoronalPlane:
            xAxis1[0] = p2[0];
            xAxis1[1] = p2[2];

            xAxis2[0] = p1[0];
            xAxis2[1] = p2[2];

            yAxis1[0] = p2[0];
            yAxis1[1] = p2[2];

            yAxis2[0] = p2[0];
            yAxis2[1] = p1[2];

            viewCoord = 1;
            break;
    }

    double distance1 = MathTools::getDistance3D( p1, p2 );
    double distance2 = MathTools::getDistance3D( p2, p3 );

    xRadius =  MathTools::minimum( distance1, distance2 ) / 4.0;
    m_radius = xRadius;
    yRadius = xRadius;

    intersection[0] = ((yAxis2[0] - yAxis1[0]) / 2.0) + yAxis1[0];
    intersection[1] = ((xAxis2[1] - xAxis1[1]) / 2.0) + xAxis1[1];

    double *pv = MathTools::vectorialProduct(vd1, vd2);

    initialI = 360 - m_initialDegreeArc;
    finalI = int(360 - ( angle+m_initialDegreeArc ) );

    if ( pv[viewCoord] > 0 )
    {
        finalI = int(angle-m_initialDegreeArc);
    }

    if ( (initialI-finalI) <= 180 )
    {
        for ( int i = initialI; i > finalI; i-- )
        {
            degrees = i*1.0*vtkMath::DoubleDegreesToRadians();
            newPoint = new double[3];

            switch( view )
            {
                case QViewer::AxialPlane:
                    newPoint[0] = cos( degrees )*xRadius + intersection[0];
                    newPoint[1] = sin( degrees )*yRadius + intersection[1];
                    newPoint[2] = 0.0;
                    break;

                case QViewer::SagitalPlane:
                    newPoint[0] = 0.0;
                    newPoint[1] = cos( degrees )*yRadius + intersection[1];
                    newPoint[2] = sin( degrees )*xRadius + intersection[0];
                    break;

                case QViewer::CoronalPlane:
                    newPoint[0] = sin( degrees )*xRadius + intersection[0];
                    newPoint[1] = 0.0;
                    newPoint[2] = cos( degrees )*yRadius + intersection[1];
                    break;
            }
            m_circumferencePolyline->addPoint( newPoint );
        }
    }
    else
    {
        initialI = -m_initialDegreeArc;
        finalI = int( angle-m_initialDegreeArc );
        for ( int i = initialI; i < finalI; i++ )
        {
            degrees = i*1.0*vtkMath::DoubleDegreesToRadians();
            newPoint = new double[3];

            switch( view )
            {
                case QViewer::AxialPlane:
                    newPoint[0] = cos( degrees )*xRadius + intersection[0];
                    newPoint[1] = sin( degrees )*yRadius + intersection[1];
                    newPoint[2] = 0.0;
                    break;

                case QViewer::SagitalPlane:
                    newPoint[0] = 0.0;
                    newPoint[1] = cos( degrees )*yRadius + intersection[1];
                    newPoint[2] = sin( degrees )*xRadius + intersection[0];
                    break;

                case QViewer::CoronalPlane:
                    newPoint[0] = sin( degrees )*xRadius + intersection[0];
                    newPoint[1] = 0.0;
                    newPoint[2] = cos( degrees )*yRadius + intersection[1];
                    break;
            }
            m_circumferencePolyline->addPoint( newPoint );
        }
    }
    m_circumferencePolyline->update( DrawerPrimitive::VTKRepresentation );*/
}

void NonClosedAngleTool::computeAngle()
{
    DrawerLine *middleLine = new DrawerLine;
    middleLine->setLinePattern(DrawerPrimitive::DiscontinuousLinePattern);

    double *p1 = m_firstLine->getFirstPoint();
    double *p2 = m_firstLine->getSecondPoint();
    double *p3 = m_secondLine->getFirstPoint();
    double *p4 = m_secondLine->getSecondPoint();

    double *vd1, *vd2;
    double dist11, dist12, dist21, dist22, minDist;

    //distàncies mínimes (punts més propers de les dues rectes)
    dist11 = MathTools::getDistance3D(p1,p3);
    dist12 = MathTools::getDistance3D(p1,p4);
    dist21 = MathTools::getDistance3D(p2,p3);
    dist22 = MathTools::getDistance3D(p2,p4);

    minDist = minimum(dist11,dist12,dist21,dist22);

    //Els vectors han d'anar en sentit al punt d'intersecció
    if ( minDist == dist11 )
    {
        vd1 = MathTools::directorVector( p2, p1 );
        vd2 = MathTools::directorVector( p4, p3 );
        middleLine->setFirstPoint(p1);
        middleLine->setSecondPoint(p3);
    }
    else if ( minDist == dist12 )
    {
        vd1 = MathTools::directorVector( p2, p1 );
        vd2 = MathTools::directorVector( p3, p4 );
        middleLine->setFirstPoint(p1);
        middleLine->setSecondPoint(p4);
    }
    else if ( minDist == dist21 )
    {
        vd1 = MathTools::directorVector( p1, p2 );
        vd2 = MathTools::directorVector( p4, p3 );
        middleLine->setFirstPoint(p2);
        middleLine->setSecondPoint(p3);
    }
    else    //minDist == dist22
    {
        vd1 = MathTools::directorVector( p1, p2 );
        vd2 = MathTools::directorVector( p3, p4 );
        middleLine->setFirstPoint(p2);
        middleLine->setSecondPoint(p4);
    }

    //dibuixem la línia auxiliar
    m_2DViewer->getDrawer()->draw( middleLine , m_2DViewer->getView(), m_2DViewer->getCurrentSlice() );

    for (int i = 0; i < 3; i++)
    {
        if ( fabs( vd1[i] ) < 0.0001 )
            vd1[i] = 0.0;

        if ( fabs( vd2[i] ) < 0.0001 )
            vd2[i] = 0.0;
    }

    double angle = MathTools::angleInDegrees( vd1, vd2 );

    DrawerText * text = new DrawerText;
    text->setText( tr("%1 degrees").arg( angle,0,'f',1) );
    textPosition( middleLine->getFirstPoint(), middleLine->getSecondPoint(), text );

    text->update( DrawerPrimitive::VTKRepresentation );
    m_2DViewer->getDrawer()->draw( text , m_2DViewer->getView(), m_2DViewer->getCurrentSlice() );
    m_2DViewer->getDrawer()->refresh();

    middleLine = NULL;

}

void NonClosedAngleTool::textPosition( double *p1, double *p2, DrawerText *angleText )
{
    double position[3];
    int horizontalCoord, verticalCoord;

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

    position[horizontalCoord] = ( p1[horizontalCoord] + p2[horizontalCoord] ) / 2.0;
    position[verticalCoord] = ( p1[verticalCoord] + p2[verticalCoord] ) / 2.0;

    angleText->setAttatchmentPoint(position);

}

double NonClosedAngleTool::minimum(double d1, double d2, double d3, double d4)
{
    if (d1 <= d2 && d1 <= d3 && d1 <= d4)
        return d1;
    else if (d2 <= d1 && d2 <= d3 && d2 <= d4)
        return d2;
    else if (d3 <= d1 && d3 <= d2 && d3 <= d4)
        return d3;
    else
        return d4;
}
}
