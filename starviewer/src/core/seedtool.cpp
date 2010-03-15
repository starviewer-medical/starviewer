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
#include "drawer.h"
#include "logging.h"
// Vtk's
#include <vtkCommand.h>
// Qt's
#include <QVector>

namespace udg {

SeedTool::SeedTool( QViewer *viewer, QObject *parent ) : Tool( viewer, parent )
{
    m_toolName = "SeedTool";
    m_hasSharedData = false;
    m_hasPersistentData = true;

    m_myData = new SeedToolData;

    m_2DViewer = qobject_cast<Q2DViewer *>(viewer);
    if( !m_2DViewer )
        DEBUG_LOG(QString("El casting no ha funcionat!!! És possible que viewer no sigui un Q2DViewer!!!-> ") + viewer->metaObject()->className() );

    m_state = NONE;
    m_drawn = false;
    m_myData->setVolume( m_2DViewer->getInput() );
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

void SeedTool::setToolData(ToolData * data)
{
    //Fem aquesta comparació perquè a vegades ens passa la data que ja tenim a m_myData
    if( m_myData != data )
    { 
        // creem de nou les dades
        m_toolData = data;
        m_myData = qobject_cast<SeedToolData *>(data);
        //si tenim dades vol dir que ja hem pintat abans la seed si el volume ha canviat
        if(m_2DViewer->getInput() != m_myData->getVolume())
        {
            //canvi de input
            m_drawn = false;
            m_myData->setVolume( m_2DViewer->getInput() );
            //si tenim dades vol dir que el viewer ha eliminat el punt pel que el posem a 0 perquè es torni a crear
            m_myData->setPoint( NULL );
        }else{
            //canvi de tool
            m_drawn = true;
            m_2DViewer->getDrawer()->erasePrimitive(m_myData->getPoint());
            m_myData->setPoint(NULL);
            m_myData->setSeedPosition(m_myData->getSeedPosition());
            m_2DViewer->getDrawer()->draw( m_myData->getPoint() , m_2DViewer->getView(), m_2DViewer->getCurrentSlice() );
        }
    }
}

void SeedTool::setSeed()
{
    Q_ASSERT( m_2DViewer );
    
    m_state=SEEDING;
    updateSeedPosition();
}

void SeedTool::doSeeding()
{
    Q_ASSERT( m_2DViewer );

    if( m_state==SEEDING )
    {
        updateSeedPosition();
    }
}

void SeedTool::endSeeding()
{
    m_state = NONE;
}

void SeedTool::updateSeedPosition()
{
    Q_ASSERT( m_2DViewer );

	double xyz[3];
    if( m_2DViewer->getCurrentCursorImageCoordinate( xyz ) )
    {
        QVector<double> seedPosition(3);
        seedPosition[0]=xyz[0];
        seedPosition[1]=xyz[1];
        seedPosition[2]=xyz[2];

        //DEBUG_LOG(QString("Seed Pos: [%1,%2,%3]").arg(seedPosition[0]).arg(seedPosition[1]).arg(seedPosition[2]));

        m_myData->setSeedPosition( seedPosition );
        // TODO Apanyo perquè funcioni de moment, però s'ha d'arreglar
        // s'hauria d'emetre únicament "seedChanged()" i prou
        m_2DViewer->setSeedPosition( xyz );
        emit seedChanged(seedPosition[0],seedPosition[1],seedPosition[2]);
        
        if(!m_drawn)
        {
            m_2DViewer->getDrawer()->draw( m_myData->getPoint() , m_2DViewer->getView(), m_2DViewer->getCurrentSlice() );
            m_drawn = true;
        }else{
            m_myData->getPoint()->update( DrawerPrimitive::VTKRepresentation );
            m_2DViewer->refresh();
            //m_2DViewer->getDrawer()->refresh();
        }

        //m_2DViewer->getDrawer()->draw( m_myData->getPoint() , m_2DViewer->getView(), m_2DViewer->getCurrentSlice() );
    }
}

void SeedTool::setSeed(QVector<double> seedPosition, int slice)
{
    Q_ASSERT( m_2DViewer );
    
    m_myData->setSeedPosition( seedPosition );
    double xyz[3];
    xyz[0]=seedPosition[0];
    xyz[1]=seedPosition[1];
    xyz[2]=seedPosition[2];
    m_2DViewer->setSeedPosition( xyz );
    emit seedChanged(seedPosition[0],seedPosition[1],seedPosition[2]);
    //DEBUG_LOG(QString("Seed Pos: [%1,%2,%3], slice = %4").arg(seedPosition[0]).arg(seedPosition[1]).arg(seedPosition[2]).arg(slice));
    m_2DViewer->getDrawer()->draw( m_myData->getPoint() , m_2DViewer->getView(), slice );
}

ToolData *SeedTool::getToolData() const
{
    return m_myData;
}

}


