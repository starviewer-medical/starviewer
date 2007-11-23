/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "synchronizetool.h"
#include "q2dviewer.h"
#include "synchronizetooldata.h"
#include "logging.h"

//TODO treure aixoooooooo
#include <vtkMath.h>

namespace udg {

SynchronizeTool::SynchronizeTool( QViewer *viewer, QObject *parent)
: Tool( viewer, parent )
{
    m_toolData = 0;
    m_toolName = "SynchronizeTool";
    m_hasSharedData = true;
    m_q2viewer = dynamic_cast<Q2DViewer*>(viewer);
    m_lastSlice = m_q2viewer->getCurrentSlice();
    connect( viewer, SIGNAL(sliceChanged( int ) ), this, SLOT( setIncrement( int ) ) );
    setToolData( new SynchronizeToolData() );
}

SynchronizeTool::~SynchronizeTool()
{

}

void SynchronizeTool::setToolData( ToolData *data )
{
    if( m_toolData )
    {
        disconnect( m_toolData, SIGNAL(dataChanged( ) ), this, SLOT( applyChanges() ) );
    }
    this->m_toolData = dynamic_cast<SynchronizeToolData*>(data);
    connect( m_toolData, SIGNAL(dataChanged( ) ), this, SLOT( applyChanges() ) );
}

ToolData * SynchronizeTool::getToolData() const
{
    return this->m_toolData;
}

void SynchronizeTool::setIncrement( int slice )
{
    int increment = slice - m_lastSlice;
    m_lastSlice = slice;
    disconnect( m_toolData, SIGNAL(dataChanged( ) ), this, SLOT( applyChanges() ) );
    this->m_toolData->setIncrement( increment );
    connect( m_toolData, SIGNAL(dataChanged( ) ), this, SLOT( applyChanges() ) );

}

void SynchronizeTool::applyChanges()
{
    int increment = this->m_toolData->getIncrement();
    disconnect( m_viewer, SIGNAL(sliceChanged( int ) ), this, SLOT( setIncrement( int ) ) );
    m_q2viewer->setSlice( m_lastSlice+increment );
    m_lastSlice += increment;
    connect( m_viewer, SIGNAL(sliceChanged( int ) ), this, SLOT( setIncrement( int ) ) );

}

void SynchronizeTool::handleEvent( unsigned long eventID )
{
   
}
}
