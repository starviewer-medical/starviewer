/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "qviewer.h"
#include "volume.h"
#include "series.h"
#include "image.h"
#include "toolproxy.h"
#include "patientbrowsermenu.h"
#include "windowlevelpresetstooldata.h" // per poder afegir i modificar els presets que visualitzem
#include "transferfunction.h"

//TODO: Ouch! SuperGuarrada (tm). Per poder fer sortir el menú i tenir accés al Patient principal. S'ha d'arreglar en quan es tregui les dependències de interface, pacs, etc.etc.!!
#include "../interface/qapplicationmainwindow.h"

// Qt
#include <QHBoxLayout>
#include <QContextMenuEvent>
#include <QMessageBox>

// include's vtk
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
// necessari pel zoom
#include <vtkCamera.h>
// per grabar el vídeo

#ifndef Q_OS_LINUX
#include <vtkMPEG2Writer.h>
#endif

namespace udg {

QViewer::QViewer(QWidget *parent)
 : QWidget(parent), m_mainVolume(0), m_contextMenuActive(true), m_mouseHasMoved(false), m_windowLevelData(0), m_defaultWindow(.0), m_defaultLevel(.0), m_isRenderingEnabled(true), m_isActive(false), m_hasDefaultWindowLevelDefined(false), m_transferFunction(0)
{
    // TODO: De moment es desactiven els warnings en release i windows perquè no apareixi la finestra vtkOutputWindow
    // però la solució bona és que els viewers no donin warnings.
#ifdef QT_NO_DEBUG
    vtkObject::GlobalWarningDisplayOff();
#endif

    m_vtkWidget = new QVTKWidget(this);
    m_vtkWidget->setFocusPolicy(Qt::WheelFocus);
    // Creem el renderer i li assignem a la render window
    m_renderer = vtkRenderer::New();
    getRenderWindow()->AddRenderer(m_renderer);
    m_renderer->Delete();
    // Forcem 2x buffer
    getRenderWindow()->DoubleBufferOn();

    // Posem a punt el filtre per guardar captures de pantalla
    m_windowToImageFilter = vtkWindowToImageFilter::New();
    m_windowToImageFilter->SetInput(getRenderWindow());

    // Connectem els events
    setupInteraction();

    m_toolProxy = new ToolProxy(this);
    connect(this, SIGNAL(eventReceived(unsigned long)), m_toolProxy, SLOT(forwardEvent(unsigned long)));

    // Inicialitzem el window level data
    setWindowLevelData(new WindowLevelPresetsToolData(this));

    // Afegim el layout
    QHBoxLayout *viewerLayout = new QHBoxLayout(this);
    viewerLayout->setSpacing(0);
    viewerLayout->setMargin(0);
    viewerLayout->addWidget(m_vtkWidget);
    
    this->setMouseTracking(false);
    m_patientBrowserMenu = new PatientBrowserMenu(0);
    // Ara mateix el comportament per defecte serà que un cop seleccionat un volum li assignem immediatament com a input
    // TODO Aquest comportament es podria flexibilitzar proporcionant paràmetres o una interfície per poder
    // escollir altres comportaments que ens poden ser útils en altres contextes, com per exemple, a les extensions
    connect(m_patientBrowserMenu, SIGNAL(selectedVolume(Volume *)), SLOT(setInput(Volume *)));
}

QViewer::~QViewer()
{
    // Cal que la eliminació del vtkWidget sigui al final ja que els altres
    // objectes que eliminem en poden fer ús durant la seva destrucció
    delete m_toolProxy;
    delete m_patientBrowserMenu;
    m_windowToImageFilter->Delete();
    delete m_vtkWidget;
}

vtkRenderWindowInteractor* QViewer::getInteractor()
{
    return m_vtkWidget->GetRenderWindow()->GetInteractor();
}

vtkRenderer* QViewer::getRenderer()
{
    return m_renderer;
}

vtkRenderWindow* QViewer::getRenderWindow()
{
    return m_vtkWidget->GetRenderWindow();
}

int* QViewer::getRenderWindowSize()
{
    return this->getRenderWindow()->GetSize();
}

int* QViewer::getEventPosition()
{
    return this->getInteractor()->GetEventPosition();
}

int* QViewer::getLastEventPosition()
{
    return this->getInteractor()->GetLastEventPosition();
}

void QViewer::getEventPosition(int position[2])
{
    this->getInteractor()->GetEventPosition(position);
}

void QViewer::getLastEventPosition(int position[2])
{
    this->getInteractor()->GetLastEventPosition(position);
}

int QViewer::getEventPositionX()
{
    return this->getInteractor()->GetEventPosition()[0];
}

int QViewer::getEventPositionY()
{
    return this->getInteractor()->GetEventPosition()[1];
}

int QViewer::getLastEventPositionX()
{
    return this->getInteractor()->GetLastEventPosition()[0];
}

int QViewer::getLastEventPositionY()
{
    return this->getInteractor()->GetLastEventPosition()[1];
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
    // Quan la finestra sigui "seleccionada" s'emetrà un senyal indicant-ho. Entenem seleccionada quan s'ha clicat o mogut la rodeta per sobre del visor. \TODO ara resulta ineficient perquè un cop seleccionat no caldria re-enviar aquesta senyal. Cal millorar el sistema
    switch (vtkEvent)
    {
        case QVTKWidget::ContextMenuEvent:
        case vtkCommand::LeftButtonPressEvent:
        case vtkCommand::RightButtonPressEvent:
        case vtkCommand::MiddleButtonPressEvent:
        case vtkCommand::MouseWheelForwardEvent:
        case vtkCommand::MouseWheelBackwardEvent:
            m_mouseHasMoved = false;
            if (!m_isActive)
            {
                emit selected();
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
    m_isActive = active;
}
void QViewer::computeDisplayToWorld(double x, double y, double z, double worldPoint[4])
{
    vtkRenderer *renderer = this->getRenderer();
    if (renderer)
    {
        renderer->SetDisplayPoint(x, y, z);
        renderer->DisplayToWorld();
        renderer->GetWorldPoint(worldPoint);
        if (worldPoint[3])
        {
            worldPoint[0] /= worldPoint[3];
            worldPoint[1] /= worldPoint[3];
            worldPoint[2] /= worldPoint[3];
            worldPoint[3] = 1.0;
        }
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
    int position[2];
    
    if (current)
    {
        this->getEventPosition(position);
    }
    else
    {
        this->getLastEventPosition(position);
    }
    
    double computedCoordinate[4];
    this->computeDisplayToWorld(position[0], position[1], 0, computedCoordinate);
    worldCoordinate[0] = computedCoordinate[0];
    worldCoordinate[1] = computedCoordinate[1];
    worldCoordinate[2] = computedCoordinate[2];
}

void QViewer::setupInteraction()
{
    Q_ASSERT(m_renderer);  

    // \TODO fer això aquí? o fer-ho en el tool manager?
    this->getInteractor()->RemoveObservers(vtkCommand::LeftButtonPressEvent);
    this->getInteractor()->RemoveObservers(vtkCommand::RightButtonPressEvent);
    this->getInteractor()->RemoveObservers(vtkCommand::MouseWheelForwardEvent);
    this->getInteractor()->RemoveObservers(vtkCommand::MouseWheelBackwardEvent);
    this->getInteractor()->RemoveObservers(vtkCommand::MiddleButtonPressEvent);
    this->getInteractor()->RemoveObservers(vtkCommand::CharEvent);

    m_vtkQtConnections = vtkEventQtSlotConnect::New();
    // despatxa qualsevol event-> tools
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

            // \TODO el format tiff fa petar al desar, mirar si és problema de compatibilitat del sistema o de les pròpies vtk
            case TIFF:
                writer = vtkTIFFWriter::New();
                fileExtension = "tif";
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
                // TODO a suportar
                DEBUG_LOG("El format DICOM encara no està suportat per guardar imatges");
                break;

            case META:
                // TODO a suportar
                DEBUG_LOG("El format META encara no està suportat per guardar imatges");
                break;
        }
        int count = m_grabList.count();
        if (count == 1)
        {
            // només grabem una sola imatge
            writer->SetInput(m_grabList.at(0));
            writer->SetFileName(qPrintable(QString("%1.%2").arg(baseName).arg(fileExtension)));
            writer->Write();
        }
        else if (count > 1)
        {
            // tenim més d'una imatge, per tant li afegim 
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
        // buidem la llista
        m_grabList.clear();
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
        switch (format) // TODO de moment només suportem MPEG2
        {
            case MPEG2:
                videoWriter = vtkMPEG2Writer::New();
                fileExtension = ".mpg";
                break;
        }

        int count = m_grabList.count();
        // TODO fer alguna cosa especial si només hi ha una sola imatge????
        
        vtkImageData *data = m_grabList.at(0);

        videoWriter->SetFileName(qPrintable(baseName+fileExtension));
        videoWriter->SetInput(data);
        videoWriter->Start();

        // TODO falta activar el procés de notificació de procés de gravació
        //int progressIncrement = static_cast<int>((1.0/(double)count) * 100);
        //int progress = 0;
        for (unsigned int i = 0; i < count; i++)
        {
            videoWriter->SetInput(m_grabList.at(i));

            for (int j = 0; j < 3; j++) // TODO perquè un loop de 3?
            {
                videoWriter->Write();
            }
            //progress += progressIncrement;
            //emit recording(progress);
        }
        videoWriter->End();
        clearGrabbedViews();
        return true;
    }
    else
    {
        return false;
    }
#else
    return false;
#endif
}

void QViewer::render()
{
    if (m_isRenderingEnabled)
    {
        this->getRenderWindow()->Render();
    }
}

void QViewer::zoom(double factor)
{
    // TODO potser caldria una comprovació de seguretat pel que torna cadascuna d'aquestes crides
    vtkRenderer *renderer = getRenderer();
    if (renderer)
    {
        // codi extret de void vtkInteractorStyleTrackballCamera::Dolly(double factor)
        vtkCamera *camera = getActiveCamera();
        if (camera->GetParallelProjection())
        {
            camera->SetParallelScale(camera->GetParallelScale() / factor);
        }
        else
        {
            camera->Dolly(factor);
            //if (vtkInteractorStyle::SafeDownCast(this->getInteractor()->GetInteractorStyle())->GetAutoAdjustCameraClippingRange())
            //{
                // TODO en principi sempre ens interessarà fer això? ens podriem enstalviar l'if??
                renderer->ResetCameraClippingRange();
            //}
        }
        if (this->getInteractor()->GetLightFollowCamera())
        {
            renderer->UpdateLightsGeometryToFollowCamera();
        }

        emit cameraChanged();
        emit zoomFactorChanged(factor);
        this->render();
    }
    else
    {
        DEBUG_LOG("::zoom(double factor): El renderer és NUL!");
    }
}

void QViewer::pan(double motionVector[3])
{
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

    emit cameraChanged();
    emit panChanged(motionVector);
    this->render();
}

void QViewer::scaleToFit3D(double topCorner[3], double bottomCorner[3], double marginRate)
{
    if (!m_mainVolume)
    {
        return;
    }

    // Calcular la width i height en coordenades de display
    double displayTopLeft[3], displayBottomRight[3];
    this->computeWorldToDisplay(topCorner[0], topCorner[1], topCorner[2], displayTopLeft);
    this->computeWorldToDisplay(bottomCorner[0], bottomCorner[1], bottomCorner[2], displayBottomRight);

    // recalculem tenint en compte el display
    double width, height;
    width = fabs(displayTopLeft[0] - displayBottomRight[0]);
    height = fabs(displayTopLeft[1] - displayBottomRight[1]);

    // Ajustem la imatge segons si la finestra és més estreta per ample o per alçada. Si volem que es vegi tota la regió que em escollit, ajustarem per el que sigui més estret, si ajustèssim pel més ample perderiem imatge per l'altre part
    int *size = this->getRenderWindowSize();
    if ((width / size[0]) > (height / size[1]))
    {
        this->zoom((size[0] / (float)width) * (1.0 - marginRate));
    }
    else
    {
        this->zoom((size[1] / (float)height) * (1.0 - marginRate));
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
        disconnect(m_windowLevelData,0,this,0);
        delete m_windowLevelData;
    }

    m_windowLevelData = windowLevelData;
    connect(m_windowLevelData, SIGNAL(currentWindowLevel(double,double)), SLOT(setWindowLevel(double,double)));
}
void QViewer::grabCurrentView()
{
    m_windowToImageFilter->Update();
    m_windowToImageFilter->Modified();

    vtkImageData *image = vtkImageData::New();
    image->ShallowCopy(m_windowToImageFilter->GetOutput());
    m_grabList << image;
}

void QViewer::getDefaultWindowLevel(double windowLevel[2])
{
    if (!m_hasDefaultWindowLevelDefined)
    {
        if (m_mainVolume)
        {
            double *range = m_mainVolume->getVtkData()->GetScalarRange();
            this->setDefaultWindowLevel(range[1] - range[0], (range[1] / 2.) + range[0]);
        }
        else
        {
            DEBUG_LOG("QViewer::getDefaultWindowLevel() : No tenim input");
        }
    }
    windowLevel[0] = m_defaultWindow;
    windowLevel[1] = m_defaultLevel;
}

void QViewer::enableContextMenu()
{
    m_contextMenuActive = true;
}

void QViewer::disableContextMenu()
{
    m_contextMenuActive = false;
}

void QViewer::setDefaultWindowLevel(double window, double level)
{
    m_defaultWindow = window;
    m_defaultLevel = level;
    m_hasDefaultWindowLevelDefined = true;
}

TransferFunction* QViewer::getTransferFunction() const
{
    return m_transferFunction;
}

void QViewer::resetWindowLevelToDefault()
{
    this->setWindowLevel(m_defaultWindow, m_defaultLevel);
}

void QViewer::contextMenuRelease()
{
    // Extret dels exemples de vtkEventQtSlotConnect

    // Obtenim la posició de l'event
    int eventPositionX = this->getEventPositionX();
    int eventPositionY = this->getEventPositionY();

    int *size = this->getRenderWindowSize();
    // remember to flip y
    QPoint point = QPoint(eventPositionX, size[1]-eventPositionY);

    // map to global
    QPoint globalPoint = this->mapToGlobal(point);
//     emit showContextMenu(globalPoint);
    this->contextMenuEvent(new QContextMenuEvent(QContextMenuEvent::Mouse, point, globalPoint));
}

void QViewer::updateWindowLevelData()
{
    if (!m_mainVolume)
    {
        return;
    }

    const QString DefaultWindowLevelName = tr("Default");

    m_windowLevelData->removePresetsFromGroup(WindowLevelPresetsToolData::FileDefined);

    // agafem el window level de la imatge central per evitar problemes
    // de que tinguem diferents windows levels a cada imatge i el de la
    // primera imatge sigui massa diferent a la resta. No deixa de ser un hack cutre.
    int index = m_mainVolume->getNumberOfSlicesPerPhase() / 2;

    int windowLevelCount = 0;
    Image *image = m_mainVolume->getImage(index);
    if (image)
    {
        windowLevelCount = image->getNumberOfWindowLevels();
    }
    else
    {
        DEBUG_LOG(QString("Índex [%1] fora de rang. No s'ha pogut obtenir la imatge indicada del volum actual.").arg(index));
    }

    if (windowLevelCount > 0)
    {
        for (int i = 0; i < windowLevelCount; i++)
        {
            QPair<double, double> windowLevel = image->getWindowLevel(i);
            QString description = image->getWindowLevelExplanation(i);
            if (description.isEmpty())
            {
                description = tr("Default %1").arg(i);
            }

            if (windowLevel.first == 0.0 || windowLevel.second == 0.0)
            {
                double windowLevel[2];
                this->getDefaultWindowLevel(windowLevel);
                m_windowLevelData->addPreset(description, windowLevel[0], windowLevel[1], WindowLevelPresetsToolData::FileDefined);
            }
            else
            {
                m_windowLevelData->addPreset(description, windowLevel.first, windowLevel.second, WindowLevelPresetsToolData::FileDefined);
            }
            if (i == 0)
            {
                m_windowLevelData->activatePreset(description);
            }
        }
    }
    else // no n'hi ha de definits al volum, agafem el que ens doni el viewer
    {
        double windowLevel[2];
        this->getDefaultWindowLevel(windowLevel);
        m_windowLevelData->addPreset(DefaultWindowLevelName, windowLevel[0], windowLevel[1], WindowLevelPresetsToolData::FileDefined);
        m_windowLevelData->activatePreset(DefaultWindowLevelName);
    }
}

void QViewer::setCameraOrientation(int orientation)
{
    vtkCamera *camera = getActiveCamera();
    if (camera)
    {
        switch (orientation)
        {
            case Axial:
                camera->SetFocalPoint(0,0,0);
                camera->SetPosition(0,0,-1); // -1 if medical ?
                camera->SetViewUp(0,-1,0);
                break;

            case Coronal:
                camera->SetFocalPoint(0,0,0);
                camera->SetPosition(0,-1,0); // 1 if medical ?
                camera->SetViewUp(0,0,1);
                break;

            case Sagital:
                camera->SetFocalPoint(0,0,0);
                camera->SetPosition(1,0,0); // -1 if medical ?
                camera->SetViewUp(0,0,1);
                break;
        }
        this->getRenderer()->ResetCamera();
        this->render();
    }
}

void QViewer::contextMenuEvent(QContextMenuEvent *event)
{
    if (m_contextMenuActive)
    {
        // És possible que en alguns moments (quan es carrega el pacient i surten altres diàlegs)
        // no hi hagi window activa o que aquesta ni sigui una QApplicationMainWindow i ho sigui un diàleg,
        // per tant, ens pot tornar NULL i en algunes ocasions ens feia petar l'aplicació. Així ens curem en salut
        // TODO estaria bé comprovar
        QApplicationMainWindow *mainWindow = QApplicationMainWindow::getActiveApplicationMainWindow();
        if (!mainWindow)
        {
            return;
        }

        // Li actualitzem l'input perquè mostri els estudis actuals
        m_patientBrowserMenu->setPatient(mainWindow->getCurrentPatient());

        QString selectedItem;
        if (m_mainVolume)
        {
            selectedItem = QString::number(m_mainVolume->getIdentifier().getValue());
        }
        m_patientBrowserMenu->popup(event->globalPos(), selectedItem); //->globalPos() ?
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

};  // end namespace udg
