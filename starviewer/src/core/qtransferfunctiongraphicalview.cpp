#include "qtransferfunctiongraphicalview.h"

#include "transferfunctioneditor.h"

namespace udg {

QTransferFunctionGraphicalView::QTransferFunctionGraphicalView(TransferFunctionEditor *editor, QWidget *parent)
    : QTransferFunctionView(editor, parent), m_editingConnectionsEnabled(false)
{
    setupUi(this);

    makeConnections();

    setTransferFunction(m_editor->transferFunction());
}

void QTransferFunctionGraphicalView::makeConnections()
{
    connect(m_editor, SIGNAL(transferFunctionChanged(TransferFunction)), SLOT(setTransferFunction(TransferFunction)));
    connect(m_editor, SIGNAL(nameChanged(QString)), SLOT(setName(QString)));
    connect(m_editor, SIGNAL(colorTransferFunctionChanged(ColorTransferFunction)), SLOT(setColorTransferFunction(ColorTransferFunction)));

    // textEdited només s'emet quan s'edita el text a la interfície, no quan fem setText, o sigui que podem fer la connexió permanent
    connect(m_nameLineEdit, SIGNAL(textEdited(QString)), m_editor, SLOT(setName(QString)));
}

void QTransferFunctionGraphicalView::enableEditingConnections()
{
    if (m_editingConnectionsEnabled)
    {
        return;
    }

    connect(m_colorView, SIGNAL(nodeAdded(double,QColor)), m_editor, SLOT(addColorPoint(double,QColor)));
    connect(m_colorView, SIGNAL(nodeRemoved(double)), m_editor, SLOT(removeColorPoint(double)));
    connect(m_colorView, SIGNAL(nodesMoved(QList<QPair<double,double>>)), m_editor, SLOT(moveColorPoints(QList<QPair<double,double>>)));

    m_editingConnectionsEnabled = true;
}

void QTransferFunctionGraphicalView::disableEditingConnections()
{
    if (!m_editingConnectionsEnabled)
    {
        return;
    }

    disconnect(m_colorView, SIGNAL(nodeAdded(double,QColor)), m_editor, SLOT(addColorPoint(double,QColor)));
    disconnect(m_colorView, SIGNAL(nodeRemoved(double)), m_editor, SLOT(removeColorPoint(double)));
    disconnect(m_colorView, SIGNAL(nodesMoved(QList<QPair<double,double>>)), m_editor, SLOT(moveColorPoints(QList<QPair<double,double>>)));

    m_editingConnectionsEnabled = false;
}

void QTransferFunctionGraphicalView::setTransferFunction(const TransferFunction &transferFunction)
{
    disableEditingConnections();
    setName(transferFunction.name());
    setColorTransferFunction(transferFunction.colorTransferFunction());
    enableEditingConnections();
}

void QTransferFunctionGraphicalView::setName(const QString &name)
{
    disableEditingConnections();
    m_nameLineEdit->setText(name);
    enableEditingConnections();
}

void QTransferFunctionGraphicalView::setColorTransferFunction(const ColorTransferFunction &colorTransferFunction)
{
    disableEditingConnections();
    m_colorView->setColorTransferFunction(colorTransferFunction);
    enableEditingConnections();
}

/*
void QTransferFunctionGraphicalView::moveColorPoints(const QList< QPair<double, double> > &moves)
{
    disableEditingConnections();
    
    foreach (const QPair &move, moves)
    {
        //m_editor->moveToThread..
    }

    enableEditingConnections();
}
*/

} // namespace udg
