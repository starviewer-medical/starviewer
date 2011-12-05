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
class AutomaticSynchronizationManager;
// Estructura pacient
class Patient;
class Study;
// Visualitzador
class Q2DViewerWidget;
class Q2DViewer;
// Menus
class MenuGridWidget;
class TableMenu;
class QDICOMDumpBrowser;
class StatsWatcher;

#ifndef STARVIEWER_LITE
class QPreviousStudiesWidget;
class PreviousStudiesManager;
class HangingProtocolManager;
#endif

/**
    Extensió que s'executarà per defecte a l'obrir un model
  */
class Q2DViewerExtension : public QWidget, private Ui::Q2DViewerExtensionBase {
Q_OBJECT
public:
    Q2DViewerExtension(QWidget *parent = 0);
    ~Q2DViewerExtension();

    /// Li assigna el volum principal
    void setInput(Volume *input);

    /// Mètode per assignar un pacient
    void setPatient(Patient *patient);

    /// Mètode per obtenir el pacient
    Patient* getPatient() const;

public slots:
    /// Mostrar menu per seleccionar grid predefinit
    /// TODO S'ha de canviar el mètode per tal que no es crei l'objecte cada cop
    void showPredefinedGrid();

    /// Mostrar el menu de la taula per seleccionar grids
    /// TODO S'ha de canviar el mètode per tal que no es crei l'objecte cada cop
    void showInteractiveTable();

    /// Mostrar menu per seleccionar grid predefinit dins una serie
    /// TODO S'ha de canviar el mètode per tal que no es crei l'objecte cada cop
    void showPredefinedImageGrid();

    /// Mostrar el menu de la taula per seleccionar grids dins una serie
    /// TODO S'ha de canviar el mètode per tal que no es crei l'objecte cada cop
    void showInteractiveImageTable();

#ifndef STARVIEWER_LITE
    /// Mostra el widget per poder descarregar els estudis previs de l'estudi actual
    void showPreviousStudiesWidget();
#endif

    /// Mostra o amaga tota la informació sobreposada a cada visualitzador (informació textual i overlays)
    void showViewersLayers(bool show);

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

    /// Activa o desactiva només el botó de la sincronització manual
    void enableSynchronizationButton(bool enable);

#ifndef STARVIEWER_LITE
    /// Apilicar un hanging protocol
    void setHangingProtocol(int hangingProtocolNumber);

    /// Mètode que busca els hanging protocols aplicables i aplica el millor de tots
    void searchAndApplyBestHangingProtocol();

    /// Mètode per buscar les prèvies de l'estudi carregat més recent.
    void searchPreviousStudiesOfMostRecentStudy();

    /// Mètode encarregat d'actualitzar la llista del widget d'estudis previs per marcar aquells nous estudis que s'han carregat a memòria.
    void updatePreviousStudiesWidget();

    /// Inicialitza la sincronització automàtica agafant de referència el visor que tenim actiu (seleccionat)
    void enableAutomaticSynchronizationToViewer(bool enable);

#endif

    /// Aplica un grid regular al layout, i elimina l'etiqueta si algun estudi previ està en descàrrega
    void setGrid(int rows, int columns);

private:
    /// Crea les connexions entre signals i slots
    void createConnections();

    /// Llegir/Escriure la configuració de l'aplicació
    void readSettings();
    void writeSettings();

    /// Inicialitza les tools que tindrà l'extensió
    void initializeTools();

#ifndef STARVIEWER_LITE
    /// Buscar estudis prèvis
    void searchPreviousStudiesWithHangingProtocols();
#endif

    /// Segons les dades del pacient carregat escollirà quina és l'eina del botó esquerre més adient que cal activar per defecte 
    void setupDefaultLeftButtonTool();

private slots:
#ifndef STARVIEWER_LITE
    /// Comprova si el nou volum té fases i per tant hem d'activar/descativar la vista coronal+sagital
    void validePhases();
#endif

    /// Habilita o deshabilita el botó de Dump d'info DICOM segons en la vista en la que es trobi
    /// l'actual visor seleccionat. Si no hi ha cap visor actiu, dehsabilitarà el botó.
    void updateDICOMInformationButton();

    /// Mètode que tracta el nou visualitzador afegit
    void activateNewViewer(Q2DViewerWidget *newViewerWidget);

    /// Mètode que tracta el canvi de visualitzador seleccionat
    void changeSelectedViewer(Q2DViewerWidget *viewerWidget);

    ///
    void enableAutomaticSynchonizationEditor(bool enable);

#ifndef STARVIEWER_LITE
    /// Comprova els estats dels viewers quan la sincronització automàtica està activada per determinar si es pot activar l'edició de sincronització
    void checkSynchronizationEditCanBeEnabled();

    ///  Mètode que busca els hanging protocols aplicables
    void searchHangingProtocols();

    /// Mètodes utilitzats per modificar la icona del botó d'estudis previs per saber si s'estan descarregat estudis sense haver d'obrir el widget.
    void changeToPreviousStudiesDownloadingIcon();
    void changeToPreviousStudiesDefaultIcon();

    /// Mètode que afegeix els hanging protocols amb prèvies
    void addPreviousHangingProtocols(QList<Study*> studies);

#endif

private:
    /// El volum principal
    Volume *m_mainVolume;

    /// Accions
    QAction *m_singleShotAction;
    QAction *m_multipleShotAction;
    QAction *m_screenShotTriggerAction;
    QAction *m_showOverlaysAction;
    QAction *m_showViewersTextualInformationAction;
    QAction *m_showDisplayShuttersAction;

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

    /// Menús per seleccionar el grid de series(predefinits + taula)
    MenuGridWidget *m_predefinedSeriesGrid;
    TableMenu *m_seriesTableGrid;

    /// Menús per seleccionar el grid de les llesques d'una serie (predefinits + taula)
    MenuGridWidget *m_predefinedSlicesGrid;
    TableMenu *m_sliceTableGrid;

    /// ToolManager per configurar l'entorn deToolConfiguration * configuration = getConfiguration();
    ToolManager *m_toolManager;

    /// Perfil a mostrar la interficie
    QString m_profile;

    /// Últim visualitzador seleccionat
    Q2DViewerWidget *m_lastSelectedViewer;

    /// Per fer estadístiques d'usabilitat
    StatsWatcher *m_statsWatcher;

#ifndef STARVIEWER_LITE
    /// Widget per poder seleccionar estudis previs
    QPreviousStudiesWidget *m_previousStudiesWidget;

    /// Manager per estudis previs
    PreviousStudiesManager *m_previousStudiesManager;

    /// Manager de hanging protocols
    HangingProtocolManager *m_hangingProtocolManager;

    /// Manager de la sincronització automàtica
    AutomaticSynchronizationManager *m_automaticSynchronizationManager;

#endif

};

} // end namespace udg

#endif
