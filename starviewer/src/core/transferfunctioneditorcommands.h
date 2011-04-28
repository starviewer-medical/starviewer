#ifndef TRANSFERFUNCTIONEDITORCOMMANDS_H
#define TRANSFERFUNCTIONEDITORCOMMANDS_H

/**
    \file Aquí es defineixen tots els commands de TransferFunctionEditor, per no omplir massa transferfunctioneditor.h.
    No els documentarem individualment, però tots tenen el constructor, un redo() i un undo().
 */

#include <QUndoCommand>

#include "transferfunctioneditor.h"

#include <QColor>

namespace udg {

class TransferFunctionEditor::SetTransferFunctionCommand : public QUndoCommand {
public:
    SetTransferFunctionCommand(TransferFunctionEditor *editor, const TransferFunction &transferFunction);
    virtual void redo();
    virtual void undo();
private:
    TransferFunctionEditor *m_editor;
    TransferFunction m_oldTransferFunction;
    TransferFunction m_newTransferFunction;
};

class TransferFunctionEditor::SetNameCommand : public QUndoCommand {
public:
    SetNameCommand(TransferFunctionEditor *editor, const QString &name);
    virtual void redo();
    virtual void undo();
private:
    TransferFunctionEditor *m_editor;
    QString m_oldName;
    QString m_newName;
};

class TransferFunctionEditor::SetColorTransferFunctionCommand : public QUndoCommand {
public:
    SetColorTransferFunctionCommand(TransferFunctionEditor *editor, const ColorTransferFunction &colorTransferFunction);
    virtual void undo();
    virtual void redo();
private:
    TransferFunctionEditor *m_editor;
    ColorTransferFunction m_oldColorTransferFunction;
    ColorTransferFunction m_newColorTransferFunction;
};

class TransferFunctionEditor::SetScalarOpacityTransferFunctionCommand : public QUndoCommand {
public:
    SetScalarOpacityTransferFunctionCommand(TransferFunctionEditor *editor, const OpacityTransferFunction &scalarOpacityTransferFunction);
    virtual void undo();
    virtual void redo();
private:
    TransferFunctionEditor *m_editor;
    OpacityTransferFunction m_oldScalarOpacityTransferFunction;
    OpacityTransferFunction m_newScalarOpacityTransferFunction;
};

class TransferFunctionEditor::AddColorPointCommand : public QUndoCommand {
public:
    AddColorPointCommand(TransferFunctionEditor *editor, double x, const QColor &color);
    virtual void undo();
    virtual void redo();
private:
    TransferFunctionEditor *m_editor;
    double m_x;
    QColor m_color;
};

class TransferFunctionEditor::RemoveColorPointCommand : public QUndoCommand {
public:
    RemoveColorPointCommand(TransferFunctionEditor *editor, double x);
    virtual void undo();
    virtual void redo();
private:
    TransferFunctionEditor *m_editor;
    double m_x;
    QColor m_color;
};

class TransferFunctionEditor::MoveColorPointCommand : public QUndoCommand {
public:
    MoveColorPointCommand(TransferFunctionEditor *editor, double origin, double destination);
    virtual void undo();
    virtual void redo();
private:
    TransferFunctionEditor *m_editor;
    double m_origin;
    double m_destination;
};

class TransferFunctionEditor::ChangeColorPointCommand : public QUndoCommand {
public:
    ChangeColorPointCommand(TransferFunctionEditor *editor, double x, const QColor &color);
    virtual void undo();
    virtual void redo();
private:
    TransferFunctionEditor *m_editor;
    double m_x;
    QColor m_oldColor;
    QColor m_newColor;
};

class TransferFunctionEditor::AddScalarOpacityPointCommand : public QUndoCommand {
public:
    AddScalarOpacityPointCommand(TransferFunctionEditor *editor, double x, double opacity);
    virtual void undo();
    virtual void redo();
private:
    TransferFunctionEditor *m_editor;
    double m_x;
    double m_opacity;
};

class TransferFunctionEditor::RemoveScalarOpacityPointCommand : public QUndoCommand {
public:
    RemoveScalarOpacityPointCommand(TransferFunctionEditor *editor, double x);
    virtual void undo();
    virtual void redo();
private:
    TransferFunctionEditor *m_editor;
    double m_x;
    double m_opacity;
};

} // namespace udg

#endif // TRANSFERFUNCTIONEDITORCOMMANDS_H
