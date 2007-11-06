/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "thickslabwidget.h"
#include "volume.h"
#include "logging.h"

#include "series.h"
#include <QAction>
#include <QPalette>




#include <vtkLookupTable.h>
#include <vtkImageMapToWindowLevelColors.h>




namespace udg {

ThickSlabWidget::ThickSlabWidget(QWidget *parent)
 : QFrame(parent), m_mainVolume(0)
{
    setupUi( this );
    createConnections();
    setAutoFillBackground( true );
}

ThickSlabWidget::~ThickSlabWidget()
{
}

void ThickSlabWidget::createConnections()
{
    connect( m_slider , SIGNAL( valueChanged( int ) ) , m_spinBox , SLOT( setValue( int ) ) );
    connect( m_spinBox , SIGNAL( valueChanged( int ) ) , m_2DView , SLOT( setSlice( int ) ) );
    connect( m_2DView , SIGNAL( sliceChanged( int ) ) , m_slider , SLOT( setValue( int ) ) );
    connect( m_2DView, SIGNAL ( selected() ), this, SLOT( emitSelectedViewer() ) );
    connect( m_2DView, SIGNAL( volumeChanged( Volume * ) ), this, SLOT( setInput( Volume *) ) );
}

void ThickSlabWidget::setInput( Volume *input )
{

    vtkLookupTable * hueLut = vtkLookupTable::New();
    hueLut->SetNumberOfTableValues( 255 );
    hueLut->SetTableRange( 0.0, 255.0 );
//     hueLut->SetHueRange( 1.0, 0.0 );
//     hueLut->SetSaturationRange( 1.0, 1.0 );
//     hueLut->SetValueRange( 1.0, 1.0 );
//     hueLut->SetAlphaRange( 1.0, 1.0 );
    hueLut->SetRampToLinear();

    hueLut->Build();    //effective built
//     int nvalues=hueLut->GetNumberOfTableValues();
//     double* tvalue= new double[4];
//     for(int i=0;i<(nvalues/3);i++)
//     {
//         tvalue=hueLut->GetTableValue(i);
//         tvalue[0]=0.0;  //Posem els valors transparents
//         tvalue[1]=0.0;  //Posem els valors transparents
//         tvalue[2]=0.0;  //Posem els valors transparents
//         tvalue[3]=0.0;  //Posem els valors transparents
//         hueLut->SetTableValue(i, tvalue);
//     }
//     hueLut->Build();    //effective built

//     vtkUnsignedCharArray * table = hueLut->GetTable();
//     unsigned char tuple[4] = { 0, 0, 0, 0 };
//     table->SetTupleValue( 0, tuple );
//     table->SetTupleValue( table->GetNumberOfTuples() - 1, tuple );


    for ( int i = 0; i <= 32; i++ )
        hueLut->SetTableValue( i, 0.0, 0.0, 0.0 );
    for ( int i = 33; i <= 96; i++ )
        hueLut->SetTableValue( i, 2.0 / 3.0, 0.0, 0.0 );
    for ( int i = 97; i <= 159; i++ )
        hueLut->SetTableValue( i, 1.0, 1.0, 1.0 );
    for ( int i = 160; i <= 223; i++ )
        hueLut->SetTableValue( i, 1.0, 1.0, 0.0 );
    for ( int i = 224; i <= 255; i++ )
        hueLut->SetTableValue( i, 1.0, 2.0 / 3.0, 0.0 );


    m_2DView->getWindowLevelMapper()->SetLookupTable( hueLut );







    m_mainVolume = input;
    m_2DView->setInput( input );
    changeViewToAxial();
    m_2DView->render();
}

void ThickSlabWidget::mousePressEvent( QMouseEvent * event )
{
    emit selected( this );
}

void ThickSlabWidget::emitSelectedViewer()
{
    emit selected( this );
}

void ThickSlabWidget::changeViewToAxial()
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
    m_2DView->setViewToAxial();
    m_2DView->render();

    INFO_LOG("Visor per defecte: Canviem a vista axial");
}

void ThickSlabWidget::changeViewToSagital()
{
    if( !m_mainVolume )
        return;

    int extent[6];
    m_mainVolume->getWholeExtent( extent );

    m_spinBox->setMinimum( extent[0] );
    m_spinBox->setMaximum( extent[1] );
    m_slider->setMaximum( extent[1] );
    m_viewText->setText( tr( "XY : Sagital" ) );
    m_2DView->setViewToSagittal();
    m_2DView->render();

    INFO_LOG( "Visor per defecte: Canviem a vista sagital" );
}

void ThickSlabWidget::changeViewToCoronal()
{
    if( !m_mainVolume )
        return;

    int extent[6];
    m_mainVolume->getWholeExtent( extent );

    m_spinBox->setMinimum( extent[2] );
    m_spinBox->setMaximum( extent[3] );
    m_slider->setMaximum( extent[3] );
    m_viewText->setText( tr( "XY : Coronal" ) );
    m_2DView->setViewToCoronal();
    m_2DView->render();

    INFO_LOG( "Visor per defecte: Canviem a vista coronal" );
}

void ThickSlabWidget::setSelected( bool option )
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
}

Q2DViewer * ThickSlabWidget::getViewer()
{
    return m_2DView;
}

bool ThickSlabWidget::hasPhases()
{

    int phases = 0 ;

    if( m_mainVolume )
    {
        phases = m_mainVolume->getSeries()->getNumberOfPhases();
    }


    DEBUG_LOG( QString( tr("Phases: %1").arg( phases) ) );


    return ( phases > 1 ) ;
}

}
