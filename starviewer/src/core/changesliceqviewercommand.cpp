#include "changesliceqviewercommand.h"

#include "q2dviewer.h"

namespace udg {

ChangeSliceQViewerCommand::ChangeSliceQViewerCommand(Q2DViewer *viewer, SlicePosition slice, QObject *parent)
    : QViewerCommand(parent)
{
    m_viewer = viewer;
    m_slicePosition = slice;
    m_customSliceNumber = 0;
}

ChangeSliceQViewerCommand::ChangeSliceQViewerCommand(Q2DViewer *viewer, int slice, QObject *parent)
    : QViewerCommand(parent)
{
    m_viewer = viewer;
    m_slicePosition = CustomSlice;
    m_customSliceNumber = slice;
}

void ChangeSliceQViewerCommand::execute()
{
    switch (m_slicePosition)
    {
        case MaximumSlice:
            m_viewer->setSlice(m_viewer->getMaximumSlice());
            break;
        case MinimumSlice:
            m_viewer->setSlice(m_viewer->getMinimumSlice());
            break;
        case MiddleSlice:
            {
                int middleSlice = (m_viewer->getMaximumSlice() - m_viewer->getMinimumSlice()) / 2;
                m_viewer->setSlice(middleSlice);
            }
            break;
        case CustomSlice:
            if (m_customSliceNumber < m_viewer->getMinimumSlice())
            {
                m_viewer->setSlice(m_viewer->getMinimumSlice());
            }
            else if (m_customSliceNumber > m_viewer->getMaximumSlice())
            {
                m_viewer->setSlice(m_viewer->getMaximumSlice());
            }
            else
            {
                m_viewer->setSlice(m_customSliceNumber);
            }
            break;
    }
}

}
