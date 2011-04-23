#ifndef QTRANSFERFUNCTIONVIEW_H
#define QTRANSFERFUNCTIONVIEW_H

#include <QWidget>

namespace udg {

class TransferFunctionEditor;

/**
    Classe base per a totes les vistes de funcions de transferència.
 */
class QTransferFunctionView : public QWidget {

    Q_OBJECT

public:

    /// Crea la vista, que treballarà amb l'editor donat.
    explicit QTransferFunctionView(TransferFunctionEditor *editor, QWidget *parent = 0);

protected:

    /// L'editor.
    TransferFunctionEditor *m_editor;

private:

    Q_DISABLE_COPY(QTransferFunctionView)

};

} // namespace udg

#endif // QTRANSFERFUNCTIONVIEW_H
