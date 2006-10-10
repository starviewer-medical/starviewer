/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "q2dviewerextension.h"

#include "volume.h"
#include "logging.h"
#include "qwindowlevelcombobox.h"
#include <QAction>
#include <QToolBar>
#include <QSignalMapper>
// VTK
#include <vtkRenderer.h>

namespace udg {

Q2DViewerExtension::Q2DViewerExtension( QWidget *parent )
 : QWidget( parent )
{
    setupUi( this );
    m_mainVolume = 0;
    m_secondaryVolume = 0;
    
    createActions();
    createToolBars();
    createConnections();
}

Q2DViewerExtension::~Q2DViewerExtension()
{
}

void Q2DViewerExtension::createActions()
{
    m_signalMapper = new QSignalMapper( this );
    
    m_axialViewAction = new QAction( 0 );
    m_axialViewAction->setText( tr("&Axial View") );
    m_axialViewAction->setShortcut( tr("Ctrl+A") );
    m_axialViewAction->setStatusTip( tr("Change Current View To Axial") );
    m_axialViewAction->setIcon( QIcon(":/images/axial.png") );
    m_axialViewToolButton->setDefaultAction( m_axialViewAction );
    
    m_sagitalViewAction = new QAction( 0 );
    m_sagitalViewAction->setText( tr("&Sagital View") );
    m_sagitalViewAction->setShortcut( tr("Ctrl+S") );
    m_sagitalViewAction->setStatusTip( tr("Change Current View To Sagital") );
    m_sagitalViewAction->setIcon( QIcon(":/images/sagital.png") );
    m_sagitalViewToolButton->setDefaultAction( m_sagitalViewAction );
    
    m_coronalViewAction = new QAction( 0 );
    m_coronalViewAction->setText( tr("&Coronal View") );
    m_coronalViewAction->setShortcut( tr("Ctrl+C") );
    m_coronalViewAction->setStatusTip( tr("Change Current View To Coronal") );
    m_coronalViewAction->setIcon( QIcon(":/images/coronal.png") );
    m_coronalViewToolButton->setDefaultAction( m_coronalViewAction );

    m_singleViewAction = new QAction( 0 );
    m_singleViewAction->setText( tr("&Single View") );
    m_singleViewAction->setShortcut( tr("Ctrl+S") );
    m_singleViewAction->setStatusTip( tr("Change To Single View Mode") );
    m_singleViewAction->setIcon( QIcon(":/images/singleView.png") );
    m_singleViewToolButton->setDefaultAction( m_singleViewAction );

    m_doubleViewAction = new QAction( 0 );
    m_doubleViewAction->setText( tr("&Double View") );
    m_doubleViewAction->setShortcut( tr("Ctrl+D") );
    m_doubleViewAction->setStatusTip( tr("Change To Double View Mode") );
    m_doubleViewAction->setIcon( QIcon(":/images/addViewRight.png") );
    m_doubleViewToolButton->setDefaultAction( m_doubleViewAction );

    // Tools
    // \TODO aquestes accions ens les hauria de donar cada tool, o el tool manager dels visualitzadors
    m_slicingAction = new QAction( 0 );
    m_slicingAction->setText( tr("Slicer") );
    m_slicingAction->setStatusTip( tr("Enable/Disable slicing tool") );
    m_slicingAction->setIcon( QIcon(":/images/slicing.png") );
    m_signalMapper->setMapping( m_slicingAction , "Slicing2DTool" );
    connect( m_slicingAction , SIGNAL( triggered() ) , m_signalMapper , SLOT( map() ) );
    m_slicingToolButton->setDefaultAction( m_slicingAction );

    m_windowLevelAction = new QAction( 0 );
    m_windowLevelAction->setText( tr("Window Level") );
    m_windowLevelAction->setStatusTip( tr("Enable/Disable Window Level tool") );
    m_windowLevelAction->setIcon( QIcon(":/images/windowLevel.png") );
    m_signalMapper->setMapping( m_windowLevelAction , "WindowLevelTool" );
    connect( m_windowLevelAction , SIGNAL( triggered() ) , m_signalMapper , SLOT( map() ) );
    m_windowLevelToolButton->setDefaultAction( m_windowLevelAction );

    m_zoomAction = new QAction( 0 );
    m_zoomAction->setText( tr("Zoom") );
    m_zoomAction->setStatusTip( tr("Enable/Disable Zoom tool") );
    m_zoomAction->setIcon( QIcon(":/images/zoom.png") );
    m_signalMapper->setMapping( m_zoomAction , "ZoomTool" );
    connect( m_zoomAction , SIGNAL( triggered() ) , m_signalMapper , SLOT( map() ) );
    m_zoomToolButton->setDefaultAction( m_zoomAction );

    m_moveAction = new QAction( 0 );
    m_moveAction->setText( tr("Move") );
    m_moveAction->setStatusTip( tr("Enable/Disable Move tool") );
    m_moveAction->setIcon( QIcon(":/images/move.png") );
    m_signalMapper->setMapping( m_moveAction , "MoveTool" );
    connect( m_moveAction , SIGNAL( triggered() ) , m_signalMapper , SLOT( map() ) );
    m_moveToolButton->setDefaultAction( m_moveAction );
    
    connect( m_signalMapper, SIGNAL( mapped(QString) ), m_2DView , SLOT( setTool(QString) ) );
}

void Q2DViewerExtension::createToolBars()
{
    m_toolsToolBar = new QToolBar(0);
    m_toolsToolBar->addAction( m_axialViewAction );
    m_toolsToolBar->addAction( m_sagitalViewAction );
    m_toolsToolBar->addAction( m_coronalViewAction );
    
}

void Q2DViewerExtension::createConnections()
{
    connect( m_slider , SIGNAL( valueChanged(int) ) , m_spinBox , SLOT( setValue(int) ) );
    connect( m_spinBox , SIGNAL( valueChanged(int) ) , m_2DView , SLOT( setSlice(int) ) );
    connect( m_2DView , SIGNAL( sliceChanged(int) ) , m_slider , SLOT( setValue(int) ) );
    
    // adicionals, \TODO ara es fa "a saco" però s'ha de millorar
    connect( m_slider2_1 , SIGNAL( valueChanged(int) ) , m_spinBox2_1 , SLOT( setValue(int) ) );
    connect( m_spinBox2_1 , SIGNAL( valueChanged(int) ) , m_2DView2_1 , SLOT( setSlice(int) ) );
    connect( m_2DView2_1 , SIGNAL( sliceChanged(int) ) , m_slider2_1 , SLOT( setValue(int) ) );

    connect( m_slider2_2 , SIGNAL( valueChanged(int) ) , m_spinBox2_2 , SLOT( setValue(int) ) );
    connect( m_spinBox2_2 , SIGNAL( valueChanged(int) ) , m_2DView2_2 , SLOT( setSlice(int) ) );
    connect( m_2DView2_2 , SIGNAL( sliceChanged(int) ) , m_slider2_2 , SLOT( setValue(int) ) );

    // sincronisme window level \TODO veure perquè no hi ha sincronisme entre una pàgina i l'altre
    connect( m_2DView , SIGNAL( windowLevelChanged( double , double ) ) , m_2DView2_1 , SLOT( setWindowLevel( double , double ) ) );
    connect( m_2DView2_1 , SIGNAL( windowLevelChanged( double , double ) ) , m_2DView2_2 , SLOT( setWindowLevel( double , double ) ) );
    connect( m_2DView2_1 , SIGNAL( windowLevelChanged( double , double ) ) , m_2DView , SLOT( setWindowLevel( double , double ) ) );
    connect( m_2DView2_2 , SIGNAL( windowLevelChanged( double , double ) ) , m_2DView2_1 , SLOT( setWindowLevel( double , double ) ) );
    connect( m_2DView2_2 , SIGNAL( windowLevelChanged( double , double ) ) , m_2DView , SLOT( setWindowLevel( double , double ) ) );
    
    connect( m_axialViewAction , SIGNAL( triggered() ) , this , SLOT( changeViewToAxial() ) );
    connect( m_sagitalViewAction , SIGNAL( triggered() ) , this , SLOT( changeViewToSagital() ) );
    connect( m_coronalViewAction , SIGNAL( triggered() ) , this , SLOT( changeViewToCoronal() ) );

    connect( m_singleViewAction , SIGNAL( triggered() ) , this , SLOT( changeViewToSingle() ) );
    connect( m_doubleViewAction , SIGNAL( triggered() ) , this , SLOT( changeViewToDouble() ) );

    connect( m_stackedWidget , SIGNAL( currentChanged(int) ) , this , SLOT( pageChange(int) ) );

    connect( m_synchroCheckBox , SIGNAL( clicked(bool) ) , this , SLOT( synchronizeSlices(bool) ) );

    connect( m_chooseSeriePushButton , SIGNAL( clicked() ) , this , SLOT( chooseNewSerie() ) );

    // window level combo box
    connect( m_windowLevelComboBox , SIGNAL( windowLevel(double,double) ) , m_2DView , SLOT( setWindowLevel(double,double) ) );
    connect( m_windowLevelComboBox , SIGNAL( windowLevel(double,double) ) , m_2DView2_1 , SLOT( setWindowLevel(double,double) ) );
    connect( m_windowLevelComboBox , SIGNAL( windowLevel(double,double) ) , m_2DView2_2 , SLOT( setWindowLevel(double,double) ) );

    connect( m_windowLevelComboBox , SIGNAL( defaultValue() ) , m_2DView , SLOT( resetWindowLevelToDefault() ) );
    connect( m_windowLevelComboBox , SIGNAL( defaultValue() ) , m_2DView2_1 , SLOT( resetWindowLevelToDefault() ) );
    connect( m_windowLevelComboBox , SIGNAL( defaultValue() ) , m_2DView2_2 , SLOT( resetWindowLevelToDefault() ) );
}

void Q2DViewerExtension::setInput( Volume *input )
{
    m_mainVolume = input;
    // \TODO ara ho fem "a saco" però s'hauria de millorar
    m_2DView->setInput( m_mainVolume );
    m_2DView2_1->setInput( m_mainVolume );
    m_2DView2_2->setInput( m_mainVolume );
    double wl[2];
    m_2DView->getDefaultWindowLevel( wl );
    m_windowLevelComboBox->updateWindowLevel( wl[0] , wl[1] );
    INFO_LOG("Q2DViewerExtension: Donem l'input principal")
    changeViewToAxial();
    
}

void Q2DViewerExtension::setSecondInput( Volume *input )
{
    m_secondaryVolume = input;
    // \TODO ara ho fem "a saco" però s'hauria de millorar
    m_2DView2_2->setInput( m_secondaryVolume );
    INFO_LOG("Afegim un segon volum per comparar")
    changeViewToAxial();
    m_stackedWidget->setCurrentIndex( 1 );
}

void Q2DViewerExtension::populateToolBar( QToolBar *toolbar )
{
    if( toolbar )
    {
        toolbar->addAction( m_axialViewAction );
        toolbar->addAction( m_sagitalViewAction );
        toolbar->addAction( m_coronalViewAction );
    }
}

void Q2DViewerExtension::changeViewToAxial()
{
    m_currentView = Axial;
    int extent[6];
    m_mainVolume->getWholeExtent( extent );
    switch( m_stackedWidget->currentIndex() )
    {
    case 0:
        m_spinBox->setMinimum( extent[4] );
        m_spinBox->setMaximum( extent[5] );
        m_slider->setMaximum( extent[5] );
        m_viewText->setText( tr("XY : Axial") );
        m_2DView->setViewToAxial();
        INFO_LOG("Visor per defecte: Canviem a vista axial (Vista 1)")
        m_2DView->render();
    break;

    case 1:
        int secondExtent[6];
        if( m_secondaryVolume )
            m_secondaryVolume->getWholeExtent( secondExtent );
        else
            m_mainVolume->getWholeExtent( secondExtent );

        m_spinBox2_1->setMinimum( extent[4] );
        m_spinBox2_1->setMaximum( extent[5] );
        m_slider2_1->setMaximum( extent[5] );
        m_viewText2_1->setText( tr("XY : Axial") );
        m_2DView2_1->setViewToAxial();
        INFO_LOG("Visor per defecte: Canviem a vista axial (Vista 2.1)")
        m_2DView2_1->render();

        m_spinBox2_2->setMinimum( secondExtent[4] );
        m_spinBox2_2->setMaximum( secondExtent[5] );
        m_slider2_2->setMaximum( secondExtent[5] );
        m_viewText2_2->setText( tr("XY : Axial") );
        m_2DView2_2->setViewToAxial();
        INFO_LOG("Visor per defecte: Canviem a vista axial (Vista 2.2)")
        m_2DView2_2->render();
    break;
    
    }

}

void Q2DViewerExtension::changeViewToSagital()
{
    m_currentView = Sagital;
    int extent[6];
    m_mainVolume->getWholeExtent( extent );
    switch( m_stackedWidget->currentIndex() )
    {
    case 0:
        m_spinBox->setMinimum( extent[0] );
        m_spinBox->setMaximum( extent[1] );
        m_slider->setMaximum( extent[1] );
        m_viewText->setText( tr("YZ : Sagital") );
        m_2DView->setViewToSagittal();
        INFO_LOG("Visor per defecte: Canviem a vista sagital (Vista 1)")
        m_2DView->render();
    break;

    case 1:
        int secondExtent[6];
        if( m_secondaryVolume )
            m_secondaryVolume->getWholeExtent( secondExtent );
        else
            m_mainVolume->getWholeExtent( secondExtent );
            
        m_spinBox2_1->setMinimum( extent[0] );
        m_spinBox2_1->setMaximum( extent[1] );
        m_slider2_1->setMaximum( extent[1] );
        m_viewText2_1->setText( tr("YZ : Sagital") );
        m_2DView2_1->setViewToSagittal();
        INFO_LOG("Visor per defecte: Canviem a vista sagital (Vista 2.1)")
        m_2DView2_1->render();

        m_spinBox2_2->setMinimum( secondExtent[0] );
        m_spinBox2_2->setMaximum( secondExtent[1] );
        m_slider2_2->setMaximum( secondExtent[1] );
        m_viewText2_2->setText( tr("YZ : Sagital") );
        m_2DView2_2->setViewToSagittal();
        INFO_LOG("Visor per defecte: Canviem a vista sagital (Vista 2.2)")
        m_2DView2_2->render();
    break;
    }
}

void Q2DViewerExtension::changeViewToCoronal()
{
    m_currentView = Coronal;
    int extent[6];
    m_mainVolume->getWholeExtent( extent );
    switch( m_stackedWidget->currentIndex() )
    {
    case 0:
        m_spinBox->setMinimum( extent[2] );
        m_spinBox->setMaximum( extent[3] );
        m_slider->setMaximum( extent[3] );
        m_viewText->setText( tr("XZ : Coronal") );
        m_2DView->setViewToCoronal();
        INFO_LOG("Visor per defecte: Canviem a vista coronal (Vista 1)")
        m_2DView->render();
    break;
    
    case 1:
        int secondExtent[6];
        if( m_secondaryVolume )
            m_secondaryVolume->getWholeExtent( secondExtent );
        else
            m_mainVolume->getWholeExtent( secondExtent );
            
        m_spinBox2_1->setMinimum( extent[2] );
        m_spinBox2_1->setMaximum( extent[3] );
        m_slider2_1->setMaximum( extent[3] );
        m_viewText2_1->setText( tr("XZ : Coronal") );
        m_2DView2_1->setViewToCoronal();
        INFO_LOG("Visor per defecte: Canviem a vista coronal (Vista 2.1)")
        m_2DView2_1->render();

        m_spinBox2_2->setMinimum( secondExtent[2] );
        m_spinBox2_2->setMaximum( secondExtent[3] );
        m_slider2_2->setMaximum( secondExtent[3] );
        m_viewText2_2->setText( tr("XZ : Coronal") );
        m_2DView2_2->setViewToCoronal();
        INFO_LOG("Visor per defecte: Canviem a vista coronal (Vista 2.2)")
        m_2DView2_2->render();
    break;
    }
}

void Q2DViewerExtension::setView( ViewType view )
{
    switch( view )
    {
    case Axial:
        changeViewToAxial();
    break;
    case Sagital:
        changeViewToSagital();
    break;
    case Coronal:
        changeViewToCoronal();
    break;
    }
}

void Q2DViewerExtension::changeViewToSingle()
{
    m_stackedWidget->setCurrentIndex( 0 );
}

void Q2DViewerExtension::changeViewToDouble()
{
    m_stackedWidget->setCurrentIndex( 1 );
}

void Q2DViewerExtension::pageChange( int index )
{
    setView( m_currentView );
}

void Q2DViewerExtension::synchronizeSlices( bool ok )
{
    if( ok )
    {
        INFO_LOG("Visor per defecte: Sincronitzem llesques");
        connect( m_slider2_1 , SIGNAL( valueChanged(int) ) , m_slider2_2 , SLOT( setValue(int) ) );
        connect( m_slider2_2 , SIGNAL( valueChanged(int) ) , m_slider2_1 , SLOT( setValue(int) ) );
    }
    else
    {
        INFO_LOG("Visor per defecte: Desincronitzem llesques");
        disconnect( m_slider2_1 , SIGNAL( valueChanged(int) ) , m_slider2_2 , SLOT( setValue(int) ) );
        disconnect( m_slider2_2 , SIGNAL( valueChanged(int) ) , m_slider2_1 , SLOT( setValue(int) ) );
    }
}

void Q2DViewerExtension::chooseNewSerie()
{
    emit newSerie();
}

}
