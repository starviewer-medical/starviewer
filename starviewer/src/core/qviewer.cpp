#include "qviewer.h"
#include "volume.h"
#include "series.h"
#include "image.h"
#include "toolproxy.h"
#include "patientbrowsermenu.h"
// Per poder afegir i modificar els presets que visualitzem
#include "windowlevelpresetstooldata.h"
#include "qviewerworkinprogresswidget.h"
#include "windowlevelhelper.h"
#include "logging.h"
#include "mathtools.h"

// TODO: Ouch! SuperGuarrada (tm). Per poder fer sortir el menú i tenir accés al Patient principal. S'ha d'arreglar en quan es tregui les dependències de
// interface, pacs, etc.etc.!!
#include "../interface/qapplicationmainwindow.h"

// Qt
#include <QStackedLayout>
#include <QContextMenuEvent>
#include <QMessageBox>
#include <QDir>

// Include's vtk
#include <QVTKWidget.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkBMPWriter.h>
#include <vtkPNGWriter.h>
#include <vtkPNMWriter.h>
#include <vtkJPEGWriter.h>
#include <vtkTIFFWriter.h>
#include <vtkWindowToImageFilter.h>
#include <vtkEventQtSlotConnect.h>
// Necessari pel zoom
#include <vtkCamera.h>
// Per grabar el vídeo
#ifndef Q_OS_LINUX
#include <vtkMPEG2Writer.h>
#endif

namespace udg {

QViewer::QViewer(QWidget *parent)
 : QWidget(parent), m_mainVolume(0), m_contextMenuActive(true), m_mouseHasMoved(false), m_windowLevelData(0),
   m_isRenderingEnabled(true), m_isActive(false)
{
    m_defaultFitIntoViewportMarginRate = 0.0;
    m_vtkWidget = new QVTKWidget(this);
    m_vtkWidget->setFocusPolicy(Qt::WheelFocus);
    // Creem el renderer i li assignem a la render window
    m_renderer = vtkRenderer::New();
    getRenderWindow()->AddRenderer(m_renderer);
    m_renderer->Delete();
    // Forcem 2x buffer
    getRenderWindow()->DoubleBufferOn();
    getRenderWindow()->LineSmoothingOn();

    // Posem a punt el filtre per guardar captures de pantalla
    m_windowToImageFilter = vtkWindowToImageFilter::New();
    m_windowToImageFilter->SetInput(getRenderWindow());

    this->setCurrentViewPlane(OrthogonalPlane::XYPlane);

    // Connectem els events
    setupInteraction();

    m_toolProxy = new ToolProxy(this);
    connect(this, SIGNAL(eventReceived(unsigned long)), m_toolProxy, SLOT(forwardEvent(unsigned long)));

    // Inicialitzem el window level data
    setWindowLevelData(new WindowLevelPresetsToolData(this));

    m_workInProgressWidget = new QViewerWorkInProgressWidget(this);

    // Afegim el layout
    m_stackedLayout = new QStackedLayout(this);
    m_stackedLayout->setSpacing(0);
    m_stackedLayout->setMargin(0);
    m_stackedLayout->addWidget(m_vtkWidget);
    m_stackedLayout->addWidget(m_workInProgressWidget);

    // Inicialitzem l'status del viewer
    m_previousViewerStatus = m_viewerStatus = NoVolumeInput;
    this->setCurrentWidgetByViewerStatus(m_viewerStatus);
    this->initializeWorkInProgressByViewerStatus(m_viewerStatus);

    this->setMouseTracking(false);
    m_patientBrowserMenu = new PatientBrowserMenu(0);
    // Ara mateix el comportament per defecte serà que un cop seleccionat un volum li assignem immediatament com a input
    this->setAutomaticallyLoadPatientBrowserMenuSelectedInput(true);
}

QViewer::~QViewer()
{
    // Cal que la eliminació del vtkWidget sigui al final ja que els altres
    // objectes que eliminem en poden fer ús durant la seva destrucció
    delete m_toolProxy;
    delete m_patientBrowserMenu;
    m_windowToImageFilter->Delete();
    delete m_vtkWidget;
    m_vtkQtConnections->Delete();
}

vtkRenderWindowInteractor* QViewer::getInteractor() const
{
    return m_vtkWidget->GetRenderWindow()->GetInteractor();
}

vtkRenderer* QViewer::getRenderer() const
{
    return m_renderer;
}

Volume* QViewer::getMainInput() const
{
    return m_mainVolume;
}

int QViewer::getNumberOfInputs() const
{
    if (hasInput())
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

bool QViewer::hasInput() const
{
    if (getMainInput())
    {
        return true;
    }
    else
    {
        return false;
    }
}

vtkRenderWindow* QViewer::getRenderWindow() const
{
    return m_vtkWidget->GetRenderWindow();
}

QSize QViewer::getRenderWindowSize() const
{
    return QSize(this->getRenderWindow()->GetSize()[0], this->getRenderWindow()->GetSize()[1]);
}

QPoint QViewer::getEventPosition() const
{
    return QPoint(this->getInteractor()->GetEventPosition()[0], this->getInteractor()->GetEventPosition()[1]);
}

QPoint QViewer::getLastEventPosition() const
{
    return QPoint(this->getInteractor()->GetLastEventPosition()[0], this->getInteractor()->GetLastEventPosition()[1]);
}

bool QViewer::isActive() const
{
    return m_isActive;
}

ToolProxy* QViewer::getToolProxy() const
{
    return m_toolProxy;
}

void QViewer::eventHandler(vtkObject *object, unsigned long vtkEvent, void *clientData, void *callData, vtkCommand *command)
{
    Q_UNUSED(object);
    Q_UNUSED(clientData);
    Q_UNUSED(callData);
    Q_UNUSED(command);

    // Quan la finestra sigui "seleccionada" s'emetrà un senyal indicant-ho. Entenem seleccionada quan s'ha clicat o mogut la rodeta per sobre del visor.
    // TODO Ara resulta ineficient perquè un cop seleccionat no caldria re-enviar aquesta senyal. Cal millorar el sistema
    switch (vtkEvent)
    {
        case QVTKWidget::ContextMenuEvent:
        case vtkCommand::LeftButtonPressEvent:
        case vtkCommand::RightButtonPressEvent:
        case vtkCommand::MiddleButtonPressEvent:
        case vtkCommand::MouseWheelForwardEvent:
        case vtkCommand::MouseWheelBackwardEvent:
            m_mouseHasMoved = false;
            setActive(true);
            break;

        case vtkCommand::MouseMoveEvent:
            m_mouseHasMoved = true;
            break;

        case vtkCommand::RightButtonReleaseEvent:
            if (!m_mouseHasMoved)
            {
                contextMenuRelease();
            }
            break;
    }
    emit eventReceived(vtkEvent);
}

void QViewer::setActive(bool active)
{
    if (!m_isActive && active)
    {
        m_isActive = true;
        emit selected();
    }
    else
    {
        m_isActive = active;
    }
}

void QViewer::computeDisplayToWorld(double x, double y, double z, double worldPoint[3])
{
    vtkRenderer *renderer = this->getRenderer();
    if (renderer)
    {
        double homogeneousWorldPoint[4];
        renderer->SetDisplayPoint(x, y, z);
        renderer->DisplayToWorld();
        renderer->GetWorldPoint(homogeneousWorldPoint);
        
        double divisor = 1.0;
        if (homogeneousWorldPoint[3])
        {
            divisor = homogeneousWorldPoint[3];
        }
        worldPoint[0] = homogeneousWorldPoint[0] / divisor;
        worldPoint[1] = homogeneousWorldPoint[1] / divisor;
        worldPoint[2] = homogeneousWorldPoint[2] / divisor;
    }
}

void QViewer::computeWorldToDisplay(double x, double y, double z, double displayPoint[3])
{
    vtkRenderer *renderer = this->getRenderer();
    if (renderer)
    {
        renderer->SetWorldPoint(x, y, z, 1.0);
        renderer->WorldToDisplay();
        renderer->GetDisplayPoint(displayPoint);
    }
}

void QViewer::getEventWorldCoordinate(double worldCoordinate[3])
{
    getRecentEventWorldCoordinate(worldCoordinate, true);
}

void QViewer::getLastEventWorldCoordinate(double worldCoordinate[3])
{
    getRecentEventWorldCoordinate(worldCoordinate, false);
}

void QViewer::getRecentEventWorldCoordinate(double worldCoordinate[3], bool current)
{
    QPoint position;

    if (current)
    {
        position = this->getEventPosition();
    }
    else
    {
        position = this->getLastEventPosition();
    }

    this->computeDisplayToWorld(position.x(), position.y(), 0, worldCoordinate);
}

void QViewer::setupInteraction()
{
    Q_ASSERT(m_renderer);

    // TODO Fer això aquí? o fer-ho en el tool manager?
    this->getInteractor()->RemoveObservers(vtkCommand::LeftButtonPressEvent);
    this->getInteractor()->RemoveObservers(vtkCommand::RightButtonPressEvent);
    this->getInteractor()->RemoveObservers(vtkCommand::MouseWheelForwardEvent);
    this->getInteractor()->RemoveObservers(vtkCommand::MouseWheelBackwardEvent);
    this->getInteractor()->RemoveObservers(vtkCommand::MiddleButtonPressEvent);
    this->getInteractor()->RemoveObservers(vtkCommand::CharEvent);

    m_vtkQtConnections = vtkEventQtSlotConnect::New();
    // Despatxa qualsevol event-> tools
    m_vtkQtConnections->Connect(this->getInteractor(), vtkCommand::AnyEvent, this, SLOT(eventHandler(vtkObject*, unsigned long, void*, void*, vtkCommand*)));
}

vtkCamera* QViewer::getActiveCamera()
{
    return (this->getRenderer() ? this->getRenderer()->GetActiveCamera() : NULL);
}

bool QViewer::saveGrabbedViews(const QString &baseName, FileType extension)
{
    if (!m_grabList.empty())
    {
        vtkImageWriter *writer;
        QString fileExtension;
        switch (extension)
        {
            case PNG:
                writer = vtkPNGWriter::New();
                fileExtension = "png";
                break;

            case JPEG:
                writer = vtkJPEGWriter::New();
                fileExtension = "jpg";
                break;

            case TIFF:
                writer = vtkTIFFWriter::New();
                fileExtension = "tiff";
                break;

            case PNM:
                writer = vtkPNMWriter::New();
                fileExtension = "pnm";
                break;

            case BMP:
                writer = vtkBMPWriter::New();
                fileExtension = "bmp";
                break;

            case DICOM:
                // TODO A suportar
                DEBUG_LOG("El format DICOM encara no està suportat per guardar imatges");
                return false;

            case META:
                // TODO A suportar
                DEBUG_LOG("El format META encara no està suportat per guardar imatges");
                return false;
        }
        int count = m_grabList.count();
        if (count == 1)
        {
            // Només grabem una sola imatge
            writer->SetInput(m_grabList.at(0));
            writer->SetFileName(qPrintable(QString("%1.%2").arg(baseName).arg(fileExtension)));
            writer->Write();
        }
        else if (count > 1)
        {
            // Tenim més d'una imatge, per tant li afegim
            // un índex adicional per cada imatge automàticament
            int i = 0;
            int padding = QString::number(count).size();
            foreach (vtkImageData *image, m_grabList)
            {
                writer->SetInput(image);
                writer->SetFileName(qPrintable(QString("%1-%2.%3").arg(baseName).arg(i, padding, 10, QChar('0')).arg(fileExtension)));
                writer->Write();
                i++;
            }
        }
        writer->Delete();
        // Buidem la llista
        clearGrabbedViews();
        return true;
    }
    else
    {
        return false;
    }
}

bool QViewer::record(const QString &baseName, RecordFileFormatType format)
{
    // TODO Restaurar el poder grabar vídeos a Linux.
#if !defined(Q_OS_LINUX)
    if (!m_grabList.empty())
    {
        vtkGenericMovieWriter *videoWriter;
        QString fileExtension;
        // TODO de moment només suportem MPEG2
        switch (format)
        {
            case MPEG2:
                videoWriter = vtkMPEG2Writer::New();
                fileExtension = ".mpg";
                break;
        }

        int count = m_grabList.count();
        // TODO fer alguna cosa especial si només hi ha una sola imatge????

        vtkImageData *data = m_grabList.at(0);

        videoWriter->SetFileName(qPrintable(baseName + fileExtension));
        videoWriter->SetInput(data);
        videoWriter->Start();

        // TODO falta activar el procés de notificació de procés de gravació
        // int progressIncrement = static_cast<int>((1.0/(double)count) * 100);
        // int progress = 0;
        for (int i = 0; i < count; i++)
        {
            videoWriter->SetInput(m_grabList.at(i));

            // TODO Perquè un loop de 3?
            for (int j = 0; j < 3; j++)
            {
                videoWriter->Write();
            }
            //progress += progressIncrement;
            //emit recording(progress);
        }
        videoWriter->End();
        videoWriter->Delete();
        clearGrabbedViews();
        return true;
    }
    else
    {
        return false;
    }
#else
    Q_UNUSED(baseName)
    Q_UNUSED(format)
    return false;
#endif
}

void QViewer::clearGrabbedViews()
{
    foreach (vtkImageData *image, m_grabList)
    {
        image->Delete();
    }
    m_grabList.clear();
}

void QViewer::render()
{
    // ATENCIO És important que només es faci render quan estem en estat VisualizingVolume
    // ja que sinó pot provocar que en alguns casos es presentin problemes de rendering
    // al no obtenir-se el context de rendering openGL adequat
    if (m_isRenderingEnabled && getViewerStatus() == VisualizingVolume)
    {
        this->getRenderWindow()->Render();
    }
}

void QViewer::absoluteZoom(double factor)
{
    double currentFactor = getCurrentZoomFactor();

    zoom(currentFactor / (factor * getRenderWindowSize().height()));
}

double QViewer::getCurrentZoomFactor()
{
    double zoomFactor;

    if (getActiveCamera()->GetParallelProjection())
    {
        zoomFactor = getActiveCamera()->GetParallelScale();
    }
    else
    {
        zoomFactor = getActiveCamera()->GetViewAngle();
    }

    return zoomFactor;
}

void QViewer::zoom(double factor)
{
    if (adjustCameraScaleFactor(factor))
    {
        double zoomFactor = getCurrentZoomFactor();

        emit cameraChanged();
        emit zoomFactorChanged(zoomFactor / getRenderWindowSize().height());
        this->render();
    }
}

void QViewer::pan(double motionVector[3])
{
    if (!this->hasInput())
    {
        return;
    }

    vtkCamera *camera = getActiveCamera();
    if (!camera)
    {
        DEBUG_LOG("No hi ha càmera");
        return;
    }

    double viewFocus[4], viewPoint[3];
    camera->GetFocalPoint(viewFocus);
    camera->GetPosition(viewPoint);
    camera->SetFocalPoint(motionVector[0] + viewFocus[0], motionVector[1] + viewFocus[1], motionVector[2] + viewFocus[2]);
    camera->SetPosition(motionVector[0] + viewPoint[0], motionVector[1] + viewPoint[1], motionVector[2] + viewPoint[2]);

    // Nosaltres en principi no fem ús d'aquesta característica
    if (this->getInteractor()->GetLightFollowCamera())
    {
        vtkRenderer *renderer = getRenderer();
        Q_ASSERT(renderer);
        renderer->UpdateLightsGeometryToFollowCamera();
    }

    double xyz[3];
    getCurrentFocalPoint(xyz);

    emit cameraChanged();
    emit panChanged(xyz);
    this->render();
}

bool QViewer::getCurrentFocalPoint(double focalPoint[3])
{
    vtkCamera *camera = getActiveCamera();
    if (!camera)
    {
        DEBUG_LOG("No hi ha càmera");
        return false;
    }

    camera->GetFocalPoint(focalPoint);

    return true;
}

bool QViewer::scaleToFit3D(double topCorner[3], double bottomCorner[3], double marginRate)
{
    if (!hasInput())
    {
        return false;
    }

    // Calcular la width i height en coordenades de display
    double displayTopLeft[3], displayBottomRight[3];
    this->computeWorldToDisplay(topCorner[0], topCorner[1], topCorner[2], displayTopLeft);
    this->computeWorldToDisplay(bottomCorner[0], bottomCorner[1], bottomCorner[2], displayBottomRight);

    // Recalculem tenint en compte el display
    double width, height;
    width = fabs(displayTopLeft[0] - displayBottomRight[0]);
    height = fabs(displayTopLeft[1] - displayBottomRight[1]);

    // Ajustem la imatge segons si la finestra és més estreta per ample o per alçada. Si volem que es vegi tota la regió que em escollit, ajustarem per el que
    // sigui més estret, si ajustèssim pel més ample perderiem imatge per l'altre part
    QSize size = this->getRenderWindowSize();
    double ratio = qMin(size.width() / width, size.height() / height);
    double factor = ratio * (1.0 - marginRate);
    
    return adjustCameraScaleFactor(factor);
}

void QViewer::fitRenderingIntoViewport()
{
    // First we get the bounds of the current rendered item in world coordinates
    double bounds[6];
    getCurrentRenderedItemBounds(bounds);
    
    double topCorner[3];
    double bottomCorner[3];
    for (int i = 0; i < 3; ++i)
    {
        topCorner[i] = bounds[i * 2];
        bottomCorner[i] = bounds[i * 2 + 1];
    }

    // Scaling the viewport to fit the current item bounds
    if (scaleToFit3D(topCorner, bottomCorner, m_defaultFitIntoViewportMarginRate))
    {
        render();
    }
}

WindowLevelPresetsToolData* QViewer::getWindowLevelData() const
{
    return m_windowLevelData;
}

void QViewer::setWindowLevelData(WindowLevelPresetsToolData *windowLevelData)
{
    if (m_windowLevelData)
    {
        disconnect(m_windowLevelData, 0, this, 0);
        delete m_windowLevelData;
    }

    m_windowLevelData = windowLevelData;
    connect(m_windowLevelData, SIGNAL(currentPresetChanged(WindowLevel)), SLOT(setWindowLevelPreset(WindowLevel)));
    connect(m_windowLevelData, SIGNAL(presetSelected(WindowLevel)), SLOT(setWindowLevelPreset(WindowLevel)));
}

void QViewer::grabCurrentView()
{
    m_windowToImageFilter->Update();
    m_windowToImageFilter->Modified();

    vtkImageData *image = vtkImageData::New();
    image->ShallowCopy(m_windowToImageFilter->GetOutput());
    m_grabList << image;
}

void QViewer::resetView(const OrthogonalPlane &view)
{
    setCameraOrientation(view);
}

void QViewer::resetView(AnatomicalPlane::AnatomicalPlaneType desiredAnatomicalPlane)
{
    if (!hasInput())
    {
        return;
    }

    // HACK Disable rendering temporarily to enhance performance and avoid flickering
    enableRendering(false);

    // First we reset the view to the corresponding orthogonal plane
    const OrthogonalPlane &orthogonalPlane = getMainInput()->getCorrespondingOrthogonalPlane(desiredAnatomicalPlane);
    resetView(orthogonalPlane);
    
    // Then we apply the standard orientation for the desired projection
    setDefaultOrientation(desiredAnatomicalPlane);

    // HACK End of performance hack
    enableRendering(true);
    fitRenderingIntoViewport();

    emit anatomicalViewChanged(desiredAnatomicalPlane);
}

void QViewer::resetViewToAxial()
{
    resetView(AnatomicalPlane::Axial);
}

void QViewer::resetViewToCoronal()
{
    resetView(AnatomicalPlane::Coronal);
}

void QViewer::resetViewToSagital()
{
    resetView(AnatomicalPlane::Sagittal);
}

void QViewer::resetViewToAcquisitionPlane()
{
    resetView(OrthogonalPlane::XYPlane);
    fitRenderingIntoViewport();
}

void QViewer::enableContextMenu()
{
    m_contextMenuActive = true;
}

void QViewer::disableContextMenu()
{
    m_contextMenuActive = false;
}

void QViewer::contextMenuRelease()
{
    // Extret dels exemples de vtkEventQtSlotConnect

    // Obtenim la posició de l'event
    QPoint point = this->getEventPosition();

    // Remember to flip y
    QSize size = this->getRenderWindowSize();
    point.setY(size.height() - point.y());

    // Map to global
    QPoint globalPoint = this->mapToGlobal(point);
    QContextMenuEvent contextMenuEvent(QContextMenuEvent::Mouse, point, globalPoint);
    this->contextMenuEvent(&contextMenuEvent);
}

void QViewer::updateWindowLevelData()
{
    if (!hasInput())
    {
        return;
    }

    WindowLevelHelper().initializeWindowLevelData(m_windowLevelData, getMainInput());
}

void QViewer::setCameraOrientation(const OrthogonalPlane &orientation)
{
    vtkCamera *camera = getActiveCamera();
    if (camera)
    {
        setCameraViewPlane(orientation);
        this->getRenderer()->ResetCamera();
        this->render();
    }
}

void QViewer::setCameraViewPlane(const OrthogonalPlane &viewPlane)
{
    vtkCamera *camera = getActiveCamera();
    if (!camera)
    {
        return;
    }

    this->setCurrentViewPlane(viewPlane);

    // Ajustem els paràmetres de la càmera segons la vista
    camera->SetFocalPoint(0.0, 0.0, 0.0);
    switch (this->getCurrentViewPlane())
    {
        case OrthogonalPlane::XYPlane:
            camera->SetViewUp(0.0, -1.0, 0.0);
            camera->SetPosition(0.0, 0.0, -1.0);
            break;

        case OrthogonalPlane::YZPlane:
            camera->SetViewUp(0.0, 0.0, 1.0);
            camera->SetPosition(1.0, 0.0, 0.0);
            break;

        case OrthogonalPlane::XZPlane:
            camera->SetViewUp(0.0, 0.0, 1.0);
            camera->SetPosition(0.0, -1.0, 0.0);
            break;
    }
}

bool QViewer::adjustCameraScaleFactor(double factor)
{
    if (MathTools::isNaN(factor))
    {
        DEBUG_LOG("Scale factor is NaN. No scale factor will be applied.");
        return false;
    }

    vtkRenderer *renderer = getRenderer();
    if (!renderer)
    {
        DEBUG_LOG("Renderer is NULL. No scale factor will be applied.");
        return false;
    }
    
    getActiveCamera()->Zoom(factor);
    
    if (this->getInteractor()->GetLightFollowCamera())
    {
        renderer->UpdateLightsGeometryToFollowCamera();
    }

    return true;
}

void QViewer::setDefaultOrientation(AnatomicalPlane::AnatomicalPlaneType anatomicalPlane)
{
    Q_UNUSED(anatomicalPlane);
}

QString QViewer::getInputIdentifier() const
{
    QString selectedItem;
    if (hasInput())
    {
        selectedItem = QString::number(getMainInput()->getIdentifier().getValue());
    }

    return selectedItem;
}

void QViewer::contextMenuEvent(QContextMenuEvent *menuEvent)
{
    if (m_contextMenuActive)
    {
        // És possible que en alguns moments (quan es carrega el pacient i surten altres diàlegs)
        // no hi hagi window activa o que aquesta ni sigui una QApplicationMainWindow i ho sigui un diàleg,
        // per tant, ens pot tornar NULL i en algunes ocasions ens feia petar l'aplicació. Així ens curem en salut
        // TODO Estaria bé comprovar
        QApplicationMainWindow *mainWindow = QApplicationMainWindow::getActiveApplicationMainWindow();
        if (!mainWindow)
        {
            return;
        }

        // Li actualitzem l'input perquè mostri els estudis actuals
        m_patientBrowserMenu->setPatient(mainWindow->getCurrentPatient());

        QString selectedItem = getInputIdentifier();
        m_patientBrowserMenu->popup(menuEvent->globalPos(), selectedItem); //->globalPos() ?
    }
}

void QViewer::enableRendering(bool enable)
{
    m_isRenderingEnabled = enable;
}

PatientBrowserMenu* QViewer::getPatientBrowserMenu() const
{
    return m_patientBrowserMenu;
}

void QViewer::setAutomaticallyLoadPatientBrowserMenuSelectedInput(bool load)
{
    if (load)
    {
        connect(m_patientBrowserMenu, SIGNAL(selectedVolume(Volume*)), this, SLOT(setInputAndRender(Volume*)));
    }
    else
    {
        disconnect(m_patientBrowserMenu, SIGNAL(selectedVolume(Volume*)), this, SLOT(setInputAndRender(Volume*)));
    }
}

QViewer::ViewerStatus QViewer::getViewerStatus() const
{
    return m_viewerStatus;
}

QViewer::ViewerStatus QViewer::getPreviousViewerStatus() const
{
    return m_previousViewerStatus;
}

void QViewer::setViewerStatus(ViewerStatus status)
{
    if (m_viewerStatus != status)
    {
        m_previousViewerStatus = m_viewerStatus;
        m_viewerStatus = status;
        this->setCurrentWidgetByViewerStatus(status);
        this->initializeWorkInProgressByViewerStatus(status);
        emit viewerStatusChanged();
    }
}

void QViewer::setCurrentWidgetByViewerStatus(ViewerStatus status)
{
    if (status == NoVolumeInput || status == VisualizingVolume)
    {
        m_stackedLayout->setCurrentWidget(m_vtkWidget);
    }
    else
    {
        m_stackedLayout->setCurrentWidget(m_workInProgressWidget);
    }
}

void QViewer::initializeWorkInProgressByViewerStatus(ViewerStatus status)
{
    m_workInProgressWidget->reset();
    switch (status)
    {
        case NoVolumeInput:
        case VisualizingVolume:
            // Do nothing
            break;
        
        case DownloadingVolume:
            m_workInProgressWidget->setTitle(tr("Downloading related study..."));
            break;
        
        case LoadingVolume:
            m_workInProgressWidget->setTitle(tr("Loading data..."));
            break;
        
        case DownloadingError:
            m_workInProgressWidget->setTitle(tr("Error downloading related study"));
            m_workInProgressWidget->showError(QString());
            break;
        
        case LoadingError:
            m_workInProgressWidget->setTitle(tr("Error loading data"));
            break;
    }
}

void QViewer::setInputAndRender(Volume *volume)
{
    this->setInput(volume);
    this->render();
}

void QViewer::setWindowLevelPreset(const WindowLevel &preset)
{
    if (preset.isValid())
    {
        setWindowLevel(preset.getWidth(), preset.getCenter());
    }
}

OrthogonalPlane QViewer::getCurrentViewPlane() const
{
    return m_currentViewPlane;
}

void QViewer::setCurrentViewPlane(const OrthogonalPlane &viewPlane)
{
    m_currentViewPlane = viewPlane;
}

};  // End namespace udg
