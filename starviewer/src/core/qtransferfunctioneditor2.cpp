#include "qtransferfunctioneditor2.h"

#include "qtransferfunctiongraphicalview.h"
#include "transferfunctioneditor.h"

namespace udg {

QTransferFunctionEditor2::QTransferFunctionEditor2(QWidget *parent)
    : QWidget(parent)
{
    init(Everything, Graphical);
}

QTransferFunctionEditor2::QTransferFunctionEditor2(Features features, View view, QWidget *parent)
    : QWidget(parent)
{
    init(features, view);
}

const TransferFunction& QTransferFunctionEditor2::transferFunction() const
{
    return m_editor->transferFunction();
}

void QTransferFunctionEditor2::setTransferFunction(const TransferFunction &transferFunction)
{
    m_editor->setTransferFunction(transferFunction);
}

void QTransferFunctionEditor2::init(Features features, View view)
{
    setupUi(this);

    // els farem servir en el futur, quan ja estiguin implementades les coses bàsiques
    Q_UNUSED(features);
    Q_UNUSED(view);

    m_editor = new TransferFunctionEditor(this);
    m_view = new QTransferFunctionGraphicalView(m_editor, this);
    m_viewLayout->addWidget(m_view);
    m_undoView->setStack(m_editor->undoStack());

    makeConnections();
}

void QTransferFunctionEditor2::makeConnections()
{
    connect(m_editor, SIGNAL(transferFunctionChanged(TransferFunction)), SLOT(emitTransferFunctionChanged()));
    connect(m_editor, SIGNAL(nameChanged(QString)), SLOT(emitTransferFunctionChanged()));
    // afegir connexions per tots els signals de l'editor
}

void QTransferFunctionEditor2::emitTransferFunctionChanged()
{
    emit transferFunctionChanged(m_editor->transferFunction());
}

} // namespace udg
