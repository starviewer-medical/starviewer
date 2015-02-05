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
    connect(newViewer, SIGNAL(selected(Q2DViewerWidget*)), SLOT(setSelectedViewer(Q2DViewerWidget*)));
    connect(newViewer, SIGNAL(manualSynchronizationStateChanged(bool)), SIGNAL(manualSynchronizationStateChanged(bool)));
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
}

void ViewersLayout::setGrid(int rows, int columns)
{
    // Clean maximization data
    m_maximizedViewers.clear();

    int requestedViewers = rows * columns;

    // Hide viewers in excess
    while (m_layout->count() > requestedViewers)
    {
        QLayoutItem *item = m_layout->takeAt(m_layout->count() - 1);
        Q2DViewerWidget *viewer = qobject_cast<Q2DViewerWidget*>(item->widget());
        m_hiddenViewers.push(viewer);
        hideViewer(viewer);
        delete item;
    }

    ExtendedGridIterator iterator(rows, columns);

    // Resize current viewers
    for (int i = 0; i < m_layout->count(); i++)
    {
        m_layout->setGeometryAt(i, iterator.getRelativeGeometryForCurrentCell());

        // A viewer may be hidden due to maximization of another viewer; make sure it's shown again
        QLayoutItem *item = m_layout->itemAt(i);
        Q2DViewerWidget *viewer = qobject_cast<Q2DViewerWidget*>(item->widget());
        if (viewer->isHidden())
        {
            showViewer(viewer);
        }

        iterator.next();
    }

    // Show hidden viewers
    while (m_layout->count() < requestedViewers && !m_hiddenViewers.isEmpty())
    {
        Q2DViewerWidget *viewer = m_hiddenViewers.pop();
        m_layout->addWidget(viewer, iterator.getRelativeGeometryForCurrentCell());
        showViewer(viewer);
        iterator.next();
    }

    // Add new viewers
    while (m_layout->count() < requestedViewers)
    {
        Q2DViewerWidget *viewer = this->getNewQ2DViewerWidget();
        m_layout->addWidget(viewer, iterator.getRelativeGeometryForCurrentCell());
        showViewer(viewer);
        iterator.next();
    }

    // If the current selected viewer gets hidden, then select the first one by default
    if (m_selectedViewer && m_selectedViewer->isHidden())
    {
        setSelectedViewer(getViewerWidget(0));
    }

    // Invalidate the layout. This is needed when the distribution has changed but not the number of viewers.
    m_layout->invalidate();
}

Q2DViewerWidget* ViewersLayout::addViewer(const QString &geometry)
{
    Q2DViewerWidget *newViewer = getNewQ2DViewerWidget();
    setViewerGeometry(newViewer, geometry);
    showViewer(newViewer);
    return newViewer;
}

void ViewersLayout::setSelectedViewer(Q2DViewerWidget *viewer)
{
    if (!viewer)
    {
        // Si "viewer" és nul, llavors entenem que no volem cap de seleccionat
        m_selectedViewer = 0;
        emit selectedViewerChanged(m_selectedViewer);
        return;
    }

    if (viewer != m_selectedViewer)
    {
        if (m_selectedViewer)
        {
            m_selectedViewer->setSelected(false);
        }
        m_selectedViewer = viewer;
        m_selectedViewer->setSelected(true);
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

        for (int i = 0; i < m_layout->count(); i++)
        {
            Q2DViewerWidget *occludedViewer = qobject_cast<Q2DViewerWidget*>(m_layout->itemAt(i)->widget());
            QRectF occludedGeometry = m_layout->geometryAt(i);

            if (occludedViewer != viewer && geometry.contains(occludedGeometry))
            {
                hideViewer(occludedViewer);
                m_maximizedViewers[viewer].occludedViewers.insert(occludedViewer);
            }
        }
    }
}

void ViewersLayout::cleanUp()
{
    // No hi ha cap visor seleccionat
    setSelectedViewer(0);

    // Eliminem tots els widgets que contingui viewers layout
    // i els propis widgets
    while (m_layout->count() > 0)
    {
        QLayoutItem *item = m_layout->takeAt(0);
        Q2DViewerWidget *viewer = qobject_cast<Q2DViewerWidget*>(item->widget());
        deleteQ2DViewerWidget(viewer);
        delete item;
    }

    // Delete hidden viewers
    while (!m_hiddenViewers.isEmpty())
    {
        deleteQ2DViewerWidget(m_hiddenViewers.pop());
    }

    // Clean maximization data
    m_maximizedViewers.clear();
}

int ViewersLayout::getNumberOfViewers() const
{
    return m_layout->count();
}

Q2DViewerWidget* ViewersLayout::getViewerWidget(int number) const
{
    return qobject_cast<Q2DViewerWidget*>(m_layout->itemAt(number)->widget());
}

void ViewersLayout::setViewerGeometry(Q2DViewerWidget *viewer, const QString &geometry)
{
    if (!viewer)
    {
        DEBUG_LOG("El viewer proporcionat és nul! No s'aplicarà cap geometria.");
        return;
    }

    QStringList splittedGeometryList = geometry.split("\\");
    if (splittedGeometryList.count() < 4)
    {
        DEBUG_LOG("La geometria proporcionada no conté el nombre d'elements necessaris o està mal formada. Geometry dump: [" +
                  geometry + "]. No s'aplicarà cap geometria al viewer proporcinat.");
        WARN_LOG("La geometria proporcionada no conté el nombre d'elements necessaris o està mal formada. Geometry dump: [" +
                 geometry + "]. No s'aplicarà cap geometria al viewer proporcinat.");
        return;
    }

    double x1;
    double y1;
    double x2;
    double y2;
    x1 = splittedGeometryList.at(0).toDouble();
    y1 = splittedGeometryList.at(1).toDouble();
    x2 = splittedGeometryList.at(2).toDouble();
    y2 = splittedGeometryList.at(3).toDouble();

    // Invert Y axis
    y1 = 1.0 - y1;
    y2 = 1.0 - y2;

    m_layout->addWidget(viewer, QRectF(x1, y1, x2 - x1, y2 - y1));
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

}
