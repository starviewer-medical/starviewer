#include "editortooldata.h"
#include "logging.h"

namespace udg {

EditorToolData::EditorToolData(QObject *parent)
 : ToolData(parent)
{
    m_volumeCont = 0;
}

EditorToolData::~EditorToolData()
{
}

void EditorToolData::setVolumeVoxels(int vol)
{
    m_volumeCont = vol;
}

int EditorToolData::getVolumeVoxels()
{
    return m_volumeCont;
}

}
