/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "qmpr3dextension.h"
#include "q3dmprviewer.h"
#include "logging.h"
#include "toolmanager.h"
#include "volume.h"
#include "series.h"
#include <QToolButton>
#include <QAction>

namespace udg {

QMPR3DExtension::QMPR3DExtension( QWidget *parent )
 : QWidget( parent )
{
    setupUi( this );

    initializeTools();
    createConnections();

    m_axialViewEnabledButton->setChecked( true );
    m_sagitalViewEnabledButton->setChecked( true );
    m_coronalViewEnabledButton->setChecked( true );
    m_mpr3DView->orientationMarkerOff();
}

QMPR3DExtension::~QMPR3DExtension()
{
}

void QMPR3DExtension::initializeTools()
{
    m_toolManager = new ToolManager(this);
    // obtenim les accions de cada tool que volem
    m_zoomToolButton->setDefaultAction( m_toolManager->getToolAction("ZoomTool") );
    m_rotate3DToolButton->setDefaultAction( m_toolManager->getToolAction("Rotate3DTool") );
    m_moveToolButton->setDefaultAction( m_toolManager->getToolAction("TranslateTool") );
    m_screenShotToolButton->setDefaultAction( m_toolManager->getToolAction("ScreenShotTool") );

    // Activem les tools que volem tenir per defecte, això és com si clickéssim a cadascun dels ToolButton
    m_zoomToolButton->defaultAction()->trigger();
    m_moveToolButton->defaultAction()->trigger();
    m_rotate3DToolButton->defaultAction()->trigger();

    // registrem al manager les tools que van amb el viewer principal
    QStringList toolsList;
    toolsList << "ZoomTool" << "TranslateTool" << "Rotate3DTool" << "ScreenShotTool";
    m_toolManager->setViewerTools( m_mpr3DView, toolsList );
    m_toolManager->refreshConnections();
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

    connect( m_mpr3DView, SIGNAL( volumeChanged(Volume *) ), SLOT( updateExtension(Volume *) ) );
}

void QMPR3DExtension::setInput( Volume *input )
{
    m_volume = input;
    m_mpr3DView->setInput( m_volume );
    updateExtension(m_volume);
    INFO_LOG("QMPR3DExtension:: Donem Input ");
}

void QMPR3DExtension::updateExtension( Volume * volume )
{
    m_volume = volume;
    if( m_volume->getSeries()->getNumberOfPhases() > 1 )
        m_phasesAlertLabel->setVisible(true);
    else
        m_phasesAlertLabel->setVisible(false);

    double wl[2];
    m_mpr3DView->getWindowLevel( wl );
    m_windowLevelComboBox->updateWindowLevel( wl[0] , wl[1] );
    // TODO faltaria actualitzar els botons que calgui i/o altre elements de l'interfície

}

};  // end namespace udg
