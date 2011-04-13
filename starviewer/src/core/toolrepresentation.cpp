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
