#ifndef UDGSLICINGKEYBOARDTOOL_H
#define UDGSLICINGKEYBOARDTOOL_H

#include "tool.h"

namespace udg {

class Q2DViewer;
class Volume;

/**
    Tool per la sincronització de llesques a partir de teclat
*/
class SlicingKeyboardTool : public Tool {
Q_OBJECT
public:

    SlicingKeyboardTool(QViewer *viewer, QObject *parent = 0);
    ~SlicingKeyboardTool();

    void handleEvent(unsigned long eventID);

private:
    /// Ens guardem aquest punter per ser més còmode
    Q2DViewer *m_2DViewer;
};

}

#endif
