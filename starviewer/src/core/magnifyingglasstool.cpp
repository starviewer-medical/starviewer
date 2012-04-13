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
    m_magnifyingRendererIsVisible = false;
    
    m_2DViewer = qobject_cast<Q2DViewer*>(viewer);
    if (!m_2DViewer)
    {
        DEBUG_LOG(QString("El casting no ha funcionat!!! És possible que viewer no sigui un Q2DViewer!!!-> ") + viewer->metaObject()->className());
    }

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
            m_magnifiedRenderer->AddViewProp(m_2DViewer->getVtkImageActor());
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
    
    double xyz[3];
    if (!m_2DViewer->getCurrentCursorImageCoordinate(xyz))
    {
        m_2DViewer->getEventWorldCoordinate(xyz);
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
    setFocalPoint(xyz);
    m_magnifiedRenderer->ResetCameraClippingRange();
    m_2DViewer->render();
}

void MagnifyingGlassTool::setFocalPoint(const double cursorPosition[3])
{
    // Passem el punt a coordenades de display
    double pointInDisplay[3];
    m_2DViewer->computeWorldToDisplay(cursorPosition[0], cursorPosition[1], cursorPosition[2], pointInDisplay);

    double viewportBounds[4];
    m_magnifiedRenderer->GetViewport(viewportBounds);

    QSize renderWindowSize = m_2DViewer->getRenderWindowSize();

    double magnifyingWindowHalfWidht = (viewportBounds[2] - viewportBounds[0]) * renderWindowSize.width() / 2.0;
    double magnifyingWindowHalfHeight = (viewportBounds[3] - viewportBounds[1]) * renderWindowSize.height() / 2.0;

    // Calculem la diferencia entre el centre del viewport i on esta el cursor per calcular el punt central real
    double offsetXMin = qMax(0.0, magnifyingWindowHalfWidht - pointInDisplay[0]);
    double offsetXMax = qMax(0.0, (pointInDisplay[0] + magnifyingWindowHalfWidht) - renderWindowSize.width());
    double offsetYMin = qMax(0.0, magnifyingWindowHalfHeight - pointInDisplay[1]);
    double offsetYMax = qMax(0.0, (pointInDisplay[1] + magnifyingWindowHalfHeight) - renderWindowSize.height());

    // Apliquem el factor de zoom perque volem la distancia en el render magnificat
    double zoomFactor = getZoomFactor();
    double offsetX = (offsetXMax - offsetXMin) / zoomFactor;
    double offsetY = (offsetYMax - offsetYMin) / zoomFactor;

    double temporalWorldPoint[4];
    m_2DViewer->computeDisplayToWorld(pointInDisplay[0] - offsetX, pointInDisplay[1] - offsetY, pointInDisplay[2], temporalWorldPoint);

    double focalPoint[3];
    focalPoint[0] = temporalWorldPoint[0];
    focalPoint[1] = temporalWorldPoint[1];
    focalPoint[2] = temporalWorldPoint[2];

    m_magnifiedCamera->SetFocalPoint(focalPoint);

    int xIndex, yIndex, zIndex;
    Q2DViewer::getXYZIndexesForView(xIndex, yIndex, zIndex, m_2DViewer->getView());

    double position[3];
    m_magnifiedCamera->GetPosition(position);
    position[xIndex] = temporalWorldPoint[xIndex];
    position[yIndex] = temporalWorldPoint[yIndex];
    m_magnifiedCamera->SetPosition(position);
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
