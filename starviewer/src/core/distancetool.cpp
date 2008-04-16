/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "distancetool.h"

#include "q2dviewer.h"
#include "logging.h"
#include "drawer.h"
#include "drawerline.h"
#include "drawertext.h"

// Vtk's
#include <vtkRenderWindowInteractor.h>
#include <vtkCommand.h>
#include <vtkLine.h>

namespace udg {

DistanceTool::DistanceTool( QViewer *viewer, QObject *parent )
 : Tool(viewer, parent)
{
    m_toolName = "DistanceTool";
    m_hasSharedData = false;

    m_2DViewer = qobject_cast<Q2DViewer *>( viewer );
    if( !m_2DViewer )
        DEBUG_LOG(QString("El casting no ha funcionat!!! És possible que viewer no sigui un Q2DViewer!!!-> ")+ viewer->metaObject()->className() );

    m_line = NULL;
    m_hasFirstPoint = false;
    m_hasSecondPoint = false;

    //DEBUG_LOG("DISTANCE TOOL CREADA ");
}

DistanceTool::~DistanceTool()
{
    if ( m_line )
    {
        delete m_line;
        m_2DViewer->getDrawer()->refresh();
    }
}

void DistanceTool::handleEvent( long unsigned eventID )
{
    switch( eventID )
    {
        case vtkCommand::LeftButtonPressEvent:
            this->annotateNewPoint();
            m_2DViewer->getDrawer()->refresh();
            break;
        case vtkCommand::MouseMoveEvent:
            if( m_line )
            {
                this->simulateLine();
                m_2DViewer->getDrawer()->refresh();
            }
            break;
        case vtkCommand::LeaveEvent:
            //en aquest cas considerem que l'usuari no vol anotar, per tant ens carreguem les primitives.
            if ( !m_line.isNull() )
                delete m_line;
        
            m_2DViewer->getDrawer()->refresh();
            break;
    }
}

void DistanceTool::annotateNewPoint()
{
    if ( !m_line )
    {
        m_line = new DrawerLine;
        m_hasFirstPoint = false;
        m_hasSecondPoint = false;
    }

    double position[4];
    double computed[3];

    //capturem l'event de clic esquerre
    int x = m_2DViewer->getEventPositionX();
    int y = m_2DViewer->getEventPositionY();
    m_2DViewer->computeDisplayToWorld( m_2DViewer->getRenderer() , x, y, 0, position );
    computed[0] = position[0];
    computed[1] = position[1];
    computed[2] = position[2];

    //afegim el punt
    if( !m_hasFirstPoint )
    {
        m_line->setFirstPoint( computed );
        m_hasFirstPoint = true;
    }
    else
    {
        m_line->setSecondPoint( computed );
        //actualitzem els atributs de la linia

        if( !m_hasSecondPoint )
        {
            m_2DViewer->getDrawer()->draw( m_line , m_2DViewer->getView(), m_2DViewer->getCurrentSlice() );
            m_hasSecondPoint = true;
        }
        else
            m_line->update( DrawerPrimitive::VTKRepresentation );

        //Posem el text

        double *middlePoint = m_line->getMiddlePoint();

        DrawerText * text = new DrawerText;
        text->setText( tr("%1 mm").arg( m_line->computeDistance() ) );
        text->setAttatchmentPoint( middlePoint );
        text->update( DrawerPrimitive::VTKRepresentation );
        m_2DViewer->getDrawer()->draw( text , m_2DViewer->getView(), m_2DViewer->getCurrentSlice() );

        m_line = NULL;//Acabem la linia. Encara no sabem com s'obtindran per modificar
    }

}

void DistanceTool::simulateLine()
{
    double position[4];
    double computed[3];

    //capturem l'event de clic esquerre
    int x = m_2DViewer->getEventPositionX();
    int y = m_2DViewer->getEventPositionY();
    m_2DViewer->computeDisplayToWorld( m_2DViewer->getRenderer() , x, y, 0, position );
    computed[0] = position[0];
    computed[1] = position[1];
    computed[2] = position[2];

    m_line->setSecondPoint( computed );
    //actualitzem els atributs de la linia

    if( !m_hasSecondPoint )
    {
        m_2DViewer->getDrawer()->draw( m_line , m_2DViewer->getView(), m_2DViewer->getCurrentSlice() );
        m_hasSecondPoint = true;
    }
    else
        m_line->update( DrawerPrimitive::VTKRepresentation );

}

}