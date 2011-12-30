#include "magnifyingglasstool.h"

#include "q2dviewer.h"
#include "logging.h"

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

    createConnections();
}

MagnifyingGlassTool::~MagnifyingGlassTool()
{
    hideMagnifiedRenderer();
    m_magnifiedRenderer->Delete();
}

void MagnifyingGlassTool::handleEvent(unsigned long eventID)
{
    switch (eventID)
    {
        case vtkCommand::MouseMoveEvent:
            updateMagnifiedImage();
            break;

        case vtkCommand::EnterEvent:
            if (!m_magnifyingWindowShown)
            {
                updateMagnifiedRenderer();
            }
            break;

        case vtkCommand::LeaveEvent:
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
        xMin = 0;
        xMax = (magnifyingWindowSize / size.width()) * 2;
    }
    if (yMin < 0)
    {
        yMin = 0;
        yMax = (magnifyingWindowSize / size.height()) * 2;
    }
    if (xMax > 1)
    {
        xMin = 1 - ((magnifyingWindowSize / size.width()) * 2);
        xMax = 1;
    }
    if (yMax > 1)
    {
        yMin = 1 - ((magnifyingWindowSize / size.height()) * 2);
        yMax = 1;
    }

    m_magnifiedRenderer->SetViewport(xMin, yMin, xMax, yMax);
    m_2DViewer->render();
}

void MagnifyingGlassTool::hideMagnifiedRenderer()
{
    m_2DViewer->setCursor(QCursor(Qt::ArrowCursor));
    m_2DViewer->getRenderWindow()->RemoveRenderer(m_magnifiedRenderer);
    m_2DViewer->render();
    m_magnifyingWindowShown = false;
}

void MagnifyingGlassTool::updateMagnifiedRenderer()
{
    // TODO Nomès s'afegeix una sola vegada si ja existeix??? Comprovar!
    m_2DViewer->getRenderWindow()->AddRenderer(m_magnifiedRenderer);
    
    updateCamera();

    // TODO Nomès s'afegeix una sola vegada si ja existeix??? Comprovar!
    m_magnifiedRenderer->AddViewProp(m_2DViewer->getVtkImageActor());
    m_magnifyingWindowShown = true;
}

void MagnifyingGlassTool::updateMagnifiedImage()
{
    double xyz[3];
    if (m_2DViewer->getCurrentCursorImageCoordinate(xyz))
    {
        m_2DViewer->setCursor(QCursor(Qt::BlankCursor));
        if (!m_magnifyingWindowShown)
        {
            updateMagnifiedRenderer();
        }
        // Actualitzem la posició de la imatge
        m_magnifiedCamera->SetFocalPoint(xyz);
        m_magnifiedRenderer->ResetCameraClippingRange();
        updateMagnifiedViewportPosition();
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
    }
    m_magnifiedCamera->DeepCopy(m_2DViewer->getRenderer()->GetActiveCamera());
    m_magnifiedRenderer->SetActiveCamera(m_magnifiedCamera);

    // Codi extret de QViewer::zoom(). TODO Fer refactoring
    double factor = 4.0;
    if (m_magnifiedCamera->GetParallelProjection())
    {
        m_magnifiedCamera->SetParallelScale(m_magnifiedCamera->GetParallelScale() / factor);
    }
    else
    {
        m_magnifiedCamera->Dolly(factor);
        //if (vtkInteractorStyle::SafeDownCast(this->getInteractor()->GetInteractorStyle())->GetAutoAdjustCameraClippingRange())
        //{
            // TODO en principi sempre ens interessarà fer això? ens podriem enstalviar l'if??
            m_magnifiedRenderer->ResetCameraClippingRange();
        //}
    }
}

void MagnifyingGlassTool::update()
{
    updateCamera();
    updateMagnifiedImage();
}

void MagnifyingGlassTool::hideAndUpdate()
{
    hideMagnifiedRenderer();
    update();
}

void MagnifyingGlassTool::createConnections()
{
    connect(m_2DViewer, SIGNAL(volumeChanged(Volume*)), SLOT(hideAndUpdate()));
    connect(m_2DViewer, SIGNAL(sliceChanged(int)), SLOT(updateMagnifiedImage()));
    connect(m_2DViewer, SIGNAL(cameraChanged()), SLOT(update()));
}

}
