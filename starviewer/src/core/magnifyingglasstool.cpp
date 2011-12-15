#include "magnifyingglasstool.h"

#include "q2dviewer.h"
#include "logging.h"
#include "magnifyingglasstooldata.h"

// vtk
#include <vtkCommand.h>

namespace udg {

MagnifyingGlassTool::MagnifyingGlassTool(QViewer *viewer, QObject *parent)
 : Tool(viewer,parent)
{
    m_hasSharedData = true;
    if (!m_toolData)
    {
        m_myData = new MagnifyingGlassToolData(viewer->parent());
        m_toolData = m_myData;
    }
    else
    {
        m_myData = dynamic_cast<MagnifyingGlassToolData*>(m_toolData);
    }
        
    m_toolName = "MagnifyingGlassTool";
    m_2DViewer = qobject_cast<Q2DViewer*>(viewer);
    if (!m_2DViewer)
    {
        DEBUG_LOG(QString("El casting no ha funcionat!!! És possible que viewer no sigui un Q2DViewer!!!-> ")+ viewer->metaObject()->className());
    }

    createConnections();
}

MagnifyingGlassTool::~MagnifyingGlassTool()
{
    m_myData->get2DMagnifyingGlassViewer()->close();
}

void MagnifyingGlassTool::handleEvent(unsigned long eventID)
{
    switch (eventID)
    {
        case vtkCommand::MouseMoveEvent:
            this->updateMagnifyingGlassWidgetPosition();
            updateMagnifiedImagePosition();
            break;

        case vtkCommand::EnterEvent:
            updateMagnifyingGlassWidgetPosition();
            updateMagnifiedView();
            updateMagnifiedImagePosition();
            m_myData->get2DMagnifyingGlassViewer()->show();
            break;

        case vtkCommand::LeaveEvent:
            m_myData->get2DMagnifyingGlassViewer()->hide();
            break;

        default:
            break;
    }
}

void MagnifyingGlassTool::setVolume(Volume *volume)
{
    m_myData->get2DMagnifyingGlassViewer()->setInput(volume);
    m_myData->get2DMagnifyingGlassViewer()->zoom(m_myData->getZoomFactor());
}

void MagnifyingGlassTool::setSlice(int slice)
{
    m_myData->get2DMagnifyingGlassViewer()->enableRendering(false);
    m_myData->get2DMagnifyingGlassViewer()->setSlice(slice);
    // TODO Arreglar-ho perquè sigui automàtic i tingui el mateix WW/WL que la finestra que magnifica
    setWindowLevel(m_window, m_level);
    m_myData->get2DMagnifyingGlassViewer()->enableRendering(true);
    m_myData->get2DMagnifyingGlassViewer()->render();
}

void MagnifyingGlassTool::setPhase(int phase)
{
    m_myData->get2DMagnifyingGlassViewer()->enableRendering(false);
    m_myData->get2DMagnifyingGlassViewer()->setPhase(phase);
    // TODO Arreglar-ho perquè sigui automàtic i tingui el mateix WW/WL que la finestra que magnifica
    setWindowLevel(m_window, m_level);
    
    m_myData->get2DMagnifyingGlassViewer()->enableRendering(true);
    m_myData->get2DMagnifyingGlassViewer()->render();
}

void MagnifyingGlassTool::setWindowLevel(double window, double level)
{
    m_myData->get2DMagnifyingGlassViewer()->setWindowLevel(window, level);
    m_window = window;
    m_level = level;
}

void MagnifyingGlassTool::setZoom(double zoom)
{
    m_myData->get2DMagnifyingGlassViewer()->zoom(zoom);
}

void MagnifyingGlassTool::setSlabThickness(int thickness)
{
    m_myData->get2DMagnifyingGlassViewer()->setSlabThickness(thickness);
}

void MagnifyingGlassTool::updateRotationFactor(int rotationFactor)
{
    m_myData->get2DMagnifyingGlassViewer()->setRotationFactor(rotationFactor);
}

void MagnifyingGlassTool::horizontalFlip()
{
    m_myData->get2DMagnifyingGlassViewer()->horizontalFlip();
}

void MagnifyingGlassTool::setView(int view)
{
    m_myData->get2DMagnifyingGlassViewer()->resetView(static_cast<QViewer::CameraOrientationType>(view));
    m_myData->get2DMagnifyingGlassViewer()->zoom(m_myData->getZoomFactor());
    
    setSlice(m_2DViewer->getCurrentSlice());
}

void MagnifyingGlassTool::updateMagnifyingGlassWidgetPosition()
{
    // Movem la finestra per que acompanyi el cursor
    QPoint eventPosition = m_2DViewer->getEventPosition();

    // Remember to flip y
    QSize windowSize = m_2DViewer->getRenderWindowSize();
    eventPosition.setY(windowSize.height() - eventPosition.y());

    // Map to global 
    QPoint globalPoint = m_2DViewer->mapToGlobal(eventPosition);

    m_myData->get2DMagnifyingGlassViewer()->move(globalPoint.x() + 10, globalPoint.y() + 10);
}

void MagnifyingGlassTool::updateMagnifiedImagePosition()
{
    QSize size = m_myData->get2DMagnifyingGlassViewer()->getRenderWindowSize();

    double newPickPoint[4];
    m_myData->get2DMagnifyingGlassViewer()->computeDisplayToWorld(size.width() / 2, size.height() / 2, 0, newPickPoint);

    double oldPickPoint[4];
    m_2DViewer->getLastEventWorldCoordinate(oldPickPoint);

    double motionVector[3];
    motionVector[0] = oldPickPoint[0] - newPickPoint[0];
    motionVector[1] = oldPickPoint[1] - newPickPoint[1];
    motionVector[2] = oldPickPoint[2] - newPickPoint[2];

    m_myData->get2DMagnifyingGlassViewer()->pan(motionVector);
}

void MagnifyingGlassTool::updateMagnifiedView()
{
    m_myData->get2DMagnifyingGlassViewer()->enableRendering(false);

    setVolume(m_2DViewer->getInput());
    setView(m_2DViewer->getView());
    setPhase(m_2DViewer->getCurrentPhase());
    setWindowLevel(m_2DViewer->getCurrentColorWindow(), m_2DViewer->getCurrentColorLevel());
    m_window = m_2DViewer->getCurrentColorWindow();
    m_level = m_2DViewer->getCurrentColorLevel();
    setSlabThickness(m_2DViewer->getSlabThickness());
    m_myData->get2DMagnifyingGlassViewer()->setRotationFactor(m_2DViewer->getRotationFactor());
    if (m_2DViewer->isImageFlipped())
    {
        m_myData->get2DMagnifyingGlassViewer()->horizontalFlip();
    }    
    m_myData->get2DMagnifyingGlassViewer()->enableRendering(true);
    m_myData->get2DMagnifyingGlassViewer()->render();
}

void MagnifyingGlassTool::setToolData(ToolData *data)
{
    m_toolData = data;
    m_myData = qobject_cast<MagnifyingGlassToolData*>(data);

    disconnect(m_2DViewer, SIGNAL(volumeChanged(Volume *)), this, SLOT(setVolume(Volume *)));
    disconnect(m_2DViewer, SIGNAL(viewChanged(int)), this, SLOT(setView(int)));
    disconnect(m_2DViewer, SIGNAL(sliceChanged(int)), this, SLOT(setSlice(int)));
    disconnect(m_2DViewer, SIGNAL(phaseChanged(int)), this, SLOT(setPhase(int)));
    disconnect(m_2DViewer, SIGNAL(windowLevelChanged(double, double)), this, SLOT(setWindowLevel(double, double)));
    disconnect(m_2DViewer, SIGNAL(zoomFactorChanged(double)), this, SLOT(setZoom(double)));
    disconnect(m_2DViewer, SIGNAL(slabThicknessChanged(int)), this, SLOT(setSlabThickness(int)));
    disconnect(m_2DViewer, SIGNAL(rotationFactorChanged(int)), this, SLOT(updateRotationFactor(int)));
    disconnect(m_2DViewer, SIGNAL(flippedHorizontally()), this, SLOT(horizontalFlip()));

    createConnections();
}

void MagnifyingGlassTool::createConnections()
{
    connect(m_2DViewer, SIGNAL(volumeChanged(Volume *)), SLOT(setVolume(Volume *)));
    connect(m_2DViewer, SIGNAL(viewChanged(int)), SLOT(setView(int)));
    connect(m_2DViewer, SIGNAL(sliceChanged(int)), SLOT(setSlice(int)));
    connect(m_2DViewer, SIGNAL(phaseChanged(int)), SLOT(setPhase(int)));
    connect(m_2DViewer, SIGNAL(windowLevelChanged(double, double)), SLOT(setWindowLevel(double, double)));
    connect(m_2DViewer, SIGNAL(zoomFactorChanged(double)), SLOT(setZoom(double)));
    connect(m_2DViewer, SIGNAL(slabThicknessChanged(int)), SLOT(setSlabThickness(int)));
    connect(m_2DViewer, SIGNAL(rotationFactorChanged(int)), SLOT(updateRotationFactor(int)));
    connect(m_2DViewer, SIGNAL(flippedHorizontally()), SLOT(horizontalFlip()));
}

}
