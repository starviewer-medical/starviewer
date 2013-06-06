#include "tool.h"

#include "q2dviewer.h"
#include "logging.h"

namespace udg {

Tool::Tool(QViewer *viewer, QObject *parent)
 : QObject(parent), m_viewer(viewer), m_toolConfiguration(0), m_toolData(0), m_hasSharedData(false), m_hasPersistentData(false)
{
}

Tool::~Tool()
{
}

void Tool::setConfiguration(ToolConfiguration *configuration)
{
    m_toolConfiguration = configuration;
}

ToolConfiguration* Tool::getConfiguration() const
{
    return m_toolConfiguration;
}

void Tool::setToolData(ToolData *data)
{
    m_toolData = data;
}

ToolData* Tool::getToolData() const
{
    return m_toolData;
}

bool Tool::hasSharedData() const
{
    return m_hasSharedData;
}

bool Tool::hasPersistentData() const
{
    return m_hasPersistentData;
}

QString Tool::toolName()
{
    return m_toolName;
}

Q2DViewer* Tool::castToQ2DViewer(QViewer *viewer)
{
    if (!viewer)
    {
        DEBUG_LOG(QString("Cannot cas a null pointer"));
        return 0;
    }

    Q2DViewer *viewer2D = qobject_cast<Q2DViewer*>(viewer);
    if (!viewer2D)
    {
        DEBUG_LOG(QString("Q2DViewer casting failed! Provided viewer may not be a Q2DViewer object: %1").arg(viewer->metaObject()->className()));
    }

    return viewer2D;
}

}
