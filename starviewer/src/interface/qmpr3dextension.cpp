/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "qmpr3dextension.h"
#include "q3dmprviewer.h"
#include "logging.h"
#include "toolsactionfactory.h"
#include <QToolButton>
#include <QMessageBox>
#include <QAction>

namespace udg {

QMPR3DExtension::QMPR3DExtension( QWidget *parent )
 : QWidget( parent )
{
    setupUi( this );

    createTools();
    createConnections();

    m_axialViewEnabledButton->setChecked( true );
    m_sagitalViewEnabledButton->setChecked( true );
    m_coronalViewEnabledButton->setChecked( true );
    m_mpr3DView->orientationMarkerOff();
}


QMPR3DExtension::~QMPR3DExtension()
{
}

void QMPR3DExtension::createTools()
{
    m_mpr3DView->enableTools();
    m_actionFactory = new ToolsActionFactory( 0 );

    m_zoomAction = m_actionFactory->getActionFrom( "ZoomTool" );
    m_zoomToolButton->setDefaultAction( m_zoomAction );

    m_moveAction = m_actionFactory->getActionFrom( "TranslateTool" );
    m_moveToolButton->setDefaultAction( m_moveAction );

    m_screenShotAction = m_actionFactory->getActionFrom( "ScreenShotTool" );
    m_screenShotToolButton->setDefaultAction( m_screenShotAction );

    m_rotate3DAction = m_actionFactory->getActionFrom( "3DRotationTool" );
    m_rotate3DToolButton->setDefaultAction( m_rotate3DAction );

    connect( m_actionFactory , SIGNAL( triggeredTool(QString) ) , m_mpr3DView , SLOT( setTool(QString) ) );

    m_toolsActionGroup = new QActionGroup( 0 );
    m_toolsActionGroup->setExclusive( true );
    m_toolsActionGroup->addAction( m_zoomAction );
    m_toolsActionGroup->addAction( m_moveAction );
    m_toolsActionGroup->addAction( m_screenShotAction );
    m_toolsActionGroup->addAction( m_rotate3DAction );
    // activem la tool de zoom per defecte
    m_zoomAction->trigger();
}

void QMPR3DExtension::createConnections()
{
    connect( m_axialViewEnabledButton , SIGNAL( toggled(bool) ) , m_mpr3DView , SLOT( setAxialVisibility(bool) ) );
    connect( m_sagitalViewEnabledButton , SIGNAL( toggled(bool) ) , m_mpr3DView , SLOT( setSagitalVisibility(bool) ) );
    connect( m_coronalViewEnabledButton , SIGNAL( toggled(bool) ) , m_mpr3DView , SLOT( setCoronalVisibility(bool) ) );

    connect( m_sagitalOrientationButton , SIGNAL( clicked() ) , m_mpr3DView , SLOT( resetViewToSagital() ) );
    connect( m_coronalOrientationButton , SIGNAL( clicked() ) , m_mpr3DView , SLOT( resetViewToCoronal() ) );
    connect( m_axialOrientationButton , SIGNAL( clicked() ) , m_mpr3DView , SLOT( resetViewToAxial() ) );

    connect( m_windowLevelComboBox , SIGNAL( windowLevel(double,double) ) , m_mpr3DView , SLOT( setWindowLevel(double,double) ) );
    connect( m_windowLevelComboBox , SIGNAL( defaultValue() ) , m_mpr3DView , SLOT( resetWindowLevelToDefault() ) );
}

void QMPR3DExtension::setInput( Volume *input )
{
    m_volume = input;
    m_mpr3DView->setInput( m_volume );
    double wl[2];
    m_mpr3DView->getWindowLevel( wl );
    m_windowLevelComboBox->updateWindowLevel( wl[0] , wl[1] );
    INFO_LOG("QMPR3DExtension:: Donem Input ");
}

};  // end namespace udg
