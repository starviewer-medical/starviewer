#ifndef UDGRENDERQVIEWERCOMMAND_H
#define UDGRENDERQVIEWERCOMMAND_H

#include "qviewercommand.h"

namespace udg {

class Q2DViewer;

/**
  Command de Q2Viewer que realitza un render() del viewer.
  */

class RenderQViewerCommand : public QViewerCommand
{
    Q_OBJECT
public:
    RenderQViewerCommand(Q2DViewer *viewer, QObject *parent = 0);

public slots:
    void execute();

private:
    Q2DViewer *m_viewer;
};

}

#endif // UDGRENDERQVIEWERCOMMAND_H
