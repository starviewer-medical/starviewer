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
    QTransferFunctionView::makeConnections();

    // textEdited només s'emet quan s'edita el text a la interfície, no quan fem setText, o sigui que podem fer la connexió permanent
    connect(m_nameLineEdit, SIGNAL(textEdited(QString)), m_editor, SLOT(setName(QString)));
}

void QTransferFunctionGraphicalView::setTransferFunction(const TransferFunction &transferFunction)
{
    disableEditingConnections();
    setName(transferFunction.name());
    setColorTransferFunction(transferFunction.colorTransferFunction());
    setScalarOpacityTransferFunction(transferFunction.scalarOpacityTransferFunction());
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

void QTransferFunctionGraphicalView::setScalarOpacityTransferFunction(const OpacityTransferFunction &scalarOpacityTransferFunction)
{
    disableEditingConnections();
    m_scalarOpacityView->setOpacityTransferFunction(scalarOpacityTransferFunction);
    enableEditingConnections();
}

void QTransferFunctionGraphicalView::addColorPoint(double x, const QColor &color)
{
    // TODO: de moment sincronitzem tota la funció per tenir un prototipus més aviat, però es podria optimitzar
    Q_UNUSED(x)
    Q_UNUSED(color)
    disableEditingConnections();
    m_colorView->setColorTransferFunction(m_editor->transferFunction().colorTransferFunction());
    enableEditingConnections();
}

void QTransferFunctionGraphicalView::removeColorPoint(double x)
{
    // TODO: de moment sincronitzem tota la funció per tenir un prototipus més aviat, però es podria optimitzar
    Q_UNUSED(x)
    disableEditingConnections();
    m_colorView->setColorTransferFunction(m_editor->transferFunction().colorTransferFunction());
    enableEditingConnections();
}

void QTransferFunctionGraphicalView::addScalarOpacityPoint(double x, double opacity)
{
    // TODO: de moment sincronitzem tota la funció per tenir un prototipus més aviat, però es podria optimitzar
    Q_UNUSED(x)
    Q_UNUSED(opacity)
    disableEditingConnections();
    m_scalarOpacityView->setOpacityTransferFunction(m_editor->transferFunction().scalarOpacityTransferFunction());
    enableEditingConnections();
}

void QTransferFunctionGraphicalView::removeScalarOpacityPoint(double x)
{
    // TODO: de moment sincronitzem tota la funció per tenir un prototipus més aviat, però es podria optimitzar
    Q_UNUSED(x)
    disableEditingConnections();
    m_scalarOpacityView->setOpacityTransferFunction(m_editor->transferFunction().scalarOpacityTransferFunction());
    enableEditingConnections();
}

void QTransferFunctionGraphicalView::enableEditingConnections()
{
    if (m_editingConnectionsEnabled)
    {
        return;
    }

    connect(m_colorView, SIGNAL(nodeAdded(double,QColor)), m_editor, SLOT(addColorPoint(double,QColor)));
    connect(m_colorView, SIGNAL(nodeRemoved(double)),m_editor, SLOT(removeColorPoint(double)));
    connect(m_colorView, SIGNAL(nodeMoved(double,double)), m_editor, SLOT(moveColorPoint(double,double)));
    connect(m_colorView, SIGNAL(nodesMoved(QList<double>,double)), m_editor, SLOT(moveColorPoints(QList<double>,double)));
    connect(m_colorView, SIGNAL(nodeChangedColor(double,QColor)), m_editor, SLOT(changeColorPoint(double,QColor)));

    connect(m_scalarOpacityView, SIGNAL(nodeAdded(double,double)), m_editor, SLOT(addScalarOpacityPoint(double,double)));

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
    disconnect(m_colorView, SIGNAL(nodeMoved(double,double)), m_editor, SLOT(moveColorPoint(double,double)));
    disconnect(m_colorView, SIGNAL(nodesMoved(QList<double>,double)), m_editor, SLOT(moveColorPoints(QList<double>,double)));
    disconnect(m_colorView, SIGNAL(nodeChangedColor(double,QColor)), m_editor, SLOT(changeColorPoint(double,QColor)));

    disconnect(m_scalarOpacityView, SIGNAL(nodeAdded(double,double)), m_editor, SLOT(addScalarOpacityPoint(double,double)));

    m_editingConnectionsEnabled = false;
}

} // namespace udg
