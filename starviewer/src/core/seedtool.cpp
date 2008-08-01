/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "seedtool.h"
#include "seedtooldata.h"
#include "q2dviewer.h"
#include "drawerpoint.h"
#include "volume.h"
#include "drawer.h"
#include "drawerpoint.h"

// Vtk's
#include <vtkCommand.h>
// Qt's
#include <QVector>


namespace udg {

SeedTool::SeedTool( QViewer *viewer, QObject *parent ) : Tool( viewer, parent )
{
    m_toolName = "SeedTool";
    m_hasSharedData = false;

    m_myData = new SeedToolData;

    m_2DViewer = qobject_cast<Q2DViewer *>(viewer);
    if( !m_2DViewer )
        DEBUG_LOG(QString("El casting no ha funcionat!!! És possible que viewer no sigui un Q2DViewer!!!-> ") + viewer->metaObject()->className() );

    m_point = NULL;
    m_state = NONE;

    DEBUG_LOG("SEED TOOL CREADA ");
}


SeedTool::~SeedTool()
{
}

void SeedTool::handleEvent( unsigned long eventID )
{
    switch( eventID )
    {
    case vtkCommand::LeftButtonPressEvent:
        setSeed();
    break;

    case vtkCommand::MouseMoveEvent:
        doSeeding();
    break;

    case vtkCommand::LeftButtonReleaseEvent:
        endSeeding();
    break;

    default:
    break;
    }
}

void SeedTool::setSeed()
{
    if( m_2DViewer )
    {
        m_state=SEEDING;

        QVector<double> seedPosition(3);
        double xyz[3];
        m_2DViewer->getCurrentCursorPosition( xyz );
        seedPosition[0]=xyz[0];
        seedPosition[1]=xyz[1];
        seedPosition[2]=xyz[2];

        //es calcula correctament el valor de profunditat per a corretgir el bug #245
        int slice = m_2DViewer->getCurrentSlice();
        double *spacing = m_2DViewer->getInput()->getSpacing();
        double *origin = m_2DViewer->getInput()->getOrigin();

            switch( m_2DViewer->getView() )
            {
                case Q2DViewer::Axial:
                    seedPosition[2] = origin[2] + (slice * spacing[2]);
                    xyz[2] = seedPosition[2];
                break;
                case Q2DViewer::Sagital:
                    seedPosition[0] = origin[0] + (slice * spacing[0]);
                    xyz[0] = seedPosition[0];
                break;
                case Q2DViewer::Coronal:
                    seedPosition[1] = origin[1] + (slice * spacing[1]);
                    xyz[1] = seedPosition[1];
                break;
            }

        m_myData->setSeedPosition( seedPosition );
        //Apanyo perquè funcioni de moment, però s'ha d'arreglar
        m_2DViewer->setSeedPosition( xyz );

        if ( !m_point )
        {
            m_point = new DrawerPoint;
            QColor color( 217, 33, 66 );
            m_point->setColor( color );
        }
        m_point->setPosition(seedPosition);
        m_2DViewer->getDrawer()->draw( m_point , m_2DViewer->getView(), m_2DViewer->getCurrentSlice() );
    }
}

void SeedTool::doSeeding( )
{
    if( m_2DViewer && m_state==SEEDING )
    {
        QVector<double> seedPosition(3);
        double xyz[3];
        m_2DViewer->getCurrentCursorPosition( xyz );
        seedPosition[0]=xyz[0];
        seedPosition[1]=xyz[1];
        seedPosition[2]=xyz[2];

        //es calcula correctament el valor de profunditat per a corretgir el bug #245
        int slice = m_2DViewer->getCurrentSlice();
        double *spacing = m_2DViewer->getInput()->getSpacing();
        double *origin = m_2DViewer->getInput()->getOrigin();

            switch( m_2DViewer->getView() )
            {
                case Q2DViewer::Axial:
                    seedPosition[2] = origin[2] + (slice * spacing[2]);
                    xyz[2] = seedPosition[2];
                break;
                case Q2DViewer::Sagital:
                    seedPosition[0] = origin[0] + (slice * spacing[0]);
                    xyz[0] = seedPosition[0];
                break;
                case Q2DViewer::Coronal:
                    seedPosition[1] = origin[1] + (slice * spacing[1]);
                    xyz[1] = seedPosition[1];
                break;
            }

        m_myData->setSeedPosition( seedPosition );
        //Apanyo perquè funcioni de moment, però s'ha d'arreglar
        m_2DViewer->setSeedPosition( xyz );

        if ( !m_point )
        {
            m_point = new DrawerPoint;
            QColor color( 217, 33, 66 );
            m_point->setColor( color );
        }
        m_point->setPosition(seedPosition);
        m_2DViewer->getDrawer()->draw( m_point , m_2DViewer->getView(), m_2DViewer->getCurrentSlice() );
        m_2DViewer->getDrawer()->refresh();
    }
}

void SeedTool::endSeeding( )
{
    m_state = NONE;
}

ToolData *SeedTool::getToolData() const
{
    return m_myData;
}


}


