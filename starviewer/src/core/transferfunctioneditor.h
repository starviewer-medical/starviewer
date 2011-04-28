#ifndef TRANSFERFUNCTIONEDITOR_H
#define TRANSFERFUNCTIONEDITOR_H

#include <QObject>

#include "transferfunction.h"

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
    /// S'emet quan s'afegeix un punt de color.
    void colorPointAdded(double x, const QColor &color);
    /// S'emet quan s'esborra un punt de color.
    void colorPointRemoved(double x);
    /// S'emet quan es mou un punt de color.
    void colorPointMoved(double origin, double destination);
    /// S'emet quan es canvia el color d'un punt.
    void colorPointChanged(double x, const QColor &color);
    /// S'emet quan s'afegeix un punt d'opacitat escalar.
    void scalarOpacityPointAdded(double x, double opacity);
    /// S'emet quan s'esborra un punt d'opacitat escalar.
    void scalarOpacityPointRemoved(double x);

private:

    ///@{
    /// Commands per fer les tasques d'edició.
    class SetTransferFunctionCommand;
    class SetNameCommand;
    class SetColorTransferFunctionCommand;
    class SetScalarOpacityTransferFunctionCommand;
    class AddColorPointCommand;
    class RemoveColorPointCommand;
    class MoveColorPointCommand;
    class ChangeColorPointCommand;
    class AddScalarOpacityPointCommand;
    class RemoveScalarOpacityPointCommand;
    ///@}

    Q_DISABLE_COPY(TransferFunctionEditor)

    ///@{
    /// Mètodes privats que són cridats pels commands i que fan realment l'edició.
    void setTransferFunctionCommand(const TransferFunction &transferFunction);
    void setNameCommand(const QString &name);
    void setColorTransferFunctionCommand(const ColorTransferFunction &colorTransferFunction);
    void setScalarOpacityTransferFunctionCommand(const OpacityTransferFunction &scalarOpacityTransferFunction);
    void addColorPointCommand(double x, const QColor &color);
    void removeColorPointCommand(double x);
    void moveColorPointCommand(double origin, double destination);
    void changeColorPointCommand(double x, const QColor &color);
    void addScalarOpacityPointCommand(double x, double opacity);
    void removeScalarOpacityPointCommand(double x);
    ///@}

private:

    /// La funció de transferència.
    TransferFunction m_transferFunction;
    /// Pila de desfer/refer.
    QUndoStack *m_undoStack;

};

} // namespace udg

#endif // TRANSFERFUNCTIONEDITOR_H
