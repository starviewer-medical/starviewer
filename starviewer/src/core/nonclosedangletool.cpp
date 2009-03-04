/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "nonclosedangletool.h"
#include "q2dviewer.h"
#include "logging.h"
#include "drawer.h"
#include "drawerline.h"
#include "drawertext.h"
#include "mathtools.h"
//vtk
#include <vtkRenderWindowInteractor.h>
#include <vtkCommand.h>

namespace udg {

NonClosedAngleTool::NonClosedAngleTool( QViewer *viewer, QObject *parent )
 : Tool(viewer, parent)
{
    m_toolName = "NonClosedAngleTool";
    m_hasSharedData = false;

    m_2DViewer = qobject_cast<Q2DViewer *>( viewer );
    if( !m_2DViewer )
        DEBUG_LOG(QString("El casting no ha funcionat!!! És possible que viewer no sigui un Q2DViewer!!!-> ")+ viewer->metaObject()->className() );

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
                this->simulateSecondLine();

            m_2DViewer->getDrawer()->refresh();

        break;
    }
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
    if( !m_hasFirstPoint )
    {
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
    if( !m_hasFirstPoint )
    {
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

void NonClosedAngleTool::computeAngle()
{
    if ( !m_middleLine )
    {
        m_middleLine = new DrawerLine;
    }
    m_middleLine->setLinePattern(DrawerPrimitive::DiscontinuousLinePattern);

    double *p1 = m_firstLine->getFirstPoint();
    double *p2 = m_firstLine->getSecondPoint();
    double *p3 = m_secondLine->getFirstPoint();
    double *p4 = m_secondLine->getSecondPoint();

    double *vd1, *vd2;

    double *intersection;
    int state;

    intersection = intersectionPoint(p1,p2,p3,p4,state);

    double dist1, dist2, dist3, dist4;
    dist1 = MathTools::getDistance3D(intersection, p1);
    dist2 = MathTools::getDistance3D(intersection, p2);
    dist3 = MathTools::getDistance3D(intersection, p3);
    dist4 = MathTools::getDistance3D(intersection, p4);

    if ( dist1 <= dist2 )
    {
        if ( dist3 <= dist4 )
        {
            vd1 = MathTools::directorVector( p1, intersection );
            vd2 = MathTools::directorVector( p3, intersection );
            m_middleLine->setFirstPoint(p1);
            m_middleLine->setSecondPoint(p3);
        }
        else
        {
            vd1 = MathTools::directorVector( p1, intersection );
            vd2 = MathTools::directorVector( p4, intersection );
            m_middleLine->setFirstPoint(p1);
            m_middleLine->setSecondPoint(p4);
        }
    }
    else
    {
        if ( dist3 <= dist4 )
        {
            vd1 = MathTools::directorVector( p2, intersection );
            vd2 = MathTools::directorVector( p3, intersection );
            m_middleLine->setFirstPoint(p2);
            m_middleLine->setSecondPoint(p3);
        }
        else
        {
            vd1 = MathTools::directorVector( p2, intersection );
            vd2 = MathTools::directorVector( p4, intersection );
            m_middleLine->setFirstPoint(p2);
            m_middleLine->setSecondPoint(p4);
        }
    }

    //dibuixem la línia auxiliar
    m_2DViewer->getDrawer()->draw( m_middleLine , m_2DViewer->getView(), m_2DViewer->getCurrentSlice() );

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
    textPosition( m_middleLine->getFirstPoint(), m_middleLine->getSecondPoint(), text );

    text->update( DrawerPrimitive::VTKRepresentation );
    m_2DViewer->getDrawer()->draw( text , m_2DViewer->getView(), m_2DViewer->getCurrentSlice() );
    m_2DViewer->getDrawer()->refresh();

    m_middleLine = NULL;
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

double *NonClosedAngleTool::intersectionPoint(double *p1, double *p2, double *p3, double *p4, int &state)
{
    //using parametric equations:
    //line 1: y = a + b·x
    //line 2: y = u + v·x
    double a, b, u, v;
    double *intersection;
    int horizontalCoord, verticalCoord, thirdCoord;

    switch( m_2DViewer->getView() )
    {
        case Q2DViewer::Axial:
            horizontalCoord = 0;
            verticalCoord = 1;
            thirdCoord = 2;
            break;

        case Q2DViewer::Sagital:
            horizontalCoord = 1;
            verticalCoord = 2;
            thirdCoord = 0;
            break;

        case Q2DViewer::Coronal:
            horizontalCoord = 0;
            verticalCoord = 2;
            thirdCoord = 1;
            break;
    }

    intersection = new double[3];
    intersection[0] = 0;
    intersection[1] = 0;
    intersection[2] = 0;

    if ( ( p2[horizontalCoord] - p1[horizontalCoord] ) != 0 && ( p4[horizontalCoord] - p3[horizontalCoord] ) != 0)
    //not vertical
    {
        b = ( p2[verticalCoord] - p1[verticalCoord] ) / ( p2[horizontalCoord] - p1[horizontalCoord] );
        v = ( p4[verticalCoord] - p3[verticalCoord] ) / ( p4[horizontalCoord] - p3[horizontalCoord] );

        a = p1[verticalCoord] - ( b * p1[horizontalCoord] ) ;
        u = p3[verticalCoord] - ( v * p3[horizontalCoord] ) ;

        if ( ( b - v ) != 0)
        //not parallel
        {
            intersection[horizontalCoord] = ( ( - ( a - u ) ) / ( b - v ) );
            intersection[verticalCoord] = ( a + ( b * intersection[horizontalCoord] ) );
            intersection[thirdCoord] = p1[thirdCoord];

            state = INTERSECT;
            return intersection;
        }
        else
        {
            state = PARALLEL;
            return intersection;
        }
    }
    else
    {
        if( ( p2[horizontalCoord] - p1[horizontalCoord] ) != 0 )
        //line 2 is vertical
        {
            //line 1 parametric equation
            b = ( p2[verticalCoord] - p1[verticalCoord] ) / ( p2[horizontalCoord] - p1[horizontalCoord] );
            u = p3[verticalCoord] - ( v * p3[horizontalCoord] ) ;

            intersection[horizontalCoord] = p1[horizontalCoord];
            intersection[verticalCoord] = ( a + ( b * intersection[horizontalCoord] ) );
            intersection[thirdCoord] = p1[thirdCoord];

            state = INTERSECT;
            return intersection;

        }
        else if( ( p4[horizontalCoord] - p3[horizontalCoord] ) != 0 )
        //line 1 is vertical
        {
            //line 2 parametric equation
            v = ( p4[verticalCoord] - p3[verticalCoord] ) / ( p4[horizontalCoord] - p3[horizontalCoord] );
            a = p1[verticalCoord] - ( b * p1[horizontalCoord] ) ;

            intersection[horizontalCoord] = p1[horizontalCoord];
            intersection[verticalCoord] = ( u + ( v * intersection[horizontalCoord] ) );
            intersection[thirdCoord] = p1[thirdCoord];

            state = INTERSECT;
            return intersection;
        }
        else
        //both lines vertical, so -> parallel
        {
            state = PARALLEL;
            return intersection;
        }
    }

    return intersection;
}

}
