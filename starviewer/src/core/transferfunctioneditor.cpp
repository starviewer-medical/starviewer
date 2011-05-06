#include "transferfunctioneditor.h"

#include "transferfunctioneditorcommands.h"

#include <QPair>
#include <QUndoStack>

namespace udg {

TransferFunctionEditor::TransferFunctionEditor(QObject *parent)
    : QObject(parent)
{
    m_undoStack = new QUndoStack(this);
}

const TransferFunction& TransferFunctionEditor::transferFunction() const
{
    return m_transferFunction;
}

QUndoStack* TransferFunctionEditor::undoStack() const
{
    return m_undoStack;
}

void TransferFunctionEditor::setTransferFunction(const TransferFunction &transferFunction)
{
    m_undoStack->push(new SetTransferFunctionCommand(this, transferFunction));
}

void TransferFunctionEditor::setTransferFunctionCommand(const TransferFunction &transferFunction)
{
    m_transferFunction = transferFunction;
    emit transferFunctionChanged(transferFunction);
}

void TransferFunctionEditor::setName(const QString &name)
{
    m_undoStack->push(new SetNameCommand(this, name));
}

void TransferFunctionEditor::setNameCommand(const QString &name)
{
    m_transferFunction.setName(name);
    emit nameChanged(name);
}

void TransferFunctionEditor::setColorTransferFunction(const ColorTransferFunction &colorTransferFunction)
{
    m_undoStack->push(new SetColorTransferFunctionCommand(this, colorTransferFunction));
}

void TransferFunctionEditor::setColorTransferFunctionCommand(const ColorTransferFunction &colorTransferFunction)
{
    m_transferFunction.setColorTransferFunction(colorTransferFunction);
    emit colorTransferFunctionChanged(colorTransferFunction);
}

void TransferFunctionEditor::setScalarOpacityTransferFunction(const OpacityTransferFunction &scalarOpacityTransferFunction)
{
    m_undoStack->push(new SetScalarOpacityTransferFunctionCommand(this, scalarOpacityTransferFunction));
}

void TransferFunctionEditor::setScalarOpacityTransferFunctionCommand(const OpacityTransferFunction &scalarOpacityTransferFunction)
{
    m_transferFunction.setScalarOpacityTransferFunction(scalarOpacityTransferFunction);
    emit scalarOpacityTransferFunctionChanged(scalarOpacityTransferFunction);
}

void TransferFunctionEditor::setGradientOpacityTransferFunction(const OpacityTransferFunction &gradientOpacityTransferFunction)
{
    m_undoStack->push(new SetGradientOpacityTransferFunctionCommand(this, gradientOpacityTransferFunction));
}

void TransferFunctionEditor::setGradientOpacityTransferFunctionCommand(const OpacityTransferFunction &gradientOpacityTransferFunction)
{
    m_transferFunction.setGradientOpacityTransferFunction(gradientOpacityTransferFunction);
    emit gradientOpacityTransferFunctionChanged(gradientOpacityTransferFunction);
}

void TransferFunctionEditor::addColorPoint(double x, const QColor &color)
{
    if (m_transferFunction.isSetColor(x))
    {
        m_undoStack->push(new ChangeColorPointCommand(this, x, color));
    }
    else
    {
        m_undoStack->push(new AddColorPointCommand(this, x, color));
    }
}

void TransferFunctionEditor::addColorPointCommand(double x, const QColor &color)
{
    Q_ASSERT(!m_transferFunction.isSetColor(x));
    m_transferFunction.setColor(x, color);
    emit colorPointAdded(x, color);
}

void TransferFunctionEditor::removeColorPoint(double x)
{
    if (m_transferFunction.isSetColor(x))
    {
        m_undoStack->push(new RemoveColorPointCommand(this, x));
    }
}

void TransferFunctionEditor::removeColorPointCommand(double x)
{
    Q_ASSERT(m_transferFunction.isSetColor(x));
    m_transferFunction.unsetColor(x);
    emit colorPointRemoved(x);
}

void TransferFunctionEditor::moveColorPoint(double origin, double destination)
{
    if (m_transferFunction.isSetColor(origin))
    {
        if (m_transferFunction.isSetColor(destination))
        {
            m_undoStack->beginMacro(tr("Move color point from %1 to %2").arg(origin).arg(destination));
            m_undoStack->push(new ChangeColorPointCommand(this, destination, m_transferFunction.getColor(origin)));
            m_undoStack->push(new RemoveColorPointCommand(this, origin));
            m_undoStack->endMacro();
        }
        else
        {
            m_undoStack->push(new MoveColorPointCommand(this, origin, destination));
        }
    }
}

void TransferFunctionEditor::moveColorPointCommand(double origin, double destination)
{
    QColor color = m_transferFunction.getColor(origin);
    m_transferFunction.unsetColor(origin);
    m_transferFunction.setColor(destination, color);
    emit colorPointMoved(origin, destination);
}

// TODO: és correcta la implementació?
void TransferFunctionEditor::moveColorPoints(const QList<double> &origins, double offset)
{
    if (origins.isEmpty())
    {
        return;
    }

    // Creem una llista d'accions a fer per cada origen i destinació

    // fem servir enters perquè C++2003 no accepta un tipus local (per exemple enum) com a argument de template
    // amb C++0x sí que es pot posar un enum
    const char Add = 0, Change = 1, Remove = 2;
    QMap<double, QPair<QColor, char> > actions;
    int numberOfMoves = 0;

    foreach (double origin, origins)
    {
        if (!m_transferFunction.isSetColor(origin)) // l'origen no està definit a la funció, passem
        {
            continue;
        }

        double destination = origin + offset;

        if (actions.contains(origin))   // l'origen ja existeix a la llista, hauria de ser com a destinació, és a dir, Add o Change
        {
            Q_ASSERT(actions[origin].second != Remove);
        }
        else    // l'origen no existeix a la llista, el marquem per esborrar
        {
            actions[origin] = qMakePair(QColor(), Remove);
        }

        if (actions.contains(destination))  // la destinació ja existeix a la llista, hauria de ser com a origen, és a dir, Remove; la marquem per canviar
        {
            Q_ASSERT(actions[destination].second == Remove);
            actions[destination] = qMakePair(m_transferFunction.getColor(origin), Change);
        }
        else    // la destinació no existeix a la llista
        {
            if (m_transferFunction.isSetColor(destination)) // la destinació està definida a la funció, la marquem per canviar
            {
                actions[destination] = qMakePair(m_transferFunction.getColor(origin), Change);
            }
            else    // la destinació no està definida a la funció, la marquem per afegir
            {
                actions[destination] = qMakePair(m_transferFunction.getColor(origin), Add);
            }
        }

        numberOfMoves++;
    }

    // Executem les accions decidides

    QList<double> keys = actions.keys();
    m_undoStack->beginMacro(tr("Move %1 color points").arg(numberOfMoves));

    for (int i = 0; i < keys.size(); i++)
    {
        double x = keys.at(i);
        QColor color = actions[x].first;
        char action = actions[x].second;

        switch (action)
        {
            case Add: m_undoStack->push(new AddColorPointCommand(this, x, color)); break;
            case Change: m_undoStack->push(new ChangeColorPointCommand(this, x, color)); break;
            case Remove: m_undoStack->push(new RemoveColorPointCommand(this, x)); break;
            default: Q_ASSERT(false); break;    // no ha de passar
        }
    }

    m_undoStack->endMacro();
}

void TransferFunctionEditor::changeColorPoint(double x, const QColor &color)
{
    m_undoStack->push(new ChangeColorPointCommand(this, x, color));
}

void TransferFunctionEditor::changeColorPointCommand(double x, const QColor &color)
{
    Q_ASSERT(m_transferFunction.isSetColor(x));
    m_transferFunction.setColor(x, color);
    emit colorPointChanged(x, color);
}

void TransferFunctionEditor::addScalarOpacityPoint(double x, double opacity)
{
    if (m_transferFunction.isSetScalarOpacity(x))
    {
        changeScalarOpacityPoint(x, opacity);
    }
    else
    {
        m_undoStack->push(new AddScalarOpacityPointCommand(this, x, opacity));
    }
}

void TransferFunctionEditor::addScalarOpacityPointCommand(double x, double opacity)
{
    Q_ASSERT(!m_transferFunction.isSetScalarOpacity(x));
    m_transferFunction.setScalarOpacity(x, opacity);
    emit scalarOpacityPointAdded(x, opacity);
}

void TransferFunctionEditor::removeScalarOpacityPoint(double x)
{
    if (m_transferFunction.isSetScalarOpacity(x))
    {
        m_undoStack->push(new RemoveScalarOpacityPointCommand(this, x));
    }
}

void TransferFunctionEditor::removeScalarOpacityPointCommand(double x)
{
    Q_ASSERT(m_transferFunction.isSetScalarOpacity(x));
    m_transferFunction.unsetScalarOpacity(x);
    emit scalarOpacityPointRemoved(x);
}

void TransferFunctionEditor::moveScalarOpacityPoint(double origin, double destination)
{
    if (m_transferFunction.isSetScalarOpacity(origin))
    {
        m_undoStack->beginMacro(tr("Move scalar opacity point from %1 to %2").arg(origin).arg(destination));
        if (m_transferFunction.isSetScalarOpacity(destination))
        {
            changeScalarOpacityPoint(destination, m_transferFunction.getScalarOpacity(origin));
        }
        else
        {
            addScalarOpacityPoint(destination, m_transferFunction.getScalarOpacity(origin));
        }
        removeScalarOpacityPoint(origin);
        m_undoStack->endMacro();
    }
}

void TransferFunctionEditor::moveScalarOpacityPoints(const QList<double> &origins, double offset)
{
    if (origins.isEmpty())
    {
        return;
    }
    
    // Recorrem els orígens per apuntar els punts que hem d'esborrar i els que hem d'afegir
    QList<double> removes;
    removes.reserve(origins.size());
    QList< QPair<double, double> > adds;
    adds.reserve(origins.size());
    foreach (double origin, origins)
    {
        if (m_transferFunction.isSetScalarOpacity(origin))
        {
            removes.append(origin); // apuntem el punt que hem d'esborrar
            adds.append(qMakePair(origin + offset, m_transferFunction.getScalarOpacity(origin)));   // apuntem el punt que hem d'afegir
        }
    }

    if (removes.isEmpty())
    {
        return;
    }

    // Fem l'edició
    m_undoStack->beginMacro(tr("Move %1 scalar opacity points").arg(removes.size()));
    for (int i = 0; i < removes.size(); i++)    // esborrem punts
    {
        removeScalarOpacityPoint(removes.at(i));
    }
    for (int i = 0; i < adds.size(); i++)   // afegim punts
    {
        addScalarOpacityPoint(adds.at(i).first, adds.at(i).second);
    }
    m_undoStack->endMacro();
}

void TransferFunctionEditor::changeScalarOpacityPoint(double x, double opacity)
{
    if (m_transferFunction.isSetScalarOpacity(x))
    {
        m_undoStack->beginMacro(tr("Change scalar opacity point at %1 from %2 to %3").arg(x).arg(m_transferFunction.getScalarOpacity(x)).arg(opacity));
        removeScalarOpacityPoint(x);
        addScalarOpacityPoint(x, opacity);
        m_undoStack->endMacro();
    }
}

void TransferFunctionEditor::addGradientOpacityPoint(double y, double opacity)
{
    if (m_transferFunction.isSetGradientOpacity(y))
    {
        changeGradientOpacityPoint(y, opacity);
    }
    else
    {
        m_undoStack->push(new AddGradientOpacityPointCommand(this, y, opacity));
    }
}

void TransferFunctionEditor::addGradientOpacityPointCommand(double y, double opacity)
{
    Q_ASSERT(!m_transferFunction.isSetGradientOpacity(y));
    m_transferFunction.setGradientOpacity(y, opacity);
    emit gradientOpacityPointAdded(y, opacity);
}

void TransferFunctionEditor::removeGradientOpacityPoint(double y)
{
    if (m_transferFunction.isSetGradientOpacity(y))
    {
        m_undoStack->push(new RemoveGradientOpacityPointCommand(this, y));
    }
}

void TransferFunctionEditor::removeGradientOpacityPointCommand(double y)
{
    Q_ASSERT(m_transferFunction.isSetGradientOpacity(y));
    m_transferFunction.unsetGradientOpacity(y);
    emit gradientOpacityPointRemoved(y);
}

void TransferFunctionEditor::moveGradientOpacityPoint(double origin, double destination)
{
    if (m_transferFunction.isSetGradientOpacity(origin))
    {
        m_undoStack->beginMacro(tr("Move gradient opacity point from %1 to %2").arg(origin).arg(destination));
        if (m_transferFunction.isSetGradientOpacity(destination))
        {
            changeGradientOpacityPoint(destination, m_transferFunction.getGradientOpacity(origin));
        }
        else
        {
            addGradientOpacityPoint(destination, m_transferFunction.getGradientOpacity(origin));
        }
        removeGradientOpacityPoint(origin);
        m_undoStack->endMacro();
    }
}

void TransferFunctionEditor::moveGradientOpacityPoints(const QList<double> &origins, double offset)
{
    if (origins.isEmpty())
    {
        return;
    }
    
    // Recorrem els orígens per apuntar els punts que hem d'esborrar i els que hem d'afegir
    QList<double> removes;
    removes.reserve(origins.size());
    QList< QPair<double, double> > adds;
    adds.reserve(origins.size());
    foreach (double origin, origins)
    {
        if (m_transferFunction.isSetGradientOpacity(origin))
        {
            removes.append(origin); // apuntem el punt que hem d'esborrar
            adds.append(qMakePair(origin + offset, m_transferFunction.getGradientOpacity(origin))); // apuntem el punt que hem d'afegir
        }
    }

    if (removes.isEmpty())
    {
        return;
    }

    // Fem l'edició
    m_undoStack->beginMacro(tr("Move %1 gradient opacity points").arg(removes.size()));
    for (int i = 0; i < removes.size(); i++)    // esborrem punts
    {
        removeGradientOpacityPoint(removes.at(i));
    }
    for (int i = 0; i < adds.size(); i++)   // afegim punts
    {
        addGradientOpacityPoint(adds.at(i).first, adds.at(i).second);
    }
    m_undoStack->endMacro();
}

void TransferFunctionEditor::changeGradientOpacityPoint(double y, double opacity)
{
    if (m_transferFunction.isSetGradientOpacity(y))
    {
        m_undoStack->beginMacro(tr("Change gradient opacity point at %1 from %2 to %3").arg(y).arg(m_transferFunction.getGradientOpacity(y)).arg(opacity));
        removeGradientOpacityPoint(y);
        addGradientOpacityPoint(y, opacity);
        m_undoStack->endMacro();
    }
}

void TransferFunctionEditor::redo()
{
    m_undoStack->redo();
}

void TransferFunctionEditor::undo()
{
    m_undoStack->undo();
}

} // namespace udg
