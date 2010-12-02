/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "representationslayer.h"
#include "q2dviewer.h"
#include "logging.h"
#include "drawer.h"
#include "drawerprimitive.h"

namespace udg {

ToolRepresentation::ToolRepresentation( Drawer *drawer, QObject *parent )
 : QObject(parent), m_drawer(drawer)
{
}

ToolRepresentation::~ToolRepresentation()
{
    foreach( DrawerPrimitive *primitive, m_primitivesList )
    {
        delete primitive;
    }
}

void ToolRepresentation::refresh()
{
    // TODO Mantenir els canvis que s'hagin fet a la branca sobre aquest mètode
}

}
