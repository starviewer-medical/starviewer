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
class QRelatedStudiesWidget;
class RelatedStudiesManager;
class HangingProtocolManager;
class AutomaticSynchronizationManager;
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

    /// Fa les accions necessàries quan les dades del pacient actual s'han actualitzat
    /// TODO S'ha d'acabar de perfilar, es podria dir, onStudyAdded()
    void onPatientUpdated();

private:
    /// Crea les connexions entre signals i slots
    void createConnections();

    /// Llegir/Escriure la configuració de l'aplicació
    void readSettings();
    void writeSettings();

    /// Inicialitza les tools que tindrà l'extensió
    void initializeTools();

#ifndef STARVIEWER_LITE
    /// Inicialitza el l'entorn dels hanging protocols per poder-se utilitzar
    void setupHangingProtocols();
    
    /// Mètode que busca els hanging protocols aplicables i aplica el millor de tots
    void searchAndApplyBestHangingProtocol();

    /// Mètode encarregat d'actualitzar la llista del widget d'estudis relacionats per marcar aquells nous estudis que s'han carregat a memòria.
    void updateRelatedStudiesWidget();

    /// Buscar estudis prèvis
    void searchPreviousStudiesWithHangingProtocols();
#endif
    
    /// Configura les tools que cal activar per defecte segons la llista de modalitats donada
    void setupDefaultToolsForModalities(const QStringList &modalities);
    
    /// Segons les dades del pacient carregat escollirà quina és l'eina del botó esquerre més adient que cal activar per defecte 
    void setupDefaultLeftButtonTool();

    /// Col·loca i ordena les icones i el menú de les eines del botó donat segons l'última eina seleccionada
    void rearrangeToolsMenu(QToolButton *menuButton);

    /// Determina com fem el layout del pacient que es carrega, si amb layout automàtic o amb hanging protocols
    void applyProperLayoutChoice();

private slots:
#ifndef STARVIEWER_LITE
    /// Comprova si el nou volum té fases i per tant hem d'activar/descativar la vista coronal+sagital
    void validePhases();

    /// Col·loca i ordena les icones i el menú de les eines de ROI segons l'última tool de ROI seleccionada
    void rearrangeROIToolsMenu();

    /// Col·loca i ordena les icones i el menu de les eines d'angles segons l'última tool d'angles seleccionada
    void rearrangeAngleToolsMenu();

    /// Col·loca i ordena les icones i el menu de les eines de zoom segons l'última tool de Zoom seleccionada
    void rearrangeZoomToolsMenu();

#endif

    /// Col·loca i ordena les icones i el menú de les eines de distància segons l'última eina de distància seleccionada
    void rearrangeDistanceToolsMenu();

    /// Habilita o deshabilita el botó de Dump d'info DICOM segons en la vista en la que es trobi
    /// l'actual visor seleccionat. Si no hi ha cap visor actiu, dehsabilitarà el botó.
    void updateDICOMInformationButton();

    /// Mètode que tracta el nou visualitzador afegit
    void activateNewViewer(Q2DViewerWidget *newViewerWidget);

    /// Mètode que tracta el canvi de visualitzador seleccionat
    void changeSelectedViewer(Q2DViewerWidget *viewerWidget);

    /// Si enable és true, activa el gruix màxim a tots els visors on hi hagi algun mode de thickslab actiu
    void enableMaximumThicknessMode(bool enable);

    /// Mostrar menu per seleccionar grid predefinit
    /// TODO S'ha de canviar el mètode per tal que no es crei l'objecte cada cop
    void showPredefinedGrid();

    /// Mostrar el menu de la taula per seleccionar grids
    /// TODO S'ha de canviar el mètode per tal que no es crei l'objecte cada cop
    void showInteractiveTable();

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
    /// Apilicar un hanging protocol
    void setHangingProtocol(int hangingProtocolNumber);

    /// Inicialitza la sincronització automàtica agafant de referència el visor que tenim actiu (seleccionat)
    void enableAutomaticSynchronizationToViewer(bool enable);

    ///  Mètode que busca els hanging protocols aplicables
    void searchHangingProtocols();

    /// Mètodes utilitzats per modificar la icona del botó d'estudis relacionats per saber si s'estan descarregat estudis sense haver d'obrir el widget.
    void changeToRelatedStudiesDownloadingIcon();
    void changeToRelatedStudiesDefaultIcon();

    /// Mètode que afegeix els hanging protocols amb prèvies
    void addHangingProtocolsWithPrevious(QList<Study*> studies);

    /// Mostra el widget per poder descarregar els estudis relacionats amb l'estudi actual
    void showRelatedStudiesWidget();
    
    /// Activa la sincronització manual a tots els visors
    void activateManualSynchronizationInAllViewers();

    // Desactiva la sincronització manual de tots els visors
    void deactivateManualSynchronizationInAllViewers();

    // Tracta l'event de quan s'ha seleccionat/desactivat la sincronització manual en qualsevol dels visors
    void manualSynchronizationActivated(bool enable);

#endif

    /// Aplica un grid regular al layout, i elimina l'etiqueta si algun estudi relacionat està en descàrrega
    void setGrid(int rows, int columns);

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
    QAction *m_synchronizeAllAction;
    QAction *m_desynchronizeAllAction;

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
    MenuGridWidget *m_hangingProtocolsMenu;

    /// Widget per escollir una distribució de visors definida per l'usuari
    TableMenu *m_seriesTableGrid;

    /// ToolManager per configurar l'entorn deToolConfiguration * configuration = getConfiguration();
    ToolManager *m_toolManager;

    /// Perfil a mostrar la interficie
    QString m_profile;

    /// Últim visualitzador seleccionat
    Q2DViewerWidget *m_lastSelectedViewer;

    /// Per fer estadístiques d'usabilitat
    StatsWatcher *m_statsWatcher;

#ifndef STARVIEWER_LITE
    /// Widget per poder seleccionar estudis relacionats
    QRelatedStudiesWidget *m_relatedStudiesWidget;

    /// Manager per estudis relacionats
    RelatedStudiesManager *m_relatedStudiesManager;

    /// Manager de hanging protocols
    HangingProtocolManager *m_hangingProtocolManager;

    /// Manager de la sincronització automàtica
    AutomaticSynchronizationManager *m_automaticSynchronizationManager;

#endif

};

} // end namespace udg

#endif
