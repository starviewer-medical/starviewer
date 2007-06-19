/***************************************************************************
 *   Copyright (C) 2007 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "qvolume3dviewtestingextension.h"
#include "volume.h"
#include "toolsactionfactory.h"

#include <QAction>

namespace udg {

QVolume3DViewTestingExtension::QVolume3DViewTestingExtension( QWidget * parent )
 : QWidget( parent )
{
    setupUi( this );

    createTools();
    createConnections();
}

QVolume3DViewTestingExtension::~QVolume3DViewTestingExtension()
{
}

void QVolume3DViewTestingExtension::createTools()
{
    m_3DView->enableTools();
    m_actionFactory = new ToolsActionFactory( 0 );

    m_zoomAction = m_actionFactory->getActionFrom( "ZoomTool" );
    m_zoomToolButton->setDefaultAction( m_zoomAction );

    m_moveAction = m_actionFactory->getActionFrom( "TranslateTool" );
    m_panToolButton->setDefaultAction( m_moveAction );

    m_rotate3DAction = m_actionFactory->getActionFrom( "3DRotationTool" );
    m_rotate3DToolButton->setDefaultAction( m_rotate3DAction );

    connect( m_actionFactory , SIGNAL( triggeredTool(QString) ) , m_3DView , SLOT( setTool(QString) ) );

    m_toolsActionGroup = new QActionGroup( 0 );
    m_toolsActionGroup->setExclusive( true );
    m_toolsActionGroup->addAction( m_zoomAction );
    m_toolsActionGroup->addAction( m_moveAction );
    m_toolsActionGroup->addAction( m_rotate3DAction );
    // activem la tool per defecte
    m_rotate3DAction->trigger();
}

void QVolume3DViewTestingExtension::createConnections()
{
    connect( m_axialOrientationButton , SIGNAL( clicked() ) , m_3DView , SLOT( resetViewToAxial() ) );
    connect( m_sagitalOrientationButton , SIGNAL( clicked() ) , m_3DView , SLOT( resetViewToSagital() ) );
    connect( m_coronalOrientationButton , SIGNAL( clicked() ) , m_3DView , SLOT( resetViewToCoronal() ) );
}

void QVolume3DViewTestingExtension::setInput( Volume * input )
{
    m_input = input;
    m_3DView->setInput( m_input );
    m_3DView->render();
}

}

