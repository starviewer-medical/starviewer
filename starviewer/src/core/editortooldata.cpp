/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
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

void EditorToolData::setVolumeVoxels( int vol )
{
    m_volumeCont = vol;
}

int EditorToolData::getVolumeVoxels( )
{
    return m_volumeCont;
}

}
