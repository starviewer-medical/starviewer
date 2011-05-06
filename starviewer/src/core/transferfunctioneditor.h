#ifndef TRANSFERFUNCTIONEDITOR_H
#define TRANSFERFUNCTIONEDITOR_H

#include <QObject>

#include "transferfunction.h"

#include <QPair>

class QPointF;
class QUndoStack;

namespace udg {

/**
    Editor de funcions de transferència des del codi (no és un widget). Ofereix algunes comoditats a l'edició de funcions, que no es tenen si es fa "a pèl",
    com per exemple alguns mètodes addicionals (move*), signals i slots, i la possiblitat de desfer i refer els canvis.
    És qui fa realment la "feina bruta" per a QTransferFuncionEditor2.
 */
class TransferFunctionEditor : public QObject {

    Q_OBJECT

public:

    /// Constructor.
    explicit TransferFunctionEditor(QObject *parent = 0);

    /// Retorna la funció de transferència representada actualment.
    const TransferFunction& transferFunction() const;

    /// Retorna la pila de desfer/refer.
    /// \warning Aquest mètode ha de desaparèixer quan estigui l'editor acabat.
    QUndoStack* undoStack() const;

public slots:

    /// Assigna la funció de transferència.
    void setTransferFunction(const TransferFunction &transferFunction);

    /// Assigna el nom de la funció de transferència.
    void setName(const QString &name);
    /// Assigna la funció de transferència de color.
    void setColorTransferFunction(const ColorTransferFunction &colorTransferFunction);
    /// Assigna la funció de transferència d'opacitat escalar.
    void setScalarOpacityTransferFunction(const OpacityTransferFunction &scalarOpacityTransferFunction);
    /// Assigna la funció de transferència d'opacitat del gradient.
    void setGradientOpacityTransferFunction(const OpacityTransferFunction &gradientOpacityTransferFunction);

    /// Afegeix un punt de color. Si ja existeix un punt de color a x li canvia el color.
    void addColorPoint(double x, const QColor &color);
    /// Esborra un punt de color. Si no existeix no fa res.
    void removeColorPoint(double x);
    /// Mou un punt de color. Si no existeix l'origen no fa res. Si ja existeix la destinació esborra l'origen i assigna el color de l'origen a la destinació.
    void moveColorPoint(double origin, double destination);
    /// Mou diversos punts de color alhora, sumant offset a cada origen.
    void moveColorPoints(const QList<double> &origins, double offset);
    /// Canvia el color d'un punt. Si no existeix no fa res.
    void changeColorPoint(double x, const QColor &color);

    /// Afegeix un punt d'opacitat escalar. Si ja existeix un punt d'opacitat escalar a x li canvia l'opacitat.
    void addScalarOpacityPoint(double x, double opacity);
    /// Esborra un punt d'opacitat escalar. Si no existeix no fa res.
    void removeScalarOpacityPoint(double x);
    /// Mou un punt d'opacitat escalar. Si no existeix l'origen no fa res.
    /// Si ja existeix la destinació esborra l'origen i assigna l'opacitat de l'origen a la destinació.
    void moveScalarOpacityPoint(double origin, double destination);
    /// Canvia l'opacitat escalar d'un punt. Si no existeix no fa res.
    void changeScalarOpacityPoint(double x, double opacity);
    /// Mou i canvia l'opacitat escalar de diversos punts alhora. La llista de moviments té el format [(x1, (x2,o2)), ...].
    void moveAndChangeScalarOpacityPoints(const QList< QPair<double, QPointF> > &moves);

    /// Afegeix un punt d'opacitat del gradient. Si ja existeix un punt d'opacitat del gradient a x li canvia l'opacitat.
    void addGradientOpacityPoint(double y, double opacity);
    /// Esborra un punt d'opacitat del gradient. Si no existeix no fa res.
    void removeGradientOpacityPoint(double y);
    /// Mou un punt d'opacitat del gradient. Si no existeix l'origen no fa res.
    /// Si ja existeix la destinació esborra l'origen i assigna l'opacitat de l'origen a la destinació.
    void moveGradientOpacityPoint(double origin, double destination);
    /// Canvia l'opacitat del gradient d'un punt. Si no existeix no fa res.
    void changeGradientOpacityPoint(double y, double opacity);
    /// Mou i canvia l'opacitat del gradient de diversos punts alhora. La llista de moviments té el format [(y1, (y2,o2)), ...].
    void moveAndChangeGradientOpacityPoints(const QList< QPair<double, QPointF> > &moves);

    /// Desfà l'última edició feta.
    void redo();
    /// Refà l'última edició desfeta.
    void undo();

signals:

    /// S'emet quan canvia la funció de transferència.
    void transferFunctionChanged(const TransferFunction &transferFunction);
    /// S'emet quan canvia el nom de la funció de transferència.
    void nameChanged(const QString &name);
    /// S'emet quan canvia la funció de transferència de color.
    void colorTransferFunctionChanged(const ColorTransferFunction &colorTransferFunction);
    /// S'emet quan canvia la funció de transferència d'opacitat escalar.
    void scalarOpacityTransferFunctionChanged(const OpacityTransferFunction &scalarOpacityTransferFunction);
    /// S'emet quan canvia la funció de transferència d'opacitat del gradient.
    void gradientOpacityTransferFunctionChanged(const OpacityTransferFunction &gradientOpacityTransferFunction);
    /// S'emet quan s'afegeix un punt de color.
    void colorPointAdded(double x, const QColor &color);
    /// S'emet quan s'esborra un punt de color.
    void colorPointRemoved(double x);
    /// S'emet quan s'afegeix un punt d'opacitat escalar.
    void scalarOpacityPointAdded(double x, double opacity);
    /// S'emet quan s'esborra un punt d'opacitat escalar.
    void scalarOpacityPointRemoved(double x);
    /// S'emet quan s'afegeix un punt d'opacitat del gradient.
    void gradientOpacityPointAdded(double y, double opacity);
    /// S'emet quan s'esborra un punt d'opacitat del gradient.
    void gradientOpacityPointRemoved(double y);

private:

    ///@{
    /// Commands per fer les tasques d'edició.
    class SetTransferFunctionCommand;
    class SetNameCommand;
    class SetColorTransferFunctionCommand;
    class SetScalarOpacityTransferFunctionCommand;
    class SetGradientOpacityTransferFunctionCommand;
    class AddColorPointCommand;
    class RemoveColorPointCommand;
    class AddScalarOpacityPointCommand;
    class RemoveScalarOpacityPointCommand;
    class AddGradientOpacityPointCommand;
    class RemoveGradientOpacityPointCommand;
    ///@}

    Q_DISABLE_COPY(TransferFunctionEditor)

    ///@{
    /// Mètodes privats que són cridats pels commands i que fan realment l'edició.
    void setTransferFunctionCommand(const TransferFunction &transferFunction);
    void setNameCommand(const QString &name);
    void setColorTransferFunctionCommand(const ColorTransferFunction &colorTransferFunction);
    void setScalarOpacityTransferFunctionCommand(const OpacityTransferFunction &scalarOpacityTransferFunction);
    void setGradientOpacityTransferFunctionCommand(const OpacityTransferFunction &gradientOpacityTransferFunction);
    void addColorPointCommand(double x, const QColor &color);
    void removeColorPointCommand(double x);
    void addScalarOpacityPointCommand(double x, double opacity);
    void removeScalarOpacityPointCommand(double x);
    void addGradientOpacityPointCommand(double y, double opacity);
    void removeGradientOpacityPointCommand(double y);
    ///@}

private:

    /// La funció de transferència.
    TransferFunction m_transferFunction;
    /// Pila de desfer/refer.
    QUndoStack *m_undoStack;

};

} // namespace udg

#endif // TRANSFERFUNCTIONEDITOR_H
