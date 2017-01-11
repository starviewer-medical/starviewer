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

#include "magnifyingglasstool.h"

#include "q2dviewer.h"
#include "coresettings.h"

// vtk
#include <vtkCommand.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkCamera.h>
#include <vtkImageSlice.h>

namespace udg {

MagnifyingGlassTool::MagnifyingGlassTool(QViewer *viewer, QObject *parent)
 : Tool(viewer,parent)
{
    m_toolName = "MagnifyingGlassTool";
    m_magnifyingRendererIsVisible = false;
    
    m_2DViewer = Q2DViewer::castFromQViewer(viewer);

    m_magnifiedRenderer = vtkRenderer::New();
}

MagnifyingGlassTool::~MagnifyingGlassTool()
{
    removeMagnifiedRenderer();
    m_magnifiedRenderer->Delete();
    m_2DViewer->unsetCursor();
}

void MagnifyingGlassTool::handleEvent(unsigned long eventID)
{
    switch (eventID)
    {
        case vtkCommand::LeftButtonPressEvent:
            enableConnections();
            updateMagnifiedView();
            break;

        case vtkCommand::MouseMoveEvent:
            if (m_magnifyingRendererIsVisible)
            {
                updateMagnifiedView();
            }
            break;

        case vtkCommand::RightButtonPressEvent:
        case vtkCommand::LeftButtonReleaseEvent:
            enableConnections(false);
            removeMagnifiedRenderer();
            break;

        default:
            break;
    }
}

void MagnifyingGlassTool::updateMagnifiedRendererViewport(const QPoint &center, const QSize &viewerSize)
{
    double magnifyingWindowSize = 150.0;

    double xMin = center.x() / (double)viewerSize.width() - magnifyingWindowSize / viewerSize.width();
    double xMax = center.x() / (double)viewerSize.width() + magnifyingWindowSize / viewerSize.width();
    double yMin = center.y() / (double)viewerSize.height() - magnifyingWindowSize / viewerSize.height();
    double yMax = center.y() / (double)viewerSize.height() + magnifyingWindowSize / viewerSize.height();

    xMin = qMax(0.0, xMin);
    yMin = qMax(0.0, yMin);
    xMax = qMin(1.0, xMax);
    yMax = qMin(1.0, yMax);

    m_magnifiedRenderer->SetViewport(xMin, yMin, xMax, yMax);
}

void MagnifyingGlassTool::removeMagnifiedRenderer()
{
    if (m_magnifyingRendererIsVisible)
    {
        m_2DViewer->unsetCursor();
        m_magnifiedRenderer->RemoveAllViewProps();
        m_2DViewer->getRenderWindow()->RemoveRenderer(m_magnifiedRenderer);
        m_2DViewer->render();
        m_magnifyingRendererIsVisible = false;
    }
}

double MagnifyingGlassTool::getZoomFactor()
{
    Settings settings;
    double factor = settings.getValue(CoreSettings::MagnifyingGlassZoomFactor).toDouble();
    if (factor == 0.0)
    {
        // En cas que el setting no tingui un valor vàlid, li assignem un valor per defecte de 4.0
        factor = 4.0;
        settings.setValue(CoreSettings::MagnifyingGlassZoomFactor, "4");
    }

    return factor;
}

void MagnifyingGlassTool::addMagnifiedRenderer()
{
    if (!m_2DViewer->getRenderWindow()->HasRenderer(m_magnifiedRenderer))
    {
        m_2DViewer->getRenderWindow()->AddRenderer(m_magnifiedRenderer);
        
        if (m_magnifiedRenderer->GetActors()->GetNumberOfItems() == 0)
        {
            m_magnifiedRenderer->AddViewProp(m_2DViewer->getImageProp());
        }
    }
    
    m_magnifyingRendererIsVisible = true;
}

void MagnifyingGlassTool::updateMagnifiedView()
{
    QPoint eventPosition = m_2DViewer->getEventPosition();
    QSize renderWindowSize = m_2DViewer->getRenderWindowSize();

    QRect renderWindowBounds(QPoint(0, 0), renderWindowSize);
    if (!renderWindowBounds.contains(eventPosition))
    {
        // Si el punt està fora de la render window amaguem i sortim
        removeMagnifiedRenderer();
        return;
    }
    
    m_2DViewer->setCursor(QCursor(Qt::BlankCursor));
    
    // Actualitzem el viewport
    updateMagnifiedRendererViewport(eventPosition, renderWindowSize);
    updateCamera();
    
    if (!m_magnifyingRendererIsVisible)
    {
        addMagnifiedRenderer();
    }
    
    // Actualitzem la posició que enfoca la càmera
    setFocalPoint();
    m_magnifiedRenderer->ResetCameraClippingRange();
    m_2DViewer->render();
}

void MagnifyingGlassTool::setFocalPoint()
{
    // Compute offset in pixels from the viewer center and the mouse position
    QSize renderWindowSize = m_2DViewer->getRenderWindowSize();
    QPoint renderWindowCenter(renderWindowSize.width() / 2, renderWindowSize.height() / 2);
    QPoint mousePosition = m_2DViewer->getEventPosition();
    QPoint offset = mousePosition - renderWindowCenter;

    // Compute right and up vectors in world coordinates with a length of 1 pixel in display
    Vector3 zero = m_2DViewer->computeDisplayToWorld(Vector3(0, 0, 0));
    Vector3 x1 = m_2DViewer->computeDisplayToWorld(Vector3(1, 0, 0));
    Vector3 y1 = m_2DViewer->computeDisplayToWorld(Vector3(0, 1, 0));
    Vector3 right = x1 - zero;
    Vector3 up = y1 - zero;

    // Compute offset vector in world coordinates to add to the camera
    Vector3 offsetVector = right * offset.x() + up * offset.y();

    // Add the computed offset to magnified camera's position and focal point
    Vector3 position(m_magnifiedCamera->GetPosition());
    Vector3 focalPoint(m_magnifiedCamera->GetFocalPoint());
    m_magnifiedCamera->SetPosition((position + offsetVector).data());
    m_magnifiedCamera->SetFocalPoint((focalPoint + offsetVector).data());
}

void MagnifyingGlassTool::updateCamera()
{
    if (!m_magnifiedCamera)
    {
        m_magnifiedCamera = vtkSmartPointer<vtkCamera>::New();
        m_magnifiedRenderer->SetActiveCamera(m_magnifiedCamera);
    }
    vtkCamera *viewerCamera = m_2DViewer->getRenderer()->GetActiveCamera();
    m_magnifiedCamera->DeepCopy(viewerCamera);

    // Ajustem la càmera a la mateixa proporció que el renderer principal
    // Cal prendre la proporció del viewport magnificat respecte el viewer en sí
    double viewportsProportion;
    double viewportPoints[4];
    m_magnifiedRenderer->GetViewport(viewportPoints);
    viewportsProportion = fabs(viewportPoints[3] - viewportPoints[1]);

    // Fixem el mateix zoom que en el renderer principal
    if (viewerCamera->GetParallelProjection())
    {
        m_magnifiedCamera->SetParallelScale(viewerCamera->GetParallelScale() * viewportsProportion);
    }
    else
    {
        m_magnifiedCamera->SetViewAngle(viewerCamera->GetViewAngle() * viewportsProportion);
    }
    
    // Apliquem el factor de magnificació
    m_magnifiedCamera->Zoom(getZoomFactor());
}

void MagnifyingGlassTool::update()
{
    updateCamera();
    updateMagnifiedView();
}

void MagnifyingGlassTool::enableConnections(bool enable)
{
    if (enable)
    {
        connect(m_2DViewer, SIGNAL(volumeChanged(Volume*)), SLOT(removeMagnifiedRenderer()));
        connect(m_2DViewer, SIGNAL(sliceChanged(int)), SLOT(updateMagnifiedView()));
        connect(m_2DViewer, SIGNAL(cameraChanged()), SLOT(update()));
    }
    else
    {
        disconnect(m_2DViewer, SIGNAL(volumeChanged(Volume*)), this, SLOT(removeMagnifiedRenderer()));
        disconnect(m_2DViewer, SIGNAL(sliceChanged(int)), this, SLOT(updateMagnifiedView()));
        disconnect(m_2DViewer, SIGNAL(cameraChanged()), this, SLOT(update()));
    }
}

}
