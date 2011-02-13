/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "qmprextension.h"

#include "drawer.h"
#include "logging.h"
#include "mathtools.h"  // Per càlculs d'interseccions
#include "mprsettings.h"
#include "patientbrowsermenu.h"
#include "q3dviewer.h"
#include "qexportertool.h"
#include "screenshottool.h"
#include "toolmanager.h"
#include "toolproxy.h"
#include "volume.h"
#include "windowlevelpresetstooldata.h"
// Qt
#include <QMessageBox>
// VTK
#include <vtkAxisActor2D.h>
#include <vtkCommand.h> // Pels events
#include <vtkImageChangeInformation.h>  // Per portar a l'origen
#include <vtkImageReslice.h>
#include <vtkPlaneSource.h>
#include <vtkProperty2D.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTransform.h>

namespace udg {

const double QMPRExtension::PickingDistanceThreshold = 7.0;

QMPRExtension::QMPRExtension(QWidget *parent)
 : QWidget(parent), m_axialZeroSliceCoordinate(.0)
{
    setupUi(this);
    MPRSettings().init();

    init();
    createActions();
    createConnections();
    createActors();
    readSettings();
    // Ajustaments de window level pel combo box
    m_windowLevelComboBox->setPresetsData(m_axial2DView->getWindowLevelData());
    m_sagital2DView->setWindowLevelData(m_axial2DView->getWindowLevelData());
    m_coronal2DView->setWindowLevelData(m_axial2DView->getWindowLevelData());
    m_windowLevelComboBox->selectPreset(m_axial2DView->getWindowLevelData()->getCurrentPreset());

    initializeTools();

    m_thickSlab = 0.0;

    // TODO Ocultem botons que no son del tot necessaris o que no es faran servir
    // Amb "visible" posem a l'abast o no el MIP 3D
    bool visible = false;
    m_thickSlabLabel->setVisible(visible);
    m_thickSlabSlider->setVisible(visible);
    m_thickSlabSpinBox->setVisible(visible);
    m_mipToolButton->setVisible(visible);
    // Aquests els amaguem per guanyar espai
    m_windowLevelToolButton->setVisible(false);
    m_moveToolButton->setVisible(false);

    m_screenshotsExporterToolButton->setToolTip(tr("Export viewer image(s) to DICOM and send them to a PACS server"));
    m_viewerInformationToolButton->setToolTip(tr("Show/Hide viewer's textual information"));
    m_windowLevelComboBox->setToolTip(tr("Choose Window/Level Presets"));
}

QMPRExtension::~QMPRExtension()
{
    writeSettings();
    // Fent això o no sembla que s'allibera la mateixa memòria gràcies als smart pointers
    m_sagitalReslice->Delete();
    m_coronalReslice->Delete();

    m_transform->Delete();

    m_sagitalOverAxialAxisActor->Delete();
    m_axialOverSagitalIntersectionAxis->Delete();
    m_coronalOverAxialIntersectionAxis->Delete();
    m_coronalOverSagitalIntersectionAxis->Delete();
    m_thickSlabOverAxialActor->Delete();
    m_thickSlabOverSagitalActor->Delete();

    m_axialPlaneSource->Delete();
    m_sagitalPlaneSource->Delete();
    m_coronalPlaneSource->Delete();
    m_thickSlabPlaneSource->Delete();

    if (m_pickedActorReslice)
    {
        m_pickedActorReslice->Delete();
    }

    if (m_mipViewer)
    {
        delete m_mipViewer;
    }
    delete m_coronal2DView;
}

void QMPRExtension::init()
{
    m_axialPlaneSource = vtkPlaneSource::New();
    m_axialPlaneSource->SetXResolution(1); // Així estan configurats a vtkImagePlaneWidget
    m_axialPlaneSource->SetYResolution(1);

    m_sagitalPlaneSource = vtkPlaneSource::New();
    m_sagitalPlaneSource->SetXResolution(1);
    m_sagitalPlaneSource->SetYResolution(1);

    m_coronalPlaneSource = vtkPlaneSource::New();
    m_coronalPlaneSource->SetXResolution(1);
    m_coronalPlaneSource->SetYResolution(1);

    m_thickSlabPlaneSource = vtkPlaneSource::New();
    m_thickSlabPlaneSource->SetXResolution(1);
    m_thickSlabPlaneSource->SetYResolution(1);

    m_sagitalReslice = vtkImageReslice::New();
    m_sagitalReslice->AutoCropOutputOn(); // Perquè l'extent d'output sigui suficient i no es "mengi" dades
    m_sagitalReslice->SetInterpolationModeToCubic();

    m_coronalReslice = vtkImageReslice::New();
    m_coronalReslice->AutoCropOutputOn();
    m_coronalReslice->SetInterpolationModeToCubic();

    // Configurem les annotacions que volem veure
    m_sagital2DView->removeAnnotation(Q2DViewer::PatientOrientationAnnotation | Q2DViewer::PatientInformationAnnotation | Q2DViewer::SliceAnnotation );
    m_coronal2DView->removeAnnotation(Q2DViewer::PatientOrientationAnnotation | Q2DViewer::PatientInformationAnnotation | Q2DViewer::SliceAnnotation);
    showViewerInformation(m_viewerInformationToolButton->isChecked());

    m_sagital2DView->disableContextMenu();
    m_coronal2DView->disableContextMenu();

    // Per defecte isomètric
    m_axialSpacing[0] = 1.;
    m_axialSpacing[1] = 1.;
    m_axialSpacing[2] = 1.;

    m_state = None;

    m_transform = vtkTransform::New();

    m_pickedActorPlaneSource = 0;
    m_pickedActorReslice = 0;
    m_mipViewer = 0;

    m_extensionToolsList << "ZoomTool" << "SlicingTool" << "TranslateTool" << "VoxelInformationTool" << "WindowLevelTool" << "ScreenShotTool" << "DistanceTool" << "PolylineROITool" << "EraserTool";
}

void QMPRExtension::createActions()
{
    m_horizontalLayoutAction = new QAction(0);
    m_horizontalLayoutAction->setText(tr("Switch horizontal layout"));
    m_horizontalLayoutAction->setStatusTip(tr("Switch horizontal layout"));
    m_horizontalLayoutAction->setIcon(QIcon(":/images/view_left_right.png"));
    m_horizontalLayoutToolButton->setDefaultAction(m_horizontalLayoutAction);

    m_mipAction = new QAction(0);
    m_mipAction->setText(tr("&MIP"));
    m_mipAction->setShortcut(tr("Ctrl+M"));
    m_mipAction->setStatusTip(tr("Maximum Intensity Projection"));
    m_mipAction->setIcon(QIcon(":/images/mip.png"));
    m_mipAction->setCheckable(true);
    m_mipToolButton->setDefaultAction(m_mipAction);
}

void QMPRExtension::initializeTools()
{
    // Creem el tool manager
    m_toolManager = new ToolManager(this);
    // Obtenim les accions de cada tool que volem
    m_zoomToolButton->setDefaultAction(m_toolManager->registerTool("ZoomTool"));
    m_slicingToolButton->setDefaultAction(m_toolManager->registerTool("SlicingTool"));
    m_moveToolButton->setDefaultAction(m_toolManager->registerTool("TranslateTool"));
    m_windowLevelToolButton->setDefaultAction(m_toolManager->registerTool("WindowLevelTool"));
    m_voxelInformationToolButton->setDefaultAction(m_toolManager->registerTool("VoxelInformationTool"));
    m_toolManager->registerTool("ScreenShotTool");
    m_screenShotToolButton->setToolTip(m_toolManager->getRegisteredToolAction("ScreenShotTool")->toolTip());
    m_distanceToolButton->setDefaultAction(m_toolManager->registerTool("DistanceTool"));
    m_polylineROIToolButton->setDefaultAction(m_toolManager->registerTool("PolylineROITool"));
    m_angleToolButton->setDefaultAction(m_toolManager->registerTool("AngleTool"));
    m_openAngleToolButton->setDefaultAction(m_toolManager->registerTool("NonClosedAngleTool"));
    m_eraserToolButton->setDefaultAction(m_toolManager->registerTool("EraserTool"));
    m_toolManager->registerTool("WindowLevelPresetsTool");
    m_toolManager->registerTool("SlicingKeyboardTool");

    // Definim els grups exclusius
    QStringList leftButtonExclusiveTools;
    leftButtonExclusiveTools << "ZoomTool" << "SlicingTool" << "DistanceTool" << "PolylineROITool" << "EraserTool" << "AngleTool" << "NonClosedAngleTool";
    m_toolManager->addExclusiveToolsGroup("LeftButtonGroup", leftButtonExclusiveTools);

    QStringList middleButtonExclusiveTools;
    middleButtonExclusiveTools << "TranslateTool";
    m_toolManager->addExclusiveToolsGroup("MiddleButtonGroup", middleButtonExclusiveTools);

    QStringList rightButtonExclusiveTools;
    rightButtonExclusiveTools << "WindowLevelTool";
    m_toolManager->addExclusiveToolsGroup("RighttButtonGroup", rightButtonExclusiveTools);

    // Activem les tools que volem tenir per defecte, això és com si clickéssim a cadascun dels ToolButton
    QStringList defaultTools;
    defaultTools << "WindowLevelPresetsTool" << "SlicingTool" << "WindowLevelTool" << "TranslateTool" << "ScreenShotTool" << "SlicingKeyboardTool";
    m_toolManager->triggerTools(defaultTools);

    // Registrem al manager les tools que van als diferents viewers
    m_toolManager->setupRegisteredTools(m_axial2DView);
    // Als altres visors volem totes les registrades, excepte l'slicing
    QStringList toolsList = m_toolManager->getRegisteredToolsList();
    toolsList.removeAt(toolsList.indexOf("SlicingTool"));
    m_toolManager->setViewerTools(m_sagital2DView, toolsList);
    m_toolManager->setViewerTools(m_coronal2DView, toolsList);
}

void QMPRExtension::createConnections()
{
    // Conectem els sliders i demés visors
    connect(m_axial2DView, SIGNAL(sliceChanged(int)), m_axialSlider, SLOT(setValue(int)));
    connect(m_axialSlider, SIGNAL(valueChanged(int)), m_axial2DView, SLOT(setSlice(int)));

    connect(m_axial2DView, SIGNAL(sliceChanged(int)), SLOT(axialSliceUpdated(int)));
    connect(m_axial2DView, SIGNAL(sliceChanged(int)), SLOT(updateProjectionLabel()));

    // Gestionen els events de les finestres per poder manipular els plans
    connect(m_axial2DView, SIGNAL(eventReceived(unsigned long)), SLOT(handleAxialViewEvents(unsigned long)));
    connect(m_sagital2DView, SIGNAL(eventReceived(unsigned long)), SLOT(handleSagitalViewEvents(unsigned long)));

    connect(m_thickSlabSpinBox, SIGNAL(valueChanged(double)), SLOT(updateThickSlab(double)));
    connect(m_thickSlabSlider, SIGNAL(valueChanged(int)), SLOT(updateThickSlab(int)));

    // Layouts
    connect(m_horizontalLayoutAction, SIGNAL(triggered()), SLOT(switchHorizontalLayout()));
    connect(m_mipAction, SIGNAL(triggered(bool)), SLOT(switchToMIPLayout(bool)));

    // Fem que no s'assigni automàticament l'input que s'ha seleccionat amb el menú de pacient, ja que fem tractaments adicionals
    // sobre el volum seleccionat i l'input final del visor pot diferir de l'inicial i és l'extensió qui decideix finalment quin input
    // se li vol donar a cada viewer. Capturem la senyal de quin volum s'ha escollit i a partir d'aquí fem el que calgui
    m_axial2DView->setAutomaticallyLoadPatientBrowserMenuSelectedInput(false);
    connect(m_axial2DView->getPatientBrowserMenu(), SIGNAL(selectedVolume(Volume *)), SLOT(setInput(Volume *)));

    // Mostrar o no la informacio del volum a cada visualitzador
    connect(m_viewerInformationToolButton, SIGNAL(toggled(bool)), SLOT(showViewerInformation(bool)));

    // HACK Per poder fer servir l'eina d'screenshot amb el botonet
    connect(m_axial2DView, SIGNAL(selected()), SLOT(changeSelectedViewer()));
    connect(m_sagital2DView, SIGNAL(selected()), SLOT(changeSelectedViewer()));
    connect(m_coronal2DView, SIGNAL(selected()), SLOT(changeSelectedViewer()));
    connect(m_screenShotToolButton, SIGNAL(clicked()), SLOT(screenShot()));

    // Per mostrar exportació
    connect(m_screenshotsExporterToolButton, SIGNAL(clicked()), SLOT(showScreenshotsExporterDialog()));
}

void QMPRExtension::changeSelectedViewer()
{
    if (this->sender() == m_axial2DView)
    {
        m_axial2DView->setActive(true);
        
        m_sagital2DView->setActive(false);
        m_coronal2DView->setActive(false);
    }
    else if (this->sender() == m_sagital2DView)
    {
        m_sagital2DView->setActive(true);
        
        m_axial2DView->setActive(false);
        m_coronal2DView->setActive(false);
    }
    else if (this->sender() == m_coronal2DView)
    {
        m_coronal2DView->setActive(true);
        
        m_axial2DView->setActive(false);
        m_sagital2DView->setActive(false);
    }
}

void QMPRExtension::screenShot()
{
    ScreenShotTool *screenShotTool = 0;
    if (m_axial2DView->isActive())
    {
        screenShotTool = dynamic_cast<ScreenShotTool *>(m_axial2DView->getToolProxy()->getTool("ScreenShotTool"));
    }
    else if (m_sagital2DView->isActive())
    {
        screenShotTool = dynamic_cast<ScreenShotTool *>(m_sagital2DView->getToolProxy()->getTool("ScreenShotTool"));
    }
    else if (m_coronal2DView->isActive())
    {
        screenShotTool = dynamic_cast<ScreenShotTool *>(m_coronal2DView->getToolProxy()->getTool("ScreenShotTool"));
    }

    if (screenShotTool)
    {
        screenShotTool->singleCapture();
    }
    else
    {
        DEBUG_LOG("No hi ha tool d'screenshot disponible");
    }
}

void QMPRExtension::showScreenshotsExporterDialog()
{
    Q2DViewer * viewer = 0;
    if (m_axial2DView->isActive())
    {
        viewer = m_axial2DView;
    }
    else if (m_sagital2DView->isActive())
    {
        viewer = m_sagital2DView;
    }
    else if (m_coronal2DView->isActive())
    {
        viewer = m_coronal2DView;
    }

    if (viewer)
    {
        QExporterTool exporter(viewer);
        exporter.exec();
    }
    else
    {
        QMessageBox::warning(this, tr("Export to DICOM"), tr("Please, select a viewer and try again."));
    }
}

void QMPRExtension::showViewerInformation(bool show)
{
    m_axial2DView->enableAnnotation(Q2DViewer::WindowInformationAnnotation | Q2DViewer::PatientOrientationAnnotation | Q2DViewer::SliceAnnotation | Q2DViewer::PatientInformationAnnotation | Q2DViewer::AcquisitionInformationAnnotation, show);
    m_sagital2DView->enableAnnotation(Q2DViewer::WindowInformationAnnotation | Q2DViewer::AcquisitionInformationAnnotation, show);
    m_coronal2DView->enableAnnotation(Q2DViewer::WindowInformationAnnotation | Q2DViewer::AcquisitionInformationAnnotation, show);
}

void QMPRExtension::updateProjectionLabel()
{
    m_projectionLabel->setText(m_axial2DView->getCurrentPlaneProjectionLabel());
}

void QMPRExtension::switchHorizontalLayout()
{
    QWidget *leftWidget, *rightWidget;
    leftWidget = m_horizontalSplitter->widget(0);
    rightWidget = m_horizontalSplitter->widget(1);

    m_horizontalSplitter->insertWidget(0, rightWidget);
    m_horizontalSplitter->insertWidget(1, leftWidget);
}

void QMPRExtension::switchToMIPLayout(bool isMIPChecked)
{
    // Desem la mida abans de canviar els widgets
    QList<int> splitterSize = m_horizontalSplitter->sizes();
    if (isMIPChecked)
    {
        if (!m_mipViewer)
        {
            m_mipViewer = new Q3DViewer;
            m_mipViewer->orientationMarkerOff();
            m_mipViewer->setRenderFunctionToMIP3D();
        }
        Volume *mipInput = new Volume;
        // TODO Això es necessari perquè tingui la informació de la sèrie, estudis, pacient...
        mipInput->setImages(m_volume->getImages());
        mipInput->setData(m_coronalReslice->GetOutput());
        m_mipViewer->setInput(mipInput);
        m_mipViewer->render();
        m_mipViewer->show();
        // Disposem la distribució de widgets
        m_horizontalSplitter->insertWidget(m_horizontalSplitter->indexOf(m_verticalSplitter), m_mipViewer);
        m_verticalSplitter->hide();
        m_horizontalSplitter->insertWidget(2, m_verticalSplitter);
    }
    else
    {
        m_horizontalSplitter->insertWidget(m_horizontalSplitter->indexOf(m_mipViewer), m_verticalSplitter);
        m_verticalSplitter->show();
        m_mipViewer->hide();
        m_horizontalSplitter->insertWidget(2, m_mipViewer);
    }
    // Recuperem les mides
    m_horizontalSplitter->setSizes(splitterSize);
}

void QMPRExtension::handleAxialViewEvents(unsigned long eventID)
{
    switch (eventID)
    {
        case vtkCommand::LeftButtonPressEvent:
            if (detectAxialViewAxisActor())
            {
                if (m_axial2DView->getInteractor()->GetControlKey())
                {
                    m_state = Pushing;
                }
                else
                {
                    m_state = Rotating;
                }
            }
            break;

        case vtkCommand::LeftButtonReleaseEvent:
            if (m_state != None)
            {
                releaseAxialViewAxisActor();
            }
            break;

        case vtkCommand::MouseMoveEvent:
            if (m_state == Rotating)
            {
                rotateAxialViewAxisActor();
            }
            else if (m_state == Pushing)
            {
                pushAxialViewAxisActor();
            }

            if ( m_pickedActorPlaneSource == m_coronalPlaneSource && m_coronal2DView->getDrawer()->getNumberOfDrawnPrimitives() > 0 )
            {
                m_coronal2DView->getDrawer()->removeAllPrimitives();
            }

            if ( m_pickedActorPlaneSource == m_sagitalPlaneSource && m_sagital2DView->getDrawer()->getNumberOfDrawnPrimitives() > 0 )
            {
                m_sagital2DView->getDrawer()->removeAllPrimitives();
            }

            break;

        default:
            break;
    }
}

void QMPRExtension::handleSagitalViewEvents(unsigned long eventID)
{
    switch (eventID)
    {
        case vtkCommand::LeftButtonPressEvent:
            if (m_sagital2DView->getInteractor()->GetControlKey())
            {
                detectPushSagitalViewAxisActor();
            }
            else
            {
                detectSagitalViewAxisActor();
            }
            break;

        case vtkCommand::LeftButtonReleaseEvent:
            if (m_state != None)
            {
                releaseSagitalViewAxisActor();
            }
            break;

        case vtkCommand::MouseMoveEvent:
            if (m_state == Rotating)
            {
                rotateSagitalViewAxisActor();
            }
            else if (m_state == Pushing)
            {
                if (m_pickedActorPlaneSource == m_coronalPlaneSource)
                {
                    pushSagitalViewCoronalAxisActor();
                }
                else
                {
                    pushSagitalViewAxialAxisActor();
                }
            }

            if ( m_pickedActorPlaneSource == m_coronalPlaneSource && m_coronal2DView->getDrawer()->getNumberOfDrawnPrimitives() > 0 )
            {
                m_coronal2DView->getDrawer()->removeAllPrimitives();
            }
            break;
        default:
            break;
    }
}

bool QMPRExtension::detectAxialViewAxisActor()
{
    bool picked = false;
    double clickedWorldPoint[3];
    m_axial2DView->getEventWorldCoordinate(clickedWorldPoint);

    // Detectem quin és l'actor més proper, l'identifiquem i llavors el deixem com a seleccionat
    double point[3] = { clickedWorldPoint[0], clickedWorldPoint[1], 0.0 };
    double *r1, *r2;
    double distanceToCoronal, distanceToSagital;

    r1 = m_coronalOverAxialIntersectionAxis->GetPositionCoordinate()->GetValue();
    r2 = m_coronalOverAxialIntersectionAxis->GetPosition2Coordinate()->GetValue();
    distanceToCoronal = MathTools::getPointToFiniteLineDistance(point, r1, r2);

    r1 = m_sagitalOverAxialAxisActor->GetPositionCoordinate()->GetValue();
    r2 = m_sagitalOverAxialAxisActor->GetPosition2Coordinate()->GetValue();
    distanceToSagital = MathTools::getPointToFiniteLineDistance(point, r1, r2);

    // Donem una "tolerància" mínima
    if (distanceToCoronal < PickingDistanceThreshold || distanceToSagital < PickingDistanceThreshold)
    {
        if (distanceToCoronal < distanceToSagital)
        {
            m_pickedActorPlaneSource = m_coronalPlaneSource;
            m_pickedActorReslice = m_coronalReslice;
        }
        else
        {
            m_pickedActorPlaneSource = m_sagitalPlaneSource;
            m_pickedActorReslice = m_sagitalReslice;
        }
        m_pickedActorReslice->SetInterpolationModeToNearestNeighbor();
        // Desactivem les tools que puguin estar actives
        m_toolManager->disableAllToolsTemporarily();
        m_initialPickX = clickedWorldPoint[0];
        m_initialPickY = clickedWorldPoint[1];
        picked = true;
    }
    return picked;
}

void QMPRExtension::rotateAxialViewAxisActor()
{
    double clickedWorldPoint[3];
    m_axial2DView->getEventWorldCoordinate(clickedWorldPoint);

    double vec1[3], vec2[3];
    double axis[3];
    double direction[3];

    vec1[0] = m_initialPickX - m_pickedActorPlaneSource->GetCenter()[0];
    vec1[1] = m_initialPickY - m_pickedActorPlaneSource->GetCenter()[1];
    vec1[2] = 0.0;

    vec2[0] = clickedWorldPoint[0] - m_pickedActorPlaneSource->GetCenter()[0];
    vec2[1] = clickedWorldPoint[1] - m_pickedActorPlaneSource->GetCenter()[1];
    vec2[2] = 0.0;

    double degrees = MathTools::angleInDegrees(vec1, vec2);

    m_initialPickX = clickedWorldPoint[0];
    m_initialPickY = clickedWorldPoint[1];

    MathTools::crossProduct(vec1, vec2, direction);
    this->getRotationAxis(m_pickedActorPlaneSource, axis);
    double dot = MathTools::dotProduct(direction, axis);

    axis[0] *= dot;
    axis[1] *= dot;
    axis[2] *= dot;
    MathTools::normalize(axis);
    rotateMiddle(degrees, axis, m_pickedActorPlaneSource);
    updatePlanes();
    updateControls();
}

void QMPRExtension::releaseAxialViewAxisActor()
{
    if (m_pickedActorReslice)
    {
        m_pickedActorReslice->SetInterpolationModeToCubic();
        // TODO No seria millor un restoreOverrideCursor?
        this->setCursor(QCursor(Qt::ArrowCursor));
        if (m_pickedActorPlaneSource == m_sagitalPlaneSource)
        {
            m_sagital2DView->render();
        }
        else
        {
            m_coronal2DView->render();
        }
        m_state = None;
        m_pickedActorReslice = 0;
        m_pickedActorPlaneSource = 0;
        // Reactivem les tools
        m_toolManager->undoDisableAllToolsTemporarily();
    }
}

void QMPRExtension::detectSagitalViewAxisActor()
{
    double clickedWorldPoint[3];
    m_sagital2DView->getEventWorldCoordinate(clickedWorldPoint);

    // Detectem quin és l'actor més proper, l'identifiquem i llavors el deixem com a seleccionat
    double point[3] = { clickedWorldPoint[0], clickedWorldPoint[1], 0.0 };
    double *r1, *r2;
    double distanceToCoronal;

    r1 = m_coronalOverSagitalIntersectionAxis->GetPositionCoordinate()->GetValue();
    r2 = m_coronalOverSagitalIntersectionAxis->GetPosition2Coordinate()->GetValue();
    distanceToCoronal = MathTools::getPointToFiniteLineDistance(point, r1, r2);

    // Donem una "tolerància" mínima
    if (distanceToCoronal < PickingDistanceThreshold)
    {
        m_pickedActorReslice = m_coronalReslice;
        m_pickedActorReslice->SetInterpolationModeToNearestNeighbor();
        m_pickedActorPlaneSource = m_coronalPlaneSource;
        // Desactivem les tools que puguin estar actives
        m_toolManager->disableAllToolsTemporarily();

        m_initialPickX = clickedWorldPoint[0];
        m_initialPickY = clickedWorldPoint[1];
        m_state = Rotating;
    }
}

void QMPRExtension::rotateSagitalViewAxisActor()
{
    double clickedWorldPoint[3];
    m_sagital2DView->getEventWorldCoordinate(clickedWorldPoint);

    double vec1[3], vec2[3];
    double axis[3];
    double direction[3];

    vec1[1] = m_initialPickX - m_pickedActorPlaneSource->GetCenter()[1];
    vec1[2] = m_initialPickY - m_pickedActorPlaneSource->GetCenter()[2];
    vec1[0] = 0.0;

    vec2[1] = clickedWorldPoint[0] - m_pickedActorPlaneSource->GetCenter()[1];
    vec2[2] = clickedWorldPoint[1] - m_pickedActorPlaneSource->GetCenter()[2];
    vec2[0] = 0.0;

    double degrees = MathTools::angleInDegrees(vec1, vec2);

    m_initialPickX = clickedWorldPoint[0];
    m_initialPickY = clickedWorldPoint[1];

    MathTools::crossProduct(vec1, vec2, direction);
    axis[0] = m_pickedActorPlaneSource->GetPoint1()[0] - m_pickedActorPlaneSource->GetOrigin()[0];
    axis[1] = m_pickedActorPlaneSource->GetPoint1()[1] - m_pickedActorPlaneSource->GetOrigin()[1];
    axis[2] = m_pickedActorPlaneSource->GetPoint1()[2] - m_pickedActorPlaneSource->GetOrigin()[2];

    double dot = MathTools::dotProduct(direction, axis);

    axis[0] *= dot;
    axis[1] *= dot;
    axis[2] *= dot;
    MathTools::normalize(axis);
    rotateMiddle(degrees, axis, m_pickedActorPlaneSource);
    updatePlanes();
    updateControls();
}

void QMPRExtension::releaseSagitalViewAxisActor()
{
    if (m_pickedActorReslice)
    {
        this->setCursor(QCursor(Qt::ArrowCursor));
        m_pickedActorReslice->SetInterpolationModeToCubic();
        m_coronal2DView->render();
        m_state = None;
        m_pickedActorReslice = 0;
        m_pickedActorPlaneSource = 0;
        // Reactivem les tools
        m_toolManager->undoDisableAllToolsTemporarily();
    }
}

void QMPRExtension::getRotationAxis(vtkPlaneSource *plane, double axis[3])
{
    if (!plane)
    {
        return;
    }

    axis[0] = plane->GetPoint2()[0] - plane->GetOrigin()[0];
    axis[1] = plane->GetPoint2()[1] - plane->GetOrigin()[1];
    axis[2] = plane->GetPoint2()[2] - plane->GetOrigin()[2];
}

void QMPRExtension::pushSagitalViewCoronalAxisActor()
{
    this->setCursor(QCursor(Qt::ClosedHandCursor));
    double clickedWorldPoint[3];
    m_sagital2DView->getEventWorldCoordinate(clickedWorldPoint);
    // Get the motion vector
    double v[3];
    v[1] = clickedWorldPoint[0] - m_initialPickX;
    v[2] = clickedWorldPoint[1] - m_initialPickY;
    v[0] = 0.0;

    m_pickedActorPlaneSource->Push(MathTools::dotProduct(v, m_pickedActorPlaneSource->GetNormal()));
    updatePlanes();
    updateControls();

    m_initialPickX = clickedWorldPoint[0];
    m_initialPickY = clickedWorldPoint[1];
}

void QMPRExtension::pushAxialViewAxisActor()
{
    this->setCursor(QCursor(Qt::ClosedHandCursor));
    double clickedWorldPoint[3];
    m_axial2DView->getEventWorldCoordinate(clickedWorldPoint);
    // Get the motion vector
    double v[3];
    v[0] = clickedWorldPoint[0] - m_initialPickX;
    v[1] = clickedWorldPoint[1] - m_initialPickY;
    v[2] = 0.0;

    m_pickedActorPlaneSource->Push(MathTools::dotProduct(v, m_pickedActorPlaneSource->GetNormal()));
    updatePlanes();
    updateControls();

    m_initialPickX = clickedWorldPoint[0];
    m_initialPickY = clickedWorldPoint[1];
}

void QMPRExtension::detectPushSagitalViewAxisActor()
{
    double clickedWorldPoint[3];
    m_sagital2DView->getEventWorldCoordinate(clickedWorldPoint);

    // Detectem quin és l'actor més proper, l'identifiquem i llavors el deixem com a seleccionat
    // Únicament mourem la vista axial. Desde la vista sagital no podrem moure l'slice de la coronal
    double point[3] = { clickedWorldPoint[0], clickedWorldPoint[1], 0.0 };
    double *r1, *r2;
    double distanceToAxial, distanceToCoronal;

    r1 = m_axialOverSagitalIntersectionAxis->GetPositionCoordinate()->GetValue();
    r2 = m_axialOverSagitalIntersectionAxis->GetPosition2Coordinate()->GetValue();
    distanceToAxial = MathTools::getPointToFiniteLineDistance(point, r1, r2);

    r1 = m_coronalOverSagitalIntersectionAxis->GetPositionCoordinate()->GetValue();
    r2 = m_coronalOverSagitalIntersectionAxis->GetPosition2Coordinate()->GetValue();
    distanceToCoronal = MathTools::getPointToFiniteLineDistance(point, r1, r2);

    // Donem una "tolerància" mínima
    if (distanceToCoronal < PickingDistanceThreshold || distanceToAxial < PickingDistanceThreshold)
    {
        this->setCursor(QCursor(Qt::OpenHandCursor));
        // Desactivem les tools perquè no facin interferència
        m_toolManager->disableAllToolsTemporarily();
        if (distanceToCoronal < distanceToAxial)
        {
            m_pickedActorPlaneSource = m_coronalPlaneSource;
            m_pickedActorReslice = m_coronalReslice;
        }
        else
        {
            m_pickedActorPlaneSource = m_axialPlaneSource;
            m_pickedActorReslice = m_sagitalReslice;
        }
        m_state = Pushing;
        m_initialPickX = clickedWorldPoint[0];
        m_initialPickY = clickedWorldPoint[1];
    }
}

void QMPRExtension::pushSagitalViewAxialAxisActor()
{
    double clickedWorldPoint[3];
    m_sagital2DView->getEventWorldCoordinate(clickedWorldPoint);

    m_axial2DView->setSlice(m_axial2DView->getMaximumSlice() - static_cast<int>(clickedWorldPoint[1] / m_axialSpacing[2]));
    updatePlanes();
    updateControls();

    m_initialPickX = clickedWorldPoint[0];
    m_initialPickY = clickedWorldPoint[1];
}

void QMPRExtension::setInput(Volume *input)
{
    if (input->getNumberOfPhases() > 1)
    {
        m_phasesAlertLabel->setVisible(true);
    }
    else
    {
        m_phasesAlertLabel->setVisible(false);
    }

    vtkImageChangeInformation *changeInfo = vtkImageChangeInformation::New();
    changeInfo->SetInput(input->getVtkData());
    changeInfo->SetOutputOrigin(.0, .0, .0);
    changeInfo->Update();

    // TODO Es crea un nou volum cada cop!
    m_volume = new Volume;
    m_volume->setImages(input->getImages());
    m_volume->setData(changeInfo->GetOutput());
    m_volume->setNumberOfPhases(input->getNumberOfPhases());
    m_volume->setNumberOfSlicesPerPhase(input->getNumberOfSlicesPerPhase());
    // Cal que li indiquem l'identificador que hi ha al repositori de volums per 
    // tal que quan mostrem el menú de pacient se'ns mostri en negreta el volum actual
    // Això no deixa de ser un HACK que deixarà d'existir quan no li fem la transformació 
    // inicial de l'origen al volum quan tinguem llest el nou mòdul d'MPR
    m_volume->setIdentifier(input->getIdentifier());

    m_volume->getSpacing(m_axialSpacing);

    m_sagitalReslice->SetInput(m_volume->getVtkData());
    m_coronalReslice->SetInput(m_volume->getVtkData());

    // Faltaria refrescar l'input dels 3 mpr
    m_axial2DView->setInput(m_volume);

    int extent[6];
    m_volume->getWholeExtent(extent);
    m_axialSlider->setMaximum( extent[5]);

    double maxThickSlab = sqrt((m_axialSpacing[0] * extent[1]) * (m_axialSpacing[0] * extent[1]) + (m_axialSpacing[1] * extent[3]) * (m_axialSpacing[1] * extent[3]) + (m_axialSpacing[2] * extent[5]) * (m_axialSpacing[2] * extent[5]));
    m_thickSlabSlider->setMaximum((int) maxThickSlab);
    m_thickSlabSpinBox->setMaximum(maxThickSlab);

    // Posta a punt dels planeSource
    initOrientation();

    Volume *sagitalResliced = new Volume;
    // TODO Això es necessari perquè tingui la informació de la sèrie, estudis, pacient...
    sagitalResliced->setImages(m_volume->getImages());
    sagitalResliced->setData(m_sagitalReslice->GetOutput());
    sagitalResliced->setNumberOfPhases(1);
    sagitalResliced->setNumberOfSlicesPerPhase(1);

    m_sagital2DView->setInput(sagitalResliced);

    Volume *coronalResliced = new Volume;
    // TODO Això es necessari perquè tingui la informació de la sèrie, estudis, pacient...
    coronalResliced->setImages(m_volume->getImages());
    coronalResliced->setData(m_coronalReslice->GetOutput());
    coronalResliced->setNumberOfPhases(1);
    coronalResliced->setNumberOfSlicesPerPhase(1);
    m_coronal2DView->setInput(coronalResliced);
}

void QMPRExtension::initOrientation()
{
/**
    IMPORTANT!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    Ara li donem a cada pla unes dimensions, extent, espaiat, etc d'acord com si aquests plans haguessin de ser ortogonals,però segons el pla de tall serà d'una manera o altre
    La vista axial mantindrà els espaiats i extents originals
    La vista sagital, com que únicament es podrà rotar sobre l'eix Y, mantindrà l'extent de la seva X igual que l'extent Y original. Els altres s'hauran d'adaptar a les distàncies corresponents a les diagonals zmax-xmax
    En la vista coronal, com que pot tenir qualsevol orientacio tindrà que adaptar els seus extents als màxims

*/
    int extent[6];
    m_volume->getWholeExtent(extent);
    double origin[3];
    m_volume->getOrigin(origin);
    double spacing[3];
    m_volume->getSpacing(spacing);

    // Prevent obscuring voxels by offsetting the plane geometry
    double xbounds[] = {origin[0] + spacing[0] * (extent[0] - 0.5) ,
                        origin[0] + spacing[0] * (extent[1] + 0.5)};
    double ybounds[] = {origin[1] + spacing[1] * (extent[2] - 0.5),
                        origin[1] + spacing[1] * (extent[3] + 0.5)};
    double zbounds[] = {origin[2] + spacing[2] * (extent[4] - 0.5),
                        origin[2] + spacing[2] * (extent[5] + 0.5)};

    if (spacing[0] < 0.0)
    {
        double t = xbounds[0];
        xbounds[0] = xbounds[1];
        xbounds[1] = t;
    }
    if (spacing[1] < 0.0)
    {
        double t = ybounds[0];
        ybounds[0] = ybounds[1];
        ybounds[1] = t;
    }
    if (spacing[2] < 0.0)
    {
        double t = zbounds[0];
        zbounds[0] = zbounds[1];
        zbounds[1] = t;
    }
    // XY, z-normal : vista axial, en principi d'aquesta vista nomès canviarem la llesca
    m_axialPlaneSource->SetOrigin(xbounds[0], ybounds[1], zbounds[1]);
    m_axialPlaneSource->SetPoint1(xbounds[1], ybounds[1], zbounds[1]);
    m_axialPlaneSource->SetPoint2(xbounds[0], ybounds[0], zbounds[1]);
    m_axialZeroSliceCoordinate = zbounds[1];

    // YZ, x-normal : vista sagital
    // Estem ajustant la mida del pla a les dimensions d'aquesta orientació
    // La mida de la Y inicial, que serà una combinació d'X i Y durant l'execució, ha de ser la diagonal del pla XY. Ampliarem la meitat a cada banda sobre la mida d'Y.
    // Atenció: estem assumint que xbounds[0] = 0. La forma correcta seria (xbounds[1] - xbounds[0] (+1?)). El mateix per y.
    double xyDiagonal = sqrt(xbounds[1] * xbounds[1] + ybounds[1] * ybounds[1]);
    double halfDeltaY = (xyDiagonal - ybounds[1]) * 0.5;
    m_sagitalPlaneSource->SetOrigin(xbounds[0], ybounds[0] - halfDeltaY, zbounds[1]);
    m_sagitalPlaneSource->SetPoint1(xbounds[0], ybounds[1] + halfDeltaY, zbounds[1]);
    m_sagitalPlaneSource->SetPoint2(xbounds[0], ybounds[0] - halfDeltaY, zbounds[0]);
    m_sagitalPlaneSource->Push(-0.5 * (xbounds[1] - xbounds[0]));
    // Calculem la translació necessària per dibuixar les interseccions dels plans a la vista sagital
    m_sagitalTranslation[0] = m_sagitalPlaneSource->GetCenter()[1] + halfDeltaY;
    m_sagitalTranslation[1] = -m_sagitalPlaneSource->GetCenter()[2];
    m_sagitalTranslation[2] = 0.0;

    // ZX, y-normal : vista coronal
    // ídem anterior
    // La mida de la X i la Z inicials, que seran una combinació d'X, Y i Z durant l'execució, ha de ser la diagonal del volum. Ampliarem la meitat a cada banda sobre la mida dels eixos X i Z.
    // Atenció: estem assumint que xbounds[0] = 0. La forma correcta seria (xbounds[1] - xbounds[0] (+1?)). El mateix per y i z.
    double diagonal = sqrt(xbounds[1] * xbounds[1] + ybounds[1] * ybounds[1] + zbounds[1] * zbounds[1]);
    double halfDeltaX = (diagonal - xbounds[1]) * 0.5;
    double halfDeltaZ = (diagonal - zbounds[1]) * 0.5;
    m_coronalPlaneSource->SetOrigin(xbounds[0] - halfDeltaX, ybounds[1], zbounds[1] + halfDeltaZ);
    m_coronalPlaneSource->SetPoint1(xbounds[1] + halfDeltaX, ybounds[1], zbounds[1] + halfDeltaZ);
    m_coronalPlaneSource->SetPoint2(xbounds[0] - halfDeltaX, ybounds[1], zbounds[0] - halfDeltaZ);
    m_coronalPlaneSource->Push(-0.5 * (ybounds[1] - ybounds[0]));

    updatePlanes();
    updateControls();
}

void QMPRExtension::createActors()
{
    // Creem els axis actors
    m_sagitalOverAxialAxisActor = vtkAxisActor2D::New();
    m_coronalOverAxialIntersectionAxis = vtkAxisActor2D::New();
    m_coronalOverSagitalIntersectionAxis = vtkAxisActor2D::New();
    m_axialOverSagitalIntersectionAxis = vtkAxisActor2D::New();
    m_thickSlabOverAxialActor = vtkAxisActor2D::New();
    m_thickSlabOverSagitalActor = vtkAxisActor2D::New();

    m_sagitalOverAxialAxisActor->AxisVisibilityOn();
    m_sagitalOverAxialAxisActor->TickVisibilityOff();
    m_sagitalOverAxialAxisActor->LabelVisibilityOff();
    m_sagitalOverAxialAxisActor->TitleVisibilityOff();
    m_sagitalOverAxialAxisActor->GetProperty()->SetColor(1., .6, .0);

    m_coronalOverAxialIntersectionAxis->TickVisibilityOff();
    m_coronalOverAxialIntersectionAxis->LabelVisibilityOff();
    m_coronalOverAxialIntersectionAxis->TitleVisibilityOff();
    m_coronalOverAxialIntersectionAxis->GetProperty()->SetColor(.0, 1., 1.);

    m_coronalOverSagitalIntersectionAxis->AxisVisibilityOn();
    m_coronalOverSagitalIntersectionAxis->TickVisibilityOff();
    m_coronalOverSagitalIntersectionAxis->LabelVisibilityOff();
    m_coronalOverSagitalIntersectionAxis->TitleVisibilityOff();
    m_coronalOverSagitalIntersectionAxis->GetProperty()->SetColor(.0, 1., 1.);

    m_axialOverSagitalIntersectionAxis->AxisVisibilityOn();
    m_axialOverSagitalIntersectionAxis->TickVisibilityOff();
    m_axialOverSagitalIntersectionAxis->LabelVisibilityOff();
    m_axialOverSagitalIntersectionAxis->TitleVisibilityOff();
    m_axialOverSagitalIntersectionAxis->GetProperty()->SetColor(1., 1., .0);

    m_thickSlabOverAxialActor->AxisVisibilityOn();
    m_thickSlabOverAxialActor->TickVisibilityOff();
    m_thickSlabOverAxialActor->LabelVisibilityOff();
    m_thickSlabOverAxialActor->TitleVisibilityOff();
    m_thickSlabOverAxialActor->GetProperty()->SetColor(.0, 1., 1.);
    m_thickSlabOverAxialActor->GetProperty()->SetLineStipplePattern(65280);

    m_thickSlabOverSagitalActor->AxisVisibilityOn();
    m_thickSlabOverSagitalActor->TickVisibilityOff();
    m_thickSlabOverSagitalActor->LabelVisibilityOff();
    m_thickSlabOverSagitalActor->TitleVisibilityOff();
    m_thickSlabOverSagitalActor->GetProperty()->SetColor(.0, 1., 1.);
    m_thickSlabOverSagitalActor->GetProperty()->SetLineStipplePattern(65280);

    m_axial2DView->getRenderer()->AddViewProp(m_sagitalOverAxialAxisActor);
    m_axial2DView->getRenderer()->AddViewProp(m_coronalOverAxialIntersectionAxis);
    m_axial2DView->getRenderer()->AddViewProp(m_thickSlabOverAxialActor);
    m_sagital2DView->getRenderer()->AddViewProp(m_coronalOverSagitalIntersectionAxis);
    m_sagital2DView->getRenderer()->AddViewProp(m_axialOverSagitalIntersectionAxis);
    m_sagital2DView->getRenderer()->AddViewProp(m_thickSlabOverSagitalActor);
}

void QMPRExtension::axialSliceUpdated(int slice)
{
    // Push relatiu que hem de fer = reubicar-nos a l'inici i colocar la llesca
    m_axialPlaneSource->Push(-(m_axialZeroSliceCoordinate - m_axialPlaneSource->GetOrigin()[2]) + (slice*m_axialSpacing[2]));
    m_axialPlaneSource->Update();
    updateControls();
}

void QMPRExtension::updateControls()
{
    // Posem la representació dels plans sobre cada vista
    updateIntersectionPoint();

    // Passem a sistema de coordenades de món
    m_sagitalOverAxialAxisActor->GetPositionCoordinate()->SetCoordinateSystemToWorld();
    m_sagitalOverAxialAxisActor->GetPosition2Coordinate()->SetCoordinateSystemToWorld();

    m_coronalOverSagitalIntersectionAxis->GetPositionCoordinate()->SetCoordinateSystemToWorld();
    m_coronalOverSagitalIntersectionAxis->GetPosition2Coordinate()->SetCoordinateSystemToWorld();

    m_coronalOverAxialIntersectionAxis->GetPositionCoordinate()->SetCoordinateSystemToWorld();
    m_coronalOverAxialIntersectionAxis->GetPosition2Coordinate()->SetCoordinateSystemToWorld();

    m_axialOverSagitalIntersectionAxis->GetPositionCoordinate()->SetCoordinateSystemToWorld();
    m_axialOverSagitalIntersectionAxis->GetPosition2Coordinate()->SetCoordinateSystemToWorld();

    m_thickSlabOverAxialActor->GetPositionCoordinate()->SetCoordinateSystemToWorld();
    m_thickSlabOverAxialActor->GetPosition2Coordinate()->SetCoordinateSystemToWorld();

    m_thickSlabOverSagitalActor->GetPositionCoordinate()->SetCoordinateSystemToWorld();
    m_thickSlabOverSagitalActor->GetPosition2Coordinate()->SetCoordinateSystemToWorld();

    m_thickSlabPlaneSource->SetOrigin(m_coronalPlaneSource->GetOrigin());
    m_thickSlabPlaneSource->SetPoint1(m_coronalPlaneSource->GetPoint1());
    m_thickSlabPlaneSource->SetPoint2(m_coronalPlaneSource->GetPoint2());
    m_thickSlabPlaneSource->Push(m_thickSlab);

    // Calculem la transformació necessària per dibuixar les interseccions dels plans a la vista sagital

    double sagitalPlaneAxis1[3];
    double sagitalPlaneAxis2[3];
    double sagitalPlaneNormal[3];
    double sagitalPlaneCenter[3];
    sagitalPlaneAxis1[0] = m_sagitalPlaneSource->GetPoint1()[0] - m_sagitalPlaneSource->GetOrigin()[0];
    sagitalPlaneAxis1[1] = m_sagitalPlaneSource->GetPoint1()[1] - m_sagitalPlaneSource->GetOrigin()[1];
    sagitalPlaneAxis1[2] = m_sagitalPlaneSource->GetPoint1()[2] - m_sagitalPlaneSource->GetOrigin()[2];
    MathTools::normalize(sagitalPlaneAxis1);
    sagitalPlaneAxis2[0] = m_sagitalPlaneSource->GetPoint2()[0] - m_sagitalPlaneSource->GetOrigin()[0];
    sagitalPlaneAxis2[1] = m_sagitalPlaneSource->GetPoint2()[1] - m_sagitalPlaneSource->GetOrigin()[1];
    sagitalPlaneAxis2[2] = m_sagitalPlaneSource->GetPoint2()[2] - m_sagitalPlaneSource->GetOrigin()[2];
    MathTools::normalize(sagitalPlaneAxis2);
    m_sagitalPlaneSource->GetNormal(sagitalPlaneNormal);
    m_sagitalPlaneSource->GetCenter(sagitalPlaneCenter);

    vtkMatrix4x4 *sagitalTransformationMatrix = vtkMatrix4x4::New();
    sagitalTransformationMatrix->Identity();

    for (int i = 0; i < 3; i++)
    {
        sagitalTransformationMatrix->SetElement(0, i, sagitalPlaneAxis1[i]);
        sagitalTransformationMatrix->SetElement(1, i, sagitalPlaneAxis2[i]);
        sagitalTransformationMatrix->SetElement(2, i, sagitalPlaneNormal[i]);
    }

    m_transform->Identity();
    m_transform->Translate(m_sagitalTranslation);
    m_transform->Concatenate(sagitalTransformationMatrix);
    m_transform->Translate(-sagitalPlaneCenter[0], -sagitalPlaneCenter[1], -sagitalPlaneCenter[2]);

    sagitalTransformationMatrix->Delete();

    // Calculem les interseccions

    double r[3], t[3], position1[3], position2[3];
    const double Length = 2000.0;

    // Projecció sagital sobre axial i viceversa
    MathTools::planeIntersection(m_axialPlaneSource->GetOrigin(), m_axialPlaneSource->GetNormal(),  m_sagitalPlaneSource->GetOrigin(), m_sagitalPlaneSource->GetNormal(), r, t);
    MathTools::normalize(t);    // normalitzem t per que sempre tinguem la mateixa llargada (1)

    position1[0] = r[0] - t[0] * Length;
    position1[1] = r[1] - t[1] * Length;
    position1[2] = r[2] - t[2] * Length;

    position2[0] = r[0] + t[0] * Length;
    position2[1] = r[1] + t[1] * Length;
    position2[2] = r[2] + t[2] * Length;

    m_sagitalOverAxialAxisActor->SetPosition( position1[0], position1[1]);
    m_sagitalOverAxialAxisActor->SetPosition2(position2[0], position2[1]);

    m_transform->TransformPoint(position1, position1);
    m_transform->TransformPoint(position2, position2);
    m_axialOverSagitalIntersectionAxis->SetPosition(position1[0], -position1[1]);
    m_axialOverSagitalIntersectionAxis->SetPosition2(position2[0], -position2[1]);

    // Projecció coronal sobre sagital

    MathTools::planeIntersection(m_coronalPlaneSource->GetOrigin(), m_coronalPlaneSource->GetNormal(), m_sagitalPlaneSource->GetOrigin(), m_sagitalPlaneSource->GetNormal(), r, t);
    MathTools::normalize(t);    // normalitzem t per que sempre tinguem la mateixa llargada (1)

    position1[0] = r[0] - t[0] * Length;
    position1[1] = r[1] - t[1] * Length;
    position1[2] = r[2] - t[2] * Length;

    position2[0] = r[0] + t[0] * Length;
    position2[1] = r[1] + t[1] * Length;
    position2[2] = r[2] + t[2] * Length;

    m_transform->TransformPoint(position1, position1);
    m_transform->TransformPoint(position2, position2);
    m_coronalOverSagitalIntersectionAxis->SetPosition(position1[0], -position1[1]);
    m_coronalOverSagitalIntersectionAxis->SetPosition2(position2[0], -position2[1]);

    // Projecció thick slab sobre sagital
    MathTools::planeIntersection(m_thickSlabPlaneSource->GetOrigin(), m_thickSlabPlaneSource->GetNormal(), m_sagitalPlaneSource->GetOrigin(), m_sagitalPlaneSource->GetNormal(), r, t);
    MathTools::normalize(t);    // normalitzem t per que sempre tinguem la mateixa llargada (1)

    position1[0] = r[0] - t[0] * Length;
    position1[1] = r[1] - t[1] * Length;
    position1[2] = r[2] - t[2] * Length;

    position2[0] = r[0] + t[0] * Length;
    position2[1] = r[1] + t[1] * Length;
    position2[2] = r[2] + t[2] * Length;

    m_transform->TransformPoint(position1, position1);
    m_transform->TransformPoint(position2, position2);
    m_thickSlabOverSagitalActor->SetPosition(position1[0], -position1[1]);
    m_thickSlabOverSagitalActor->SetPosition2(position2[0], -position2[1]);

    // Projecció coronal sobre axial
    MathTools::planeIntersection(m_coronalPlaneSource->GetOrigin(), m_coronalPlaneSource->GetNormal(), m_axialPlaneSource->GetOrigin(), m_axialPlaneSource->GetNormal(), r, t);
    MathTools::normalize(t);    // normalitzem t per que sempre tinguem la mateixa llargada (1)

    position1[0] = r[0] - t[0] * Length;
    position1[1] = r[1] - t[1] * Length;
    position1[2] = r[2] - t[2] * Length;

    position2[0] = r[0] + t[0] * Length;
    position2[1] = r[1] + t[1] * Length;
    position2[2] = r[2] + t[2] * Length;

    m_coronalOverAxialIntersectionAxis->SetPosition( position1[0], position1[1]);
    m_coronalOverAxialIntersectionAxis->SetPosition2(position2[0], position2[1]);

    // Projecció thick slab sobre axial
    MathTools::planeIntersection(m_thickSlabPlaneSource->GetOrigin(), m_thickSlabPlaneSource->GetNormal(), m_axialPlaneSource->GetOrigin(), m_axialPlaneSource->GetNormal(), r, t);
    MathTools::normalize(t);    // normalitzem t per que sempre tinguem la mateixa llargada (1)

    position1[0] = r[0] - t[0] * Length;
    position1[1] = r[1] - t[1] * Length;
    position1[2] = r[2] - t[2] * Length;

    position2[0] = r[0] + t[0] * Length;
    position2[1] = r[1] + t[1] * Length;
    position2[2] = r[2] + t[2] * Length;

    m_thickSlabOverAxialActor->SetPosition(position1[0], position1[1]);
    m_thickSlabOverAxialActor->SetPosition2(position2[0], position2[1]);

    // Repintem l'escena
    m_axial2DView->render();
    m_sagital2DView->render();
    m_coronal2DView->render();
}

void QMPRExtension::updateIntersectionPoint()
{
    MathTools::planeIntersection(m_coronalPlaneSource->GetOrigin(), m_coronalPlaneSource->GetNormal(), m_sagitalPlaneSource->GetOrigin(), m_sagitalPlaneSource->GetNormal(), m_axialPlaneSource->GetOrigin(), m_axialPlaneSource->GetNormal(),  m_intersectionPoint);
}

void QMPRExtension::updatePlanes()
{
    updatePlane(m_sagitalPlaneSource, m_sagitalReslice);
    updatePlane(m_coronalPlaneSource, m_coronalReslice);
    updateIntersectionPoint();
}

void QMPRExtension::updatePlane(vtkPlaneSource *planeSource, vtkImageReslice *reslice)
{
    if (!reslice || !(vtkImageData::SafeDownCast(reslice->GetInput())) )
    {
        return;
    }

    // Calculate appropriate pixel spacing for the reslicing
    double spacing[3];
    m_volume->getSpacing(spacing);

    int i;

//     if (this->RestrictPlaneToVolume)
//     {
        double origin[3];
        m_volume->getOrigin(origin);

        int extent[6];
        m_volume->getWholeExtent(extent);

        double bounds[] = { origin[0] + spacing[0]*extent[0], // xmin
                        origin[0] + spacing[0]*extent[1], // xmax
                        origin[1] + spacing[1]*extent[2], // ymin
                        origin[1] + spacing[1]*extent[3], // ymax
                        origin[2] + spacing[2]*extent[4], // zmin
                        origin[2] + spacing[2]*extent[5] };// zmax

        for (i = 0; i <= 4; i += 2) // Reverse bounds if necessary
        {
            if (bounds[i] > bounds[i+1])
            {
                double t = bounds[i+1];
                bounds[i+1] = bounds[i];
                bounds[i] = t;
            }
        }

        double abs_normal[3];
        planeSource->GetNormal(abs_normal);

        double planeCenter[3];
        planeSource->GetCenter(planeCenter);

        double nmax = 0.0;
        int k = 0;
        for (i = 0; i < 3; i++)
        {
            abs_normal[i] = fabs(abs_normal[i]);
            if (abs_normal[i]>nmax)
            {
                nmax = abs_normal[i];
                k = i;
            }
        }
        // Force the plane to lie within the true image bounds along its normal    
        if (planeCenter[k] > bounds[2*k+1])
        {
            planeCenter[k] = bounds[2*k+1];
        }
        else if (planeCenter[k] < bounds[2*k])
        {
            planeCenter[k] = bounds[2*k];
        }
        planeSource->SetCenter(planeCenter);
//     }

    double planeAxis1[3];
    double planeAxis2[3];
    // Obtenim els vectors
    planeAxis1[0] = planeSource->GetPoint1()[0] - planeSource->GetOrigin()[0];
    planeAxis1[1] = planeSource->GetPoint1()[1] - planeSource->GetOrigin()[1];
    planeAxis1[2] = planeSource->GetPoint1()[2] - planeSource->GetOrigin()[2];

    planeAxis2[0] = planeSource->GetPoint2()[0] - planeSource->GetOrigin()[0];
    planeAxis2[1] = planeSource->GetPoint2()[1] - planeSource->GetOrigin()[1];
    planeAxis2[2] = planeSource->GetPoint2()[2] - planeSource->GetOrigin()[2];

    // The x,y dimensions of the plane
    double planeSizeX = MathTools::normalize(planeAxis1);
    double planeSizeY = MathTools::normalize(planeAxis2);

    double normal[3];
    planeSource->GetNormal(normal);

    // Generate the slicing matrix
    //
    // Podria ser membre de classe, com era originariament o passar per paràmetre
    vtkMatrix4x4 *resliceAxes = vtkMatrix4x4::New();
    resliceAxes->Identity();
    for (i = 0; i < 3; i++)
    {
        resliceAxes->SetElement(0,i,planeAxis1[i]);
        resliceAxes->SetElement(1,i,planeAxis2[i]);
        resliceAxes->SetElement(2,i,normal[i]);
    }

    double planeOrigin[4];
    planeSource->GetOrigin(planeOrigin);
    planeOrigin[3] = 1.0;
    double originXYZW[4];
    resliceAxes->MultiplyPoint(planeOrigin,originXYZW);

    resliceAxes->Transpose();
    double neworiginXYZW[4];
    double point[] =  {originXYZW[0],originXYZW[1],originXYZW[2],originXYZW[3]};
    resliceAxes->MultiplyPoint(point,neworiginXYZW);

    resliceAxes->SetElement(0,3,neworiginXYZW[0]);
    resliceAxes->SetElement(1,3,neworiginXYZW[1]);
    resliceAxes->SetElement(2,3,neworiginXYZW[2]);

    reslice->SetResliceAxes(resliceAxes);

    double spacingX = fabs(planeAxis1[0]*spacing[0])+\
                    fabs(planeAxis1[1]*spacing[1])+\
                    fabs(planeAxis1[2]*spacing[2]);

    double spacingY = fabs(planeAxis2[0]*spacing[0])+\
                    fabs(planeAxis2[1]*spacing[1])+\
                    fabs(planeAxis2[2]*spacing[2]);


    // Pad extent up to a power of two for efficient texture mapping

    // Make sure we're working with valid values
    double realExtentX = (spacingX == 0) ? 0 : planeSizeX / spacingX;

    int extentX;
    // Sanity check the input data:
    // * if realExtentX is too large, extentX will wrap
    // * if spacingX is 0, things will blow up.
    // * if realExtentX is naturally 0 or < 0, the padding will yield an
    //   extentX of 1, which is also not desirable if the input data is invalid.
    if (realExtentX > (VTK_INT_MAX >> 1) || realExtentX < 1)
    {
        WARN_LOG("Invalid X extent. [" + QString::number(realExtentX) + "] Perhaps the input data is empty?");
        extentX = 0;
    }
    else
    {
        extentX = 1;
        while (extentX < realExtentX)
        {
            extentX = extentX << 1;
        }
    }

    // Make sure extentY doesn't wrap during padding
    double realExtentY = (spacingY == 0) ? 0 : planeSizeY / spacingY;

    int extentY;
    if (realExtentY > (VTK_INT_MAX >> 1) || realExtentY < 1)
    {
        WARN_LOG("Invalid Y extent. [" + QString::number(realExtentY) + "] Perhaps the input data is empty?");
        extentY = 0;
    }
    else
    {
        extentY = 1;
        while (extentY < realExtentY)
        {
            extentY = extentY << 1;
        }
    }

    reslice->SetOutputSpacing(planeSizeX/extentX, planeSizeY/extentY, 1);
    reslice->SetOutputOrigin(0.0, 0.0, 0.0);
    // TODO Li passem thickSlab que és double però això només accepta int's! Buscar si aquesta és la manera adequada. Potsre si volem fer servir doubles ho hauríem de combinar amb l'outputSpacing
    reslice->SetOutputExtent(0, extentX-1, 0, extentY-1, 0, static_cast<int>(m_thickSlab)); // obtenim una única llesca
    reslice->Update();
}

void QMPRExtension::getSagitalXVector(double x[3])
{
    double* p1 = m_sagitalPlaneSource->GetPoint1();
    double* o =  m_sagitalPlaneSource->GetOrigin();
    x[0] = p1[0] - o[0];
    x[1] = p1[1] - o[1];
    x[2] = p1[2] - o[2];
}

void QMPRExtension::getSagitalYVector(double y[3])
{
    double* p1 = m_sagitalPlaneSource->GetPoint2();
    double* o =  m_sagitalPlaneSource->GetOrigin();
    y[0] = p1[0] - o[0];
    y[1] = p1[1] - o[1];
    y[2] = p1[2] - o[2];
}

void QMPRExtension::getCoronalXVector(double x[3])
{
    double* p1 = m_coronalPlaneSource->GetPoint1();
    double* o =  m_coronalPlaneSource->GetOrigin();
    x[0] = p1[0] - o[0];
    x[1] = p1[1] - o[1];
    x[2] = p1[2] - o[2];
}

void QMPRExtension::getAxialXVector(double x[3])
{
    double* p1 = m_axialPlaneSource->GetPoint1();
    double* o =  m_axialPlaneSource->GetOrigin();
    x[0] = p1[0] - o[0];
    x[1] = p1[1] - o[1];
    x[2] = p1[2] - o[2];
}

void QMPRExtension::getAxialYVector(double y[3])
{
    double* p2 = m_axialPlaneSource->GetPoint2();
    double* o =  m_axialPlaneSource->GetOrigin();
    y[0] = p2[0] - o[0];
    y[1] = p2[1] - o[1];
    y[2] = p2[2] - o[2];
}
void QMPRExtension::getCoronalYVector(double y[3])
{
    double* p1 = m_coronalPlaneSource->GetPoint2();
    double* o =  m_coronalPlaneSource->GetOrigin();
    y[0] = p1[0] - o[0];
    y[1] = p1[1] - o[1];
    y[2] = p1[2] - o[2];
}

bool QMPRExtension::isParallel(double axis[3])
{
    double xyzAxis[3] = {1,0,0};
    // TODO Hauríem de tenir un mètode MathTools::areParallel(vector1, vector2)
    if (MathTools::angleInDegrees(xyzAxis, axis) == 0.0)
    {
        return true;
    }
    xyzAxis[0] = -1;
    xyzAxis[1] = 0;
    xyzAxis[2] = 0;
    if (MathTools::angleInDegrees(xyzAxis, axis) == 0.0)
    {
        return true;
    }

    xyzAxis[0] = 0;
    xyzAxis[1] = 0;
    xyzAxis[2] = 1;
    if (MathTools::angleInDegrees(xyzAxis, axis) == 0.0)
    {
        return true;
    }
    xyzAxis[0] = 0;
    xyzAxis[1] = 0;
    xyzAxis[2] = -1;
    if (MathTools::angleInDegrees(xyzAxis, axis) == 0.0)
    {
        return true;
    }

    xyzAxis[0] = 0;
    xyzAxis[1] = 1;
    xyzAxis[2] = 0;
    if (MathTools::angleInDegrees(xyzAxis, axis) == 0.0)
    {
        return true;
    }
    xyzAxis[0] = 0;
    xyzAxis[1] = -1;
    xyzAxis[2] = 0;
    if (MathTools::angleInDegrees(xyzAxis, axis) == 0.0)
    {
        return true;
    }

    return false;
}

void QMPRExtension::rotateMiddle(double degrees, double rotationAxis[3],  vtkPlaneSource* plane)
{
//     MathTools::normalize(rotationAxis);
    m_transform->Identity();
    m_transform->Translate(plane->GetCenter()[0], plane->GetCenter()[1], plane->GetCenter()[2]);
    m_transform->RotateWXYZ(degrees, rotationAxis);
    m_transform->Translate(-plane->GetCenter()[0], -plane->GetCenter()[1], -plane->GetCenter()[2]);
    // Ara que tenim la transformació, l'apliquem als punts del pla (origen, punt1, punt2)
    double newPoint[3];
    m_transform->TransformPoint(plane->GetPoint1(), newPoint);
    plane->SetPoint1(newPoint);
    m_transform->TransformPoint(plane->GetPoint2(), newPoint);
    plane->SetPoint2(newPoint);
    m_transform->TransformPoint(plane->GetOrigin(), newPoint);
    plane->SetOrigin(newPoint);
    plane->Update();
}

void QMPRExtension::rotate(double degrees, double rotationAxis[3],  vtkPlaneSource* plane)
{
    // Normalitzem l'eix de rotació, serà molt millor per les operacions a fer
    MathTools::normalize(rotationAxis);

    if (isParallel(rotationAxis))
    {
        m_transform->Identity();
        m_transform->Translate(m_intersectionPoint[0], m_intersectionPoint[1], m_intersectionPoint[2]);
        m_transform->RotateWXYZ(degrees, rotationAxis);
        m_transform->Translate(-m_intersectionPoint[0], -m_intersectionPoint[1], -m_intersectionPoint[2]);
    }
    else
    {
        // (0)
        m_transform->Identity();
        // (1)
        m_transform->Translate(m_intersectionPoint[0], m_intersectionPoint[1], m_intersectionPoint[2]);
        // (2)
        double alpha, lp;
        lp = sqrt(rotationAxis[1]*rotationAxis[1] + rotationAxis[2]*rotationAxis[2]);
        alpha = asin(rotationAxis[1] / lp) ;
//         alpha = acos(rotationAxis[2] / lp);
        m_transform->RotateX(-alpha * MathTools::RadiansToDegreesAsDouble ); // o RotateWXYZ(alpha, [1,0,0])
        // (3)
        double beta, l;
        l = sqrt(rotationAxis[0]*rotationAxis[0] + rotationAxis[1]*rotationAxis[1] + rotationAxis[2]*rotationAxis[2]);
        beta =  asin(rotationAxis[0]);
        m_transform->RotateY(beta * MathTools::RadiansToDegreesAsDouble ); // o RotateWXYZ(beta, [0,1,0])
        // (4)
        m_transform->RotateZ(degrees); // o RotateWXYZ(degrees, [0,0,1])
        // (5)
        m_transform->RotateY(-beta * MathTools::RadiansToDegreesAsDouble); // o RotateWXYZ(-beta, [0,1,0])
        // (6)
        m_transform->RotateX(alpha * MathTools::RadiansToDegreesAsDouble ); // o RotateWXYZ(-alpha, [1,0,0])
        // (7)
        m_transform->Translate(-m_intersectionPoint[0], -m_intersectionPoint[1], -m_intersectionPoint[2]);

    }
    // Ara que tenim la transformació, l'apliquem als punts del pla (origen, punt1, punt2)
    double newPoint[3];
    m_transform->TransformPoint(plane->GetPoint1(), newPoint);
    plane->SetPoint1(newPoint);
    m_transform->TransformPoint(plane->GetPoint2(), newPoint);
    plane->SetPoint2(newPoint);
    m_transform->TransformPoint(plane->GetOrigin(), newPoint);
    plane->SetOrigin(newPoint);

    plane->Update();
}

void QMPRExtension::updateThickSlab(double value)
{
    m_thickSlab = value;
    m_thickSlabSlider->setValue((int) value);
    updatePlane(m_coronalPlaneSource, m_coronalReslice);
    updateControls();
}

void QMPRExtension::updateThickSlab(int value)
{
    m_thickSlab = (double) value;
    m_thickSlabSpinBox->setValue(m_thickSlab);
    updatePlane(m_coronalPlaneSource, m_coronalReslice);
    updateControls();
}

void QMPRExtension::readSettings()
{
    Settings settings;

    if (settings.getValue(MPRSettings::HorizontalSplitterGeometry).toByteArray().isEmpty())
    {
        QList<int> list;
        list << this->size().width()/2 << this->size().width()/2;
        m_horizontalSplitter->setSizes(list);
    }
    else
    {
        settings.restoreGeometry(MPRSettings::HorizontalSplitterGeometry, m_horizontalSplitter);
    }

    if (settings.getValue(MPRSettings::VerticalSplitterGeometry).toByteArray().isEmpty())
    {
        QList<int> list;
        list << this->size().height()/2 << this->size().height()/2;
        m_verticalSplitter->setSizes(list);
    }
    else
    {
        settings.restoreGeometry(MPRSettings::VerticalSplitterGeometry, m_verticalSplitter);
    }
}

void QMPRExtension::writeSettings()
{
    Settings settings;

    settings.saveGeometry(MPRSettings::HorizontalSplitterGeometry, m_horizontalSplitter);
    settings.saveGeometry(MPRSettings::VerticalSplitterGeometry, m_verticalSplitter);
}

};  // End namespace udg


