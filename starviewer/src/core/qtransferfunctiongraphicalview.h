#ifndef QTRANSFERFUNCTIONGRAPHICALVIEW_H
#define QTRANSFERFUNCTIONGRAPHICALVIEW_H

#include "qtransferfunctionview.h"
#include "ui_qtransferfunctiongraphicalviewbase.h"

namespace udg {

class TransferFunction;

/**
    Vista gràfica d'una funció de transferència.
 */
class QTransferFunctionGraphicalView : public QTransferFunctionView, private Ui::QTransferFunctionGraphicalViewBase {

    Q_OBJECT

public:

    /// Crea la vista, que treballarà amb l'editor donat.
    explicit QTransferFunctionGraphicalView(TransferFunctionEditor *editor, QWidget *parent = 0);

protected:

    /// Fa les connexions pertinents de signals i slots.
    virtual void makeConnections();

protected slots:

    /// Assigna la funció de transferència.
    virtual void setTransferFunction(const TransferFunction &transferFunction);
    /// Assigna el nom de la funció de transferència.
    virtual void setName(const QString &name);
    /// Assigna la funció de transferència de color.
    virtual void setColorTransferFunction(const ColorTransferFunction &colorTransferFunction);
    /// Afegeix un punt de color.
    virtual void addColorPoint(double x, const QColor &color);
    /// Esborra un punt de color.
    virtual void removeColorPoint(double x);
    /// Mou un punt de color.
    virtual void moveColorPoint(double origin, double destination);
    /// Canvia el color d'un punt.
    virtual void changeColorPoint(double x, const QColor &color);

private:

    Q_DISABLE_COPY(QTransferFunctionGraphicalView)

    /// Habilita les connexions de la vista cap a l'editor.
    void enableEditingConnections();
    /// Inhabilita les connexions de la vista cap a l'editor.
    void disableEditingConnections();

private:

    /// Indica si les connexions de la vista cap a l'editor estan habilitades o no.
    bool m_editingConnectionsEnabled;

};

} // namespace udg

#endif // QTRANSFERFUNCTIONGRAPHICALVIEW_H
