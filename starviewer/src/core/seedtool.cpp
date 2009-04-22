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
        // desfem els vincles anteriors
        delete m_myData;
    
        // creem de nou les dades
        m_toolData = data;
        m_myData = qobject_cast<SeedToolData *>(data);
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
    // TODO Apanyo perquè funcioni de momen, però s'ha d'arreglar
    // s'hauria d'emetre únicament "seedChanged()" i prou
    m_2DViewer->setSeedPosition( xyz );
    emit seedChanged(seedPosition[0],seedPosition[1],seedPosition[2]);
    
    m_2DViewer->getDrawer()->draw( m_myData->getPoint() , m_2DViewer->getView(), m_2DViewer->getCurrentSlice() );
}

ToolData *SeedTool::getToolData() const
{
    return m_myData;
}

}


