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

#include "viewerslayout.h"

#include "griditerator.h"
#include "logging.h"
#include "patientbrowsermenu.h"
#include "q2dviewerwidget.h"
#include "relativegeometrylayout.h"
#include "screenmanager.h"

namespace udg {

namespace {

/**
 * @brief The ExtendedGridIterator class extends the regular GridIterator with a method that returns the relative geometry corresponding to the current cell.
 */
class ExtendedGridIterator : public GridIterator {

public:
    ExtendedGridIterator(int rows, int columns) :
        GridIterator(rows, columns)
    {
    }

    QRectF getRelativeGeometryForCurrentCell() const
    {
        double r = row(), c = column(), nr = rows(), nc = columns();
        return QRectF(c / nc, r / nr, 1.0 / nc, 1.0 / nr);
    }

};

/// This struct holds information about the screen layout of a window and the position of a viewer in it.
struct ScreenLayoutInfo {
    int horizontalNumberOfScreens;
    int verticalNumberOfScreens;
    int horizontalPosition;
    int verticalPosition;
};

/// Given a window and a viewer, returns a struct with information on the number of screns occupied by the window and on which screen is the viewer.
ScreenLayoutInfo getScreenLayoutInfo(QWidget *window, QWidget *viewer)
{
    ScreenManager screenManager;
    ScreenLayoutInfo screenLayoutInfo = { 1, 1, 0, 0 };

    if (screenManager.isMaximizedToMultipleScreens(window))
    {
        DynamicMatrix dynamicMatrix = screenManager.computeScreenMatrix(window);
        int screenId = screenManager.getScreenID(viewer);
        int row, column;

        if (dynamicMatrix.getPosition(screenId, row, column))
        {
            screenLayoutInfo.horizontalNumberOfScreens = dynamicMatrix.getNumberOfColumns();
            screenLayoutInfo.verticalNumberOfScreens = dynamicMatrix.getNumberOfRows();
            screenLayoutInfo.horizontalPosition = column - dynamicMatrix.getColumnBase();
            screenLayoutInfo.verticalPosition = row - dynamicMatrix.getRowBase();
        }
    }

    return screenLayoutInfo;
}

}

ViewersLayout::ViewersLayout(QWidget *parent)
 : QWidget(parent), m_selectedViewer(0)
{
    // Set a palette with a dark background to avoid flashes in mammography displays
    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, Qt::black);
    this->setPalette(darkPalette);
    this->setAutoFillBackground(true);

    m_layout = new RelativeGeometryLayout();
    this->setLayout(m_layout);
}

ViewersLayout::~ViewersLayout()
{
}

Q2DViewerWidget* ViewersLayout::getSelectedViewer() const
{
    return m_selectedViewer;
}

Q2DViewerWidget* ViewersLayout::getNewQ2DViewerWidget()
{
    Q2DViewerWidget *newViewer = new Q2DViewerWidget(this);
    // Set the default application palette to the viewer
    newViewer->setPalette(QGuiApplication::palette());
    connect(newViewer, SIGNAL(selected(Q2DViewerWidget*)), SLOT(setSelectedViewer(Q2DViewerWidget*)));
    connect(newViewer, SIGNAL(manualSynchronizationStateChanged(bool)), SIGNAL(manualSynchronizationStateChanged(bool)));
    connect(newViewer, SIGNAL(fusionLayout2x1FirstRequested(QList<Volume*>, AnatomicalPlane)),
            SIGNAL(fusionLayout2x1FirstRequested(QList<Volume*>, AnatomicalPlane)));
    connect(newViewer, SIGNAL(fusionLayout2x1SecondRequested(QList<Volume*>, AnatomicalPlane)),
            SIGNAL(fusionLayout2x1SecondRequested(QList<Volume*>, AnatomicalPlane)));
    connect(newViewer, SIGNAL(fusionLayout3x1Requested(QList<Volume*>, AnatomicalPlane)), SIGNAL(fusionLayout3x1Requested(QList<Volume*>, AnatomicalPlane)));
    connect(newViewer, SIGNAL(fusionLayout2x3FirstRequested(QList<Volume*>)), SIGNAL(fusionLayout2x3FirstRequested(QList<Volume*>)));
    connect(newViewer, SIGNAL(fusionLayout2x3SecondRequested(QList<Volume*>)), SIGNAL(fusionLayout2x3SecondRequested(QList<Volume*>)));
    connect(newViewer, SIGNAL(fusionLayout3x3Requested(QList<Volume*>)), SIGNAL(fusionLayout3x3Requested(QList<Volume*>)));
    connect(newViewer, SIGNAL(fusionLayoutMprRightRequested(QList<Volume*>)), SIGNAL(fusionLayoutMprRightRequested(QList<Volume*>)));
    // Per defecte no li posem cap annotació
    newViewer->getViewer()->removeAnnotation(AllAnnotations);
    newViewer->getViewer()->getPatientBrowserMenu()->setShowFusionOptions(true);

    emit viewerAdded(newViewer);
    return newViewer;
}

void ViewersLayout::deleteQ2DViewerWidget(Q2DViewerWidget *viewer)
{
    // TODO: Xapussilla que cal fer ja que si es fa un "delete viewer" directament peta al canviar d'HP i cal fer-li un deleteLater.
    // Si es fa un deleteLater i prou els viewer no s'esborren fins que s'han creat els nous. Això faria que hi hagi un moment en que
    // tindríem la memòria acumulada dels viewers vells i els nous (això inclou la memòria de textura que allotja el renderer). Això podria provar
    // que ens en quedessim sense si es tracten d'imatges grans. Per això fem un delete del viewer i després el deleteLater perquè no peti.
    emit viewerRemoved(viewer);
    delete viewer->getViewer();
    viewer->deleteLater();

    // Visual clean up
    // TODO This can go into the Q2DViewerWidget destructor if viewer is deleted directly instead of with deleteLater()
    viewer->setCurrentIndex(1);
    viewer->repaint();
}

void ViewersLayout::setGrid(int rows, int columns)
{
    setGridInArea(rows, columns, QRectF(0.0, 0.0, 1.0, 1.0));
}

void ViewersLayout::setGridInArea(int rows, int columns, const QRectF &geometry)
{
    demaximizeViewersIntersectingGeometry(geometry);

    QList<Q2DViewerWidget*> viewers = getViewersInsideGeometry(geometry);

    int requestedViewers = rows * columns;

    // Remove viewers in excess
    while (viewers.size() > requestedViewers)
    {
        Q2DViewerWidget *viewer = viewers.takeLast();
        if (m_selectedViewer == viewer)
        {
            setSelectedViewer(0);
        }
        m_layout->removeWidget(viewer);
        deleteQ2DViewerWidget(viewer);
    }

    ExtendedGridIterator iterator(rows, columns);

    // Resize current viewers
    foreach (Q2DViewerWidget *viewer, viewers)
    {
        m_layout->setGeometryAt(m_layout->indexOf(viewer), convertGeometry(iterator.getRelativeGeometryForCurrentCell(), geometry));

        // A viewer may be hidden due to maximization of another viewer; make sure it's shown again
        if (viewer->isHidden())
        {
            showViewer(viewer);
        }

        iterator.next();
    }

    // Add new viewers
    while (viewers.size() < requestedViewers)
    {
        Q2DViewerWidget *viewer = this->getNewQ2DViewerWidget();
        m_layout->addWidget(viewer, convertGeometry(iterator.getRelativeGeometryForCurrentCell(), geometry));
        viewers.append(viewer);
        showViewer(viewer);
        iterator.next();
    }

    // If the current selected viewer gets hidden, then select the first one by default
    if (!m_selectedViewer)
    {
        setSelectedViewer(viewers.first());
    }

    // Invalidate the layout. This is needed when the distribution has changed but not the number of viewers.
    m_layout->invalidate();
}

namespace {

// Returns viewerGeometry remapped from its position and size relative to oldGeometry to the equivalent relative to newGeometry.
QRectF remapGeometry(const QRectF &viewerGeometry, const QRectF &oldGeometry, const QRectF &newGeometry)
{
    double incWidth = newGeometry.width() / oldGeometry.width();
    double incHeight = newGeometry.height() / newGeometry.height();
    double incX = newGeometry.x() - oldGeometry.x();
    double incY = newGeometry.y() - oldGeometry.y();

    return QRectF(viewerGeometry.x() * incWidth + incX, viewerGeometry.y() * incHeight + incY,
                  viewerGeometry.width() * incWidth, viewerGeometry.height() * incHeight);
}

}

QRectF ViewersLayout::convertGeometry(const QRectF &viewerGeometry, const QRectF &newGeometry)
{
    return remapGeometry(viewerGeometry, QRectF(0.0, 0.0, 1.0, 1.0), newGeometry);
}

void ViewersLayout::mapViewersToNewGeometry(const QRectF &oldGeometry, const QRectF &newGeometry)
{
    QList<Q2DViewerWidget*> viewersInRegion = getViewersInsideGeometry(oldGeometry);

    foreach (Q2DViewerWidget *viewer, viewersInRegion)
    {
        QRectF changedGeometry = remapGeometry(this->getGeometryOfViewer(viewer), oldGeometry, newGeometry);
        this->setViewerGeometry(viewer, changedGeometry);
    }
}

QRectF ViewersLayout::getGeometryOfViewer(Q2DViewerWidget *viewer) const
{
    if (m_maximizedViewers.contains(viewer))
    {
        return m_maximizedViewers[viewer].normalGeometry;
    }
    else
    {
        return m_layout->geometry(viewer);
    }
}

Q2DViewerWidget* ViewersLayout::addViewer(const QRectF &geometry)
{
    Q2DViewerWidget *newViewer = getNewQ2DViewerWidget();
    setViewerGeometry(newViewer, geometry);
    showViewer(newViewer);
    return newViewer;
}

void ViewersLayout::setSelectedViewer(Q2DViewerWidget *viewer)
{
    if (viewer != m_selectedViewer)
    {
        if (m_selectedViewer)
        {
            m_selectedViewer->setSelected(false);
        }

        m_selectedViewer = viewer;

        if (m_selectedViewer)
        {
            m_selectedViewer->setSelected(true);
        }
        emit selectedViewerChanged(m_selectedViewer);
    }
}

void ViewersLayout::toggleMaximization(Q2DViewerWidget *viewer)
{
    if (m_maximizedViewers.contains(viewer))
    {
        // Demaximize
        foreach (Q2DViewerWidget *occludedViewer, m_maximizedViewers[viewer].occludedViewers)
        {
            showViewer(occludedViewer);
        }

        m_layout->setGeometry(viewer, m_maximizedViewers[viewer].normalGeometry);
        m_maximizedViewers.remove(viewer);
        m_layout->invalidate();
    }
    else
    {
        // Maximize
        ScreenLayoutInfo screenLayoutInfo = getScreenLayoutInfo(window(), viewer);

        QRectF geometry;
        geometry.setX(static_cast<double>(screenLayoutInfo.horizontalPosition) / screenLayoutInfo.horizontalNumberOfScreens);
        geometry.setY(static_cast<double>(screenLayoutInfo.verticalPosition) / screenLayoutInfo.verticalNumberOfScreens);
        geometry.setWidth(1.0 / screenLayoutInfo.horizontalNumberOfScreens);
        geometry.setHeight(1.0 / screenLayoutInfo.verticalNumberOfScreens);

        m_maximizedViewers[viewer].normalGeometry = m_layout->geometry(viewer);
        m_layout->setGeometry(viewer, geometry);
        viewer->raise();
        m_layout->invalidate();

        foreach (Q2DViewerWidget *occludedViewer, getViewersInsideGeometry(geometry))
        {
            if (occludedViewer != viewer)
            {
                hideViewer(occludedViewer);
                m_maximizedViewers[viewer].occludedViewers.insert(occludedViewer);
            }
        }
    }
}

QList<Q2DViewerWidget*> ViewersLayout::getViewersInsideGeometry(const QRectF &geometry)
{
    QList<Q2DViewerWidget*> viewers;

    for (int i = 0; i < m_layout->count(); i++)
    {
        Q2DViewerWidget *occludedViewer = qobject_cast<Q2DViewerWidget*>(m_layout->itemAt(i)->widget());
        QRectF occludedGeometry = m_layout->geometryAt(i);

        if (geometry.contains(occludedGeometry))
        {
            viewers.append(occludedViewer);
        }
    }

    return viewers;
}

void ViewersLayout::cleanUp()
{
    cleanUp(QRectF(0.0, 0.0, 1.0, 1.0));
}

void ViewersLayout::cleanUp(const QRectF &geometry)
{
    demaximizeViewersIntersectingGeometry(geometry);

    // No hi ha cap visor seleccionat
    setSelectedViewer(0);

    QList<Q2DViewerWidget*> viewers = getViewersInsideGeometry(geometry);

    // Eliminem tots els widgets que contingui viewers layout
    // i els propis widgets
    while (viewers.size() > 0)
    {
        Q2DViewerWidget *viewer = viewers.takeFirst();
        m_layout->removeWidget(viewer);
        deleteQ2DViewerWidget(viewer);
    }
}

int ViewersLayout::getNumberOfViewers() const
{
    return m_layout->count();
}

Q2DViewerWidget* ViewersLayout::getViewerWidget(int number) const
{
    return qobject_cast<Q2DViewerWidget*>(m_layout->itemAt(number)->widget());
}

void ViewersLayout::setViewerGeometry(Q2DViewerWidget *viewer, const QRectF &geometry)
{
    if (!viewer)
    {
        DEBUG_LOG("El viewer proporcionat és nul! No s'aplicarà cap geometria.");
        return;
    }

    m_layout->addWidget(viewer, geometry);
}

void ViewersLayout::hideViewer(Q2DViewerWidget *viewer)
{
    if (viewer)
    {
        viewer->hide();
        emit viewerHidden(viewer);
    }
}

void ViewersLayout::showViewer(Q2DViewerWidget *viewer)
{
    if (viewer)
    {
        viewer->show();
        emit viewerShown(viewer);
    }
}

void ViewersLayout::demaximizeViewersIntersectingGeometry(const QRectF &geometry)
{
    foreach (Q2DViewerWidget *viewer, m_maximizedViewers.keys())
    {
        if (m_layout->geometry(viewer).intersects(geometry))
        {
            toggleMaximization(viewer);
        }
    }
}

}
