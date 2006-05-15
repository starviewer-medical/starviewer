/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "qdefaultviewerextension.h"

#include "volume.h"
#include "qcustomwindowleveldialog.h"
#include <QAction>
#include <QToolBar>
// VTK
#include <vtkCamera.h>
#include <vtkRenderer.h>

namespace udg {

QDefaultViewerExtension::QDefaultViewerExtension( QWidget *parent )
 : QWidget( parent )
{
    setupUi( this );
    m_mainVolume = 0;
    
    m_customWindowLevelDialog = new QCustomWindowLevelDialog;
    
    createActions();
    createToolBars();
    createConnections();
}

QDefaultViewerExtension::~QDefaultViewerExtension()
{
}

void QDefaultViewerExtension::createActions()
{
    m_axialViewAction = new QAction( 0 );
    m_axialViewAction->setText( tr("&Axial View") );
    m_axialViewAction->setShortcut( tr("Ctrl+A") );
    m_axialViewAction->setStatusTip( tr("Change Current View To Axial") );
    m_axialViewAction->setIcon( QIcon(":/images/axial.png") );
    
    m_sagitalViewAction = new QAction( 0 );
    m_sagitalViewAction->setText( tr("&Sagital View") );
    m_sagitalViewAction->setShortcut( tr("Ctrl+S") );
    m_sagitalViewAction->setStatusTip( tr("Change Current View To Sagital") );
    m_sagitalViewAction->setIcon( QIcon(":/images/sagital.png") );
    
    m_coronalViewAction = new QAction( 0 );
    m_coronalViewAction->setText( tr("&Coronal View") );
    m_coronalViewAction->setShortcut( tr("Ctrl+C") );
    m_coronalViewAction->setStatusTip( tr("Change Current View To Coronal") );
    m_coronalViewAction->setIcon( QIcon(":/images/coronal.png") );
}

void QDefaultViewerExtension::createToolBars()
{
    m_toolsToolBar = new QToolBar(0);
    m_toolsToolBar->addAction( m_axialViewAction );
    m_toolsToolBar->addAction( m_sagitalViewAction );
    m_toolsToolBar->addAction( m_coronalViewAction );
    
}

void QDefaultViewerExtension::createConnections()
{
    connect( m_slider , SIGNAL( valueChanged(int) ) , m_spinBox , SLOT( setValue(int) ) );
    connect( m_spinBox , SIGNAL( valueChanged(int) ) , m_2DView , SLOT( setSlice(int) ) );
    connect( m_2DView , SIGNAL( sliceChanged(int) ) , m_slider , SLOT( setValue(int) ) );

    connect( m_customWindowLevelDialog , SIGNAL( windowLevel( double,double) ) , m_2DView , SLOT( setWindowLevel( double , double ) ) );
    
    // adicionals, \TODO ara es fa "a saco" però s'ha de millorar
    connect( m_slider2_1 , SIGNAL( valueChanged(int) ) , m_spinBox2_1 , SLOT( setValue(int) ) );
    connect( m_spinBox2_1 , SIGNAL( valueChanged(int) ) , m_2DView2_1 , SLOT( setSlice(int) ) );
    connect( m_2DView2_1 , SIGNAL( sliceChanged(int) ) , m_slider2_1 , SLOT( setValue(int) ) );

    connect( m_slider2_2 , SIGNAL( valueChanged(int) ) , m_spinBox2_2 , SLOT( setValue(int) ) );
    connect( m_spinBox2_2 , SIGNAL( valueChanged(int) ) , m_2DView2_2 , SLOT( setSlice(int) ) );
    connect( m_2DView2_2 , SIGNAL( sliceChanged(int) ) , m_slider2_2 , SLOT( setValue(int) ) );

    connect( m_axialViewAction , SIGNAL( triggered() ) , this , SLOT( changeViewToAxial() ) );
    connect( m_sagitalViewAction , SIGNAL( triggered() ) , this , SLOT( changeViewToSagital() ) );
    connect( m_coronalViewAction , SIGNAL( triggered() ) , this , SLOT( changeViewToCoronal() ) );

    connect( m_windowLevelComboBox , SIGNAL( activated(int) ) , this , SLOT( changeDefaultWindowLevel( int ) ) );

    connect( m_pageSelectorSpinBox , SIGNAL( valueChanged(int) ) , m_stackedWidget , SLOT( setCurrentIndex(int) ) );
    connect( m_stackedWidget , SIGNAL( currentChanged(int) ) , this , SLOT( pageChange(int) ) );

    connect( m_synchroCheckBox , SIGNAL( clicked(bool) ) , this , SLOT( synchronizeSlices(bool) ) );

    connect( m_chooseSeriePushButton , SIGNAL( clicked() ) , this , SLOT( chooseNewSerie() ) );
}

void QDefaultViewerExtension::setInput( Volume *input )
{
    m_mainVolume = input;
    // \TODO ara ho fem "a saco" però s'hauria de millorar
    m_2DView->setInput( m_mainVolume );
    m_2DView2_1->setInput( m_mainVolume );
    m_2DView2_2->setInput( m_mainVolume );
    changeViewToAxial();
    
}

void QDefaultViewerExtension::setSecondInput( Volume *input )
{
    m_secondaryVolume = input;
    // \TODO ara ho fem "a saco" però s'hauria de millorar
    m_2DView2_2->setInput( m_secondaryVolume );
    changeViewToAxial();
}

void QDefaultViewerExtension::populateToolBar( QToolBar *toolbar )
{
    if( toolbar )
    {
        toolbar->addAction( m_axialViewAction );
        toolbar->addAction( m_sagitalViewAction );
        toolbar->addAction( m_coronalViewAction );
    }
}

void QDefaultViewerExtension::changeViewToAxial()
{
    m_currentView = Axial;
    switch( m_stackedWidget->currentIndex() )
    {
    case 0:
        m_spinBox->setMinimum( 0 );
        m_spinBox->setMaximum( m_mainVolume->getDimensions()[2] );
        m_slider->setMaximum(  m_mainVolume->getDimensions()[2] );
        m_slider->setValue( m_2DView->getSlice() );
        m_viewText->setText( tr("XY : Axial") );
        m_2DView->setViewToAxial();
        vtkCamera *axialCam = m_2DView->getRenderer() ? m_2DView->getRenderer()->GetActiveCamera() : NULL;
        if ( axialCam )
        {
            axialCam->SetViewUp(0,-1,0);
        }
        m_2DView->render();
    break;

    case 1:
        m_spinBox2_1->setMinimum( 0 );
        m_spinBox2_1->setMaximum( m_mainVolume->getDimensions()[2] );
        m_slider2_1->setMaximum(  m_mainVolume->getDimensions()[2] );
        m_slider2_1->setValue( m_2DView2_1->getSlice() );
        m_viewText2_1->setText( tr("XY : Axial") );
        m_2DView2_1->setViewToAxial();
        vtkCamera *axialCam2 = m_2DView2_1->getRenderer() ? m_2DView2_1->getRenderer()->GetActiveCamera() : NULL;
        if ( axialCam2 )
        {
            axialCam2->SetViewUp(0,-1,0);
        }
        m_2DView2_1->render();

        m_spinBox2_2->setMinimum( 0 );
        m_spinBox2_2->setMaximum( m_mainVolume->getDimensions()[2] );
        m_slider2_2->setMaximum(  m_mainVolume->getDimensions()[2] );
        m_slider2_2->setValue( m_2DView2_2->getSlice() );
        m_viewText2_2->setText( tr("XY : Axial") );
        m_2DView2_2->setViewToAxial();
        axialCam2 = m_2DView2_2->getRenderer() ? m_2DView2_2->getRenderer()->GetActiveCamera() : NULL;
        if ( axialCam2 )
        {
            axialCam2->SetViewUp(0,-1,0);
        }
        m_2DView2_2->render();
    break;
    
    }

}

void QDefaultViewerExtension::changeViewToSagital()
{
    m_currentView = Sagital;
    switch( m_stackedWidget->currentIndex() )
    {
    case 0:
        m_spinBox->setMinimum( 0 );
        m_spinBox->setMaximum( m_mainVolume->getDimensions()[0] );
        m_slider->setMaximum(  m_mainVolume->getDimensions()[0] );
        m_slider->setValue( m_mainVolume->getDimensions()[0]/2 );
        m_viewText->setText( tr("YZ : Sagital") );
        m_2DView->setViewToSagittal();
        m_2DView->render();
    break;

    case 1:
        m_spinBox2_1->setMinimum( 0 );
        m_spinBox2_1->setMaximum( m_mainVolume->getDimensions()[0] );
        m_slider2_1->setMaximum(  m_mainVolume->getDimensions()[0] );
        m_slider2_1->setValue( m_2DView2_1->getSlice() );
        m_viewText2_1->setText( tr("YZ : Sagital") );
        m_2DView2_1->setViewToSagittal();
        m_2DView2_1->render();

        m_spinBox2_2->setMinimum( 0 );
        m_spinBox2_2->setMaximum( m_mainVolume->getDimensions()[0] );
        m_slider2_2->setMaximum(  m_mainVolume->getDimensions()[0] );
        m_slider2_2->setValue( m_2DView2_2->getSlice() );
        m_viewText2_2->setText( tr("YZ : Sagital") );
        m_2DView2_2->setViewToSagittal();
        m_2DView2_2->render();
    break;
    }
}

void QDefaultViewerExtension::changeViewToCoronal()
{
    m_currentView = Coronal;
    switch( m_stackedWidget->currentIndex() )
    {
    case 0:
        m_spinBox->setMinimum( 0 );
        m_spinBox->setMaximum( m_mainVolume->getDimensions()[1] );
        m_slider->setMaximum(  m_mainVolume->getDimensions()[1] );
        m_slider->setValue( m_mainVolume->getDimensions()[1]/2 );
        m_viewText->setText( tr("XZ : Coronal") );
        m_2DView->setViewToCoronal();
        m_2DView->render();
    break;
    
    case 1:
        m_spinBox2_1->setMinimum( 0 );
        m_spinBox2_1->setMaximum( m_mainVolume->getDimensions()[1] );
        m_slider2_1->setMaximum(  m_mainVolume->getDimensions()[1] );
        m_slider2_1->setValue( m_2DView2_1->getSlice() );
        m_viewText2_1->setText( tr("XZ : Coronal") );
        m_2DView2_1->setViewToCoronal();
        m_2DView2_1->render();

        m_spinBox2_2->setMinimum( 0 );
        m_spinBox2_2->setMaximum( m_mainVolume->getDimensions()[1] );
        m_slider2_2->setMaximum(  m_mainVolume->getDimensions()[1] );
        m_slider2_2->setValue( m_2DView2_2->getSlice() );
        m_viewText2_2->setText( tr("XZ : Coronal") );
        m_2DView2_2->setViewToCoronal();
        m_2DView2_2->render();
    break;
    }
}

void QDefaultViewerExtension::changeDefaultWindowLevel( int which )
{
    // \TODO ara anem a saco però 'shauria de fer una manera perquè només es cridessin els que cal
    switch( which )
    {
    case 0:
        m_2DView->resetWindowLevelToDefault();
        m_2DView2_1->resetWindowLevelToDefault();
        m_2DView2_2->resetWindowLevelToDefault();
    break;

    case 1:
        m_2DView->resetWindowLevelToBone();
        m_2DView2_1->resetWindowLevelToBone();
        m_2DView2_2->resetWindowLevelToBone();
    break;

    case 2:
        m_2DView->resetWindowLevelToLung();
        m_2DView2_1->resetWindowLevelToLung();
        m_2DView2_2->resetWindowLevelToLung();
    break;

    case 3:
        m_2DView->resetWindowLevelToSoftTissuesNonContrast();
        m_2DView2_1->resetWindowLevelToSoftTissuesNonContrast();
        m_2DView2_2->resetWindowLevelToSoftTissuesNonContrast();
    break;

    case 4:
        m_2DView->resetWindowLevelToLiverNonContrast();
        m_2DView2_1->resetWindowLevelToLiverNonContrast();
        m_2DView2_2->resetWindowLevelToLiverNonContrast();
    break;

    case 5:
        m_2DView->resetWindowLevelToSoftTissuesContrastMedium();
        m_2DView2_1->resetWindowLevelToSoftTissuesContrastMedium();
        m_2DView2_2->resetWindowLevelToSoftTissuesContrastMedium();
    break;

    case 6:
        m_2DView->resetWindowLevelToLiverContrastMedium();
        m_2DView2_1->resetWindowLevelToLiverContrastMedium();
        m_2DView2_2->resetWindowLevelToLiverContrastMedium();
    break;

    case 7:
        m_2DView->resetWindowLevelToNeckContrastMedium();
        m_2DView2_1->resetWindowLevelToNeckContrastMedium();
        m_2DView2_2->resetWindowLevelToNeckContrastMedium();
    break;

    case 8:
        m_2DView->resetWindowLevelToAngiography();
        m_2DView2_1->resetWindowLevelToAngiography();
        m_2DView2_2->resetWindowLevelToAngiography();
    break;

    case 9:
        m_2DView->resetWindowLevelToOsteoporosis();
        m_2DView2_1->resetWindowLevelToOsteoporosis();
        m_2DView2_2->resetWindowLevelToOsteoporosis();
    break;

    case 10:
        m_2DView->resetWindowLevelToEmphysema();
        m_2DView2_1->resetWindowLevelToEmphysema();
        m_2DView2_2->resetWindowLevelToEmphysema();
    break;
    
    case 11:
        m_2DView->resetWindowLevelToPetrousBone();
        m_2DView2_1->resetWindowLevelToPetrousBone();
        m_2DView2_2->resetWindowLevelToPetrousBone();
    break;

    case 12:
        // custom
        m_customWindowLevelDialog->exec();
    break;

    default:
        m_2DView->resetWindowLevelToDefault();
        m_2DView2_1->resetWindowLevelToDefault();
        m_2DView2_2->resetWindowLevelToDefault();
    break;
    
    }
}

void QDefaultViewerExtension::setView( ViewType view )
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

void QDefaultViewerExtension::pageChange( int index )
{
    setView( m_currentView );
    switch( index )
    {
    case 0:
    break;

    case 1:
    break;
    }
}

void QDefaultViewerExtension::synchronizeSlices( bool ok )
{
    if( ok )
    {
        connect( m_slider2_1 , SIGNAL( valueChanged(int) ) , m_slider2_2 , SLOT( setValue(int) ) );
        connect( m_slider2_2 , SIGNAL( valueChanged(int) ) , m_slider2_1 , SLOT( setValue(int) ) );
    }
    else
    {
        disconnect( m_slider2_1 , SIGNAL( valueChanged(int) ) , m_slider2_2 , SLOT( setValue(int) ) );
        disconnect( m_slider2_2 , SIGNAL( valueChanged(int) ) , m_slider2_1 , SLOT( setValue(int) ) );
    }
}

void QDefaultViewerExtension::chooseNewSerie()
{
    emit newSerie();
}

}
