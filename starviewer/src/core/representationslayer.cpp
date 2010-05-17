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

namespace udg {

RepresentationsLayer::RepresentationsLayer( Q2DViewer *viewer, QObject *parent )
 : QObject(parent)
{
    m_2DViewer = viewer;
    connect( m_2DViewer, SIGNAL(sliceChanged(int)), SLOT(refresh()) );
    connect( m_2DViewer, SIGNAL(viewChanged(int)), SLOT(refresh()) );
}

RepresentationsLayer::~RepresentationsLayer()
{
}

void RepresentationsLayer::addRepresentation(ToolRepresentation *toolRepresentation)
{
    switch (m_currentPlane )
    {
    case QViewer::AxialPlane:
        m_axialToolRepresentations.insert( m_currentSlice, toolRepresentation );
    break;

    case QViewer::SagitalPlane:
        m_sagitalToolRepresentations.insert( m_currentSlice, toolRepresentation );
    break;

    case QViewer::CoronalPlane:
        m_coronalToolRepresentations.insert( m_currentSlice, toolRepresentation );
    break;

    default:
        DEBUG_LOG("Pla no definit!");
        return;
    break;
    }
}

void RepresentationsLayer::clearViewer()
{
    QList<ToolRepresentation *> list;
    QMultiMap< int, ToolRepresentation *>::const_iterator it;
    switch( m_currentPlane )
    {
    case QViewer::AxialPlane:
        it = m_axialToolRepresentations.find( m_currentSlice );
        while( it != m_axialToolRepresentations.end() && it.key() == m_currentSlice )
        {
            list << it.value();
            it++;
        }
        //elimina les primitives del contenidor
        m_axialToolRepresentations.remove( m_currentSlice );
    break;

    case QViewer::SagitalPlane:
        it = m_sagitalToolRepresentations.find( m_currentSlice );
        while( it != m_sagitalToolRepresentations.end() && it.key() == m_currentSlice )
        {
            list << it.value();
            it++;
        }
        //elimina les primitives del contenidor
        m_sagitalToolRepresentations.remove( m_currentSlice );
    break;

    case QViewer::CoronalPlane:
        it = m_coronalToolRepresentations.find( m_currentSlice );
        while( it != m_coronalToolRepresentations.end() && it.key() == m_currentSlice )
        {
            list << it.value();
            it++;
        }
        //elimina les primitives del contenidor
        m_coronalToolRepresentations.remove( m_currentSlice );
    break;

    default:
        DEBUG_LOG("Pla no definit!");
        return;
    break;
    }

    foreach(ToolRepresentation *representation, list)
    {
        delete representation;
    }

    m_2DViewer->refresh();
}

void RepresentationsLayer::clearAll()
{
    QMultiMap< int, ToolRepresentation *>::const_iterator it;

    //Axial
    it = m_axialToolRepresentations.begin();
    while( it != m_axialToolRepresentations.end() )
    {
        delete it.value();
        it++;
    }

    //Sagital
    it = m_sagitalToolRepresentations.begin();
    while( it != m_sagitalToolRepresentations.end() )
    {
        delete it.value();
        it++;
    }

    //Coronal
    it = m_coronalToolRepresentations.begin();
    while( it != m_coronalToolRepresentations.end() )
    {
        delete it.value();
        it++;
    }
}

void RepresentationsLayer::handleEvent(long unsigned eventID, double posX, double posY)
{
    //De moment no fa res...
}

void RepresentationsLayer::refresh()
{
    m_currentPlane = m_2DViewer->getView();
    m_currentSlice = m_2DViewer->getCurrentSlice();
}


}
