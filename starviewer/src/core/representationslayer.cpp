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
#include "toolhandler.h"
#include "toolrepresentation.h"
#include "drawerprimitive.h"

#include <vtkCommand.h>
#include <vtkRenderWindowInteractor.h>

namespace udg {

RepresentationsLayer::RepresentationsLayer(Q2DViewer *viewer, QObject *parent)
 : QObject(parent)
{
    m_2DViewer = viewer;
    connect(m_2DViewer, SIGNAL(sliceChanged(int)), SLOT(refresh()));
    connect(m_2DViewer, SIGNAL(viewChanged(int)), SLOT(refresh()));

    m_editionMode = true;
    m_clearRepresentationBeforeAdd = true;
    m_connectedToolHandler = NULL;
}

RepresentationsLayer::~RepresentationsLayer()
{
}

void RepresentationsLayer::addRepresentation(ToolRepresentation *toolRepresentation, int plane, int slice)
{
    switch (plane)
    {
        case QViewer::Top2DPlane:
            m_top2DPlaneRepresentations << toolRepresentation;
            break;

        default:
            m_toolRepresentations.insert(slice, toolRepresentation);
            break;
    }

    connect(toolRepresentation, SIGNAL(selected(ToolRepresentation *)), this, SLOT(selectRepresentation(ToolRepresentation *)));

    foreach (ToolHandler *toolHandler, toolRepresentation->getToolHandlers())
    {
        connect(toolHandler, SIGNAL(clicked(ToolHandler *)), this, SLOT(connectToolHandler(ToolHandler *)));
        connect(this, SIGNAL(screenClicked(double *)), toolHandler, SLOT(isClickNearMe(double *)));
    }

    m_toolHandlers << toolRepresentation->getToolHandlers();
}

void RepresentationsLayer::addPrimitive(DrawerPrimitive *primitive, int plane, int slice)
{
    switch (plane)
    {
        case QViewer::Top2DPlane:
            m_top2DPlanePrimitives << primitive;
            break;

        default:
            m_primitives.insert(slice, primitive);
            break;
    }
}

void RepresentationsLayer::addPrimitiveToGroup(DrawerPrimitive *primitive, const QString &groupName)
{
    // No comprovem si ja existeix ni si està en cap altre de les llistes, no cal.
    m_primitiveGroups.insert(groupName, primitive);
}

void RepresentationsLayer::hidePrimitivesGroup(const QString &groupName)
{
    QList<DrawerPrimitive *> primitiveList = m_primitiveGroups.values(groupName);
    foreach (DrawerPrimitive *primitive, primitiveList)
    {
        if (primitive->isModified() || primitive->isVisible())
        {
            primitive->visibilityOff();
            primitive->update();
        }
    }
    this->refresh();
}

void RepresentationsLayer::showPrimitivesGroup(const QString &groupName)
{
    QList<DrawerPrimitive *> primitiveList = m_primitiveGroups.values(groupName);
    foreach (DrawerPrimitive *primitive, primitiveList)
    {
        if (primitive->isModified() || !primitive->isVisible())
        {
            primitive->visibilityOn();
            primitive->update();
        }
    }
    this->refresh();
}

void RepresentationsLayer::clearViewer()
{
    QList<ToolRepresentation *> list;
    QMultiMap<int, ToolRepresentation *>::const_iterator it;

    it = m_toolRepresentations.find(m_currentSlice);
    while (it != m_toolRepresentations.end() && it.key() == m_currentSlice)
    {
        list << it.value();
        it++;
    }
    // Elimina les primitives del contenidor
    m_toolRepresentations.remove(m_currentSlice);

    foreach (ToolRepresentation *representation, list)
    {
        delete representation;
    }

    // Primitives
    QList<DrawerPrimitive *> primitivesList;
    QMultiMap<int, DrawerPrimitive *>::const_iterator primitives_it;

    primitives_it = m_primitives.find(m_currentSlice);
    while (primitives_it != m_primitives.end() && primitives_it.key() == m_currentSlice)
    {
        primitivesList << primitives_it.value();
        primitives_it++;
    }
    // Elimina les primitives del contenidor
    m_primitives.remove(m_currentSlice);

    foreach (DrawerPrimitive *primitive, primitivesList)
    {
        QList<DrawerPrimitive *> primitivesInGroups;
        primitivesInGroups = m_primitiveGroups.values();
        if (primitivesInGroups.contains(primitive))
        {
            m_primitiveGroups.remove(m_primitiveGroups.key(primitive));
        }

        delete primitive;
    }

    refresh();
}

void RepresentationsLayer::clearAll()
{
    QList<ToolRepresentation *> list = m_toolRepresentations.values();
    list += m_top2DPlaneRepresentations;

    foreach (ToolRepresentation *representation, list)
    {
        delete representation;
    }

    m_toolRepresentations.clear();
    m_top2DPlaneRepresentations.clear();

    // Primitives
    QList<DrawerPrimitive *> primitivesList = m_primitives.values();
    primitivesList += m_top2DPlanePrimitives;

    foreach (DrawerPrimitive *primitive, primitivesList)
    {
        delete primitive;
    }

    m_primitives.clear();
    m_top2DPlanePrimitives.clear();
    m_primitiveGroups.clear();

    refresh();
}

void RepresentationsLayer::handleEvent(unsigned long eventID)
{
    if (m_editionMode)
    {
        switch(eventID)
        {
            case vtkCommand::LeftButtonPressEvent:
                if (m_2DViewer->getInteractor()->GetRepeatCount() == 0)
                {
                    if (m_2DViewer->getInteractor()->GetControlKey() != 0) //Ctrl Key Pressed
                    {
                        m_clearRepresentationBeforeAdd = false;
                    }
                    else
                    {
                        m_clearRepresentationBeforeAdd = true;
                    }

                    double clickedWorldPoint[3];
                    m_2DViewer->getEventWorldCoordinate(clickedWorldPoint);
                    emit screenClicked(clickedWorldPoint);
                }
                break;

            case vtkCommand::MouseMoveEvent:
                if (m_connectedToolHandler)
                {
                    double movedWorldPoint[3];
                    m_2DViewer->getEventWorldCoordinate(movedWorldPoint);
                    emit forwardEvent(eventID, movedWorldPoint);
                    
                    foreach (ToolRepresentation *representation, m_selectedRepresentations)
                    {
                        representation->calculate();
                    }
                }
                break;

            case vtkCommand::LeftButtonReleaseEvent:
                if (m_connectedToolHandler)
                {
                    disconnectToolHandler();
                }
                break;

            case vtkCommand::KeyPressEvent:
                int key = m_2DViewer->getInteractor()->GetKeyCode();
                if (key == 127) //ASCII Code for DEL key
                {
                    deleteSelectedRepresentations();
                }
                break;
        }
    }
}

void RepresentationsLayer::refresh()
{
    if (m_currentPlane == m_2DViewer->getView())
    {
        if (m_currentSlice != m_2DViewer->getCurrentSlice())
        {
            //canviar primitives visibles per llesca
            hide(m_currentSlice);
            m_currentSlice = m_2DViewer->getCurrentSlice();
            show(m_currentSlice);
        }
		m_2DViewer->update();
    }
    else
    {
        m_currentSlice = m_2DViewer->getCurrentSlice();
        m_currentPlane = m_2DViewer->getView();
        //eliminar les primitives en canviar de pla, no s'ha de mostrar res
        clearAll();
    }    
}

void RepresentationsLayer::hide(int slice)
{
    QList<ToolRepresentation *> representationsList;
    representationsList = m_toolRepresentations.values(slice);

    foreach (ToolRepresentation *representation, representationsList)
    {
        representation->hide();
    }

    // Primitives
    QList<DrawerPrimitive *> primitivesList;
    primitivesList = m_primitives.values(slice);

    foreach (DrawerPrimitive *primitive, primitivesList)
    {
        primitive->visibilityOff();
    }
}

void RepresentationsLayer::show(int slice)
{
    QList<ToolRepresentation *> representationsList;
    representationsList = m_toolRepresentations.values(slice);

    foreach (ToolRepresentation *representation, representationsList)
    {
        representation->show();
    }

    //Primitives
    QList<DrawerPrimitive *> primitivesList;
    primitivesList = m_primitives.values(slice);

    foreach (DrawerPrimitive *primitive, primitivesList)
    {
        primitive->visibilityOn();
    }
}

void RepresentationsLayer::connectToolHandler(ToolHandler *toolHandler)
{
    if (!m_connectedToolHandler)
    {
        m_connectedToolHandler = toolHandler;
        connect(this, SIGNAL(forwardEvent(unsigned long, double *)), m_connectedToolHandler, SLOT(handleEvents(unsigned long, double *)));

        m_connectedToolHandler->select();
    }

    emit editing(true);
}

void RepresentationsLayer::disconnectToolHandler()
{
    disconnect(this, SIGNAL(forwardEvent(unsigned long, double *)), m_connectedToolHandler, SLOT(handleEvents(unsigned long, double *)));

    m_connectedToolHandler = NULL;

    foreach (ToolRepresentation *representation, m_selectedRepresentations)
    {
        representation->calculate();
    }

    m_2DViewer->update();

    emit editing(false);
}

void RepresentationsLayer::clearSelectedRepresentations()
{
    foreach (ToolRepresentation *representation, m_selectedRepresentations)
    {
        representation->deselect();
    }

    m_selectedRepresentations.clear();
}

void RepresentationsLayer::deleteSelectedRepresentations()
{
    foreach (ToolRepresentation *representation, m_selectedRepresentations)
    {
        removeRepresentation(representation);
    }
}

void RepresentationsLayer::addSelectedRepresentation(ToolRepresentation *toolRepresentation)
{
    if (m_clearRepresentationBeforeAdd)
    {
        if (m_selectedRepresentations.contains(toolRepresentation))
        {
            foreach (ToolRepresentation *rep, m_selectedRepresentations)
            {
                if (rep != toolRepresentation)
                {
                    deleteSelectedRepresentation(rep);
                }
            }
        }
        else
        {
            clearSelectedRepresentations();
            m_selectedRepresentations.append(toolRepresentation);
        }
    }
    else
    {
        if (!m_selectedRepresentations.contains(toolRepresentation))
        {
            m_selectedRepresentations.append(toolRepresentation);
        }
    }
}

void RepresentationsLayer::deleteSelectedRepresentation(ToolRepresentation *toolRepresentation)
{
    toolRepresentation->deselect();
    m_selectedRepresentations.removeOne(toolRepresentation);
}

void RepresentationsLayer::removeRepresentation(ToolRepresentation *toolRepresentation)
{
    // Eliminem connexions amb handlers
    foreach (ToolHandler *toolHandler, toolRepresentation->getToolHandlers())
    {
        disconnect(toolHandler, SIGNAL(clicked(ToolHandler *)), this, SLOT(connectToolHandler(ToolHandler *)));
        disconnect(this, SIGNAL(screenClicked(double *)), toolHandler, SLOT(isClickNearMe(double *)));
    }

    if (m_selectedRepresentations.contains(toolRepresentation))
    {
        m_selectedRepresentations.removeOne(toolRepresentation);
    }

    int key = m_toolRepresentations.key(toolRepresentation);
    m_toolRepresentations.remove(key, toolRepresentation);

    delete toolRepresentation;

    refresh();
}

void RepresentationsLayer::selectRepresentation(ToolRepresentation *toolRepresentation)
{
    this->addSelectedRepresentation(toolRepresentation);
}

void RepresentationsLayer::removePrimitive(DrawerPrimitive *primitive)
{
    if (!primitive)
    {
        return;
    }
    // TODO atenció amb aquest tractament pel sucedani d'smart pointer.
    // HACK només esborrarem si ningú és propietari
    if (primitive->hasOwners())
    {
        DEBUG_LOG("No esborrem la primitiva. Tenim propietaris");
        return;
    }

    // Mirem si està en algun grup
    QMutableMapIterator<QString, DrawerPrimitive *> groupsIterator(m_primitiveGroups);
    while (groupsIterator.hasNext())
    {
        groupsIterator.next();
        if (primitive == groupsIterator.value())
        {
            groupsIterator.remove();
        }
    }

    bool found = false;
    QMutableMapIterator<int, DrawerPrimitive *> primitivesIterator(m_primitives);
    while (primitivesIterator.hasNext() && !found)
    {
        primitivesIterator.next();
        if (primitive == primitivesIterator.value())
        {
            found = true;
            primitivesIterator.remove();
            //m_2DViewer->getRenderer()->RemoveViewProp(primitive->getAsVtkProp());
        }
    }

    if (found)
    {
        return;
    }

    if (m_top2DPlanePrimitives.contains(primitive))
    {
        found = true;
        m_top2DPlanePrimitives.removeAt(m_top2DPlanePrimitives.indexOf(primitive));
        //m_2DViewer->getRenderer()->RemoveViewProp(primitive->getAsVtkProp());
        //m_2DViewer->refresh();
    }
}

void RepresentationsLayer::removePrimitivesInsideBounds(double bounds[6])
{
    QList<ToolRepresentation*> primitivesInsideBounds = m_toolRepresentations.values(m_2DViewer->getCurrentSlice());

    foreach (ToolRepresentation *representation, primitivesInsideBounds)
    {
        if (representation->isInsideOfBounds(bounds,m_2DViewer->getView()))
        {
            m_toolRepresentations.remove(m_2DViewer->getCurrentSlice(),representation);
            delete representation;
        }
    }
}

}
