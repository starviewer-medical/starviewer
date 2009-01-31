#include "qgraphictransferfunctioneditor.h"


namespace udg {


QGraphicTransferFunctionEditor::QGraphicTransferFunctionEditor( QWidget *parent )
 : QWidget( parent )
{
    setupUi( this );

    connect( m_nameLineEdit, SIGNAL( textChanged(QString) ), m_basicEditor, SLOT( setTransferFunctionName(const QString&) ) );
}


QGraphicTransferFunctionEditor::~QGraphicTransferFunctionEditor()
{
}


void QGraphicTransferFunctionEditor::setRange( double minimum, double maximum )
{
    m_basicEditor->setRange( minimum, maximum );
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


} // namespace udg
