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
class ExternalApplication;

class QApplicationMainWindow : public QMainWindow {
Q_OBJECT
public:
    /// El constructor del Mainwindow. Aquí es crea tot l'esquelet de l'interfície: Menús, Barres d'eines, toolbox, working area, etc..
    QApplicationMainWindow(QWidget *parent = 0);

    ~QApplicationMainWindow();

    /// Checks for a new version and shows release notes, if applicable.
    void checkNewVersionAndShowReleaseNotes();

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

    /// Returns the last application main window that has been active at some some point. If one is currently active it will be that.
    static QApplicationMainWindow* getLastActiveApplicationMainWindow();

    /// Mètode que retorna el workspace a on poder afegir extensions
    ExtensionWorkspace* getExtensionWorkspace();

    /// Connecta els volums d'un pacient al mètode que notifica la càrrega de volums
    void connectPatientVolumesToNotifier(Patient *patient);

    /// Requests to load and view the study with the given Study Instance UID from the database in this window.
    void viewStudy(const QString &studyInstanceUid);
    /// Requests to load the study with the given Study Instance UID from the database in this window. The study is not visualized unless it is from a different
    /// patient than the current one.
    void loadStudy(const QString &studyInstanceUid);

#ifdef STARVIEWER_CE
    /// Shows the information regarding the use of the application as a medical device (if not disabled by the user).
    void showMedicalDeviceInformationDialog();

public slots:
    /// Shows the information regarding the use of the application as a medical device (always).
    void showMedicalDeviceInformationDialogUnconditionally();
#endif // STARVIEWER_CE

protected:
    /// Aquest event ocurreix quanes tanca la finestra. És el moment en que es realitzen algunes tasques com desar la configuració
    void closeEvent(QCloseEvent *event) override;

    void resizeEvent(QResizeEvent *event) override;

    void showEvent(QShowEvent *event) override;

    /// Reimplemented to keep track of the latest active window.
    bool event(QEvent *event) override;

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

    /// Fa accessibles les extensions a través dels menús. S'hauria de cridar quan ja
    /// tenim un input vàlid i es poden carregar volums
    void enableExtensions();

    /// Marca aquesta aplicació com a aplicació beta, mostrant informació a l'usuari.
    void markAsBetaVersion();

    /// Actualitza la informació que es mostra a l'usuari en el menú com a versió beta.
    void updateBetaVersionTextPosition();

private slots:
    /// Mètode genèric que s'assabenta del progrés de càrrega d'un volum i el notifica d'alguna manera en l'interfície
    /// com per exemple un QProgressDialog o en un label
    /// @param progress valor del progrés de càrrega
    void updateVolumeLoadProgressNotification(int progress);

    /// Mostra el diàleg on s'explica que és una versió beta.
    void showBetaVersionDialog();

    /// Crea una nova finestra i la mostra. Retorna una referència a la finestra creada.
    QApplicationMainWindow* openBlankWindow();

    /// Mostra el formulari d'about
    void about();

    /// Canvia a l'idioma indicat
    void switchToLanguage(QString locale);

    /// Launches the corresponding external application. (Where the number is
    /// the position on the list). If application launch has failed, shows an
    /// error message box.
    void launchExternalApplication(int i);

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

    /// Shows or hides patient identifying information in the window title according to m_showPatientIdentificationInWindowTitleAction state.
    void updateWindowTitle();

    /// @brief External applications submenu with the defined external applications.
    ///
    /// When called multiple times, deletes the previous menu and regenerates a new one.
    ///
    /// If no external applications are defined, the menu is not created.
    void createExternalApplicationsMenu();

private:
    /// List of all open main windows ordered from least to most recently active.
    static QList<QApplicationMainWindow*> m_lastActiveMainWindows;

    /// L'àrea de mini-aplicacions
    ExtensionWorkspace *m_extensionWorkspace;

    /// Gestor de serveis i miniaplicacions
    ExtensionHandler *m_extensionHandler;

    /// Menús
    QMenu *m_fileMenu;
    QMenu *m_visualizationMenu;
    QMenu *m_windowMenu;
    QMenu *m_languageMenu;
    QMenu *m_externalApplicationsMenu;
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
    QAction *m_moveToPreviousDesktopAction;
    QAction *m_moveToNextDesktopAction;
    QWidgetAction *m_moveToDesktopAction;

    QAction *m_logViewerAction;
    QAction *m_configurationAction;
    QAction *m_openUserGuideAction;
    QAction *m_openQuickStartGuideAction;
    QAction *m_openShortcutsGuideAction;
#ifdef STARVIEWER_CE
    QAction *m_showMedicalDeviceInformationAction;
#endif // STARVIEWER_CE
    QAction *m_openReleaseNotesAction;
    QAction *m_runDiagnosisTestsAction;
    QAction *m_showPatientIdentificationInWindowTitleAction;

    QLabel *m_betaVersionMenuText;

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

};

}; // fi namespace udg

#endif
