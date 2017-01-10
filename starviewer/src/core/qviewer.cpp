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

#include "qviewer.h"
#include "volume.h"
#include "series.h"
#include "image.h"
#include "toolproxy.h"
#include "patientbrowsermenu.h"
// Per poder afegir i modificar els presets que visualitzem
#include "voilutpresetstooldata.h"
#include "qviewerworkinprogresswidget.h"
#include "voiluthelper.h"
#include "logging.h"
#include "mathtools.h"
#include "starviewerapplication.h"
#include "coresettings.h"

// TODO: Ouch! SuperGuarrada (tm). Per poder fer sortir el menú i tenir accés al Patient principal. S'ha d'arreglar en quan es tregui les dependències de
// interface, pacs, etc.etc.!!
#include "../interface/qapplicationmainwindow.h"

// Qt
#include <QStackedLayout>
#include <QContextMenuEvent>
#include <QMessageBox>
#include <QDir>
#include <QScreen>

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

namespace udg {

QViewer::QViewer(QWidget *parent)
 : QWidget(parent), m_mainVolume(0), m_contextMenuActive(true), m_mouseHasMoved(false), m_voiLutData(0),
   m_isRenderingEnabled(true), m_isActive(false)
{
    m_defaultFitIntoViewportMarginRate = 0.0;
    m_vtkWidget = new QVTKWidget(this);
    m_vtkWidget->setFocusPolicy(Qt::WheelFocus);
    m_renderer = vtkRenderer::New();

    m_windowToImageFilter = vtkWindowToImageFilter::New();

    setupRenderWindow();

    this->setCurrentViewPlane(OrthogonalPlane::XYPlane);

    // Connectem els events
    setupInteraction();

    m_toolProxy = new ToolProxy(this);
    connect(this, SIGNAL(eventReceived(unsigned long)), m_toolProxy, SLOT(forwardEvent(unsigned long)));

    // Inicialitzem el window level data
    setVoiLutData(new VoiLutPresetsToolData(this));

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
    m_patientBrowserMenu->deleteLater();
    m_windowToImageFilter->Delete();
    delete m_vtkWidget;
    m_vtkQtConnections->Delete();
    m_renderer->Delete();
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
    QPoint point(this->getInteractor()->GetEventPosition()[0], this->getInteractor()->GetEventPosition()[1]);
    point *= this->devicePixelRatioF();
    return point;

}

QPoint QViewer::getLastEventPosition() const
{
    QPoint point(this->getInteractor()->GetLastEventPosition()[0], this->getInteractor()->GetLastEventPosition()[1]);
    point *= this->devicePixelRatioF();
    return point;
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

#ifdef Q_OS_OSX
    // MouseWheel doesn't work as expected when using a trackpad on Mac because a MouseWheel event is emitted for both vertical and horizontal
    // orientation movements. Only vertical events with a delta different to 0 are captured.
    switch (vtkEvent)
    {
        case vtkCommand::MouseWheelForwardEvent:
        case vtkCommand::MouseWheelBackwardEvent:
        {
            QWheelEvent *e = (QWheelEvent*)callData;
            if (e)
            {
                if (e->delta() == 0 || e->orientation() == Qt::Horizontal)
                {
                    return;
                }
            }
        }
    }
#endif

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
            if (vtkEvent == vtkCommand::LeftButtonPressEvent && getInteractor()->GetRepeatCount() == 1)
            {
                emit doubleClicked();
            }
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

Vector3 QViewer::computeDisplayToWorld(const Vector3 &displayPoint)
{
    vtkRenderer *renderer = this->getRenderer();
    if (renderer)
    {
        double homogeneousWorldPoint[4];
        renderer->SetDisplayPoint(displayPoint.x, displayPoint.y, displayPoint.z);
        renderer->DisplayToWorld();
        renderer->GetWorldPoint(homogeneousWorldPoint);
        
        double divisor = 1.0;
        if (homogeneousWorldPoint[3] != 0.0)
        {
            divisor = homogeneousWorldPoint[3];
        }

        return Vector3(homogeneousWorldPoint) / divisor;
    }
    else
    {
        return displayPoint;
    }
}

Vector3 QViewer::computeWorldToDisplay(const Vector3 &worldPoint)
{
    vtkRenderer *renderer = this->getRenderer();
    if (renderer)
    {
        renderer->SetWorldPoint(worldPoint.x, worldPoint.y, worldPoint.z, 1.0);
        renderer->WorldToDisplay();
        return Vector3(renderer->GetDisplayPoint());
    }
    else
    {
        return worldPoint;
    }
}

Vector3 QViewer::getEventWorldCoordinate()
{
    QPoint position = this->getEventPosition();
    return this->computeDisplayToWorld(Vector3(position.x(), position.y(), 0));
}

Vector3 QViewer::getLastEventWorldCoordinate()
{
    QPoint position = this->getLastEventPosition();
    return this->computeDisplayToWorld(Vector3(position.x(), position.y(), 0));
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
            writer->SetInputData(m_grabList.at(0));
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
                writer->SetInputData(image);
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
        try
        {
            this->getRenderWindow()->Render();
        }
        catch (const std::bad_alloc &ba)
        {
            WARN_LOG(QString("bad_alloc when trying to render: ").arg(ba.what()));
            handleNotEnoughMemoryForVisualizationError();
        }
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

void QViewer::pan(const Vector3 &motionVector)
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

    Vector3 viewFocus = camera->GetFocalPoint();
    Vector3 viewPoint = camera->GetPosition();
    camera->SetFocalPoint((viewFocus + motionVector).data());
    camera->SetPosition((viewPoint + motionVector).data());

    // Nosaltres en principi no fem ús d'aquesta característica
    if (this->getInteractor()->GetLightFollowCamera())
    {
        vtkRenderer *renderer = getRenderer();
        Q_ASSERT(renderer);
        renderer->UpdateLightsGeometryToFollowCamera();
    }

    emit cameraChanged();
    emit panChanged(getCurrentFocalPoint());
    this->render();
}

Vector3 QViewer::getCurrentFocalPoint()
{
    vtkCamera *camera = getActiveCamera();
    if (camera)
    {
        return camera->GetFocalPoint();
    }
    else
    {
        DEBUG_LOG("No hi ha càmera");
        return Vector3();
    }
}

Vector3 QViewer::getDirectionOfProjection()
{
    if (vtkCamera *camera = getActiveCamera())
    {
        return Vector3(camera->GetDirectionOfProjection());
    }
    else
    {
        DEBUG_LOG("No active camera");
        return Vector3();
    }
}

VoiLut QViewer::getCurrentVoiLut() const
{
    return VoiLut();
}

bool QViewer::scaleToFit3D(double topCorner[3], double bottomCorner[3], double marginRate)
{
    if (!hasInput())
    {
        return false;
    }

    // Calcular la width i height en coordenades de display
    Vector3 displayTopLeft = this->computeWorldToDisplay(topCorner);
    Vector3 displayBottomRight = this->computeWorldToDisplay(bottomCorner);

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

VoiLutPresetsToolData* QViewer::getVoiLutData() const
{
    return m_voiLutData;
}

void QViewer::setVoiLutData(VoiLutPresetsToolData *voiLutData)
{
    if (m_voiLutData)
    {
        disconnect(m_voiLutData, 0, this, 0);
        delete m_voiLutData;
    }

    m_voiLutData = voiLutData;
    connect(m_voiLutData, SIGNAL(currentPresetChanged(VoiLut)), SLOT(setVoiLut(VoiLut)));
    connect(m_voiLutData, SIGNAL(presetSelected(VoiLut)), SLOT(setVoiLut(VoiLut)));
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

void QViewer::resetView(const AnatomicalPlane &desiredAnatomicalPlane)
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
    point /= this->devicePixelRatioF(); // Vtk pixels are real pixels, Qt wants logical pixels.

    // Remember to flip y
    QSize size = this->getRenderWindowSize();
    size /= this->devicePixelRatioF(); // Vtk pixels are real pixels, Qt wants logical pixels.
    point.setY(size.height() - point.y());



    // Map to global
    QPoint globalPoint = this->mapToGlobal(point);
    QContextMenuEvent contextMenuEvent(QContextMenuEvent::Mouse, point, globalPoint);
    this->contextMenuEvent(&contextMenuEvent);
}

void QViewer::updateVoiLutData()
{
    if (!hasInput())
    {
        return;
    }

    VoiLutHelper().initializeVoiLutData(m_voiLutData, getMainInput());
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

void QViewer::setDefaultOrientation(const AnatomicalPlane &anatomicalPlane)
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

        case VisualizingError:
            m_workInProgressWidget->setTitle(tr("Error visualizing data"));
            break;
    }
}

void QViewer::setInputAndRender(Volume *volume)
{
    this->setInput(volume);
    this->render();
}

void QViewer::setVoiLut(const VoiLut &voiLut)
{
    Q_UNUSED(voiLut)
}

OrthogonalPlane QViewer::getCurrentViewPlane() const
{
    return m_currentViewPlane;
}

void QViewer::setCurrentViewPlane(const OrthogonalPlane &viewPlane)
{
    m_currentViewPlane = viewPlane;
}

void QViewer::handleNotEnoughMemoryForVisualizationError()
{
    setViewerStatus(VisualizingError);
    m_workInProgressWidget->showError(tr("There's not enough memory for the rendering process. Try to close all the open %1 windows, restart the application "
        "and try again. If the problem persists, adding more RAM memory or switching to a 64-bit operating system may solve the problem.")
        .arg(ApplicationNameString));
    // The cursor may have been changed by a tool that hasn't finished its operation and won't receive a mouse button release event,
    // thus the cursor is reset to its default form here
    // TODO Tools should be able to handle this situation by themselves
    unsetCursor();
    // In case of error during rendering the render window is left unusable, so we must create a new one
    setupRenderWindow();
}

void QViewer::setupRenderWindow()
{
    vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
    // TODO getInteractor() forces m_vtkWiget to create a render window the first time just to return the interactor, that render window is unused afterwards.
    //      Could this be improved?
    renderWindow->SetInteractor(getInteractor());
    renderWindow->AddRenderer(getRenderer());
    renderWindow->DoubleBufferOn();
    renderWindow->LineSmoothingOn();
    renderWindow->SetDPI(QGuiApplication::primaryScreen()->logicalDotsPerInch());

    if (!Settings().getValue(CoreSettings::DontForceMultiSampling).toBool())
    {
        // This is the default of VTK except on Mac due to some alleged problems in some models, and is needed to get smooth lines
        // The setting will allow to avoid those problems if they arise, at the cost of getting aliased lines
        renderWindow->SetMultiSamples(8);
    }

    // TODO This is needed for the rendering process to work correctly if coming from handleNotEnoughMemoryForVisualizationError().
    //      Alternatively the rendering process also works correctly after a Q2DViewer::restore().
    //      Why?
    getRenderWindow()->RemoveRenderer(getRenderer());

    m_vtkWidget->SetRenderWindow(renderWindow);
    m_windowToImageFilter->SetInput(renderWindow);
}

};  // End namespace udg
