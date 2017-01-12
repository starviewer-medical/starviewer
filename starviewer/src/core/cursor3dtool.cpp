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

#include "cursor3dtool.h"

#include "cursor3dtool.h"
#include "cursor3dtooldata.h"
#include "q2dviewer.h"
#include "logging.h"
#include "volume.h"
#include "series.h"
#include "image.h"
#include "imageplane.h"
#include "drawercrosshair.h"
#include "drawer.h"
#include "slicelocator.h"
// Vtk
#include <vtkCommand.h>

namespace udg {

Cursor3DTool::Cursor3DTool(QViewer *viewer, QObject *parent)
 : Tool(viewer, parent)
{
    m_toolName = "Cursor3DTool";
    m_hasSharedData = true;

    m_myData = new Cursor3DToolData;
    m_toolData = m_myData;
    connect(m_toolData, SIGNAL(changed()), SLOT(updateProjectedPoint()));
    connect(m_toolData, SIGNAL(turnOffCursor()), SLOT(hideCursor()));

    m_2DViewer = Q2DViewer::castFromQViewer(viewer);

    m_state = None;

    m_sliceLocator = new SliceLocator;
    
    // Cada cop que el viewer canvïi d'input, hem d'actualitzar el frame of reference
    connect(m_2DViewer, SIGNAL(volumeChanged(Volume*)), SLOT(refreshReferenceViewerData()));
    connect(m_2DViewer, SIGNAL(selected()), SLOT(refreshReferenceViewerData()));
    connect(m_2DViewer, SIGNAL(volumeChanged(Volume*)), SLOT(handleImageChange()));
    connect(m_2DViewer, SIGNAL(anatomicalViewChanged(AnatomicalPlane)), SLOT(handleImageChange()));
    connect(m_2DViewer, SIGNAL(sliceChanged(int)), SLOT(handleImageChange()));

    refreshReferenceViewerData();

    m_crossHair = NULL;
}

Cursor3DTool::~Cursor3DTool()
{
    if (m_crossHair)
    {
        // HACK succedani d'Smart Pointer per tal que el drawer no elimini el crossHair quan s'activi el thickslab
        m_crossHair->decreaseReferenceCount();
        delete m_crossHair;
    }
    m_viewer->unsetCursor();
    delete m_sliceLocator;
}

void Cursor3DTool::setToolData(ToolData *data)
{
    m_toolData = data;
    m_myData = qobject_cast<Cursor3DToolData*>(data);
    connect(m_toolData, SIGNAL(changed()), SLOT(updateProjectedPoint()));
    connect(m_toolData, SIGNAL(turnOffCursor()), SLOT(hideCursor()));

    if (m_2DViewer->isActive())
    {
        refreshReferenceViewerData();
    }
}

void Cursor3DTool::handleEvent(long unsigned eventID)
{
    switch (eventID)
    {
        case vtkCommand::LeftButtonPressEvent:
            initializePosition();
            break;

        case vtkCommand::MouseMoveEvent:
            updatePosition();
            break;

        case vtkCommand::LeftButtonReleaseEvent:
            removePosition();
            break;
    }
}

void Cursor3DTool::initializePosition()
{
    if (!m_2DViewer->hasInput())
    {
        return;
    }

    m_viewer->setCursor(QCursor(Qt::BlankCursor));
    m_state = Computing;

    if (!m_crossHair)
    {
        createNewCrossHair();
    }

    updatePosition();
}

void Cursor3DTool::updatePosition()
{
    // En cas que no sigui el viewer que estem modificant
    // i que l'estat sigui l'indicat
    if (m_2DViewer->isActive() && m_state == Computing)
    {
        double xyz[3];

        // Cal fer els càlculs per passar del món VTK al mon que té el DICOM per guardar el punt en dicom a les dades compartides de la tool.
        // 1.- Trobar el punt correcte en el món VTK
        if (m_2DViewer->getCurrentCursorImageCoordinate(xyz))
        {
            // 4.- Modificar les dades compartides del punt per tal que els altres s'actualitzin i situar el punt origen
            m_crossHair->setPosition(xyz);
            m_crossHair->setVisibility(true);
            m_crossHair->update();
            m_2DViewer->render();
            // Punt al món real (DICOM)
            m_myData->setOriginPointPosition(xyz);
        }

    }
}

void Cursor3DTool::removePosition()
{
    if (m_state == Computing)
    {
        m_state = None;
        m_viewer->unsetCursor();
    }
}

void Cursor3DTool::updateProjectedPoint()
{
    // En cas que no sigui el viewer que estem modificant
    if (!m_2DViewer->isActive() && m_2DViewer->hasInput())
    {
        if (!m_crossHair && m_2DViewer->hasInput())
        {
            createNewCrossHair();
        }

        // Només podem projectar si tenen el mateix frame of reference UID
        if (m_myFrameOfReferenceUID == m_myData->getFrameOfReferenceUID())
        {
            projectPoint();
        }
        else
        {
            m_crossHair->setVisibility(false);
        }

        m_crossHair->update();
        m_2DViewer->render();
    }
}

void Cursor3DTool::projectPoint()
{
    m_sliceLocator->setVolume(m_2DViewer->getMainInput());
    m_sliceLocator->setPlane(m_2DViewer->getView());
    int nearestSlice = m_sliceLocator->getNearestSlice(m_myData->getOriginPointPosition());

    if (nearestSlice != -1)
    {
        m_2DViewer->setSlice(nearestSlice);
        m_crossHair->setPosition(m_myData->getOriginPointPosition());
        m_crossHair->setVisibility(true);
    }
    else
    {
        // L'amaguem perquè sinó estariem mostrant un punt incorrecte
        m_crossHair->setVisibility(false);
    }
}

void Cursor3DTool::updateFrameOfReference()
{
    // Hi ha d'haver input per força
    Q_ASSERT(m_2DViewer->hasInput());

    // TODO De moment agafem la primera imatge perquè assumim que totes pertanyen a la mateixa sèrie.
    // També ho fem així de moment per evitar problemes amb imatges multiframe, que encara no tractem correctament
    Image *image = m_2DViewer->getMainInput()->getImage(0);
    if (image)
    {
        Series *series = image->getParentSeries();
        // Ens guardem el nostre
        m_myFrameOfReferenceUID = series->getFrameOfReferenceUID();
        m_myInstanceUID = series->getInstanceUID();
        // I actualitzem el de les dades
        m_myData->setFrameOfReferenceUID(m_myFrameOfReferenceUID);
        m_myData->setInstanceUID(m_myInstanceUID);
    }
    else
    {
        DEBUG_LOG("EL nou volum no té series NUL!");
    }
}

void Cursor3DTool::refreshReferenceViewerData()
{
    // Si es projectaven plans sobre el nostre drawer, les amaguem
    if (m_2DViewer->hasInput())
    {
        updateFrameOfReference();
    }
}

void Cursor3DTool::handleImageChange()
{
    if (m_2DViewer->isActive() && m_state == None)
    {
        m_myData->hideCursors();
    }
    else
    {
        updatePosition();
    }
}

void Cursor3DTool::hideCursor()
{
    if (!m_crossHair)
    {
        return;
    }

    if (m_crossHair->isVisible())
    {
        m_crossHair->setVisibility(false);
        m_crossHair->update();
        m_2DViewer->render();
    }
}

void Cursor3DTool::createNewCrossHair()
{
    m_crossHair = new DrawerCrossHair;

    // HACK Succedani d'Smart Pointer per tal que el drawer no elimini el crossHair quan s'activi el thickslab
    m_crossHair->increaseReferenceCount();

    m_crossHair->setVisibility(false);
    m_2DViewer->getDrawer()->draw(m_crossHair);
}

}
