#include "qgraphictransferfunctioneditor.h"


namespace udg {


QGraphicTransferFunctionEditor::QGraphicTransferFunctionEditor( QWidget *parent )
 : QWidget( parent )
{
    setupUi( this );

    connect( m_nameLineEdit, SIGNAL( textChanged(QString) ), m_basicEditor, SLOT( setTransferFunctionName(const QString&) ) );
    connect( m_minimumDoubleSpinBox, SIGNAL( valueChanged(double) ), SLOT( setMinimum(double) ) );
    connect( m_maximumDoubleSpinBox, SIGNAL( valueChanged(double) ), SLOT( setMaximum(double) ) );
    connect( m_keepRangePushButton, SIGNAL( toggled(bool) ), SLOT( keepRange(bool) ) );
    connect( m_scrollBar, SIGNAL( valueChanged(int) ), SLOT( setMinimum(int) ) );
}


QGraphicTransferFunctionEditor::~QGraphicTransferFunctionEditor()
{
}


void QGraphicTransferFunctionEditor::setRange( double minimum, double maximum )
{
    int pageStep = static_cast<int>( maximum - minimum );
    m_scrollBar->setPageStep( pageStep );
    m_scrollBar->setMaximum( static_cast<int>( m_maximumDoubleSpinBox->maximum() - pageStep ) );
    m_minimumDoubleSpinBox->setValue( minimum );
    m_maximumDoubleSpinBox->setValue( maximum );
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


void QGraphicTransferFunctionEditor::setMinimum( double minimum )
{
    disconnect( m_minimumDoubleSpinBox, SIGNAL( valueChanged(double) ), this, SLOT( setMinimum(double) ) );
    disconnect( m_maximumDoubleSpinBox, SIGNAL( valueChanged(double) ), this, SLOT( setMaximum(double) ) );
    disconnect( m_scrollBar, SIGNAL( valueChanged(int) ), this, SLOT( setMinimum(int) ) );

    if ( m_keepRangePushButton->isChecked() )
    {
        m_maximumDoubleSpinBox->setValue( minimum + m_range );
        m_minimumDoubleSpinBox->setValue( m_maximumDoubleSpinBox->value() - m_range );
    }

    if ( minimum >= m_maximumDoubleSpinBox->value() )
    {
        m_maximumDoubleSpinBox->setValue( minimum + 1.0 );
        m_minimumDoubleSpinBox->setValue( m_maximumDoubleSpinBox->value() - 1.0 );
    }

    m_basicEditor->setRange( m_minimumDoubleSpinBox->value(), m_maximumDoubleSpinBox->value() );

    int pageStep = static_cast<int>( m_maximumDoubleSpinBox->value() - m_minimumDoubleSpinBox->value() );
    int value = static_cast<int>( m_minimumDoubleSpinBox->value() );
    m_scrollBar->setPageStep( pageStep );
    m_scrollBar->setMaximum( static_cast<int>( m_maximumDoubleSpinBox->maximum() - pageStep ) );
    m_scrollBar->setValue( value );

    connect( m_minimumDoubleSpinBox, SIGNAL( valueChanged(double) ), SLOT( setMinimum(double) ) );
    connect( m_maximumDoubleSpinBox, SIGNAL( valueChanged(double) ), SLOT( setMaximum(double) ) );
    connect( m_scrollBar, SIGNAL( valueChanged(int) ), SLOT( setMinimum(int) ) );
}


void QGraphicTransferFunctionEditor::setMaximum( double maximum )
{
    disconnect( m_minimumDoubleSpinBox, SIGNAL( valueChanged(double) ), this, SLOT( setMinimum(double) ) );
    disconnect( m_maximumDoubleSpinBox, SIGNAL( valueChanged(double) ), this, SLOT( setMaximum(double) ) );
    disconnect( m_scrollBar, SIGNAL( valueChanged(int) ), this, SLOT( setMinimum(int) ) );

    if ( m_keepRangePushButton->isChecked() )
    {
        m_minimumDoubleSpinBox->setValue( maximum - m_range );
        m_maximumDoubleSpinBox->setValue( m_minimumDoubleSpinBox->value() + m_range );
    }

    if ( maximum <= m_minimumDoubleSpinBox->value() )
    {
        m_minimumDoubleSpinBox->setValue( maximum - 1.0 );
        m_maximumDoubleSpinBox->setValue( m_minimumDoubleSpinBox->value() + 1.0 );
    }

    m_basicEditor->setRange( m_minimumDoubleSpinBox->value(), m_maximumDoubleSpinBox->value() );

    int pageStep = static_cast<int>( m_maximumDoubleSpinBox->value() - m_minimumDoubleSpinBox->value() );
    int value = static_cast<int>( m_minimumDoubleSpinBox->value() );
    m_scrollBar->setPageStep( pageStep );
    m_scrollBar->setMaximum( static_cast<int>( m_maximumDoubleSpinBox->maximum() - pageStep ) );
    m_scrollBar->setValue( value );

    connect( m_minimumDoubleSpinBox, SIGNAL( valueChanged(double) ), SLOT( setMinimum(double) ) );
    connect( m_maximumDoubleSpinBox, SIGNAL( valueChanged(double) ), SLOT( setMaximum(double) ) );
    connect( m_scrollBar, SIGNAL( valueChanged(int) ), SLOT( setMinimum(int) ) );
}


void QGraphicTransferFunctionEditor::keepRange( bool keep )
{
    if ( keep ) m_range = m_maximumDoubleSpinBox->value() - m_minimumDoubleSpinBox->value();
}


void QGraphicTransferFunctionEditor::setMinimum( int minimum )
{
    disconnect( m_minimumDoubleSpinBox, SIGNAL( valueChanged(double) ), this, SLOT( setMinimum(double) ) );
    disconnect( m_maximumDoubleSpinBox, SIGNAL( valueChanged(double) ), this, SLOT( setMaximum(double) ) );
    disconnect( m_scrollBar, SIGNAL( valueChanged(int) ), this, SLOT( setMinimum(int) ) );

    m_minimumDoubleSpinBox->setValue( minimum );
    m_maximumDoubleSpinBox->setValue( minimum + m_scrollBar->pageStep() );

    m_basicEditor->setRange( m_minimumDoubleSpinBox->value(), m_maximumDoubleSpinBox->value() );

    connect( m_minimumDoubleSpinBox, SIGNAL( valueChanged(double) ), SLOT( setMinimum(double) ) );
    connect( m_maximumDoubleSpinBox, SIGNAL( valueChanged(double) ), SLOT( setMaximum(double) ) );
    connect( m_scrollBar, SIGNAL( valueChanged(int) ), SLOT( setMinimum(int) ) );
}


} // namespace udg
