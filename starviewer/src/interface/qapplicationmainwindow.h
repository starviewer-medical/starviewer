#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// QT
#include <QMainWindow>
#include <QStringList>

// Forward declarations
class QAction;
class QMenu;
class QCloseEvent;
class QShowEvent;
class QSignalMapper;
class QLabel;
class QProgressDialog;
class QWidgetAction;
class QShortcut;

namespace udg {
// Forward declarations
class ExtensionWorkspace;
class ExtensionHandler;
class ExtensionContext;
class QLogViewer;
class Patient;
class StatsWatcher;
class ApplicationVersionChecker;

class QApplicationMainWindow : public QMainWindow {
Q_OBJECT
public:
    /// El constructor del Mainwindow. Aquí es crea tot l'esquelet de l'interfície: Menús, Barres d'eines, toolbox, working area, etc..
    QApplicationMainWindow(QWidget *parent = 0);

    ~QApplicationMainWindow();

    /// Assigna un pacient a la finestra.
    /// Farà les accions pertinents segons si ja es tenia un pacient o bé és el primer pacient que s'assigna
    /// @param patient El pacient
    void setPatient(Patient *patient);

    /// Crea una nova finestra i l'obre. Si li proporcionem dades de pacient, els hi afegeix
    /// @param context Dades de pacient que volem que contingui la nova finestra
    QApplicationMainWindow* setPatientInNewWindow(Patient *patient);

    /// Ens retorna el punter al pacient que té ara
    Patient* getCurrentPatient();

    /// Retorna el numero de finestres amb diferents models que tenim obertes, el num. de QApplicationMainWindow
    unsigned int getCountQApplicationMainWindow();

    /// Retorna totes finestres que tenim obertes
    static QList<QApplicationMainWindow*> getQApplicationMainWindows();

    /// Retorna la finestra activa actual
    static QApplicationMainWindow* getActiveApplicationMainWindow();

    /// Mètode que retorna el workspace a on poder afegir extensions
    ExtensionWorkspace* getExtensionWorkspace();

    /// Connecta els volums d'un pacient al mètode que notifica la càrrega de volums
    void connectPatientVolumesToNotifier(Patient *patient);

protected:
    /// Aquest event ocurreix quanes tanca la finestra. És el moment en que es realitzen algunes tasques com desar la configuració
    virtual void closeEvent(QCloseEvent *event);

    virtual void resizeEvent(QResizeEvent *event);

    virtual void showEvent(QShowEvent *event);

private:
    /// Crea i inicialitza les accions de l'aplicació
    void createActions();

    /// Crea els menús.
    void createMenus();

    /// Crea el menú per escollir l'idioma de l'aplicació
    void createLanguageMenu();

    /// Crea una acció per canviar d'idioma passant l'idioma (ex. Catalan) i el locale al que s'ha de canviar (ex. ca_ES)
    QAction* createLanguageAction(const QString &language, const QString &locale);

    /// Elimina totes les extensions que contingui el workspace i elimina els volums del pacient actual
    /// TODO: Cal que s'hi esborrin les dades de Patient, Study, Series, etc. també.
    void killBill();

    /// Llegeix la configuració inicial amb la que engega el programa
    void readSettings();

    /// Escriu la configuració amb la que s'engegarà el programa el pròxim cop. \TODO: s'hauria de cridar també quan obrim una finestra nova?
    void writeSettings();

    // Fa accessibles les extensions a través dels menús. S'hauria de cridar quan ja
    // tenim un input vàlid i es poden carregar volums
    void enableExtensions();

    /// Marca aquesta aplicació com a aplicació beta, mostrant informació a l'usuari.
    void markAsBetaVersion();

    /// Actualitza la informació que es mostra a l'usuari en el menú com a versió beta.
    void updateBetaVersionTextPosition();

    /// Envia una petició per descarregar un estudi a través del seu accession number
    void sendRequestRetrieveStudyWithAccessionNumberToLocalStarviewer(QString accessionNumber);

    /// Calcula el tamany de la lletra del text de les tools si CoreSettings::AutoToolTextSize es true.
    /// Altrament s'agafarà el valor que ja hi havia a CoreSettings::DefalutToolTextSize.
    void computeDefaultToolTextSize();

private slots:
    // Mètode genèric que s'assabenta del progrés de càrrega d'un volum i el notifica d'alguna manera en l'interfície
    // com per exemple un QProgressDialog o en un label
    // @param progress valor del progrés de càrrega
    void updateVolumeLoadProgressNotification(int progress);

    /// Mostra el diàleg on s'explica que és una versió beta.
    void showBetaVersionDialog();

    /// Crea una nova finestra i la mostra. Retorna una referència a la finestra creada.
    QApplicationMainWindow* openBlankWindow();

    /// Mostra el formulari d'about
    void about();

    /// Canvia a l'idioma indicat
    void switchToLanguage(QString locale);

    /// Maximitza a tantes pantalles com es pugui
    void maximizeMultipleScreens();

    /// Moure la finestra de pantalla
    void moveToDesktop(int screenIndex);
    /// Moure la finestra a la pantalla anterior
    void moveToPreviousDesktop();
    /// Moure la finestra a la següent pantalla
    void moveToNextDesktop();

    /// Mostra el diàleg de configuració de l'aplicació.
    /// TODO Millorar, de moment és bastant "xapussa"
    void showConfigurationDialog();

    void newCommandLineOptionsToRun();

    /// Obre el manual d'usuari
    void openUserGuide();

    /// Obre la guia rapida
    void openQuickStartGuide();

    /// Obre l'esquema de shortcuts
    void openShortcutsGuide();

    /// Obre les release notes
    void openReleaseNotes();

    /// Mostra el diàleg que executa els diagnosis test
    void showDiagnosisTestDialog();

private:
    /// L'àrea de mini-aplicacions
    ExtensionWorkspace *m_extensionWorkspace;

    /// Gestor de serveis i miniaplicacions
    ExtensionHandler *m_extensionHandler;

    /// Menús
    QMenu *m_fileMenu;
    QMenu *m_visualizationMenu;
    QMenu *m_windowMenu;
    QMenu *m_languageMenu;
    QMenu *m_helpMenu;
    QMenu *m_toolsMenu;
    QMenu *m_moveWindowToDesktopMenu;

    /// Accions
    QAction *m_localDatabaseAction;
    QAction *m_pacsAction;
    QAction *m_openDICOMDIRAction;

    QAction *m_newAction;
    QAction *m_openAction;
    QAction *m_openDirAction;
    QAction *m_closeAction;
    QAction *m_exitAction;
    QAction *m_aboutAction;
    QAction *m_maximizeAction;
    QWidgetAction *m_moveToDesktopAction;

    QAction *m_logViewerAction;
    QAction *m_configurationAction;
    QAction *m_openUserGuideAction;
    QAction *m_openQuickStartGuideAction;
    QAction *m_openShortcutsGuideAction;
    QAction *m_openReleaseNotesAction;
    QAction *m_runDiagnosisTestsAction;

    QLabel *m_betaVersionMenuText;

    /// Mapeig de signals
    QSignalMapper *m_signalMapper;
    QList<QAction*> m_actionsList;

    /// Per veure els logs
    QLogViewer *m_logViewer;

    /// El pacient que li correspon a aquesta instància
    Patient *m_patient;

    bool m_isBetaVersion;

    /// Progress dialog per mostrar el progrés de càrrega dels volums
    QProgressDialog *m_progressDialog;

    /// Estadístiques d'usabilitat
    StatsWatcher *m_statsWatcher;

    /// Les Release Notes o les notes de la nova versió
    ApplicationVersionChecker *m_applicationVersionChecker;

    /// Els shortcuts per moure la finestra a una altra pantalla
    QShortcut *m_moveToNextDesktopShortcut;
    QShortcut *m_moveToPreviousDesktopShortcut;
};

}; // fi namespace udg

#endif
