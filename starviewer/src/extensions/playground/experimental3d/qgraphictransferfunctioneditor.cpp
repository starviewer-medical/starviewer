#include "qgraphictransferfunctioneditor.h"

#include <cmath>


namespace udg {


QGraphicTransferFunctionEditor::QGraphicTransferFunctionEditor( QWidget *parent )
 : QWidget( parent )
{
    setupUi( this );

    connect( m_nameLineEdit, SIGNAL( textChanged(QString) ), m_basicEditor, SLOT( setTransferFunctionName(const QString&) ) );
    connect( m_basicEditor, SIGNAL( rangeChanged(double,double) ), SLOT( setBasicRange(double,double) ) );
    connect( m_minimumSpinBox, SIGNAL( valueChanged(int) ), SLOT( setMinimum(int) ) );
    connect( m_maximumSpinBox, SIGNAL( valueChanged(int) ), SLOT( setMaximum(int) ) );
    connect( m_scrollBar, SIGNAL( valueChanged(int) ), SLOT( setScroll(int) ) );
    connect( m_syncToMaxPushButton, SIGNAL( clicked() ), SLOT( syncToMax() ) );
    connect( m_syncToVisiblePushButton, SIGNAL( clicked() ), SLOT( syncToVisible() ) );
}


QGraphicTransferFunctionEditor::~QGraphicTransferFunctionEditor()
{
}


void QGraphicTransferFunctionEditor::setTransferFunction( const TransferFunction &transferFunction )
{
    m_nameLineEdit->setText( transferFunction.name() );
    m_basicEditor->setTransferFunction( transferFunction );
}


const TransferFunction& QGraphicTransferFunctionEditor::transferFunction() const
{
    return m_basicEditor->transferFunction();
}


void QGraphicTransferFunctionEditor::setRange( int minimum, int maximum )
{
    Q_ASSERT( minimum < maximum );

    disconnect( m_basicEditor, SIGNAL( rangeChanged(double,double) ), this, SLOT( setBasicRange(double,double) ) );
    disconnect( m_minimumSpinBox, SIGNAL( valueChanged(int) ), this, SLOT( setMinimum(int) ) );
    disconnect( m_maximumSpinBox, SIGNAL( valueChanged(int) ), this, SLOT( setMaximum(int) ) );
    disconnect( m_scrollBar, SIGNAL( valueChanged(int) ), this, SLOT( setScroll(int) ) );

    int min = qMax( minimum, m_minimumSpinBox->minimum() );
    m_minimumSpinBox->setValue( min );
    int max = qMin( maximum, m_maximumSpinBox->maximum() );
    m_maximumSpinBox->setValue( max );

    int basicMin = static_cast<int>( std::floor( m_basicEditor->minimum() ) );
    int basicMax = static_cast<int>( std::ceil( m_basicEditor->maximum() ) );
    basicMin = qMax( basicMin, min );
    basicMax = qMin( basicMax, max );
    m_basicEditor->setRange( basicMin, basicMax );

    int pageStep = basicMax - basicMin;
    int scroll = basicMin - min;
    m_scrollBar->setPageStep( pageStep );
    m_scrollBar->setMaximum( ( max - min ) - pageStep );
    m_scrollBar->setValue( scroll );

    connect( m_basicEditor, SIGNAL( rangeChanged(double,double) ), SLOT( setBasicRange(double,double) ) );
    connect( m_minimumSpinBox, SIGNAL( valueChanged(int) ), SLOT( setMinimum(int) ) );
    connect( m_maximumSpinBox, SIGNAL( valueChanged(int) ), SLOT( setMaximum(int) ) );
    connect( m_scrollBar, SIGNAL( valueChanged(int) ), SLOT( setScroll(int) ) );
}


void QGraphicTransferFunctionEditor::setBasicRange( double minimum, double maximum )
{
    Q_ASSERT( minimum < maximum );

    disconnect( m_basicEditor, SIGNAL( rangeChanged(double,double) ), this, SLOT( setBasicRange(double,double) ) );
    disconnect( m_minimumSpinBox, SIGNAL( valueChanged(int) ), this, SLOT( setMinimum(int) ) );
    disconnect( m_maximumSpinBox, SIGNAL( valueChanged(int) ), this, SLOT( setMaximum(int) ) );
    disconnect( m_scrollBar, SIGNAL( valueChanged(int) ), this, SLOT( setScroll(int) ) );

    int basicMin = static_cast<int>( std::floor( minimum ) );
    int basicMax = static_cast<int>( std::ceil( maximum ) );
    basicMin = qMax( basicMin, m_minimumSpinBox->minimum() );
    basicMax = qMin( basicMax, m_maximumSpinBox->maximum() );
    m_basicEditor->setRange( basicMin, basicMax );

    int min = qMin( basicMin, m_minimumSpinBox->value() );
    m_minimumSpinBox->setValue( min );
    int max = qMax( basicMax, m_maximumSpinBox->value() );
    m_maximumSpinBox->setValue( max );

    int pageStep = basicMax - basicMin;
    int scroll = basicMin - min;
    m_scrollBar->setPageStep( pageStep );
    m_scrollBar->setMaximum( ( max - min ) - pageStep );
    m_scrollBar->setValue( scroll );

    connect( m_basicEditor, SIGNAL( rangeChanged(double,double) ), SLOT( setBasicRange(double,double) ) );
    connect( m_minimumSpinBox, SIGNAL( valueChanged(int) ), SLOT( setMinimum(int) ) );
    connect( m_maximumSpinBox, SIGNAL( valueChanged(int) ), SLOT( setMaximum(int) ) );
    connect( m_scrollBar, SIGNAL( valueChanged(int) ), SLOT( setScroll(int) ) );
}


void QGraphicTransferFunctionEditor::setMinimum( int minimum )
{
    disconnect( m_basicEditor, SIGNAL( rangeChanged(double,double) ), this, SLOT( setBasicRange(double,double) ) );
    disconnect( m_minimumSpinBox, SIGNAL( valueChanged(int) ), this, SLOT( setMinimum(int) ) );
    disconnect( m_maximumSpinBox, SIGNAL( valueChanged(int) ), this, SLOT( setMaximum(int) ) );
    disconnect( m_scrollBar, SIGNAL( valueChanged(int) ), this, SLOT( setScroll(int) ) );

    if ( minimum >= m_maximumSpinBox->value() )
    {
        m_maximumSpinBox->setValue( minimum + 1 );
        m_minimumSpinBox->setValue( m_maximumSpinBox->value() - 1 );
    }

    int min = m_minimumSpinBox->value();
    int max = m_maximumSpinBox->value();

    int basicMin = static_cast<int>( std::floor( m_basicEditor->minimum() ) );
    int basicMax = static_cast<int>( std::ceil( m_basicEditor->maximum() ) );
    basicMin = qMax( basicMin, min );
    basicMax = qMax( basicMin + 1, basicMax );
    m_basicEditor->setRange( basicMin, basicMax );

    int pageStep = basicMax - basicMin;
    int scroll = basicMin - min;
    m_scrollBar->setPageStep( pageStep );
    m_scrollBar->setMaximum( ( max - min ) - pageStep );
    m_scrollBar->setValue( scroll );

    connect( m_basicEditor, SIGNAL( rangeChanged(double,double) ), SLOT( setBasicRange(double,double) ) );
    connect( m_minimumSpinBox, SIGNAL( valueChanged(int) ), SLOT( setMinimum(int) ) );
    connect( m_maximumSpinBox, SIGNAL( valueChanged(int) ), SLOT( setMaximum(int) ) );
    connect( m_scrollBar, SIGNAL( valueChanged(int) ), SLOT( setScroll(int) ) );
}


void QGraphicTransferFunctionEditor::setMaximum( int maximum )
{
    disconnect( m_basicEditor, SIGNAL( rangeChanged(double,double) ), this, SLOT( setBasicRange(double,double) ) );
    disconnect( m_minimumSpinBox, SIGNAL( valueChanged(int) ), this, SLOT( setMinimum(int) ) );
    disconnect( m_maximumSpinBox, SIGNAL( valueChanged(int) ), this, SLOT( setMaximum(int) ) );
    disconnect( m_scrollBar, SIGNAL( valueChanged(int) ), this, SLOT( setScroll(int) ) );

    if ( maximum <= m_minimumSpinBox->value() )
    {
        m_minimumSpinBox->setValue( maximum - 1 );
        m_maximumSpinBox->setValue( m_minimumSpinBox->value() + 1 );
    }

    int min = m_minimumSpinBox->value();
    int max = m_maximumSpinBox->value();

    int basicMin = static_cast<int>( std::floor( m_basicEditor->minimum() ) );
    int basicMax = static_cast<int>( std::ceil( m_basicEditor->maximum() ) );
    basicMax = qMin( basicMax, max );
    basicMin = qMin( basicMin, basicMax - 1 );
    m_basicEditor->setRange( basicMin, basicMax );

    int pageStep = basicMax - basicMin;
    int scroll = basicMin - min;
    m_scrollBar->setPageStep( pageStep );
    m_scrollBar->setMaximum( ( max - min ) - pageStep );
    m_scrollBar->setValue( scroll );

    connect( m_basicEditor, SIGNAL( rangeChanged(double,double) ), SLOT( setBasicRange(double,double) ) );
    connect( m_minimumSpinBox, SIGNAL( valueChanged(int) ), SLOT( setMinimum(int) ) );
    connect( m_maximumSpinBox, SIGNAL( valueChanged(int) ), SLOT( setMaximum(int) ) );
    connect( m_scrollBar, SIGNAL( valueChanged(int) ), SLOT( setScroll(int) ) );
}


void QGraphicTransferFunctionEditor::setScroll( int scroll )
{
    disconnect( m_basicEditor, SIGNAL( rangeChanged(double,double) ), this, SLOT( setBasicRange(double,double) ) );

    int basicMin = m_minimumSpinBox->value() + scroll;
    int basicMax = basicMin + m_scrollBar->pageStep();
    m_basicEditor->setRange( basicMin, basicMax );

    connect( m_basicEditor, SIGNAL( rangeChanged(double,double) ), SLOT( setBasicRange(double,double) ) );
}


void QGraphicTransferFunctionEditor::syncToMax()
{
    m_basicEditor->setRange( m_minimumSpinBox->value(), m_maximumSpinBox->value() );
}


void QGraphicTransferFunctionEditor::syncToVisible()
{
    m_minimumSpinBox->setValue( static_cast<int>( std::floor( m_basicEditor->minimum() ) ) );
    m_maximumSpinBox->setValue( static_cast<int>( std::ceil( m_basicEditor->maximum() ) ) );
}


} // namespace udg
