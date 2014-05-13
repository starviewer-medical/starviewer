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

namespace udg {

RepresentationsLayer::RepresentationsLayer(Q2DViewer *viewer, QObject *parent)
 : QObject(parent)
{
    m_2DViewer = viewer;
    connect(m_2DViewer, SIGNAL(sliceChanged(int)), SLOT(refresh()));
    connect(m_2DViewer, SIGNAL(viewChanged(int)), SLOT(refresh()));
}

RepresentationsLayer::~RepresentationsLayer()
{
}

void RepresentationsLayer::addRepresentation(ToolRepresentation *toolRepresentation)
{
    switch (m_currentPlane)
    {
        case OrthogonalPlane::XYPlane:
            m_axialToolRepresentations.insert(m_currentSlice, toolRepresentation);
            break;

        case OrthogonalPlane::YZPlane:
            m_sagitalToolRepresentations.insert(m_currentSlice, toolRepresentation);
            break;

        case OrthogonalPlane::XZPlane:
            m_coronalToolRepresentations.insert(m_currentSlice, toolRepresentation);
            break;

        default:
            DEBUG_LOG("Pla no definit!");
            return;
            break;
    }
}

void RepresentationsLayer::clearViewer()
{
    QList<ToolRepresentation*> list;
    QMultiMap<int, ToolRepresentation*>::const_iterator it;
    switch (m_currentPlane)
    {
        case OrthogonalPlane::XYPlane:
            it = m_axialToolRepresentations.find(m_currentSlice);
            while (it != m_axialToolRepresentations.end() && it.key() == m_currentSlice)
            {
                list << it.value();
                it++;
            }
            // Elimina les primitives del contenidor
            m_axialToolRepresentations.remove(m_currentSlice);
            break;

        case OrthogonalPlane::YZPlane:
            it = m_sagitalToolRepresentations.find(m_currentSlice);
            while (it != m_sagitalToolRepresentations.end() && it.key() == m_currentSlice)
            {
                list << it.value();
                it++;
            }
            // Elimina les primitives del contenidor
            m_sagitalToolRepresentations.remove(m_currentSlice);
            break;

        case OrthogonalPlane::XZPlane:
            it = m_coronalToolRepresentations.find(m_currentSlice);
            while (it != m_coronalToolRepresentations.end() && it.key() == m_currentSlice)
            {
                list << it.value();
                it++;
            }
            // Elimina les primitives del contenidor
            m_coronalToolRepresentations.remove(m_currentSlice);
            break;

        default:
            DEBUG_LOG("Pla no definit!");
            return;
            break;
    }

    foreach (ToolRepresentation *representation, list)
    {
        delete representation;
    }

    m_2DViewer->render();
}

void RepresentationsLayer::clearAll()
{
    QMultiMap<int, ToolRepresentation*>::const_iterator it;

    // Axial
    it = m_axialToolRepresentations.begin();
    while (it != m_axialToolRepresentations.end())
    {
        delete it.value();
        it++;
    }

    // Sagital
    it = m_sagitalToolRepresentations.begin();
    while (it != m_sagitalToolRepresentations.end())
    {
        delete it.value();
        it++;
    }

    // Coronal
    it = m_coronalToolRepresentations.begin();
    while (it != m_coronalToolRepresentations.end())
    {
        delete it.value();
        it++;
    }
}

void RepresentationsLayer::refresh()
{
    m_currentPlane = m_2DViewer->getView();
    m_currentSlice = m_2DViewer->getCurrentSlice();
}

}
