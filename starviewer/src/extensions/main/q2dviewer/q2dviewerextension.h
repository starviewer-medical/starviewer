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

#ifndef UDGQ2DVIEWEREXTENSION_H
#define UDGQ2DVIEWEREXTENSION_H

#include "ui_q2dviewerextensionbase.h"
#include <QWidget>

// FWD declarations
class QAction;

namespace udg {

// FWD declarations
class Volume;
class ToolManager;
class ToolConfiguration;
class HangingProtocol;
class TransferFunctionModel;
// Estructura pacient
class Patient;
class Study;
// Visualitzador
class Q2DViewerWidget;
class Q2DViewer;
// Menus
class QHangingProtocolsWidget;
class TableMenu;
class QDICOMDumpBrowser;
class StatsWatcher;
class LayoutManager;

#ifndef STARVIEWER_LITE
class QRelatedStudiesWidget;
class RelatedStudiesManager;
class AutomaticSynchronizationManager;
class SyncActionManager;
class ViewersLayoutToSyncActionManagerAdapter;
class SyncActionsConfigurationHandler;
#endif

/**
    Extensió que s'executarà per defecte a l'obrir un model
  */
class Q2DViewerExtension : public QWidget, private Ui::Q2DViewerExtensionBase {
Q_OBJECT
public:
    Q2DViewerExtension(QWidget *parent = 0);
    ~Q2DViewerExtension();

#ifdef STARVIEWER_LITE
    /// Li assigna el volum principal
    void setInput(Volume *input);
#endif

    /// Mètode per assignar un pacient
    void setPatient(Patient *patient);

    /// Mètode per obtenir el pacient
    Patient* getPatient() const;

    /// Sets the study from the current patient with the given studyUID as the current patient.
    void setCurrentStudy(const QString &studyUID);

private:
    /// Crea les connexions entre signals i slots
    void createConnections();

    /// Llegir/Escriure la configuració de l'aplicació
    void readSettings();
    void writeSettings();

    /// Inicialitza les tools que tindrà l'extensió
    void initializeTools();

    /// Configura les tools que cal activar per defecte segons la llista de modalitats donada
    void setupDefaultToolsForModalities(const QStringList &modalities);
    
    /// Segons les dades del pacient carregat escollirà quina és l'eina del botó esquerre més adient que cal activar per defecte 
    void setupDefaultLeftButtonTool();

    /// Sets up layout manager and its connections properly
    void setupLayoutManager();

    /// Shows the given widget below the button, as if it was the button's menu
    void showWidgetBelowButton(QWidget *widget, QAbstractButton *button);

    /// Sets the given transfer function model to the transfer function combo box and selects the index of the transfer function applied in the current viewer.
    void updateTransferFunctionComboBox(TransferFunctionModel *transferFunctionModel);

private slots:
#ifndef STARVIEWER_LITE
    /// Comprova si el nou volum té fases i per tant hem d'activar/descativar la vista coronal+sagital
    void validePhases();
#endif

    /// Habilita o deshabilita el botó de Dump d'info DICOM segons en la vista en la que es trobi
    /// l'actual visor seleccionat. Si no hi ha cap visor actiu, dehsabilitarà el botó.
    void updateDICOMInformationButton();

    /// Enables or disables the exporter tool button according to the status of the selected viewer.
    void updateExporterToolButton();

    /// Mètode que tracta el nou visualitzador afegit
    void activateNewViewer(Q2DViewerWidget *newViewerWidget);

    /// Mètode que tracta el canvi de visualitzador seleccionat
    void changeSelectedViewer(Q2DViewerWidget *viewerWidget);

    /// Si enable és true, activa el gruix màxim a tots els visors on hi hagi algun mode de thickslab actiu
    void enableMaximumThicknessMode(bool enable);

    /// Shows a widget with the available Hanging Protocols for the current Patient below its button
    void showAvailableHangingProtocolsMenu();

    /// Shows a widget with an interactive grid table to select the desired viewers layout below its corresponding button
    void showViewersLayoutGridTable();

    /// Mostra o amaga els overlays de cada visualitzador
    void showImageOverlays(bool show);

    /// Mostra o amaga els display shutters de cada visualitzador
    void showDisplayShutters(bool show);

    /// Mostra o amaga la informació textual de cada visualitzador
    void showViewersTextualInformation(bool show);
    
    /// Mostrar la pantalla de Dicom dump, amb informació de la imatge que es visualitza en aquell moment
    void showDicomDumpCurrentDisplayedImage();

#ifndef STARVIEWER_LITE
    /// Mostrar el diàleg per exportar la sèrie del visor seleccionat.
    void showScreenshotsExporterDialog();
#endif

    /// TODO Mètode per solucionar problemes perquè la sincronització
    /// encara no està adaptada a la resta de les tools
    /// Desactiva l'eina de sincronització.
    void disableSynchronization();

#ifndef STARVIEWER_LITE
    /// Inicialitza la sincronització automàtica agafant de referència el visor que tenim actiu (seleccionat)
    void enableAutomaticSynchronizationToViewer(bool enable);

    /// Mètodes utilitzats per modificar la icona del botó d'estudis relacionats per saber si s'estan descarregat estudis sense haver d'obrir el widget.
    void changeToRelatedStudiesDownloadingIcon();
    void changeToRelatedStudiesDefaultIcon();

    /// Mostra el widget per poder descarregar els estudis relacionats amb l'estudi actual
    void showRelatedStudiesWidget();
    
    /// Activa la sincronització manual a tots els visors
    void activateManualSynchronizationInAllViewers();

    // Desactiva la sincronització manual de tots els visors
    void deactivateManualSynchronizationInAllViewers();

    // Tracta l'event de quan s'ha seleccionat/desactivat la sincronització manual en qualsevol dels visors
    void manualSynchronizationActivated(bool enable);

    /// Sets up if propagation must be enabled or not depending on the current patient modalities
    void setupPropagation();
#endif

    /// Aplica un grid regular al layout, i elimina l'etiqueta si algun estudi relacionat està en descàrrega
    void setGrid(int rows, int columns);

    /// Sets the transfer function model of the current viewer to the transfer function combo box and selects the index of the transfer function applied in the
    /// current viewer.
    void updateTransferFunctionComboBoxWithCurrentViewerModel();

    /// Updates external application parameters from the currently active view.
    void updateExternalApplicationsWithCurrentView(Volume* volume);

    /// Sets the transfer function at the given index in the current model to the current viewer.
    void setTransferFunctionToCurrentViewer(int transferFunctionIndex);

    /// When a viewer receives a double click, this method checks which tools are active and
    /// if none of them processes single or double clicks then maximizes or demaximizes the viewer.
    void handleViewerDoubleClick(Q2DViewerWidget *viewerWidget);

    /// Set the working studies (current and prior studies).
    void setWorkingStudies(const QString &currentStudyUID, const QString &priorStudyUID);

    /// Sets a 2x1 CT+fused fusion layout with the given volumes and anatomical plane.
    void setFusionLayout2x1First(const QList<Volume*> &volumes, const AnatomicalPlane &anatomicalPlane);
    /// Sets a 2x1 fused+PT/NM fusion layout with the given volumes and anatomical plane.
    void setFusionLayout2x1Second(const QList<Volume*> &volumes, const AnatomicalPlane &anatomicalPlane);
    /// Sets a 3x1 fusion layout with the given volumes and anatomical plane.
    void setFusionLayout3x1(const QList<Volume*> &volumes, const AnatomicalPlane &anatomicalPlane);
    /// Sets a 2x3 CT+fused fusion layout with the given volumes.
    void setFusionLayout2x3First(const QList<Volume*> &volumes);
    /// Sets a 2x3 fused+PT/NM fusion layout with the given volumes.
    void setFusionLayout2x3Second(const QList<Volume*> &volumes);
    /// Sets a 3x3 fusion layout with the given volumes.
    void setFusionLayout3x3(const QList<Volume*> &volumes);

private:
    /// Accions
    QAction *m_singleShotAction;
    QAction *m_multipleShotAction;
    QAction *m_screenShotTriggerAction;
    QAction *m_showOverlaysAction;
    QAction *m_showViewersTextualInformationAction;
    QAction *m_showDisplayShuttersAction;
    QAction *m_synchronizeAllAction;
    QAction *m_desynchronizeAllAction;
    QAction *m_propagationAction;

#ifndef STARVIEWER_LITE
    QAction *m_sagitalViewAction;
    QAction *m_coronalViewAction;
#endif

    /// El diàleg per escollir un window level ajustat per l'usuari
    QCustomWindowLevelDialog *m_customWindowLevelDialog;

    /// Obre la finestra de dicomdump per la imatge que tenim seleccionada en aquests moments
    QDICOMDumpBrowser *m_dicomDumpCurrentDisplayedImage;

    /// Pacient que estem tractant
    Patient *m_patient;

    /// Menú per seleccionar es hanging protocols
    QHangingProtocolsWidget *m_hangingProtocolsMenu;

    /// Widget per escollir una distribució de visors definida per l'usuari
    TableMenu *m_viewersLayoutGrid;

    /// ToolManager per configurar l'entorn deToolConfiguration * configuration = getConfiguration();
    ToolManager *m_toolManager;

    /// Perfil a mostrar la interficie
    QString m_profile;

    /// Últim visualitzador seleccionat
    Q2DViewerWidget *m_lastSelectedViewer;

    /// Per fer estadístiques d'usabilitat
    StatsWatcher *m_statsWatcher;
    
    /// Handles hanging protocols and automatic layouts
    LayoutManager *m_layoutManager;

#ifndef STARVIEWER_LITE
    /// Widget per poder seleccionar estudis relacionats
    QRelatedStudiesWidget *m_relatedStudiesWidget;

    /// Manager per estudis relacionats
    RelatedStudiesManager *m_relatedStudiesManager;

    SyncActionsConfigurationHandler *m_syncActionsConfigurationHandler;
    SyncActionManager *m_syncActionManager;
    ViewersLayoutToSyncActionManagerAdapter *m_layoutToSyncActionManagerAdapter;
#endif

    /// Transfer function model to use when a viewer returns a null model or an empty viewer is selected.
    TransferFunctionModel *m_emptyTransferFunctionModel;

    /// Study Instance UID of current study.
    QString m_currentStudyUID;

};

} // end namespace udg

#endif
