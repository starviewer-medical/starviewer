#include "magnifyingglasstool.h"

#include "q2dviewer.h"
#include "logging.h"
#include "coresettings.h"

// vtk
#include <vtkCommand.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkCamera.h>
#include <vtkImageActor.h>

namespace udg {

MagnifyingGlassTool::MagnifyingGlassTool(QViewer *viewer, QObject *parent)
 : Tool(viewer,parent)
{
    m_toolName = "MagnifyingGlassTool";
    m_magnifyingWindowShown = false;
    
    m_2DViewer = qobject_cast<Q2DViewer*>(viewer);
    if (!m_2DViewer)
    {
        DEBUG_LOG(QString("El casting no ha funcionat!!! És possible que viewer no sigui un Q2DViewer!!!-> ")+ viewer->metaObject()->className());
    }

    m_magnifiedRenderer = vtkRenderer::New();
}

MagnifyingGlassTool::~MagnifyingGlassTool()
{
    hideMagnifiedRenderer();
    m_magnifiedRenderer->Delete();
    m_2DViewer->unsetCursor();
}

void MagnifyingGlassTool::handleEvent(unsigned long eventID)
{
    switch (eventID)
    {
        case vtkCommand::LeftButtonPressEvent:
            enableConnections();
            updateMagnifiedImage();
            break;

        case vtkCommand::MouseMoveEvent:
            if (m_magnifyingWindowShown)
            {
                updateMagnifiedImage();
            }
            break;

        case vtkCommand::LeftButtonReleaseEvent:
        case vtkCommand::LeaveEvent:
            enableConnections(false);
            hideMagnifiedRenderer();
            break;

        default:
            break;
    }
}

void MagnifyingGlassTool::updateMagnifiedViewportPosition()
{
    // Movem la finestra per que acompanyi el cursor
    QPoint eventPosition = m_2DViewer->getEventPosition();
    QSize size = m_2DViewer->getRenderWindowSize();

    double magnifyingWindowSize = 150.0;

    double xMin = eventPosition.x() / (double)size.width() - magnifyingWindowSize / size.width();
    double xMax = eventPosition.x() / (double)size.width() + magnifyingWindowSize / size.width();
    double yMin = eventPosition.y() / (double)size.height() - magnifyingWindowSize / size.height();
    double yMax = eventPosition.y() / (double)size.height() + magnifyingWindowSize / size.height();

    if (xMin < 0)
    {
        xMax = ((magnifyingWindowSize / size.width()) * 2) + xMin;
        xMin = 0;
    }
    if (yMin < 0)
    {
        yMax = ((magnifyingWindowSize / size.height()) * 2) + yMin;
        yMin = 0;
    }
    if (xMax > 1)
    {
        xMin = xMax - ((magnifyingWindowSize / size.width()) * 2);
        xMax = 1;
    }
    if (yMax > 1)
    {
        yMin = yMax - ((magnifyingWindowSize / size.height()) * 2);
        yMax = 1;
    }

    m_magnifiedRenderer->SetViewport(xMin, yMin, xMax, yMax);
}

void MagnifyingGlassTool::hideMagnifiedRenderer()
{
    if (m_magnifyingWindowShown)
    {
        m_2DViewer->unsetCursor();
        m_magnifiedRenderer->RemoveAllViewProps();
        m_2DViewer->getRenderWindow()->RemoveRenderer(m_magnifiedRenderer);
        m_2DViewer->render();
        m_magnifyingWindowShown = false;
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

void MagnifyingGlassTool::updateMagnifiedRenderer()
{
    if (!m_2DViewer->getRenderWindow()->HasRenderer(m_magnifiedRenderer))
    {
        m_2DViewer->getRenderWindow()->AddRenderer(m_magnifiedRenderer);
        
        if (m_magnifiedRenderer->GetActors()->GetNumberOfItems() == 0)
        {
            m_magnifiedRenderer->AddViewProp(m_2DViewer->getVtkImageActor());
        }
    }
    
    m_magnifyingWindowShown = true;
}

void MagnifyingGlassTool::updateMagnifiedImage()
{
    double xyz[3];
    if (m_2DViewer->getCurrentCursorImageCoordinate(xyz))
    {
        m_2DViewer->setCursor(QCursor(Qt::BlankCursor));
        
        // Actualitzem la mida i posició del viewport
        updateMagnifiedViewportPosition();
        
        if (!m_magnifyingWindowShown)
        {
            updateMagnifiedRenderer();
            updateCamera();
        }
        
        // Actualitzem la posició que enfoca la càmera
        m_magnifiedCamera->SetFocalPoint(xyz);
        m_magnifiedRenderer->ResetCameraClippingRange();
        m_2DViewer->render();
    }
    else
    {
        if (m_magnifyingWindowShown)
        {
            hideMagnifiedRenderer();
        }
    }
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
    QSize size = m_2DViewer->getRenderWindowSize();
    double viewportsProportion;
    double viewportPoints[4];
    m_magnifiedRenderer->GetViewport(viewportPoints);
    if (size.width() > size.height())
    {
        viewportsProportion = fabs(viewportPoints[2] - viewportPoints[0]);
    }
    else
    {
        viewportsProportion = fabs(viewportPoints[3] - viewportPoints[1]);
    }

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
    updateMagnifiedImage();
}

void MagnifyingGlassTool::enableConnections(bool enable)
{
    if (enable)
    {
        connect(m_2DViewer, SIGNAL(volumeChanged(Volume*)), SLOT(hideMagnifiedRenderer()));
        connect(m_2DViewer, SIGNAL(sliceChanged(int)), SLOT(updateMagnifiedImage()));
        connect(m_2DViewer, SIGNAL(cameraChanged()), SLOT(update()));
    }
    else
    {
        disconnect(m_2DViewer, SIGNAL(volumeChanged(Volume*)), this, SLOT(hideMagnifiedRenderer()));
        disconnect(m_2DViewer, SIGNAL(sliceChanged(int)), this, SLOT(updateMagnifiedImage()));
        disconnect(m_2DViewer, SIGNAL(cameraChanged()), this, SLOT(update()));
    }
}

}
