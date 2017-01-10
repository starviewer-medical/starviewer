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

#include "drawer.h"
#include "drawerprimitive.h"
#include "logging.h"
#include "mathtools.h"
// Vtk
#include <vtkRenderer.h>
#include <QColor>

namespace udg {

Drawer::Drawer(Q2DViewer *viewer, QObject *parent)
 : QObject(parent), m_currentPlane(OrthogonalPlane::YZPlane), m_currentSlice(0)
{
    m_2DViewer = viewer;
    connect(m_2DViewer, SIGNAL(sliceChanged(int)), SLOT(refresh()));
    connect(m_2DViewer, SIGNAL(viewChanged(int)), SLOT(refresh()));
}

Drawer::~Drawer()
{
}

void Drawer::draw(DrawerPrimitive *primitive, const OrthogonalPlane &plane, int slice)
{
    switch (plane)
    {
        case OrthogonalPlane::XYPlane:
            m_XYPlanePrimitives.insert(slice, primitive);
            break;

        case OrthogonalPlane::YZPlane:
            m_YZPlanePrimitives.insert(slice, primitive);
            break;

        case OrthogonalPlane::XZPlane:
            m_XZPlanePrimitives.insert(slice, primitive);
            break;

        default:
            DEBUG_LOG("Pla no definit!");
            return;
            break;
    }

    // Segons quin sigui el pla actual caldrà comprovar
    // la visibilitat de la primitiva segons la llesca
    if (m_2DViewer->getView() == plane)
    {
        if (slice < 0 || m_2DViewer->getCurrentSlice() == slice)
        {
            primitive->setVisibility(true);
        }
        else
        {
            primitive->setVisibility(false);
        }
    }

    // Procedim a "pintar-la"
    renderPrimitive(primitive);
}

void Drawer::draw(DrawerPrimitive *primitive)
{
    m_top2DPlanePrimitives << primitive;

    // Procedim a "pintar-la"
    renderPrimitive(primitive);
}

void Drawer::clearViewer()
{
    QMultiMap<int, DrawerPrimitive*> primitivesContainer;
    switch (m_currentPlane)
    {
        case OrthogonalPlane::XYPlane:
            primitivesContainer = m_XYPlanePrimitives;
            break;

        case OrthogonalPlane::YZPlane:
            primitivesContainer = m_YZPlanePrimitives;
            break;

        case OrthogonalPlane::XZPlane:
            primitivesContainer = m_XZPlanePrimitives;
            break;

        default:
            DEBUG_LOG("Pla no definit!");
            return;
            break;
    }

    // Obtenim les primitives de la vista i llesca actuals
    QList<DrawerPrimitive*> list = primitivesContainer.values(m_currentSlice);
    // Eliminem totes aquelles primitives que estiguin a la llista, que no tinguin "propietaris" i que siguin esborrables
    // Al fer delete es cridarà el mètode erasePrimitive() que ja s'encarrega de fer la "feina bruta"
    foreach (DrawerPrimitive *primitive, list)
    {
        if (!primitive->hasOwners() && primitive->isErasable())
        {
            delete primitive;
        }
    }
    m_2DViewer->render();
}

void Drawer::addToGroup(DrawerPrimitive *primitive, const QString &groupName)
{
    // No comprovem si ja existeix ni si està en cap altre de les llistes, no cal.
    m_primitiveGroups.insert(groupName, primitive);
}

void Drawer::refresh()
{
    if (m_currentPlane == m_2DViewer->getView())
    {
        if (m_currentSlice != m_2DViewer->getCurrentSlice())
        {
            // Cal fer invisible el que es veia en aquest pla i llesca i fer visible el que hi ha a la nova llesca
            hide(m_currentPlane, m_currentSlice);
            m_currentSlice = m_2DViewer->getCurrentSlice();
            show(m_currentPlane, m_currentSlice);
        }
    }
    else
    {
        // Cal fer invisible el que es veia en aquest pla i llesca i fer visible el que hi ha al nou pla i llesca
        hide(m_currentPlane, m_currentSlice);
        m_currentSlice = m_2DViewer->getCurrentSlice();
        m_currentPlane = m_2DViewer->getView();
        show(m_currentPlane, m_currentSlice);
    }
}

void Drawer::removeAllPrimitives()
{
    QList <DrawerPrimitive*> list = m_XYPlanePrimitives.values();
    QList <DrawerPrimitive*> sagitalList = m_YZPlanePrimitives.values();
    QList <DrawerPrimitive*> coronalList = m_XZPlanePrimitives.values();

    list += sagitalList;
    list += coronalList;
    list += m_top2DPlanePrimitives;

    foreach (DrawerPrimitive *primitive, list)
    {
        // TODO Atenció amb aquest tractament pel sucedani d'smart pointer.
        // Només esborrarem si ningú és propietari, però no comprovarem si són "erasable" o no
        if (!primitive->hasOwners())
        {
            m_2DViewer->getRenderer()->RemoveViewProp(primitive->getAsVtkProp());
            delete primitive;
        }
    }
}

void Drawer::erasePrimitive(DrawerPrimitive *primitive)
{
    if (!primitive)
    {
        return;
    }
    // TODO Atenció amb aquest tractament pel sucedani d'smart pointer.
    // HACK Només esborrarem si ningú és propietari
    if (primitive->hasOwners())
    {
        DEBUG_LOG("No esborrem la primitiva. Tenim propietaris");
        return;
    }

    // Mirem si està en algun grup
    QMutableMapIterator<QString, DrawerPrimitive*> groupsIterator(m_primitiveGroups);
    while (groupsIterator.hasNext())
    {
        groupsIterator.next();
        if (primitive == groupsIterator.value())
        {
            groupsIterator.remove();
        }
    }

    // Busquem en el pla axial
    if (erasePrimitiveFromContainer(primitive, m_XYPlanePrimitives))
    {
        // En principi una mateixa primitiva només estarà en una de les llistes
        return;
    }

    // Busquem en el pla sagital
    if (erasePrimitiveFromContainer(primitive, m_YZPlanePrimitives))
    {
        return;
    }

    // Busquem en el pla coronal
    if (erasePrimitiveFromContainer(primitive, m_XZPlanePrimitives))
    {
        return;
    }

    // Busquem en la capa superior
    if (m_top2DPlanePrimitives.contains(primitive))
    {
        m_top2DPlanePrimitives.removeAt(m_top2DPlanePrimitives.indexOf(primitive));
        m_2DViewer->getRenderer()->RemoveViewProp(primitive->getAsVtkProp());
        m_2DViewer->render();
    }
}

void Drawer::hide(const OrthogonalPlane &plane, int slice)
{
    QList<DrawerPrimitive*> primitivesList;
    switch (plane)
    {
        case OrthogonalPlane::XYPlane:
            primitivesList = m_XYPlanePrimitives.values(slice);
            break;

        case OrthogonalPlane::YZPlane:
            primitivesList = m_YZPlanePrimitives.values(slice);
            break;

        case OrthogonalPlane::XZPlane:
            primitivesList = m_XZPlanePrimitives.values(slice);
            break;
    }
    foreach (DrawerPrimitive *primitive, primitivesList)
    {
        if (primitive->isVisible())
        {
            primitive->visibilityOff();
            primitive->update();
        }
    }
}

void Drawer::show(const OrthogonalPlane &plane, int slice)
{
    QList<DrawerPrimitive*> primitivesList;
    switch (plane)
    {
        case OrthogonalPlane::XYPlane:
            primitivesList = m_XYPlanePrimitives.values(slice);
            break;

        case OrthogonalPlane::YZPlane:
            primitivesList = m_YZPlanePrimitives.values(slice);
            break;

        case OrthogonalPlane::XZPlane:
            primitivesList = m_XZPlanePrimitives.values(slice);
            break;
    }

    foreach (DrawerPrimitive *primitive, primitivesList)
    {
        if (!m_disabledPrimitives.contains(primitive) && (primitive->isModified() || !primitive->isVisible()))
        {
            primitive->visibilityOn();
            primitive->update();
        }
    }
}

int Drawer::getNumberOfDrawnPrimitives()
{
    return (m_XYPlanePrimitives.size() + m_YZPlanePrimitives.size() + m_XZPlanePrimitives.size());
}

void Drawer::disableGroup(const QString &groupName)
{
    bool hasToRender = false;
    QList<DrawerPrimitive*> primitiveList = m_primitiveGroups.values(groupName);
    foreach (DrawerPrimitive *primitive, primitiveList)
    {
        if (primitive->isModified() || primitive->isVisible())
        {
            primitive->visibilityOff();
            primitive->update();
            hasToRender = true;
        }

        m_disabledPrimitives.insert(primitive);
    }

    if (hasToRender)
    {
        m_2DViewer->render();
    }
}

void Drawer::enableGroup(const QString &groupName)
{
    QList<DrawerPrimitive*> currentVisiblePrimitives;
    int currentSlice = m_2DViewer->getCurrentSlice();
    switch (m_2DViewer->getView())
    {
        case OrthogonalPlane::XYPlane:
            currentVisiblePrimitives = m_XYPlanePrimitives.values(currentSlice);
            break;

        case OrthogonalPlane::YZPlane:
            currentVisiblePrimitives = m_YZPlanePrimitives.values(currentSlice);
            break;

        case OrthogonalPlane::XZPlane:
            currentVisiblePrimitives = m_XZPlanePrimitives.values(currentSlice);
            break;
    }
    currentVisiblePrimitives << m_top2DPlanePrimitives;

    bool hasToRender = false;
    QList<DrawerPrimitive*> groupPrimitives = m_primitiveGroups.values(groupName);
    foreach (DrawerPrimitive *primitive, groupPrimitives)
    {
        // Si la primitiva compleix les condicions de visibilitat per estat enable la farem visible
        if (currentVisiblePrimitives.contains(primitive))
        {
            if (primitive->isModified() || !primitive->isVisible())
            {
                primitive->visibilityOn();
                primitive->update();
                hasToRender = true;
            }
        }

        // L'eliminem de la llista de primitives disabled
        m_disabledPrimitives.remove(primitive);
    }

    if (hasToRender)
    {
        m_2DViewer->render();
    }
}

DrawerPrimitive* Drawer::getNearestErasablePrimitiveToPoint(const Vector3 &displayPoint, const OrthogonalPlane &view, int slice, Vector3 &closestDisplayPoint)
{
    double distance;
    double minimumDistance = MathTools::DoubleMaximumValue;
    QList<DrawerPrimitive*> primitivesList;

    DrawerPrimitive *nearestPrimitive = 0;

    switch (view)
    {
        case OrthogonalPlane::XYPlane:
            primitivesList = m_XYPlanePrimitives.values(slice);
            break;

        case OrthogonalPlane::YZPlane:
            primitivesList = m_YZPlanePrimitives.values(slice);
            break;

        case OrthogonalPlane::XZPlane:
            primitivesList = m_XZPlanePrimitives.values(slice);
            break;

        default:
            break;
    }

    Vector3 localClosestPoint;

    auto worldToDisplay = [this, displayPoint](const Vector3 &point) {
        auto pointDisplay = m_2DViewer->computeWorldToDisplay(point);
        pointDisplay.z = displayPoint.z;
        return pointDisplay;
    };

    foreach (DrawerPrimitive *primitive, primitivesList)
    {
        if (primitive->isErasable())
        {
            distance = primitive->getDistanceToPointInDisplay(displayPoint, localClosestPoint, worldToDisplay);
            if (distance <= minimumDistance)
            {
                minimumDistance = distance;
                nearestPrimitive = primitive;
                closestDisplayPoint = localClosestPoint;
            }
        }
    }
    return nearestPrimitive;
}

void Drawer::erasePrimitivesInsideBounds(const std::array<double, 4> &displayBounds, const OrthogonalPlane &view, int slice)
{
    QList<DrawerPrimitive*> primitivesList;

    switch (view)
    {
        case OrthogonalPlane::XYPlane:
            primitivesList = m_XYPlanePrimitives.values(slice);
            break;

        case OrthogonalPlane::YZPlane:
            primitivesList = m_YZPlanePrimitives.values(slice);
            break;

        case OrthogonalPlane::XZPlane:
            primitivesList = m_XZPlanePrimitives.values(slice);
            break;

        default:
            break;
    }

    foreach (DrawerPrimitive *primitive, primitivesList)
    {
        if (primitive->isErasable())
        {
            if (isPrimitiveInside(primitive, displayBounds))
            {
                erasePrimitive(primitive);
            }
        }
    }
}

bool Drawer::isPrimitiveInside(DrawerPrimitive *primitive, const std::array<double, 4> &displayBounds)
{
    auto primitiveBounds = primitive->getDisplayBounds([this](const Vector3 &point) { return m_2DViewer->computeWorldToDisplay(point); });
    return displayBounds[0] <= primitiveBounds[0] && displayBounds[1] >= primitiveBounds[1] &&
           displayBounds[2] <= primitiveBounds[2] && displayBounds[3] >= primitiveBounds[3];
}

bool Drawer::erasePrimitiveFromContainer(DrawerPrimitive *primitive, QMultiMap<int, DrawerPrimitive*> &primitiveContainer)
{
    bool found = false;
    QMutableMapIterator<int, DrawerPrimitive*> containerIterator(primitiveContainer);
    while (containerIterator.hasNext() && !found)
    {
        containerIterator.next();
        if (primitive == containerIterator.value())
        {
            found = true;
            containerIterator.remove();
            m_2DViewer->getRenderer()->RemoveViewProp(primitive->getAsVtkProp());
        }
    }

    return found;
}

void Drawer::renderPrimitive(DrawerPrimitive *primitive)
{
    vtkProp *prop = primitive->getAsVtkProp();
    if (prop)
    {
        connect(primitive, SIGNAL(dying(DrawerPrimitive*)), SLOT(erasePrimitive(DrawerPrimitive*)));
        m_2DViewer->getRenderer()->AddViewProp(prop);
        if (primitive->isVisible())
        {
            m_2DViewer->render();
        }
    }
}

}
