/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "toolmanager.h"
#include "qviewer.h"
#include "tool.h"

namespace udg {

ToolManager::ToolManager(QObject *parent)
 : QObject(parent)
{
    m_currentTool = 0;
}

ToolManager::~ToolManager()
{
}

}
