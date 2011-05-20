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
    enum SlicePosition { MinimumSlice, MiddleSlice, MaximumSlice };

    /// Constructor que permet especificar en quina posició volem situar el viewer sense haver de saber quantes imatges té el volume
    ChangeSliceQViewerCommand(Q2DViewer *viewer, SlicePosition slice, QObject *parent = 0);

public slots:
    void execute();

private:
    Q2DViewer *m_viewer;
    SlicePosition m_slicePosition;
};

}

#endif // UDGCHANGESLICEQVIEWERCOMMAND_H
