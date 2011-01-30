#include "renderqviewercommand.h"

#include "q2dviewer.h"

namespace udg {

RenderQViewerCommand::RenderQViewerCommand(Q2DViewer *viewer, QObject *parent)
    : QViewerCommand(parent)
{
    m_viewer = viewer;
}

void RenderQViewerCommand::execute()
{
    m_viewer->render();
}

} // End namespace udg
