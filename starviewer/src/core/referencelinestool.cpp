/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "referencelinestool.h"

namespace udg {

ReferenceLinesTool::ReferenceLinesTool( QViewer *viewer, QObject *parent )
 : Tool(viewer, parent)
{
    m_toolName = "ReferenceLinesTool";
    m_hasSharedData = true;
}

ReferenceLinesTool::~ReferenceLinesTool()
{
}

}
