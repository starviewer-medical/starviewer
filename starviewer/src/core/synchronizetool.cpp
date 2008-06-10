/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "synchronizetool.h"
#include "q2dviewer.h"
#include "synchronizetooldata.h"
#include "toolconfiguration.h"
#include "logging.h"

namespace udg {

SynchronizeTool::SynchronizeTool( QViewer *viewer, QObject *parent)
: Tool( viewer, parent )
{
    m_toolData = 0;
    m_toolName = "SynchronizeTool";
    m_hasSharedData = true;
    m_q2dviewer = dynamic_cast<Q2DViewer*>(viewer);
    m_lastSlice = m_q2dviewer->getCurrentSlice();
    m_roundLostThickness = 0.0;

    connect( viewer, SIGNAL( sliceChanged( int ) ), SLOT( setIncrement( int ) ) );
    connect( viewer, SIGNAL( windowLevelChanged( double, double ) ), SLOT( setWindowLevel( double, double ) ) );
    connect( viewer, SIGNAL( zoomFactorChanged( double ) ), SLOT( setZoomFactor( double ) ) );
    connect( viewer, SIGNAL( panChanged( double * ) ), SLOT( setPan( double * ) ) );

    setToolData( new SynchronizeToolData() );
}

SynchronizeTool::~SynchronizeTool()
{

}

void SynchronizeTool::setConfiguration( ToolConfiguration *configuration )
{
    if( !( configuration->containsValue( "Slicing" ) ) )
        configuration->addAttribute( "Slicing", QVariant( false ) );
    if( !( configuration->containsValue( "WindowLevel" ) ) )
        configuration->addAttribute( "WindowLevel", QVariant( false ) );
    if( !( configuration->containsValue( "ZoomFactor" ) ) )
        configuration->addAttribute( "ZoomFactor", QVariant( false ) );
    if( !( configuration->containsValue( "Pan" ) ) )
        configuration->addAttribute( "Pan", QVariant( false ) );

    m_toolConfiguration = configuration;
}

void SynchronizeTool::setToolData( ToolData *data )
{
    if( m_toolData )
    {
        disconnect( m_toolData, SIGNAL(sliceChanged( ) ), this, SLOT( applySliceChanges() ) );
        disconnect( m_toolData, SIGNAL(windowLevelChanged( ) ), this, SLOT( applyWindowLevelChanges() ) );
        disconnect( m_toolData, SIGNAL(zoomFactorChanged( ) ), this, SLOT( applyZoomFactorChanges() ) );
        disconnect( m_toolData, SIGNAL(panChanged() ), this, SLOT( applyPanChanges() ) );
    }
    this->m_toolData = dynamic_cast<SynchronizeToolData*>(data);

    connect( m_toolData, SIGNAL(sliceChanged( ) ), SLOT( applySliceChanges() ) );
    connect( m_toolData, SIGNAL(windowLevelChanged( ) ), SLOT( applyWindowLevelChanges() ) );
    connect( m_toolData, SIGNAL(zoomFactorChanged( ) ), SLOT( applyZoomFactorChanges() ) );
    connect( m_toolData, SIGNAL(panChanged() ), SLOT( applyPanChanges() ) );
}

ToolData * SynchronizeTool::getToolData() const
{
    return this->m_toolData;
}

void SynchronizeTool::setIncrement( int slice )
{
    ToolConfiguration * configuration = getConfiguration();

    if( configuration && configuration->getValue( "Slicing" ).toBool() )
    {
        double increment = (slice - m_lastSlice)*m_q2dviewer->getThickness(); // Distancia incrementada
        m_lastSlice = slice;
        disconnect( m_toolData, SIGNAL(sliceChanged( ) ), this, SLOT( applySliceChanges() ) );
        this->m_toolData->setIncrement( increment );
        connect( m_toolData, SIGNAL(sliceChanged( ) ), SLOT( applySliceChanges() ) );
    }
}

void SynchronizeTool::setWindowLevel( double window , double level )
{
    ToolConfiguration * configuration = getConfiguration();

    if( configuration && configuration->getValue( "WindowLevel" ).toBool() )
    {
        disconnect( m_toolData, SIGNAL(windowLevelChanged( ) ), this, SLOT( applyWindowLevelChanges() ) );
        this->m_toolData->setWindowLevel( window , level );
        connect( m_toolData, SIGNAL(windowLevelChanged( ) ), SLOT( applyWindowLevelChanges() ) );
    }
}

void SynchronizeTool::setZoomFactor( double factor )
{
    ToolConfiguration * configuration = getConfiguration();

    if( configuration && configuration->getValue( "ZoomFactor" ).toBool() )
    {
        disconnect( m_toolData, SIGNAL(zoomFactorChanged() ), this, SLOT( applyZoomFactorChanges() ) );
        this->m_toolData->setZoomFactor( factor );
        connect( m_toolData, SIGNAL( zoomFactorChanged() ), SLOT( applyZoomFactorChanges() ) );
    }
}

void SynchronizeTool::setPan( double * motionVector )
{
    ToolConfiguration * configuration = getConfiguration();

    if( configuration && configuration->getValue( "Pan" ).toBool() )
    {
        disconnect( m_toolData, SIGNAL(panChanged() ), this, SLOT( applyPanChanges() ) );
        this->m_toolData->setPan( motionVector );
        connect( m_toolData, SIGNAL( panChanged() ), SLOT( applyPanChanges() ) );
    }
}

void SynchronizeTool::applySliceChanges()
{
    ToolConfiguration * configuration = getConfiguration();

    if( configuration && configuration->getValue( "Slicing" ).toBool() )
    {
        double sliceIncrement = (this->m_toolData->getIncrement()/m_q2dviewer->getThickness()) + m_roundLostThickness;
        int slices = qRound( sliceIncrement );
        m_roundLostThickness = sliceIncrement - slices;
        disconnect( m_viewer, SIGNAL(sliceChanged( int ) ), this, SLOT( setIncrement( int ) ) );
        m_q2dviewer->setSlice( m_lastSlice+slices );
        m_lastSlice += slices;
        connect( m_viewer, SIGNAL(sliceChanged( int ) ), SLOT( setIncrement( int ) ) );
    }
}

void SynchronizeTool::applyWindowLevelChanges()
{
    ToolConfiguration * configuration = getConfiguration();

    if( configuration && configuration->getValue( "WindowLevel" ).toBool() )
    {
        disconnect( m_viewer, SIGNAL( windowLevelChanged( double, double ) ), this, SLOT( setWindowLevel( double, double ) ) );
        m_q2dviewer->setWindowLevel( this->m_toolData->getWindow(), this->m_toolData->getLevel() );
        connect( m_viewer, SIGNAL( windowLevelChanged( double, double ) ), SLOT( setWindowLevel( double, double ) ) );
    }
}

void SynchronizeTool::applyZoomFactorChanges()
{
    ToolConfiguration * configuration = getConfiguration();

    if( configuration && configuration->getValue( "ZoomFactor" ).toBool() )
    {
        disconnect( m_viewer, SIGNAL( zoomFactorChanged( double ) ), this, SLOT( setZoomFactor( double ) ) );
        m_q2dviewer->zoom( this->m_toolData->getZoomFactor() );
        connect( m_viewer, SIGNAL( zoomFactorChanged( double ) ), SLOT( setZoomFactor( double ) ) );
    }
}

void SynchronizeTool::applyPanChanges()
{
    ToolConfiguration * configuration = getConfiguration();

    if( configuration && configuration->getValue( "Pan" ).toBool() )
    {
        disconnect( m_viewer, SIGNAL( panChanged( double * ) ), this, SLOT( setPan( double * ) ) );
        m_q2dviewer->pan( this->m_toolData->getPan() );
        connect( m_viewer, SIGNAL( panChanged( double * ) ), SLOT( setPan( double * ) ) );
    }
}

void SynchronizeTool::handleEvent( unsigned long eventID )
{
}

}
