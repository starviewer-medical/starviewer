/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "tool.h"

namespace udg {

Tool::Tool( QViewer *viewer, QObject *parent )
 : QObject(parent), m_viewer(viewer), m_toolConfiguration(0), m_toolData(0), m_hasSharedData(false)
{
}

Tool::~Tool()
{
}

void Tool::setConfiguration( ToolConfiguration *configuration )
{
    m_toolConfiguration = configuration;
}

ToolConfiguration *Tool::getConfiguration() const
{
    return m_toolConfiguration;
}

void Tool::setToolData( ToolData *data )
{
    m_toolData = data;
}

ToolData *Tool::getToolData() const
{
    return m_toolData;
}

bool Tool::hasSharedData() const
{
    return m_hasSharedData;
}

QString Tool::toolName()
{
    return m_toolName;
}

}
