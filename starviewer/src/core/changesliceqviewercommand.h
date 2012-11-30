#ifndef UDGCHANGESLICEQVIEWERCOMMAND_H
#define UDGCHANGESLICEQVIEWERCOMMAND_H

#include "qviewercommand.h"

namespace udg {

class Q2DViewer;

/**
    Command de Q2DViewer que permet especificar un canvi de llesca.
  */
class ChangeSliceQViewerCommand : public QViewerCommand {
Q_OBJECT
public:
    enum SlicePosition { MinimumSlice, MiddleSlice, MaximumSlice, CustomSlice };

    /// Constructor que permet especificar en quina posició volem situar el viewer sense haver de saber quantes imatges té el volume
    ChangeSliceQViewerCommand(Q2DViewer *viewer, SlicePosition slice, QObject *parent = 0);

    /// Constructor que permet especificar lliurement en quina llesca volem situar el volum. En cas que la llesca indicada estigui per sota del mínim
    /// la llesca assignada serà la mínima, i en cas que estigui per sobre del màxim, s'assignarà la màxima
    ChangeSliceQViewerCommand(Q2DViewer *viewer, int slice, QObject *parent = 0);

public slots:
    void execute();

private:
    Q2DViewer *m_viewer;
    SlicePosition m_slicePosition;
    int m_customSliceNumber;
};

}

#endif // UDGCHANGESLICEQVIEWERCOMMAND_H
