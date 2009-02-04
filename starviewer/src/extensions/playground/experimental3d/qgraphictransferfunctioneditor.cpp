#include "qgraphictransferfunctioneditor.h"


namespace udg {


QGraphicTransferFunctionEditor::QGraphicTransferFunctionEditor( QWidget *parent )
 : QWidget( parent )
{
    setupUi( this );

    connect( m_nameLineEdit, SIGNAL( textChanged(QString) ), m_basicEditor, SLOT( setTransferFunctionName(const QString&) ) );
    connect( m_minimumDoubleSpinBox, SIGNAL( valueChanged(double) ), SLOT( setMinimum(double) ) );
    connect( m_maximumDoubleSpinBox, SIGNAL( valueChanged(double) ), SLOT( setMaximum(double) ) );
}


QGraphicTransferFunctionEditor::~QGraphicTransferFunctionEditor()
{
}


void QGraphicTransferFunctionEditor::setRange( double minimum, double maximum )
{
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
    if ( minimum >= m_maximumDoubleSpinBox->value() ) m_maximumDoubleSpinBox->setValue( m_maximumDoubleSpinBox->value() + 1.0 );

    m_basicEditor->setRange( m_minimumDoubleSpinBox->value(), m_maximumDoubleSpinBox->value() );
}


void QGraphicTransferFunctionEditor::setMaximum( double maximum )
{
    if ( maximum <= m_minimumDoubleSpinBox->value() ) m_minimumDoubleSpinBox->setValue( m_minimumDoubleSpinBox->value() - 1.0 );

    m_basicEditor->setRange( m_minimumDoubleSpinBox->value(), m_maximumDoubleSpinBox->value() );
}


} // namespace udg
