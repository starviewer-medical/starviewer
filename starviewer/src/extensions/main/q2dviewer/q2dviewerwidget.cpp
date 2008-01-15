/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "q2dviewerwidget.h"
#include "volume.h"
#include "logging.h"

#include "series.h"
#include <QAction>
#include <QPalette>

namespace udg {

Q2DViewerWidget::Q2DViewerWidget(QWidget *parent)
 : QFrame(parent), m_mainVolume(0)
{
    setupUi( this );
    setAutoFillBackground( true );

    // Creació de l'acció del boto de sincronitzar.
    m_buttonSynchronizeAction = new QAction( 0 );
    m_buttonSynchronizeAction->setText( tr("Synchronize tool") );
    m_buttonSynchronizeAction->setStatusTip( tr("Enable/Disable Synchronize tool") );
    m_buttonSynchronizeAction->setIcon( QIcon(":/images/synchronize.png") );
    m_buttonSynchronizeAction->setCheckable( true );
    m_synchronizeButton->setDefaultAction( m_buttonSynchronizeAction );

    createConnections();
}

Q2DViewerWidget::~Q2DViewerWidget()
{
}

void Q2DViewerWidget::createConnections()
{
    connect( m_slider , SIGNAL( valueChanged( int ) ) , m_spinBox , SLOT( setValue( int ) ) );
    connect( m_spinBox , SIGNAL( valueChanged( int ) ) , m_2DView , SLOT( setSlice( int ) ) );
    connect( m_2DView , SIGNAL( sliceChanged( int ) ) , m_slider , SLOT( setValue( int ) ) );
    connect( m_2DView, SIGNAL ( selected() ), SLOT( emitSelectedViewer() ) );
    connect( m_2DView, SIGNAL( volumeChanged( Volume * ) ), SLOT( updateInput( Volume *) ) );
    connect( m_buttonSynchronizeAction, SIGNAL( triggered() ), SLOT( emitSynchronize() ) );
}

void Q2DViewerWidget::setInput( Volume *input )
{
    m_mainVolume = input;
    m_2DView->setInput( input );
    changeViewToAxial();
}

void Q2DViewerWidget::updateInput( Volume *input )
{
    m_mainVolume = input;
    changeViewToAxial();
}

void Q2DViewerWidget::mousePressEvent( QMouseEvent * event )
{
    emit selected( this );
}

void Q2DViewerWidget::emitSelectedViewer()
{
    emit selected( this );
}

void Q2DViewerWidget::changeViewToAxial()
{
    if( !m_mainVolume )
        return;

    int phases = m_mainVolume->getSeries()->getNumberOfPhases();

    int extent[6];
    m_mainVolume->getWholeExtent( extent );

    m_spinBox->setMinimum( extent[4] );
    m_spinBox->setMaximum( extent[5] / phases );
    m_slider->setMaximum( extent[5] / phases);
    m_viewText->setText( tr("XY : Axial") );
    m_2DView->resetViewToAxial();

    INFO_LOG("Visor per defecte: Canviem a vista axial");
}

void Q2DViewerWidget::changeViewToSagital()
{
    if( !m_mainVolume )
        return;

    int extent[6];
    m_mainVolume->getWholeExtent( extent );

    m_spinBox->setMinimum( extent[0] );
    m_spinBox->setMaximum( extent[1] );
    m_slider->setMaximum( extent[1] );
    m_viewText->setText( tr( "XY : Sagital" ) );
    m_2DView->resetViewToSagital();

    INFO_LOG( "Visor per defecte: Canviem a vista sagital" );
}

void Q2DViewerWidget::changeViewToCoronal()
{
    if( !m_mainVolume )
        return;

    int extent[6];
    m_mainVolume->getWholeExtent( extent );

    m_spinBox->setMinimum( extent[2] );
    m_spinBox->setMaximum( extent[3] );
    m_slider->setMaximum( extent[3] );
    m_viewText->setText( tr( "XY : Coronal" ) );
    m_2DView->resetViewToCoronal();

    INFO_LOG( "Visor per defecte: Canviem a vista coronal" );
}

void Q2DViewerWidget::setSelected( bool option )
{
    if( option )
    {
        QPalette palette = this->palette();
        QBrush selected( QColor( 85, 160, 255, 128 ) );
        selected.setStyle( Qt::SolidPattern );
        palette.setBrush( QPalette::Active, QPalette::Window, selected );
        setPalette( palette );
    }
    else
    {
        QPalette palette = this->palette();
        QBrush noSelected( QColor(239, 243, 247, 255) );
        noSelected.setStyle( Qt::SolidPattern );
        palette.setBrush( QPalette::Active, QPalette::Window, noSelected );
        setPalette( palette );
    }
    m_2DView->setActive( option );
}

Q2DViewer * Q2DViewerWidget::getViewer()
{
    return m_2DView;
}

bool Q2DViewerWidget::hasPhases()
{
    int phases = 0 ;
    if( m_mainVolume )
    {
        phases = m_mainVolume->getSeries()->getNumberOfPhases();
    }
    return ( phases > 1 ) ;
}

void Q2DViewerWidget::setDefaultAction( QAction * synchronizeAction )
{
    m_synchronizeButton->setDefaultAction( synchronizeAction );
}

void Q2DViewerWidget::emitSynchronize()
{
    emit synchronize( this, m_buttonSynchronizeAction->isChecked() );
}

}
