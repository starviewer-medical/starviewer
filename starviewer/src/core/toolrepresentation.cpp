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

#include "representationslayer.h"
#include "q2dviewer.h"
#include "logging.h"
#include "drawer.h"
#include "drawerprimitive.h"

namespace udg {

ToolRepresentation::ToolRepresentation(Drawer *drawer, QObject *parent)
 : QObject(parent), m_drawer(drawer)
{
}

ToolRepresentation::~ToolRepresentation()
{
    foreach (DrawerPrimitive *primitive, m_primitivesList)
    {
        delete primitive;
    }
}

void ToolRepresentation::refresh()
{
    // TODO Mantenir els canvis que s'hagin fet a la branca sobre aquest mètode
}

}
