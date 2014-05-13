/*************************************************************************************
  Copyright (C) 2014 Laboratori de Gràfics i Imatge, Universitat de Girona &
  Institut de Diagnòstic per la Imatge.
  Girona 2014. All rights reserved.
  http://starviewer.udg.edu

  This file is part of the Starviewer (Medical Imaging Software) open source project.
  It is subject to the license terms in the LICENSE file found in the top-level
  directory of this distribution and at http://starviewer.udg.edu/license. No part of
  the Starviewer (Medical Imaging Software) open source project, including this file,
  may be copied, modified, propagated, or distributed except according to the
  terms contained in the LICENSE file.
 *************************************************************************************/

#include "tool.h"

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

}
