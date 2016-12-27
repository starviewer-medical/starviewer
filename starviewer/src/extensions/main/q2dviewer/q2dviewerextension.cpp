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

#include "q2dviewerextension.h"

#include "volume.h"
#include "image.h"
#include "logging.h"
#include "qvoilutcombobox.h"
#include "q2dviewerwidget.h"
#include "qhangingprotocolswidget.h"
#include "tablemenu.h"
#include "patient.h"
#include "study.h"
#include "toolmanager.h"
#include "toolconfiguration.h"
#include "voilutpresetstooldata.h"
#include "qdicomdumpbrowser.h"
#include "statswatcher.h"
#include "automaticsynchronizationtool.h"
#include "automaticsynchronizationtooldata.h"
#include "coresettings.h"
// Per poder fer screenshots desde menú
#include "screenshottool.h"
#include "toolproxy.h"
#include "q2dviewersettings.h"
#include "shortcutmanager.h"
#include "transferfunctionmodel.h"
#include "transferfunctionmodelfiller.h"
#include "hangingprotocol.h"
#include "externalapplicationsmanager.h"

#ifndef STARVIEWER_LITE
#include "qrelatedstudieswidget.h"
#include "relatedstudiesmanager.h"
#include "qexportertool.h"
#include "syncactionmanager.h"
#include "syncactionsconfiguration.h"
#include "syncactionsconfigurationmenu.h"
#include "viewerslayouttosyncactionmanageradapter.h"
#include "syncactionsconfigurationhandler.h"
#endif

#include <QMenu>
#include <QAction>
#include <QPoint>
#include <QGridLayout>
#include <QProgressDialog>
#include <QMessageBox>
#include <QListView>

#include "layoutmanager.h"

namespace udg {

// Minimum size in pixels of the unfolded combo box showing all the items. This is used for the window level and transfer function combos.
const int MinimumComboBoxViewWidth = 170;

Q2DViewerExtension::Q2DViewerExtension(QWidget *parent)
 : QWidget(parent), m_patient(0), m_lastSelectedViewer(0)
{
    setupUi(this);
    Q2DViewerSettings().init();

    // We set a minimum size in the .ui file to see the widget and unset it here to avoid the button being too small if the window doesn't fit in the screen
    m_thickSlabWidget->setMinimumSize(0, 0);

#ifdef STARVIEWER_LITE
    m_axialViewToolButton->hide();
    m_coronalViewToolButton->hide();
    m_sagitalViewToolButton->hide();
    m_orientationButtonsLabel->hide();
    m_thickSlabWidget->hide();
    m_referenceLinesToolButton->hide();
    m_cursor3DToolButton->hide();
    m_relatedStudiesToolButton->hide();
    m_screenshotsExporterToolButton->hide();
    m_hangingProtocolsMenuButton->hide();
    m_automaticSynchronizationToolButton->hide();
    m_synchronizeAllViewersButton->hide();
    m_desynchronizeAllViewersButton->hide();
#else
    m_syncActionsConfigurationHandler = new SyncActionsConfigurationHandler;
    m_syncActionManager = new SyncActionManager(m_syncActionsConfigurationHandler->getConfiguration(Q2DViewerSettings::KeyPrefix), this);
    m_layoutToSyncActionManagerAdapter = new ViewersLayoutToSyncActionManagerAdapter(m_workingArea, m_syncActionManager, this);
    m_relatedStudiesManager = new RelatedStudiesManager();
#endif

    m_hangingProtocolsMenu = new QHangingProtocolsWidget(this);
    m_viewersLayoutGrid = new TableMenu(this);

    m_dicomDumpCurrentDisplayedImage = new QDICOMDumpBrowser(this);

#ifndef STARVIEWER_LITE
    m_relatedStudiesWidget = new QRelatedStudiesWidget(m_relatedStudiesManager, this);
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
    m_showViewersTextualInformationAction->setIcon(QIcon(":/images/icons/annotations.svg"));
    connect(m_showViewersTextualInformationAction, SIGNAL(toggled(bool)), SLOT(showViewersTextualInformation(bool)));
    
    m_showOverlaysAction = new QAction(this);
    m_showOverlaysAction->setCheckable(true);
    m_showOverlaysAction->setChecked(true);
    m_showOverlaysAction->setText(tr("Overlays"));
    m_showOverlaysAction->setToolTip(tr("Show/Hide image overlays"));
    m_showOverlaysAction->setStatusTip(m_showOverlaysAction->toolTip());
    m_showOverlaysAction->setIcon(QIcon(":/images/icons/overlays.svg"));
    connect(m_showOverlaysAction, SIGNAL(toggled(bool)), SLOT(showImageOverlays(bool)));

    m_showDisplayShuttersAction = new QAction(this);
    m_showDisplayShuttersAction->setCheckable(true);
    m_showDisplayShuttersAction->setChecked(true);
    m_showDisplayShuttersAction->setText(tr("Shutters"));
    m_showDisplayShuttersAction->setToolTip(tr("Show/Hide shutter layer"));
    m_showDisplayShuttersAction->setStatusTip(m_showDisplayShuttersAction->toolTip());
    m_showDisplayShuttersAction->setIcon(QIcon(":/images/icons/shutter.svg"));
    connect(m_showDisplayShuttersAction, SIGNAL(toggled(bool)), SLOT(showDisplayShutters(bool)));
    
    m_viewerLayersToolButton->setDefaultAction(m_showViewersTextualInformationAction);
    
    QMenu *viewerInformationMenu = new QMenu(this);
    viewerInformationMenu->addAction(m_showOverlaysAction);
    viewerInformationMenu->addAction(m_showDisplayShuttersAction);
    m_viewerLayersToolButton->setPopupMode(QToolButton::MenuButtonPopup);
    m_viewerLayersToolButton->setMenu(viewerInformationMenu);
    
    m_dicomDumpToolButton->setToolTip(tr("Dump DICOM information of the current image"));
    m_voiLutComboBox->setToolTip(tr("Choose a VOI LUT preset"));

    readSettings();
    createConnections();
    initializeTools();

    // Incorporem estadístiques
    m_statsWatcher = new StatsWatcher("2D Extension", this);
    m_statsWatcher->addClicksCounter(m_slicingToolButton);
    m_statsWatcher->addClicksCounter(m_zoomToolButton);
    m_statsWatcher->addClicksCounter(m_drawingToolButton);
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

    m_emptyTransferFunctionModel = new TransferFunctionModel(this);

    m_voiLutComboBox->view()->setTextElideMode(Qt::ElideRight);
    m_voiLutComboBox->view()->setMinimumWidth(MinimumComboBoxViewWidth);
    m_transferFunctionComboBox->view()->setTextElideMode(Qt::ElideRight);
    m_transferFunctionComboBox->view()->setMinimumWidth(MinimumComboBoxViewWidth);
}

Q2DViewerExtension::~Q2DViewerExtension()
{
    writeSettings();

#ifndef STARVIEWER_LITE
    delete m_syncActionsConfigurationHandler;
    delete m_relatedStudiesWidget;
    delete m_relatedStudiesManager;
#endif

    delete m_hangingProtocolsMenu;
    delete m_viewersLayoutGrid;
    delete m_dicomDumpCurrentDisplayedImage;
}

void Q2DViewerExtension::createConnections()
{
    // Menus
    connect(m_hangingProtocolsMenuButton, SIGNAL(clicked(bool)), SLOT(showAvailableHangingProtocolsMenu()));
    connect(m_viewersLayoutGridButton, SIGNAL(clicked(bool)), SLOT(showViewersLayoutGridTable()));

    // Connexions del menu
    connect(m_viewersLayoutGrid, SIGNAL(selectedGrid(int, int)), SLOT(setGrid(int, int)));

    // Per mostrar la informació DICOM de la imatge que s'està veient en aquell moment
    connect(m_dicomDumpToolButton, SIGNAL(clicked()), SLOT(showDicomDumpCurrentDisplayedImage()));

    // Connexions necessaries amb els canvis al layout
    connect(m_workingArea, SIGNAL(viewerAdded(Q2DViewerWidget*)), SLOT(activateNewViewer(Q2DViewerWidget*)));
    connect(m_workingArea, SIGNAL(selectedViewerChanged(Q2DViewerWidget*)), SLOT(changeSelectedViewer(Q2DViewerWidget*)));

#ifndef STARVIEWER_LITE
    // Per mostrar exportació
    connect(m_screenshotsExporterToolButton, SIGNAL(clicked()), SLOT(showScreenshotsExporterDialog()));

    connect(m_relatedStudiesWidget, SIGNAL(downloadingStudies()), SLOT(changeToRelatedStudiesDownloadingIcon()));
    connect(m_relatedStudiesWidget, SIGNAL(studiesDownloaded()), SLOT(changeToRelatedStudiesDefaultIcon()));
    connect(m_relatedStudiesToolButton, SIGNAL(clicked (bool)), SLOT(showRelatedStudiesWidget()));
    connect(m_workingArea, SIGNAL(manualSynchronizationStateChanged(bool)), SLOT(manualSynchronizationActivated(bool)));
#endif

    connect(m_thickSlabWidget, SIGNAL(maximumThicknessModeToggled(bool)), SLOT(enableMaximumThicknessMode(bool)));

    connect(m_workingArea, SIGNAL(fusionLayout2x1FirstRequested(QList<Volume*>,AnatomicalPlane)),
            SLOT(setFusionLayout2x1First(QList<Volume*>,AnatomicalPlane)));
    connect(m_workingArea, SIGNAL(fusionLayout2x1SecondRequested(QList<Volume*>,AnatomicalPlane)),
            SLOT(setFusionLayout2x1Second(QList<Volume*>,AnatomicalPlane)));
    connect(m_workingArea, SIGNAL(fusionLayout3x1Requested(QList<Volume*>,AnatomicalPlane)), SLOT(setFusionLayout3x1(QList<Volume*>,AnatomicalPlane)));
    connect(m_workingArea, SIGNAL(fusionLayout2x3FirstRequested(QList<Volume*>)), SLOT(setFusionLayout2x3First(QList<Volume*>)));
    connect(m_workingArea, SIGNAL(fusionLayout2x3SecondRequested(QList<Volume*>)), SLOT(setFusionLayout2x3Second(QList<Volume*>)));
    connect(m_workingArea, SIGNAL(fusionLayout3x3Requested(QList<Volume*>)), SLOT(setFusionLayout3x3(QList<Volume*>)));
}

#ifdef STARVIEWER_LITE
void Q2DViewerExtension::setInput(Volume *input)
{
    m_workingArea->setGrid(1, 1);
    m_workingArea->setSelectedViewer(m_workingArea->getViewerWidget(0));
    if (input)
    {
        m_workingArea->getViewerWidget(0)->setInput(input);
    }
}
#endif

void Q2DViewerExtension::setupDefaultToolsForModalities(const QStringList &modalities)
{
#ifndef STARVIEWER_LITE
    Settings settings;
    bool enableReferenceLinesForMR = settings.getValue(CoreSettings::EnableQ2DViewerReferenceLinesForMR).toBool();
    bool enableReferenceLinesForCT = settings.getValue(CoreSettings::EnableQ2DViewerReferenceLinesForCT).toBool();

    if ((modalities.contains("MR") && enableReferenceLinesForMR) || (modalities.contains("CT") && enableReferenceLinesForCT))
    {
        m_referenceLinesToolButton->defaultAction()->setChecked(true);
    }
    else
    {
        m_referenceLinesToolButton->defaultAction()->setChecked(false);
    }

    bool enableAutomaticSynchronizationForMR = settings.getValue(CoreSettings::EnableQ2DViewerAutomaticSynchronizationForMR).toBool();
    bool enableAutomaticSynchronizationForCT = settings.getValue(CoreSettings::EnableQ2DViewerAutomaticSynchronizationForCT).toBool();

    if ((modalities.contains("MR") && enableAutomaticSynchronizationForMR) || (modalities.contains("CT") && enableAutomaticSynchronizationForCT))
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

    Study *study = m_patient->getStudy(m_currentStudyUID);
    if (study)
    {
        Settings settings;
        bool enableZoom = false;

        QStringList modalitiesWithZoomList = settings.getValueAsQStringList(CoreSettings::ModalitiesWithZoomToolByDefault);
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

void Q2DViewerExtension::setupLayoutManager()
{
    m_layoutManager = new LayoutManager(m_patient, m_workingArea, this);
    connect(m_layoutManager, &LayoutManager::hangingProtocolCandidatesFound, m_hangingProtocolsMenu, &QHangingProtocolsWidget::setItems);
    connect(m_layoutManager, &LayoutManager::activeCombinedHangingProtocolChanged,
            m_hangingProtocolsMenu, &QHangingProtocolsWidget::setActiveCombinedHangingProtocol);
    connect(m_layoutManager, &LayoutManager::activeCurrentHangingProtocolChanged,
            m_hangingProtocolsMenu, &QHangingProtocolsWidget::setActiveCurrentHangingProtocol);
    connect(m_layoutManager, &LayoutManager::activePriorHangingProtocolChanged,
            m_hangingProtocolsMenu, &QHangingProtocolsWidget::setActivePriorHangingProtocol);
    connect(m_hangingProtocolsMenu, &QHangingProtocolsWidget::selectedCurrent, m_layoutManager, &LayoutManager::setCurrentHangingProtocol);
    connect(m_hangingProtocolsMenu, &QHangingProtocolsWidget::selectedPrior, m_layoutManager, &LayoutManager::setPriorHangingProtocol);
    connect(m_hangingProtocolsMenu, &QHangingProtocolsWidget::selectedCombined, m_layoutManager, &LayoutManager::setCombinedHangingProtocol);
    connect(m_relatedStudiesWidget, SIGNAL(workingStudiesChanged(QString, QString)), this, SLOT(setWorkingStudies(QString, QString)));

    // Actions to show the next o previous hanging protocol of the list. Currently, it can only be carried out through keyboard
    QAction *nextHangingProtocolAction = new QAction(this);
    nextHangingProtocolAction->setText("Next Hanging Protocol");
    nextHangingProtocolAction->setShortcuts(ShortcutManager::getShortcuts(Shortcuts::NextHangingProtocol));
    connect(nextHangingProtocolAction, SIGNAL(triggered()), m_layoutManager, SLOT(applyNextHangingProtocol()));

    QAction *previousHangingProtocolAction = new QAction(this);
    previousHangingProtocolAction->setText("Previous Hanging Protocol");
    previousHangingProtocolAction->setShortcuts(ShortcutManager::getShortcuts(Shortcuts::PreviousHangingProtocol));
    connect(previousHangingProtocolAction, SIGNAL(triggered()), m_layoutManager, SLOT(applyPreviousHangingProtocol()));

    QAction *toggleComparativeModeAction = new QAction(this);
    toggleComparativeModeAction->setText("Comparative Mode");
    toggleComparativeModeAction->setShortcuts(ShortcutManager::getShortcuts(Shortcuts::ToggleComparativeStudiesMode));
    connect(toggleComparativeModeAction, SIGNAL(triggered()), m_relatedStudiesWidget, SLOT(toggleComparativeMode()));

    this->addAction(previousHangingProtocolAction);
    this->addAction(nextHangingProtocolAction);
    this->addAction(toggleComparativeModeAction);

    m_layoutManager->initialize();
}

void Q2DViewerExtension::showAvailableHangingProtocolsMenu()
{
    showWidgetBelowButton(m_hangingProtocolsMenu, m_viewersLayoutGridButton);
}

void Q2DViewerExtension::showViewersLayoutGridTable()
{
    showWidgetBelowButton(m_viewersLayoutGrid, m_viewersLayoutGridButton);
}

#ifndef STARVIEWER_LITE
void Q2DViewerExtension::showRelatedStudiesWidget()
{
    showWidgetBelowButton(m_relatedStudiesWidget, m_relatedStudiesToolButton);
}
#endif

void Q2DViewerExtension::showWidgetBelowButton(QWidget *widget, QAbstractButton *button)
{
    QPoint point = button->mapToGlobal(QPoint(0, 0));
    widget->move(point.x(), (point.y() + button->frameGeometry().height()));
    widget->show();
}

Patient* Q2DViewerExtension::getPatient() const
{
    return m_patient;
}

void Q2DViewerExtension::setPatient(Patient *patient)
{
    m_patient = patient;

    if (m_patient)
    {
        m_currentStudyUID = m_patient->getStudies().first()->getInstanceUID();
        setupLayoutManager();
        setupDefaultToolsForModalities(m_patient->getModalities());
    }

    setupDefaultLeftButtonTool();

#ifndef STARVIEWER_LITE
    setupPropagation();
    // Habilitem la possibilitat de buscar estudis relacionats.
    m_relatedStudiesToolButton->setEnabled(true);
    m_relatedStudiesWidget->searchStudiesOf(m_patient);
    connect(m_patient, SIGNAL(studyAdded(Study*)), m_relatedStudiesWidget, SLOT(updateList()));
#endif
}

void Q2DViewerExtension::setCurrentStudy(const QString &studyUID)
{
    m_relatedStudiesWidget->setCurrentStudy(studyUID);
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

    m_drawingToolButton->addAction(m_toolManager->registerTool("DistanceTool"));

    m_eraserToolButton->setDefaultAction(m_toolManager->registerTool("EraserTool"));

#ifndef STARVIEWER_LITE
    m_zoomToolButton->addAction(m_toolManager->registerTool("MagnifyingGlassTool"));

    m_drawingToolButton->addAction(m_toolManager->registerTool("PerpendicularDistanceTool"));

    m_drawingToolButton->addAction(m_toolManager->registerTool("EllipticalROITool"));
    m_drawingToolButton->addAction(m_toolManager->registerTool("MagicROITool"));
    m_drawingToolButton->addAction(m_toolManager->registerTool("PolylineROITool"));
    m_drawingToolButton->addAction(m_toolManager->registerTool("CircleTool"));

    m_cursor3DToolButton->setDefaultAction(m_toolManager->registerTool("Cursor3DTool"));

    m_drawingToolButton->addAction(m_toolManager->registerTool("AngleTool"));
    m_drawingToolButton->addAction(m_toolManager->registerTool("NonClosedAngleTool"));

    m_automaticSynchronizationToolButton->setDefaultAction(m_toolManager->registerTool("AutomaticSynchronizationTool"));
#endif

    m_voxelInformationToolButton->setDefaultAction(m_toolManager->registerTool("VoxelInformationTool"));
    // Registrem les eines de valors predefinits de window level, slicing per teclat i sincronització
    m_toolManager->registerTool("VoiLutPresetsTool");
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
    m_invertToolButton->setDefaultAction(m_toolManager->registerActionTool("InvertVoiLutActionTool"));
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
    defaultTools << "VoiLutPresetsTool" << "SlicingKeyboardTool" << "SlicingTool" << "SlicingWheelTool" << "WindowLevelTool" << "TranslateTool";
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
    m_synchronizeAllAction->setIcon(QIcon(":/images/icons/insert-link.svg"));
    m_synchronizeAllAction->setText(tr("All"));

    m_synchronizeAllViewersButton->setIcon(m_synchronizeAllAction->icon());
    m_synchronizeAllViewersButton->setToolTip(m_synchronizeAllAction->toolTip());
    m_synchronizeAllViewersButton->setText(m_synchronizeAllAction->text());

    m_synchronizeAllViewersButton->setDefaultAction(m_synchronizeAllAction);
    connect(m_synchronizeAllAction, SIGNAL(triggered()), SLOT(activateManualSynchronizationInAllViewers()));

    m_desynchronizeAllAction = new QAction(this);
    m_desynchronizeAllAction->setShortcuts(ShortcutManager::getShortcuts(Shortcuts::DesynchronizeAllViewers));
    m_desynchronizeAllAction->setToolTip(tr("Deactivate manual synchronization in all viewers (%1)").arg(m_desynchronizeAllAction->shortcut().toString()));
    m_desynchronizeAllAction->setIcon(QIcon(":/images/icons/remove-link.svg"));
    m_desynchronizeAllAction->setText(tr("None"));

    m_desynchronizeAllViewersButton->setIcon(m_desynchronizeAllAction->icon());
    m_desynchronizeAllViewersButton->setToolTip(m_desynchronizeAllAction->toolTip());
    m_desynchronizeAllViewersButton->setText(m_desynchronizeAllAction->text());

    m_desynchronizeAllViewersButton->setDefaultAction(m_desynchronizeAllAction);
    connect(m_desynchronizeAllAction, SIGNAL(triggered()), SLOT(deactivateManualSynchronizationInAllViewers()));
    // Must use the button directly instead of the action because the button's toogled signal is emitted before the action's triggered signal.
    // This way we make sure that this slot, which turns off manual sync, is called before auto-sync is turned on.
    connect(m_automaticSynchronizationToolButton, SIGNAL(toggled(bool)), SLOT(enableAutomaticSynchronizationToViewer(bool)));

    m_propagationAction = new QAction(this);
    m_propagationAction->setShortcuts(ShortcutManager::getShortcuts(Shortcuts::Propagation));
    m_propagationAction->setToolTip(tr("Propagate properties between viewers (%1)").arg(m_propagationAction->shortcut().toString()));
    m_propagationAction->setIcon(QIcon(":/images/icons/feed-subscribe.svg"));
    m_propagationAction->setText(tr("Propagate"));
    m_propagationAction->setCheckable(true);
    m_propagateToolButton->setDefaultAction(m_propagationAction);
    connect(m_propagationAction, SIGNAL(toggled(bool)), m_syncActionManager, SLOT(enable(bool)));

    SyncActionsConfigurationMenu *menu = new SyncActionsConfigurationMenu(m_syncActionManager->getSyncActionsConfiguration(), this);
    m_propagateToolButton->setPopupMode(QToolButton::MenuButtonPopup);
    m_propagateToolButton->setMenu(menu);

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
    connect(newViewerWidget, SIGNAL(doubleClicked(Q2DViewerWidget*)), SLOT(handleViewerDoubleClick(Q2DViewerWidget*)));

    // Activem/Desactivem les capes d'annotacions segons l'estat del botó
    // Informació de l'estudi
    newViewerWidget->getViewer()->enableAnnotation(AllAnnotations, m_showViewersTextualInformationAction->isChecked());
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
            disconnect(m_lastSelectedViewer->getViewer(), SIGNAL(viewChanged(int)), this, SLOT(updateDICOMInformationButton()));
            disconnect(m_lastSelectedViewer->getViewer(), SIGNAL(viewerStatusChanged()), this, SLOT(updateExporterToolButton()));
            disconnect(m_lastSelectedViewer->getViewer(), SIGNAL(volumeChanged(Volume*)), this, SLOT(updateTransferFunctionComboBoxWithCurrentViewerModel()));
            disconnect(m_lastSelectedViewer->getViewer(), SIGNAL(volumeChanged(Volume*)), this, SLOT(updateExternalApplicationsWithCurrentView(Volume*)));

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

            connect(viewerWidget->getViewer(), SIGNAL(viewChanged(int)), SLOT(updateDICOMInformationButton()));
            connect(m_lastSelectedViewer->getViewer(), SIGNAL(viewerStatusChanged()), this, SLOT(updateExporterToolButton()));
            connect(selected2DViewer, SIGNAL(volumeChanged(Volume*)), this, SLOT(updateTransferFunctionComboBoxWithCurrentViewerModel()));
            connect(m_lastSelectedViewer->getViewer(), SIGNAL(volumeChanged(Volume*)), this, SLOT(updateExternalApplicationsWithCurrentView(Volume*)));

            // Update external application parameters
            if (!m_lastSelectedViewer->getViewer()->getInputs().isEmpty())
            {
                ExternalApplicationsManager::instance()->setParameters(m_lastSelectedViewer->getViewer()->getInput(0));
            }
            else
            {
                ExternalApplicationsManager::instance()->cleanParameters();
            }

            // És necessari associar cada cop al viewer actual les associacions del menú de la tool d'screen shot
            ScreenShotTool *screenShotTool = dynamic_cast<ScreenShotTool*>(viewerWidget->getViewer()->getToolProxy()->getTool("ScreenShotTool"));
            if (screenShotTool)
            {
                connect(m_singleShotAction, SIGNAL(triggered()), screenShotTool, SLOT(singleCapture()));
                connect(m_multipleShotAction, SIGNAL(triggered()), screenShotTool, SLOT(completeCapture()));
            }

            VoiLutPresetsToolData *voiLutData = selected2DViewer->getVoiLutData();
            m_voiLutComboBox->setPresetsData(voiLutData);

            updateTransferFunctionComboBox(selected2DViewer->getTransferFunctionModel());

            m_cineController->setQViewer(selected2DViewer);
            m_thickSlabWidget->link(selected2DViewer);
            updateDICOMInformationButton();
            updateExporterToolButton();

            // Activem les "ActionTool" pel visor seleccionat
            m_toolManager->enableRegisteredActionTools(selected2DViewer);
        }
        else
        {
            // Si és nul vol dir que en aquell moment o bé no tenim cap
            // visor seleccionat o bé no n'existeix cap. És per això que
            // cal desvincular els widgets adients de qualsevol visor.
            m_voiLutComboBox->clearPresets();
            m_cineController->setQViewer(0);
            m_thickSlabWidget->unlink();
            updateTransferFunctionComboBox(0);
            ExternalApplicationsManager::instance()->cleanParameters();
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
            viewer->setSlabThickness(viewer->getMaximumSlabThickness());
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
        m_workingArea->getViewerWidget(viewerNumber)->getViewer()->enableAnnotation(AllAnnotations, show);
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

#ifndef STARVIEWER_LITE
void Q2DViewerExtension::showScreenshotsExporterDialog()
{
    Q2DViewerWidget *selectedViewerWidget = m_workingArea->getSelectedViewer();
    if (selectedViewerWidget)
    {
        if (!selectedViewerWidget->getViewer()->hasInput())
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
#endif

void Q2DViewerExtension::updateDICOMInformationButton()
{
    Q2DViewerWidget *viewerWidget = m_workingArea->getSelectedViewer();
    if (!viewerWidget)
    {
        m_dicomDumpToolButton->setEnabled(false);
        return;
    }

    if (viewerWidget->getViewer()->hasInput())
    {
        if (viewerWidget->getViewer()->getView() == OrthogonalPlane::XYPlane)
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

void Q2DViewerExtension::updateExporterToolButton()
{
    Q2DViewerWidget *viewerWidget = m_workingArea->getSelectedViewer();

    if (viewerWidget)
    {
        m_screenshotsExporterToolButton->setEnabled(viewerWidget->getViewer()->getViewerStatus() == QViewer::VisualizingVolume);
    }
    else
    {
        m_screenshotsExporterToolButton->setEnabled(false);
    }
}

void Q2DViewerExtension::readSettings()
{
    Settings settings;
    m_profile = settings.getValue(Q2DViewerSettings::UserProfile).toString();
}

void Q2DViewerExtension::writeSettings()
{
#ifndef STARVIEWER_LITE
    m_syncActionsConfigurationHandler->saveConfiguration(m_syncActionManager->getSyncActionsConfiguration(), Q2DViewerSettings::KeyPrefix);
#endif
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

#ifndef STARVIEWER_LITE
void Q2DViewerExtension::changeToRelatedStudiesDownloadingIcon()
{
    m_relatedStudiesToolButton->setIcon(QIcon(QString(":images/icons/view-calendar-download.svg")));
}

void Q2DViewerExtension::changeToRelatedStudiesDefaultIcon()
{
    m_relatedStudiesToolButton->setIcon(QIcon(QString(":images/icons/view-calendar.svg")));
}

void Q2DViewerExtension::enableAutomaticSynchronizationToViewer(bool enable)
{
    if (enable)
    {
        // Desactivem sincronització manual, però no el botó
        disableSynchronization();
    }
}

void Q2DViewerExtension::activateManualSynchronizationInAllViewers()
{
    int numberOfViewers = m_workingArea->getNumberOfViewers();

    for (int i = 0; i < numberOfViewers; i++)
    {
        Q2DViewerWidget *widget = m_workingArea->getViewerWidget(i);
        
        if (widget->getViewer()->hasInput())
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
        
        if (widget->getViewer()->hasInput())
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

        // WORKAROUND: Cursor 3D is incompatible with manual synchronization, so if cursor 3D is enabled when manual sync is enabled,
        //             we enable the slicing tool to disable cursor 3D.
        if (m_cursor3DToolButton->isChecked())
        {
            m_toolManager->triggerTool("SlicingTool");
        }
    }
}

void Q2DViewerExtension::setupPropagation()
{
    if (m_patient)
    {
        Study *study = m_patient->getStudy(m_currentStudyUID);

        if (study)
        {
            Settings settings;
            QSet<QString> modalitiesWithPropagation = settings.getValueAsQStringList(CoreSettings::ModalitiesWithPropagationEnabledByDefault).toSet();

            // Propagation will be enabled if any of the configured modalities is present in the current patient modalities
            if (!modalitiesWithPropagation.intersect(study->getModalities().toSet()).isEmpty())
            {
                m_propagateToolButton->defaultAction()->setChecked(true);
            }
        }
    }
}
#endif

void Q2DViewerExtension::setGrid(int rows, int columns)
{
#ifndef STARVIEWER_LITE
    m_layoutManager->cancelOngoingOperations();
#endif
    m_layoutManager->setGrid(rows, columns);
}

void Q2DViewerExtension::updateTransferFunctionComboBoxWithCurrentViewerModel()
{
    updateTransferFunctionComboBox(m_workingArea->getSelectedViewer()->getViewer()->getTransferFunctionModel());
}

void Q2DViewerExtension::updateExternalApplicationsWithCurrentView(Volume *volume)
{
    if (volume)
    {
        ExternalApplicationsManager::instance()->setParameters(volume);
    }
}

void Q2DViewerExtension::updateTransferFunctionComboBox(TransferFunctionModel *transferFunctionModel)
{
    disconnect(m_transferFunctionComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setTransferFunctionToCurrentViewer(int)));

    if (!transferFunctionModel)
    {
        transferFunctionModel = m_emptyTransferFunctionModel;
    }

    TransferFunctionModelFiller filler;
    filler.removeEmptyTransferFunction(qobject_cast<TransferFunctionModel*>(m_transferFunctionComboBox->model()));
    filler.addEmptyTransferFunction(transferFunctionModel);
    m_transferFunctionComboBox->setModel(transferFunctionModel);

    Q2DViewerWidget *selectedViewerWidget = m_workingArea->getSelectedViewer();

    if (selectedViewerWidget)
    {
        const Q2DViewer *viewer = m_workingArea->getSelectedViewer()->getViewer();
        // For now, always get the transfer function of the last volume
        const TransferFunction &transferFunction = viewer->getVolumeTransferFunction(viewer->getNumberOfInputs() - 1);
        int index = transferFunctionModel->getIndexOf(transferFunction, true);

        if (index >= 0)
        {
            m_transferFunctionComboBox->setCurrentIndex(index);
        }
    }

    connect(m_transferFunctionComboBox, SIGNAL(currentIndexChanged(int)), SLOT(setTransferFunctionToCurrentViewer(int)));
}

void Q2DViewerExtension::setTransferFunctionToCurrentViewer(int transferFunctionIndex)
{
    Q2DViewerWidget *currentViewerWidget = m_workingArea->getSelectedViewer();

    if (!currentViewerWidget || transferFunctionIndex < 0)
    {
        return;
    }

    Q2DViewer *viewer = currentViewerWidget->getViewer();
    // For now, always set the transfer function to the last volume
    viewer->setVolumeTransferFunction(viewer->getNumberOfInputs() - 1, viewer->getTransferFunctionModel()->getTransferFunction(transferFunctionIndex));
    viewer->render();
}

void Q2DViewerExtension::handleViewerDoubleClick(Q2DViewerWidget *viewerWidget)
{
    QSet<QString> toolsIncompatibleWithDoubleClickMaximization;
    toolsIncompatibleWithDoubleClickMaximization << "AngleTool" << "NonClosedAngleTool" << "DistanceTool" << "PerpendicularDistanceTool"
                                                 << "MagicROITool" << "PolylineROITool" << "EraserTool";

    foreach (const QString &tool, toolsIncompatibleWithDoubleClickMaximization)
    {
        if (viewerWidget->getViewer()->getToolProxy()->isToolActive(tool))
        {
            return;
        }
    }

    m_workingArea->toggleMaximization(viewerWidget);
}

void Q2DViewerExtension::setWorkingStudies(const QString &currentStudyUID, const QString &priorStudyUID)
{
    m_layoutManager->setWorkingStudies(currentStudyUID, priorStudyUID);

    if (m_currentStudyUID != currentStudyUID)
    {
        m_currentStudyUID = currentStudyUID;
        Study *currentStudy = m_patient->getStudy(currentStudyUID);

        if (currentStudy)
        {
            setupDefaultToolsForModalities(currentStudy->getModalities());
            setupDefaultLeftButtonTool();
            setupPropagation();
        }

    }
}

void Q2DViewerExtension::setFusionLayout2x1First(const QList<Volume*> &volumes, const AnatomicalPlane &anatomicalPlane)
{
    bool propagationEnabled = m_syncActionManager->isEnabled();

    if (propagationEnabled)
    {
        m_syncActionManager->enable(false);
    }

    m_layoutManager->setFusionLayout2x1First(volumes, anatomicalPlane);

    if (propagationEnabled)
    {
        m_syncActionManager->enable(true);
    }
}

void Q2DViewerExtension::setFusionLayout2x1Second(const QList<Volume*> &volumes, const AnatomicalPlane &anatomicalPlane)
{
    bool propagationEnabled = m_syncActionManager->isEnabled();

    if (propagationEnabled)
    {
        m_syncActionManager->enable(false);
    }

    m_layoutManager->setFusionLayout2x1Second(volumes, anatomicalPlane);

    if (propagationEnabled)
    {
        m_syncActionManager->enable(true);
    }
}

void Q2DViewerExtension::setFusionLayout3x1(const QList<Volume*> &volumes, const AnatomicalPlane &anatomicalPlane)
{
    bool propagationEnabled = m_syncActionManager->isEnabled();

    if (propagationEnabled)
    {
        m_syncActionManager->enable(false);
    }

    m_layoutManager->setFusionLayout3x1(volumes, anatomicalPlane);

    if (propagationEnabled)
    {
        m_syncActionManager->enable(true);
    }
}

void Q2DViewerExtension::setFusionLayout2x3First(const QList<Volume*> &volumes)
{
    bool propagationEnabled = m_syncActionManager->isEnabled();

    if (propagationEnabled)
    {
        m_syncActionManager->enable(false);
    }

    m_layoutManager->setFusionLayout2x3First(volumes);

    if (propagationEnabled)
    {
        m_syncActionManager->enable(true);
    }
}

void Q2DViewerExtension::setFusionLayout2x3Second(const QList<Volume*> &volumes)
{
    bool propagationEnabled = m_syncActionManager->isEnabled();

    if (propagationEnabled)
    {
        m_syncActionManager->enable(false);
    }

    m_layoutManager->setFusionLayout2x3Second(volumes);

    if (propagationEnabled)
    {
        m_syncActionManager->enable(true);
    }
}

void Q2DViewerExtension::setFusionLayout3x3(const QList<Volume*> &volumes)
{
    bool propagationEnabled = m_syncActionManager->isEnabled();

    if (propagationEnabled)
    {
        m_syncActionManager->enable(false);
    }

    m_layoutManager->setFusionLayout3x3(volumes);

    if (propagationEnabled)
    {
        m_syncActionManager->enable(true);
    }
}

}
