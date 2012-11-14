#include "q2dviewerextension.h"

#include "volume.h"
#include "image.h"
#include "logging.h"
#include "qwindowlevelcombobox.h"
#include "q2dviewerwidget.h"
#include "menugridwidget.h"
#include "tablemenu.h"
#include "patient.h"
#include "study.h"
#include "toolmanager.h"
#include "toolconfiguration.h"
#include "windowlevelpresetstooldata.h"
#include "qdicomdumpbrowser.h"
#include "statswatcher.h"
#include "automaticsynchronizationtool.h"
#include "automaticsynchronizationmanager.h"
#include "automaticsynchronizationtooldata.h"
#include "coresettings.h"
// Per poder fer screenshots desde menú
#include "screenshottool.h"
#include "toolproxy.h"
#include "q2dviewersettings.h"
#include "shortcutmanager.h"

#ifndef STARVIEWER_LITE
#include "qrelatedstudieswidget.h"
#include "hangingprotocolmanager.h"
#include "relatedstudiesmanager.h"
#include "qexportertool.h"
#endif

#include <QMenu>
#include <QAction>
#include <QPoint>
#include <QGridLayout>
#include <QProgressDialog>
#include <QMessageBox>

namespace udg {

Q2DViewerExtension::Q2DViewerExtension(QWidget *parent)
 : QWidget(parent), m_mainVolume(0), m_patient(0), m_lastSelectedViewer(0)
{
    setupUi(this);
    Q2DViewerSettings().init();

#ifdef STARVIEWER_LITE
    m_roiButton->hide();
    m_angleToolButton->hide();
    m_openAngleToolButton->hide();
    m_axialViewToolButton->hide();
    m_coronalViewToolButton->hide();
    m_sagitalViewToolButton->hide();
    m_orientationButtonsLabel->hide();
    m_thickSlabLabel->hide();
    m_thickSlabWidget->hide();
    m_referenceLinesToolButton->hide();
    m_cursor3DToolButton->hide();
    m_relatedStudiesToolButton->hide();
    m_screenshotsExporterToolButton->hide();
    m_downButtonGrid->hide();
    m_automaticSynchronizationToolButton->hide();
    m_synchronizeAllViewersButton->hide();
    m_desynchronizeAllViewersButton->hide();

#else
    m_hangingProtocolManager = 0;
    m_automaticSynchronizationManager = 0;
    m_relatedStudiesManager = 0;
#endif

    m_hangingProtocolsMenu = new MenuGridWidget(this);
    m_seriesTableGrid = new TableMenu(this);

    m_dicomDumpCurrentDisplayedImage = new QDICOMDumpBrowser(this);

#ifndef STARVIEWER_LITE
    m_relatedStudiesWidget = new QRelatedStudiesWidget(this);
    m_relatedStudiesToolButton->setEnabled(false);
    m_relatedStudiesToolButton->setToolTip(tr("Search related studies"));
    m_screenshotsExporterToolButton->setToolTip(tr("Export viewer image(s) to DICOM and send them to a PACS server"));
#endif

    m_showViewersTextualInformationAction = new QAction(this);
    m_showViewersTextualInformationAction->setCheckable(true);
    m_showViewersTextualInformationAction->setChecked(true);
    m_showViewersTextualInformationAction->setText(tr("Text"));
    m_showViewersTextualInformationAction->setToolTip(tr("Show/Hide viewer's textual information"));
    m_showViewersTextualInformationAction->setStatusTip(m_showViewersTextualInformationAction->toolTip());
    m_showViewersTextualInformationAction->setIcon(QIcon(":/images/showViewersInformation.png"));
    connect(m_showViewersTextualInformationAction, SIGNAL(toggled(bool)), SLOT(showViewersTextualInformation(bool)));
    
    m_showOverlaysAction = new QAction(this);
    m_showOverlaysAction->setCheckable(true);
    m_showOverlaysAction->setChecked(true);
    m_showOverlaysAction->setText(tr("Overlays"));
    m_showOverlaysAction->setToolTip(tr("Show/Hide image overlays"));
    m_showOverlaysAction->setStatusTip(m_showOverlaysAction->toolTip());
    m_showOverlaysAction->setIcon(QIcon(":/images/showOverlays.png"));
    connect(m_showOverlaysAction, SIGNAL(toggled(bool)), SLOT(showImageOverlays(bool)));

    m_showDisplayShuttersAction = new QAction(this);
    m_showDisplayShuttersAction->setCheckable(true);
    m_showDisplayShuttersAction->setChecked(true);
    m_showDisplayShuttersAction->setText(tr("Shutters"));
    m_showDisplayShuttersAction->setToolTip(tr("Show/Hide shutter layer"));
    m_showDisplayShuttersAction->setStatusTip(m_showDisplayShuttersAction->toolTip());
    m_showDisplayShuttersAction->setIcon(QIcon(":/images/showDisplayShutters.png"));
    connect(m_showDisplayShuttersAction, SIGNAL(toggled(bool)), SLOT(showDisplayShutters(bool)));
    
    m_viewerLayersToolButton->setDefaultAction(m_showViewersTextualInformationAction);
    
    QMenu *viewerInformationMenu = new QMenu(this);
    viewerInformationMenu->addAction(m_showOverlaysAction);
    viewerInformationMenu->addAction(m_showDisplayShuttersAction);
    m_viewerLayersToolButton->setPopupMode(QToolButton::MenuButtonPopup);
    m_viewerLayersToolButton->setMenu(viewerInformationMenu);
    
    m_dicomDumpToolButton->setToolTip(tr("Dump DICOM information of the current image"));
    m_windowLevelComboBox->setToolTip(tr("Choose Window/Level Presets"));

    readSettings();
    createConnections();
    initializeTools();

    // Incorporem estadístiques
    m_statsWatcher = new StatsWatcher("2D Extension", this);
    m_statsWatcher->addClicksCounter(m_slicingToolButton);
    m_statsWatcher->addClicksCounter(m_zoomToolButton);
    m_statsWatcher->addClicksCounter(m_roiButton);
    m_statsWatcher->addClicksCounter(m_distanceToolButton);
    m_statsWatcher->addClicksCounter(m_angleToolButton);
    m_statsWatcher->addClicksCounter(m_eraserToolButton);

    m_statsWatcher->addClicksCounter(m_axialViewToolButton);
    m_statsWatcher->addClicksCounter(m_sagitalViewToolButton);
    m_statsWatcher->addClicksCounter(m_coronalViewToolButton);

    m_statsWatcher->addClicksCounter(m_dicomDumpToolButton);
    m_statsWatcher->addClicksCounter(m_viewerLayersToolButton);

    m_statsWatcher->addClicksCounter(m_rotateClockWiseToolButton);
    m_statsWatcher->addClicksCounter(m_flipHorizontalToolButton);
    m_statsWatcher->addClicksCounter(m_flipVerticalToolButton);

    m_statsWatcher->addClicksCounter(m_cursor3DToolButton);
    m_statsWatcher->addClicksCounter(m_referenceLinesToolButton);

    m_statsWatcher->addClicksCounter(m_relatedStudiesToolButton);
}

Q2DViewerExtension::~Q2DViewerExtension()
{
    writeSettings();

#ifndef STARVIEWER_LITE
    // L'objecte es crea quan fem un setInput. Per tant, fem la comprovació.
    if (m_relatedStudiesWidget)
    {
        delete m_relatedStudiesWidget;
    }

    if (m_hangingProtocolManager)
    {
        delete m_hangingProtocolManager;
    }
#endif

    delete m_hangingProtocolsMenu;
    delete m_seriesTableGrid;
    delete m_dicomDumpCurrentDisplayedImage;
}

void Q2DViewerExtension::createConnections()
{
    // Menus
    connect(m_downButtonGrid, SIGNAL(clicked (bool)), SLOT(showPredefinedGrid()));
    connect(m_buttonGrid, SIGNAL(clicked (bool)), SLOT(showInteractiveTable()));

    // Connexions del menu
    connect(m_hangingProtocolsMenu, SIGNAL(selectedGrid(int)), this, SLOT(setHangingProtocol(int)));
    connect(m_seriesTableGrid, SIGNAL(selectedGrid(int, int)), this, SLOT(setGrid(int, int)));

    // Per mostrar la informació DICOM de la imatge que s'està veient en aquell moment
    connect(m_dicomDumpToolButton, SIGNAL(clicked()), SLOT(showDicomDumpCurrentDisplayedImage()));

    // Connexions necessaries amb els canvis al layout
    connect(m_workingArea, SIGNAL(viewerAdded(Q2DViewerWidget*)), SLOT(activateNewViewer(Q2DViewerWidget*)));
    connect(m_workingArea, SIGNAL(selectedViewerChanged(Q2DViewerWidget*)), SLOT(changeSelectedViewer(Q2DViewerWidget*)));

#ifndef STARVIEWER_LITE
    // Per mostrar exportació
    connect(m_screenshotsExporterToolButton, SIGNAL(clicked()), SLOT(showScreenshotsExporterDialog()));

    connect(m_relatedStudiesWidget, SIGNAL(downloadingStudies()), this, SLOT(changeToRelatedStudiesDownloadingIcon()));
    connect(m_relatedStudiesWidget, SIGNAL(studiesDownloaded()), this, SLOT(changeToRelatedStudiesDefaultIcon()));
    connect(m_relatedStudiesToolButton, SIGNAL(clicked (bool)), SLOT(showRelatedStudiesWidget()));
    connect(m_workingArea, SIGNAL(manualSynchronizationStateChanged(bool)), SLOT(manualSynchronizationActivated(bool)));

#endif

    connect(m_thickSlabWidget, SIGNAL(maximumThicknessModeToggled(bool)), SLOT(enableMaximumThicknessMode(bool)));
}

void Q2DViewerExtension::setInput(Volume *input)
{
    m_mainVolume = input;

#ifdef STARVIEWER_LITE
    Q2DViewerWidget *viewerWidget = m_workingArea->addViewer("0.0\\1.0\\1.0\\0.0");
    viewerWidget->setInput(m_mainVolume);
    m_workingArea->setSelectedViewer(m_workingArea->getViewerWidget(0));
#else
    // Aplicació dels hanging protocols
    if (m_hangingProtocolManager != 0)
    {
        m_hangingProtocolManager->cancelHangingProtocolDownloading();
        delete m_hangingProtocolManager;
    }
    m_hangingProtocolManager = new HangingProtocolManager();

    QList<HangingProtocol*> hangingCandidates = m_hangingProtocolManager->searchHangingProtocols(m_patient);

    if (hangingCandidates.size() == 0)
    {
        // No hi ha hanging protocols
        Q2DViewerWidget *viewerWidget = m_workingArea->addViewer("0.0\\1.0\\1.0\\0.0");
        viewerWidget->setInputAsynchronously(m_mainVolume);
    }
    else
    {
        m_hangingProtocolManager->setBestHangingProtocol(m_patient, hangingCandidates, m_workingArea);
    }

    m_workingArea->setSelectedViewer(m_workingArea->getViewerWidget(0));
    m_hangingProtocolsMenu->setHangingItems(hangingCandidates);

    connect(m_patient, SIGNAL(patientFused()), SLOT(searchHangingProtocols()));

    // Habilitem la possibilitat de buscar estudis relacionats.
    m_relatedStudiesToolButton->setEnabled(true);
    if (m_mainVolume)
    {
        m_relatedStudiesWidget->searchStudiesOf(m_mainVolume->getPatient());
    }
    else
    {
        // Si no tenim volum, farem servir el pacient actual directament
        m_relatedStudiesWidget->searchStudiesOf(m_patient);
    }

    searchPreviousStudiesWithHangingProtocols();
#endif
}

#ifndef STARVIEWER_LITE
void Q2DViewerExtension::searchAndApplyBestHangingProtocol()
{
    QList<HangingProtocol*> hangingCandidates = m_hangingProtocolManager->searchHangingProtocols(m_patient);
    m_hangingProtocolManager->setBestHangingProtocol(m_patient, hangingCandidates, m_workingArea);
}

void Q2DViewerExtension::searchHangingProtocols()
{
    QList<HangingProtocol*> hangingCandidates = m_hangingProtocolManager->searchHangingProtocols(m_patient);
    m_hangingProtocolsMenu->setHangingItems(hangingCandidates);
    searchPreviousStudiesWithHangingProtocols();
}

void Q2DViewerExtension::searchPreviousStudiesWithHangingProtocols()
{
    m_hangingProtocolsMenu->setSearchingItem(true);

    // 2.- Creacio de la classe per trobar estudis relacionats
    m_relatedStudiesManager = new RelatedStudiesManager();

    // 3.- Es connecta el signal per quan acabi
    connect(m_relatedStudiesManager, SIGNAL(queryStudiesFinished(QList<Study*>)), SLOT(addHangingProtocolsWithPrevious(QList<Study*>)));

    // 4.- Es busquen els previs
    Study *studyToSearchFrom = 0;
    if (m_mainVolume)
    {
        studyToSearchFrom = m_mainVolume->getStudy();
        
    }
    else
    {
        // En el cas que no tinguéssim un input vàlid, ho farem a partir del pacient actual
        studyToSearchFrom = m_patient->getStudies().first();
    }
    m_relatedStudiesManager->queryMergedPreviousStudies(studyToSearchFrom);
}

void Q2DViewerExtension::addHangingProtocolsWithPrevious(QList<Study*> studies)
{
    disconnect(m_relatedStudiesManager, SIGNAL(queryStudiesFinished(QList<Study*>)), this, SLOT(addHangingProtocolsWithPrevious(QList<Study*>)));

    QList<HangingProtocol*> hangingCandidates = m_hangingProtocolManager->searchHangingProtocols(m_patient, studies);
    m_hangingProtocolsMenu->setHangingItems(hangingCandidates);
    m_hangingProtocolsMenu->setSearchingItem(false);
}
#endif

void Q2DViewerExtension::setupDefaultToolsForModalities(const QStringList &modalities)
{

#ifndef STARVIEWER_LITE

    Settings settings;

    bool enableReferenceLinesForMR = settings.getValue(CoreSettings::EnableQ2DViewerReferenceLinesForMR).toBool();
    bool enableReferenceLinesForCT = settings.getValue(CoreSettings::EnableQ2DViewerReferenceLinesForCT).toBool();
    
    if (modalities.contains("MR") && enableReferenceLinesForMR || modalities.contains("CT") && enableReferenceLinesForCT)
    {
        m_referenceLinesToolButton->defaultAction()->setChecked(true);
    }
    else
    {
        m_referenceLinesToolButton->defaultAction()->setChecked(false);
    }

    bool enableAutomaticSynchronizationForMR = settings.getValue(CoreSettings::EnableQ2DViewerAutomaticSynchronizationForMR).toBool();
    bool enableAutomaticSynchronizationForCT = settings.getValue(CoreSettings::EnableQ2DViewerAutomaticSynchronizationForCT).toBool();
    
    if (modalities.contains("MR") && enableAutomaticSynchronizationForMR || modalities.contains("CT") && enableAutomaticSynchronizationForCT)
    {
        m_automaticSynchronizationToolButton->defaultAction()->setChecked(true);
    }
    else
    {
        m_automaticSynchronizationToolButton->defaultAction()->setChecked(false);
    }

#endif
}

void Q2DViewerExtension::setupDefaultLeftButtonTool()
{
    if (!m_patient)
    {
        return;
    }

    // Ara és super txapussa i només mirarà el primer estudi
    Study *study = m_patient->getStudies().first();
    if (study)
    {
        Settings settings;
        bool enableZoom = false;

        QStringList modalitiesWithZoomList = settings.getValue(CoreSettings::ModalitiesWithZoomToolByDefault).toString().split(";", QString::SkipEmptyParts);
        foreach (const QString &modality, modalitiesWithZoomList)
        {
            if (study->getModalities().contains(modality))
            {
                enableZoom = true;
                break;
            }
        }

        if (enableZoom)
        {
            m_slicingToolButton->defaultAction()->toggle();
            m_zoomToolButton->defaultAction()->trigger();
        }
        else
        {
            m_slicingToolButton->defaultAction()->trigger();
        }
    }
}

void Q2DViewerExtension::showPredefinedGrid()
{
    QPoint point = m_buttonGrid->mapToGlobal(QPoint(0, 0));
    m_hangingProtocolsMenu->move(point.x(), (point.y() + m_buttonGrid->frameGeometry().height()));
    m_hangingProtocolsMenu->show();
}

void Q2DViewerExtension::showInteractiveTable()
{
    QPoint point = m_buttonGrid->mapToGlobal(QPoint(0, 0));
    m_seriesTableGrid->move(point.x(), (point.y() + m_buttonGrid->frameGeometry().height()));
    m_seriesTableGrid->show();
}

#ifndef STARVIEWER_LITE
void Q2DViewerExtension::showRelatedStudiesWidget()
{
    QPoint point = m_relatedStudiesToolButton->mapToGlobal(QPoint(0, 0));
    m_relatedStudiesWidget->move(point.x(), (point.y() + m_relatedStudiesToolButton->frameGeometry().height()));
    m_relatedStudiesWidget->show();
}
#endif

Patient* Q2DViewerExtension::getPatient() const
{
    return m_patient;
}

void Q2DViewerExtension::setPatient(Patient *patient)
{
    m_patient = patient;

    if (m_patient)
    {
        QStringList modalitiesList;
        foreach (Study *study, m_patient->getStudies())
        {
            if (study)
            {
                modalitiesList << study->getModalities();
            }
        }

        setupDefaultToolsForModalities(modalitiesList);
    }

    setupDefaultLeftButtonTool();
}

void Q2DViewerExtension::initializeTools()
{
    // Creem el tool manager
    m_toolManager = new ToolManager(this);
    // Obtenim les accions de cada tool que volem
    m_zoomToolButton->setDefaultAction(m_toolManager->registerTool("ZoomTool"));
    m_slicingToolButton->setDefaultAction(m_toolManager->registerTool("SlicingTool"));
    m_toolManager->registerTool("TranslateTool");
    m_toolManager->registerTool("WindowLevelTool");
    m_referenceLinesToolButton->setDefaultAction(m_toolManager->registerTool("ReferenceLinesTool"));

    m_distanceToolButton->setDefaultAction(m_toolManager->registerTool("DistanceTool"));

    m_eraserToolButton->setDefaultAction(m_toolManager->registerTool("EraserTool"));
#ifndef STARVIEWER_LITE
    // Afegim un menú al botó de zoom per incorporar la tool de zoom focalitzat
    m_zoomToolButton->setPopupMode(QToolButton::MenuButtonPopup);
    QMenu *zoomToolMenu = new QMenu(this);
    m_zoomToolButton->setMenu(zoomToolMenu);
    zoomToolMenu->addAction(m_toolManager->registerTool("MagnifyingGlassTool"));

    connect(m_toolManager->getRegisteredToolAction("ZoomTool"), SIGNAL(triggered()), SLOT(rearrangeZoomToolsMenu()));
    connect(m_toolManager->getRegisteredToolAction("MagnifyingGlassTool"), SIGNAL(triggered()), SLOT(rearrangeZoomToolsMenu()));

    // Afegim un menú al botó de distància per incorporar l'eina de distància perpendicular
    m_distanceToolButton->setPopupMode(QToolButton::MenuButtonPopup);
    QMenu *distanceToolMenu = new QMenu(this);
    m_distanceToolButton->setMenu(distanceToolMenu);
    distanceToolMenu->addAction(m_toolManager->registerTool("PerpendicularDistanceTool"));
    connect(m_toolManager->getRegisteredToolAction("DistanceTool"), SIGNAL(triggered()), SLOT(rearrangeDistanceToolsMenu()));
    connect(m_toolManager->getRegisteredToolAction("PerpendicularDistanceTool"), SIGNAL(triggered()), SLOT(rearrangeDistanceToolsMenu()));

    m_roiButton->setDefaultAction(m_toolManager->registerTool("EllipticalROITool"));
    // Afegim un menú al botó de PolylineROI per incorporar la tool de ROI el·líptica
    m_roiButton->setPopupMode(QToolButton::MenuButtonPopup);
    QMenu *roiToolMenu = new QMenu(this);
    m_roiButton->setMenu(roiToolMenu);
    roiToolMenu->addAction(m_toolManager->registerTool("MagicROITool"));
    roiToolMenu->addAction(m_toolManager->registerTool("PolylineROITool"));
    roiToolMenu->addAction(m_toolManager->registerTool("CircleTool"));
    
    connect(m_toolManager->getRegisteredToolAction("EllipticalROITool"), SIGNAL(triggered()), SLOT(rearrangeROIToolsMenu()));
    connect(m_toolManager->getRegisteredToolAction("MagicROITool"), SIGNAL(triggered()), SLOT(rearrangeROIToolsMenu()));
    connect(m_toolManager->getRegisteredToolAction("PolylineROITool"), SIGNAL(triggered()), SLOT(rearrangeROIToolsMenu()));
    connect(m_toolManager->getRegisteredToolAction("CircleTool"), SIGNAL(triggered()), SLOT(rearrangeROIToolsMenu()));
    
    m_cursor3DToolButton->setDefaultAction(m_toolManager->registerTool("Cursor3DTool"));
    
    m_angleToolButton->setDefaultAction(m_toolManager->registerTool("AngleTool"));
    // Afegim un menú al botó d'angle per incorporar la tool d'angles oberts
    m_angleToolButton->setPopupMode(QToolButton::MenuButtonPopup);
    QMenu *angleToolMenu = new QMenu(this);
    m_angleToolButton->setMenu(angleToolMenu);
    angleToolMenu->addAction(m_toolManager->registerTool("NonClosedAngleTool"));
    connect(m_toolManager->getRegisteredToolAction("AngleTool"), SIGNAL(triggered()), SLOT(rearrangeAngleToolsMenu()));
    connect(m_toolManager->getRegisteredToolAction("NonClosedAngleTool"), SIGNAL(triggered()), SLOT(rearrangeAngleToolsMenu()));

    m_automaticSynchronizationToolButton->setDefaultAction(m_toolManager->registerTool("AutomaticSynchronizationTool"));
#endif
    m_voxelInformationToolButton->setDefaultAction(m_toolManager->registerTool("VoxelInformationTool"));
    // Registrem les eines de valors predefinits de window level, slicing per teclat i sincronització
    m_toolManager->registerTool("WindowLevelPresetsTool");
    m_toolManager->registerTool("SlicingKeyboardTool");
    m_toolManager->registerTool("SlicingWheelTool");
    m_toolManager->registerTool("SynchronizeTool");
    // Cal marcar l'acció de la tool com a checked perquè sempre estigui disponible
    // Si no ho féssim, cada cop que es canviés de tool, el toolmanager faria un refreshConections() i
    // les finestres sincronitzades es desactivarien (ticket #1236)
    m_toolManager->getRegisteredToolAction("SynchronizeTool")->setChecked(true);

    // Registrem les "Action Tool"
#ifndef STARVIEWER_LITE
    m_sagitalViewAction = m_toolManager->registerActionTool("SagitalViewActionTool");
    m_coronalViewAction = m_toolManager->registerActionTool("CoronalViewActionTool");
    m_axialViewToolButton->setDefaultAction(m_toolManager->registerActionTool("AxialViewActionTool"));
    m_sagitalViewToolButton->setDefaultAction(m_sagitalViewAction);
    m_coronalViewToolButton->setDefaultAction(m_coronalViewAction);
#endif
    m_rotateClockWiseToolButton->setDefaultAction(m_toolManager->registerActionTool("RotateClockWiseActionTool"));
    m_rotateCounterClockWiseToolButton->setDefaultAction(m_toolManager->registerActionTool("RotateCounterClockWiseActionTool"));
    m_flipHorizontalToolButton->setDefaultAction(m_toolManager->registerActionTool("HorizontalFlipActionTool"));
    m_flipVerticalToolButton->setDefaultAction(m_toolManager->registerActionTool("VerticalFlipActionTool"));
    m_restoreToolButton->setDefaultAction(m_toolManager->registerActionTool("RestoreActionTool"));
    m_invertToolButton->setDefaultAction(m_toolManager->registerActionTool("InvertWindowLevelActionTool"));
    // Afegim un menú al botó d'erase per incorporar l'acció d'esborrar tot el que hi ha al visor
    m_eraserToolButton->setPopupMode(QToolButton::MenuButtonPopup);
    QMenu *eraserToolMenu = new QMenu(this);
    m_eraserToolButton->setMenu(eraserToolMenu);
    eraserToolMenu->addAction(m_toolManager->registerActionTool("ClearCurrentViewAnnotationsActionTool"));

    // Definim els grups exclusius
    QStringList leftButtonExclusiveTools;

#ifdef STARVIEWER_LITE
    leftButtonExclusiveTools << "ZoomTool" << "SlicingTool" << "DistanceTool" << "PerpendicularDistanceTool" << "EraserTool";
#else
    leftButtonExclusiveTools << "ZoomTool" << "SlicingTool" << "PolylineROITool" << "DistanceTool" << "PerpendicularDistanceTool" << "EraserTool" << "AngleTool" << "NonClosedAngleTool"
                             << "Cursor3DTool" << "EllipticalROITool" << "MagicROITool" << "CircleTool" << "MagnifyingGlassTool";
#endif

    m_toolManager->addExclusiveToolsGroup("LeftButtonGroup", leftButtonExclusiveTools);

    // Activem les tools que volem tenir per defecte, això és com si clickéssim a cadascun dels ToolButton
    QStringList defaultTools;
    defaultTools << "WindowLevelPresetsTool" << "SlicingKeyboardTool" << "SlicingTool" << "SlicingWheelTool" << "WindowLevelTool" << "TranslateTool";
    m_toolManager->triggerTools(defaultTools);

    //
    // Casos especials de Tools
    //
#ifndef STARVIEWER_LITE
    // TODO De moment fem exclusiu la tool de sincronització i la de cursor 3d manualment perque la
    // sincronització no té el model de totes les tools
    connect(m_toolManager->getRegisteredToolAction("Cursor3DTool"), SIGNAL(triggered()), SLOT(disableSynchronization()));

    // SYNCHRONIZE TOOLS
    m_synchronizeAllAction = new QAction(this);
    m_synchronizeAllAction->setShortcuts(ShortcutManager::getShortcuts(Shortcuts::SynchronizeAllViewers));
    m_synchronizeAllAction->setToolTip(tr("Activate manual synchronization in all viewers (%1)").arg(m_synchronizeAllAction->shortcut().toString()));
    m_synchronizeAllAction->setIcon(QIcon(":/images/linkAll.png"));
    m_synchronizeAllAction->setText(tr("All"));

    m_synchronizeAllViewersButton->setIcon(m_synchronizeAllAction->icon());
    m_synchronizeAllViewersButton->setToolTip(m_synchronizeAllAction->toolTip());
    m_synchronizeAllViewersButton->setText(m_synchronizeAllAction->text());

    m_synchronizeAllViewersButton->setDefaultAction(m_synchronizeAllAction);
    connect(m_synchronizeAllAction, SIGNAL(triggered()), SLOT(activateManualSynchronizationInAllViewers()));

    m_desynchronizeAllAction = new QAction(this);
    m_desynchronizeAllAction->setShortcuts(ShortcutManager::getShortcuts(Shortcuts::DesynchronizeAllViewers));
    m_desynchronizeAllAction->setToolTip(tr("Deactivate manual synchronization in all viewers (%1)").arg(m_desynchronizeAllAction->shortcut().toString()));
    m_desynchronizeAllAction->setIcon(QIcon(":/images/unlinkAll.png"));
    m_desynchronizeAllAction->setText(tr("None"));

    m_desynchronizeAllViewersButton->setIcon(m_desynchronizeAllAction->icon());
    m_desynchronizeAllViewersButton->setToolTip(m_desynchronizeAllAction->toolTip());
    m_desynchronizeAllViewersButton->setText(m_desynchronizeAllAction->text());

    m_desynchronizeAllViewersButton->setDefaultAction(m_desynchronizeAllAction);
    connect(m_desynchronizeAllAction, SIGNAL(triggered()), SLOT(deactivateManualSynchronizationInAllViewers()));
    connect(m_toolManager->getRegisteredToolAction("AutomaticSynchronizationTool"), SIGNAL(triggered(bool)), SLOT(enableAutomaticSynchronizationToViewer(bool)));
    
#endif

    // SCREEN SHOT TOOL
    // Activem l'eina d'screen shot, que sempre estarà activa
    // TODO Tot això es podria convertir més endavant en dues Action Tools en comptes d'aquesta Tool
    m_screenShotTriggerAction = m_toolManager->registerTool("ScreenShotTool");
    m_toolManager->triggerTool("ScreenShotTool");
    // Fem que l'screen shot sigui una mica més acessible afegint-li un menú en el que podem escollir quina acció realitzar
    // d'alguna manera tot això són una mica uns HACKS ja que el mecanisme de funcionament d'aquesta tool és una mica diferent
    // i caldria tenir en compte tools d'aquests tipus per donar-li cabuda en la infraestructura de tools.
    m_screenShotToolButton->setPopupMode(QToolButton::InstantPopup);
    m_screenShotToolButton->setCheckable(false);
    m_singleShotAction = new QAction(this);
    m_singleShotAction->setText(tr("Save current series image..."));
    m_singleShotAction->setShortcuts(ShortcutManager::getShortcuts(Shortcuts::SaveSingleScreenShot));
    m_singleShotAction->setToolTip(tr("Save the current image in a standard image format"));

    m_multipleShotAction = new QAction(this);
    m_multipleShotAction->setText(tr("Save all images of the current series..."));
    m_multipleShotAction->setToolTip(tr("Save all the images in the selected viewer in a standard image format"));
    m_multipleShotAction->setShortcuts(ShortcutManager::getShortcuts(Shortcuts::SaveWholeSeriesScreenShot));

    QMenu *screenShotMenu = new QMenu(this);
    m_screenShotToolButton->setMenu(screenShotMenu);

    screenShotMenu->addAction(m_singleShotAction);
    screenShotMenu->addAction(m_multipleShotAction);

    m_screenShotToolButton->setIcon(m_screenShotTriggerAction->icon());
    m_screenShotToolButton->setToolTip(m_screenShotTriggerAction->toolTip());
    m_screenShotToolButton->setText(m_screenShotTriggerAction->text());
}

void Q2DViewerExtension::activateNewViewer(Q2DViewerWidget *newViewerWidget)
{
    // Activem/Desactivem les capes d'annotacions segons l'estat del botó
    // Informació de l'estudi
    newViewerWidget->getViewer()->enableAnnotation(Q2DViewer::WindowInformationAnnotation | Q2DViewer::PatientOrientationAnnotation |
                                                   Q2DViewer::SliceAnnotation | Q2DViewer::PatientInformationAnnotation |
                                                   Q2DViewer::AcquisitionInformationAnnotation, m_showViewersTextualInformationAction->isChecked());
    // Overlays
    newViewerWidget->getViewer()->showImageOverlays(m_showOverlaysAction->isChecked());
    // Shutters
    newViewerWidget->getViewer()->showDisplayShutters(m_showDisplayShuttersAction->isChecked());

    // Afegim l'eina de sincronització pel nou viewer
    // Per defecte només configurem la sincronització a nivell d'scroll
    ToolConfiguration *synchronizeConfiguration = new ToolConfiguration();
    synchronizeConfiguration->addAttribute("Slicing", QVariant(true));
    m_toolManager->setViewerTool(newViewerWidget->getViewer(), "SynchronizeTool", synchronizeConfiguration);

    // Li indiquem les tools que li hem configurat per defecte a tothom
    m_toolManager->setupRegisteredTools(newViewerWidget->getViewer());
}

void Q2DViewerExtension::changeSelectedViewer(Q2DViewerWidget *viewerWidget)
{
    if (viewerWidget != m_lastSelectedViewer)
    {
        // TODO Canviar aquestes connexions i desconnexions per dos mètodes el qual
        // enviin el senyal al visualitzador que toca.
        if (m_lastSelectedViewer)
        {

#ifndef STARVIEWER_LITE
            disconnect(m_lastSelectedViewer->getViewer(), SIGNAL(volumeChanged(Volume*)), this, SLOT(validePhases()));
#endif

            disconnect(m_lastSelectedViewer->getViewer(), SIGNAL(viewChanged(int)), this, SLOT(updateDICOMInformationButton()));
            // És necessari associar cada cop al viewer actual les associacions del menú de la tool d'screen shot
            ScreenShotTool *screenShotTool = dynamic_cast<ScreenShotTool*>(m_lastSelectedViewer->getViewer()->getToolProxy()->getTool("ScreenShotTool"));
            disconnect(m_singleShotAction, SIGNAL(triggered()), screenShotTool, SLOT(singleCapture()));
            disconnect(m_multipleShotAction, SIGNAL(triggered()), screenShotTool, SLOT(completeCapture()));
            // Desactivem les "ActionTool" pel visor que acaba de deseleccionar-se
            m_toolManager->disableRegisteredActionTools(m_lastSelectedViewer->getViewer());
        }

        // Actualitzem l'últim viewer seleccionat
        m_lastSelectedViewer = viewerWidget;
        // Si el viewer seleccionat no és nul
        if (m_lastSelectedViewer)
        {
            Q2DViewer *selected2DViewer = viewerWidget->getViewer();

#ifndef STARVIEWER_LITE
            validePhases();
            connect(viewerWidget->getViewer(), SIGNAL(volumeChanged(Volume*)), SLOT(validePhases()));
#endif
            connect(viewerWidget->getViewer(), SIGNAL(viewChanged(int)), SLOT(updateDICOMInformationButton()));

            // És necessari associar cada cop al viewer actual les associacions del menú de la tool d'screen shot
            ScreenShotTool *screenShotTool = dynamic_cast<ScreenShotTool*>(viewerWidget->getViewer()->getToolProxy()->getTool("ScreenShotTool"));
            if (screenShotTool)
            {
                connect(m_singleShotAction, SIGNAL(triggered()), screenShotTool, SLOT(singleCapture()));
                connect(m_multipleShotAction, SIGNAL(triggered()), screenShotTool, SLOT(completeCapture()));
            }

            // TODO Potser hi hauria alguna manera més elegant, com tenir un slot a WindowLevelPresetsToolData
            // que es digués activateCurrentPreset() i el poguéssim connectar a algun signal
            WindowLevelPresetsToolData *windowLevelData = selected2DViewer->getWindowLevelData();
            m_windowLevelComboBox->setPresetsData(windowLevelData);
            // TODO Canviem m_windowLevelComboBox->selectPreset() per windowLevelData->activatePreset per solucionar els tickets
            // 1226 i 1227, però potser s'hauria de millorar una mica el funcionament i/o la interfície de les classes implicades
            // Pendent de revisar perquè tingui un disseny i interfície més adeqequats (combo box, sobre tot)
            windowLevelData->activatePreset(windowLevelData->getCurrentPreset());

            m_cineController->setQViewer(selected2DViewer);
            m_thickSlabWidget->link(selected2DViewer);
            updateDICOMInformationButton();

            // Activem les "ActionTool" pel visor seleccionat
            m_toolManager->enableRegisteredActionTools(selected2DViewer);
        }
        else
        {
            // Si és nul vol dir que en aquell moment o bé no tenim cap
            // visor seleccionat o bé no n'existeix cap. És per això que
            // cal desvincular els widgets adients de qualsevol visor.
            m_windowLevelComboBox->clearPresets();
            m_cineController->setQViewer(0);
            m_thickSlabWidget->unlink();
        }
    }
}

void Q2DViewerExtension::enableMaximumThicknessMode(bool enable)
{
    if (!enable)
    {
        return;
    }

    int numberOfViewers = m_workingArea->getNumberOfViewers();

    QApplication::setOverrideCursor(Qt::WaitCursor);
    for (int viewerNumber = 0; viewerNumber < numberOfViewers; ++viewerNumber)
    {
        Q2DViewer *viewer = m_workingArea->getViewerWidget(viewerNumber)->getViewer();
        if (viewer->isThickSlabActive() && !viewer->isActive())
        {
            viewer->setSlabThickness(viewer->getMaximumSlice() + 1);
        }
    }
    QApplication::restoreOverrideCursor();
}

void Q2DViewerExtension::showImageOverlays(bool show)
{
    int numberOfViewers = m_workingArea->getNumberOfViewers();

    for (int viewerNumber = 0; viewerNumber < numberOfViewers; ++viewerNumber)
    {
        m_workingArea->getViewerWidget(viewerNumber)->getViewer()->showImageOverlays(show);
    }
}

void Q2DViewerExtension::showDisplayShutters(bool show)
{
    int numberOfViewers = m_workingArea->getNumberOfViewers();

    for (int viewerNumber = 0; viewerNumber < numberOfViewers; ++viewerNumber)
    {
        m_workingArea->getViewerWidget(viewerNumber)->getViewer()->showDisplayShutters(show);
    }
}

void Q2DViewerExtension::showViewersTextualInformation(bool show)
{
    int numberOfViewers = m_workingArea->getNumberOfViewers();

    for (int viewerNumber = 0; viewerNumber < numberOfViewers; ++viewerNumber)
    {
        m_workingArea->getViewerWidget(viewerNumber)->getViewer()->enableAnnotation(Q2DViewer::WindowInformationAnnotation |
                                                                                    Q2DViewer::PatientOrientationAnnotation |
                                                                                    Q2DViewer::SliceAnnotation |
                                                                                    Q2DViewer::PatientInformationAnnotation |
                                                                                    Q2DViewer::AcquisitionInformationAnnotation, show);
    }
}

void Q2DViewerExtension::showDicomDumpCurrentDisplayedImage()
{
    Q2DViewerWidget *selectedViewerWidget = m_workingArea->getSelectedViewer();
    if (selectedViewerWidget)
    {
        m_dicomDumpCurrentDisplayedImage->setCurrentDisplayedImage(selectedViewerWidget->getViewer()->getCurrentDisplayedImage());
        m_dicomDumpCurrentDisplayedImage->show();
    }
}

void Q2DViewerExtension::rearrangeToolsMenu(QToolButton *menuButton)
{
    QList<QAction*> actions;
    actions << menuButton->defaultAction() << menuButton->menu()->actions();

    bool found = false;
    int i = 0;
    while (!found && i < actions.count())
    {
        if (actions.at(i)->isChecked())
        {
            found = true;
        }
        ++i;
    }

    if (found)
    {
        menuButton->setDefaultAction(actions.takeAt(i - 1));
        menuButton->menu()->clear();
        menuButton->menu()->addActions(actions);
    }
}

#ifndef STARVIEWER_LITE
void Q2DViewerExtension::showScreenshotsExporterDialog()
{
    Q2DViewerWidget *selectedViewerWidget = m_workingArea->getSelectedViewer();
    if (selectedViewerWidget)
    {
        if (selectedViewerWidget->getViewer()->getInput() == NULL)
        {
            QMessageBox::warning(this, tr("Export to DICOM"), tr("This action is not allowed because the selected viewer is empty."));
        }
        else
        {
            QExporterTool exporter(selectedViewerWidget->getViewer());
            exporter.exec();
        }
    }
}

void Q2DViewerExtension::rearrangeROIToolsMenu()
{
    rearrangeToolsMenu(m_roiButton);
}

void Q2DViewerExtension::rearrangeAngleToolsMenu()
{
    rearrangeToolsMenu(m_angleToolButton);
}

void Q2DViewerExtension::rearrangeZoomToolsMenu()
{
    rearrangeToolsMenu(m_zoomToolButton);
}

void Q2DViewerExtension::validePhases()
{
    m_axialViewToolButton->setEnabled(true);
    if (m_workingArea->getSelectedViewer()->hasPhases())
    {
        m_sagitalViewToolButton->setEnabled(false);
        m_coronalViewToolButton->setEnabled(false);
    }
    else
    {
        m_sagitalViewToolButton->setEnabled(true);
        m_coronalViewToolButton->setEnabled(true);
    }
}
#endif

void Q2DViewerExtension::rearrangeDistanceToolsMenu()
{
    rearrangeToolsMenu(m_distanceToolButton);
}

void Q2DViewerExtension::updateDICOMInformationButton()
{
    Q2DViewerWidget *viewerWidget = m_workingArea->getSelectedViewer();
    if (!viewerWidget)
    {
        m_dicomDumpToolButton->setEnabled(false);
        return;
    }

    if (viewerWidget->getViewer()->getInput())
    {
        if (viewerWidget->getViewer()->getView() == Q2DViewer::Axial)
        {
            m_dicomDumpToolButton->setEnabled(true);
        }
        else
        {
            m_dicomDumpToolButton->setEnabled(false);
        }
    }
    else
    {
        m_dicomDumpToolButton->setEnabled(false);
    }
}

void Q2DViewerExtension::readSettings()
{
    Settings settings;
    m_profile = settings.getValue(Q2DViewerSettings::UserProfile).toString();
}

void Q2DViewerExtension::writeSettings()
{
}

void Q2DViewerExtension::disableSynchronization()
{
    // TODO Mètode per desactivar l'eina i el boto de sincronització dels visualitzadors quan
    // es selecciona l'eina de cursor3D, per solucionar-ho de forma xapussa perquè l'eina de
    // sincronització encara no té el mateix format que la resta
    Q2DViewerWidget *viewer;

    int numberOfViewers = m_workingArea->getNumberOfViewers();

    for (int viewerNumber = 0; viewerNumber < numberOfViewers; ++viewerNumber)
    {
        viewer = m_workingArea->getViewerWidget(viewerNumber);
        viewer->enableSynchronization(false);
    }
}

void Q2DViewerExtension::enableSynchronizationButton(bool enableButton)
{
    Q2DViewerWidget *viewer;

    int numberOfViewers = m_workingArea->getNumberOfViewers();

    for (int viewerNumber = 0; viewerNumber < numberOfViewers; ++viewerNumber)
    {
        viewer = m_workingArea->getViewerWidget(viewerNumber);
        if (viewer->getViewer()->getInput())
        {
            viewer->enableSynchronizationButton(enableButton);
        }
    }
}

#ifndef STARVIEWER_LITE
void Q2DViewerExtension::setHangingProtocol(int hangingProtocolNumber)
{
    m_hangingProtocolManager->applyHangingProtocol(hangingProtocolNumber, m_workingArea, m_patient);
}

void Q2DViewerExtension::changeToRelatedStudiesDownloadingIcon()
{
    m_relatedStudiesToolButton->setIcon(QIcon(QString(":images/cal_downloading.png")));
}

void Q2DViewerExtension::changeToRelatedStudiesDefaultIcon()
{
    m_relatedStudiesToolButton->setIcon(QIcon(QString(":images/cal.png")));
}

void Q2DViewerExtension::searchPreviousStudiesOfMostRecentStudy()
{
    Study *recentStudy = NULL;

    // Només busquem si hi ha el volum principal
    if (m_mainVolume)
    {
        foreach (Study *study, m_mainVolume->getPatient()->getStudies())
        {
            if (recentStudy == NULL)
            {
                recentStudy = study;
            }
            else
            {
                if (study->getDateTime() > recentStudy->getDateTime())
                {
                    recentStudy = study;
                }
            }
        }
        m_relatedStudiesWidget->searchPreviousStudiesOf(recentStudy);
    }
}

void Q2DViewerExtension::updateRelatedStudiesWidget()
{
    m_relatedStudiesWidget->updateList();
}

void Q2DViewerExtension::enableAutomaticSynchronizationToViewer(bool enable)
{
    if (enable)
    {
        disableSynchronization();//Desactivem sincronització manual, però no el botó

        Tool *tool = m_workingArea->getSelectedViewer()->getViewer()->getToolProxy()->getTool("AutomaticSynchronizationTool");
        AutomaticSynchronizationToolData *toolData = dynamic_cast<AutomaticSynchronizationToolData*>(tool->getToolData());

        if (m_automaticSynchronizationManager)
        {
            delete m_automaticSynchronizationManager;
        }

        m_automaticSynchronizationManager = new AutomaticSynchronizationManager(toolData, m_workingArea);
        m_automaticSynchronizationManager->initialize();
    }
}

void Q2DViewerExtension::activateManualSynchronizationInAllViewers()
{
    int numberOfViewers = m_workingArea->getNumberOfViewers();

    for (int i = 0; i < numberOfViewers; i++)
    {
        Q2DViewerWidget *widget = m_workingArea->getViewerWidget(i);
        
        if (widget->getViewer()->getInput())
        {
            widget->enableSynchronization(true);
        }
    }
    m_automaticSynchronizationToolButton->defaultAction()->setChecked(false);
    m_toolManager->deactivateTool("AutomaticSynchronizationTool");

}

void Q2DViewerExtension::deactivateManualSynchronizationInAllViewers()
{
    int numberOfViewers = m_workingArea->getNumberOfViewers();

    for (int i = 0; i < numberOfViewers; i++)
    {
        Q2DViewerWidget *widget = m_workingArea->getViewerWidget(i);
        
        if (widget->getViewer()->getInput())
        {
            widget->enableSynchronization(false);
        }
    } 
}

void Q2DViewerExtension::manualSynchronizationActivated(bool activated)
{
    if (activated)
    {
        m_automaticSynchronizationToolButton->defaultAction()->setChecked(false);
        m_toolManager->deactivateTool("AutomaticSynchronizationTool");
    }
    
}

#endif

void Q2DViewerExtension::setGrid(int rows, int columns)
{
#ifndef STARVIEWER_LITE
    m_hangingProtocolManager->cancelHangingProtocolDownloading();
#endif
    m_workingArea->setGrid(rows, columns);
}

}
