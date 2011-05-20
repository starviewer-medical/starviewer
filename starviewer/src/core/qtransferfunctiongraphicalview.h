#ifndef QTRANSFERFUNCTIONGRAPHICALVIEW_H
#define QTRANSFERFUNCTIONGRAPHICALVIEW_H

#include "qtransferfunctionview.h"
#include "ui_qtransferfunctiongraphicalviewbase.h"

namespace udg {

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
    /// Assigna la funció de transferència d'opacitat escalar.
    virtual void setScalarOpacityTransferFunction(const OpacityTransferFunction &scalarOpacityTransferFunction);
//    /// Assigna la funció de transferència d'opacitat del gradient.
//    virtual void setGradientOpacityTransferFunction(const OpacityTransferFunction &gradientOpacityTransferFunction);

    /// Afegeix un punt de color.
    virtual void addColorPoint(double x, const QColor &color);
    /// Esborra un punt de color.
    virtual void removeColorPoint(double x);

    /// Afegeix un punt d'opacitat escalar.
    virtual void addScalarOpacityPoint(double x, double opacity);
    /// Esborra un punt d'opacitat escalar.
    virtual void removeScalarOpacityPoint(double x);

//    /// Afegeix un punt d'opacitat del gradient.
//    virtual void addGradientOpacityPoint(double y, double opacity);
//    /// Esborra un punt d'opacitat del gradient.
//    virtual void removeGradientOpacityPoint(double y);

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

} // End namespace udg

#endif // QTRANSFERFUNCTIONGRAPHICALVIEW_H
