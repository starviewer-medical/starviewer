/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "restoretool.h"
#include "q2dviewer.h"
#include "logging.h"

namespace udg {

RestoreTool::RestoreTool( QViewer *viewer, QObject *parent )
: Tool( viewer, parent )
{
    m_toolName = "RestoreTool";
    m_hasSharedData = false;

    m_2DViewer = dynamic_cast<Q2DViewer*>(viewer);
    if( !m_2DViewer )
        DEBUG_LOG(QString("El casting no ha funcionat!!! És possible que viewer no sigui un Q2DViewer!!!-> ")+ viewer->metaObject()->className() );

    if( m_2DViewer->isActive() )
        m_2DViewer->restore();
}

RestoreTool::~RestoreTool()
{

}

void RestoreTool::handleEvent( unsigned long eventID )
{
    Q_UNUSED(eventID);
}

}
