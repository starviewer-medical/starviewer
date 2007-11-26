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
    m_q2dviewer = dynamic_cast<Q2DViewer*>(viewer);
    m_lastSlice = m_q2dviewer->getCurrentSlice();
    connect( viewer, SIGNAL( sliceChanged( int ) ), this, SLOT( setIncrement( int ) ) );
    connect( viewer, SIGNAL( windowLevelChanged( double, double ) ), this, SLOT( setWindowLevel( double, double ) ) );
    connect( viewer, SIGNAL( zoomFactorChanged( double ) ), this, SLOT( setZoomFactor( double ) ) );
    setToolData( new SynchronizeToolData() );
}

SynchronizeTool::~SynchronizeTool()
{

}

void SynchronizeTool::setToolData( ToolData *data )
{
    if( m_toolData )
    {
        disconnect( m_toolData, SIGNAL(sliceChanged( ) ), this, SLOT( applySliceChanges() ) );
        disconnect( m_toolData, SIGNAL(windowLevelChanged( ) ), this, SLOT( applyWindowLevelChanges() ) );
        disconnect( m_toolData, SIGNAL(zoomFactorChanged( ) ), this, SLOT( applyZoomFactorChanges() ) );
    }
    this->m_toolData = dynamic_cast<SynchronizeToolData*>(data);
    connect( m_toolData, SIGNAL(sliceChanged( ) ), this, SLOT( applySliceChanges() ) );
    connect( m_toolData, SIGNAL(windowLevelChanged( ) ), this, SLOT( applyWindowLevelChanges() ) );
    connect( m_toolData, SIGNAL(zoomFactorChanged( ) ), this, SLOT( applyZoomFactorChanges() ) );
}

ToolData * SynchronizeTool::getToolData() const
{
    return this->m_toolData;
}

void SynchronizeTool::setIncrement( int slice )
{
    int increment = slice - m_lastSlice;
    m_lastSlice = slice;
    disconnect( m_toolData, SIGNAL(sliceChanged( ) ), this, SLOT( applySliceChanges() ) );
    this->m_toolData->setIncrement( increment );
    connect( m_toolData, SIGNAL(sliceChanged( ) ), this, SLOT( applySliceChanges() ) );

}

void SynchronizeTool::setWindowLevel( double window , double level )
{
    disconnect( m_toolData, SIGNAL(windowLevelChanged( ) ), this, SLOT( applyWindowLevelChanges() ) );
    this->m_toolData->setWindowLevel( window , level );
    connect( m_toolData, SIGNAL(windowLevelChanged( ) ), this, SLOT( applyWindowLevelChanges() ) );

}

void SynchronizeTool::setZoomFactor( double factor )
{
    disconnect( m_toolData, SIGNAL(zoomFactorChanged() ), this, SLOT( applyZoomFactorChanges() ) );
    this->m_toolData->setZoomFactor( factor );
    connect( m_toolData, SIGNAL( zoomFactorChanged() ), this, SLOT( applyZoomFactorChanges() ) );
}

void SynchronizeTool::applySliceChanges()
{
    int increment = this->m_toolData->getIncrement();
    disconnect( m_viewer, SIGNAL(sliceChanged( int ) ), this, SLOT( setIncrement( int ) ) );
    m_q2dviewer->setSlice( m_lastSlice+increment );
    m_lastSlice += increment;
    connect( m_viewer, SIGNAL(sliceChanged( int ) ), this, SLOT( setIncrement( int ) ) );
}

void SynchronizeTool::applyWindowLevelChanges()
{
    disconnect( m_viewer, SIGNAL( windowLevelChanged( double, double ) ), this, SLOT( setWindowLevel( double, double ) ) );
    m_q2dviewer->setWindowLevel( this->m_toolData->getWindow(), this->m_toolData->getLevel() );
    connect( m_viewer, SIGNAL( windowLevelChanged( double, double ) ), this, SLOT( setWindowLevel( double, double ) ) );
}

void SynchronizeTool::applyZoomFactorChanges()
{
    disconnect( m_viewer, SIGNAL( zoomFactorChanged( double ) ), this, SLOT( setZoomFactor( double ) ) );
    m_q2dviewer->zoom( this->m_toolData->getZoomFactor() );
    connect( m_viewer, SIGNAL( zoomFactorChanged( double ) ), this, SLOT( setZoomFactor( double ) ) );
}

void SynchronizeTool::handleEvent( unsigned long eventID )
{
   
}
}
