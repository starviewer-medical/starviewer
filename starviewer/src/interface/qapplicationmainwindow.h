/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// QT
#include <QMainWindow>
#include <QStringList>

// Forward declarations
class QAction;
class QMenu;
class QCloseEvent;
class QSignalMapper;

namespace udg{
// Forward declarations
class ExtensionWorkspace;
class ExtensionHandler;
class QLogViewer;
class Patient;

class QApplicationMainWindow : public QMainWindow
{
Q_OBJECT
public:
    ///El constructor del Mainwindow. Aquí es crea tot l'esquelet de l'interfície: Menús, Barres d'eines, toolbox, working area, etc..
    QApplicationMainWindow(QWidget *parent = 0, QString name = 0);

    ~QApplicationMainWindow();

    /// Donat un pacient, segons el pacient que tinguem, afegirem les dades, crearem una nova instància de la main window, etc
    void addPatient( Patient *patient );

    /// Ens retorna el punter al pacient que té ara
    Patient *getCurrentPatient();

    /// Retorna el numero de finestres amb diferents models que tenim obertes, el num. de QApplicationMainWindow
    unsigned int getCountQApplicationMainWindow();

    /// Retorna la finestra activa actual
    static QApplicationMainWindow* getActiveApplicationMainWindow();

    /// Mètode que retorna el workspace a on poder afegir extensions
    ExtensionWorkspace* getExtensionWorkspace();

protected:
    /// Aquest event ocurreix quanes tanca la finestra. És el moment en que es realitzen algunes tasques com desar la configuració
    void closeEvent(QCloseEvent *event);

private:
    ///Crea i inicialitza les accions de l'aplicació
    void createActions();

    /// Crea els menús.
    void createMenus();

    /// Crea el menú per escollir l'idioma de l'aplicació
    void createLanguageMenu();

    /// Llegeix la configuració inicial amb la que engega el programa
    void readSettings();

    /// Escriu la configuració amb la que s'engegarà el programa el pròxim cop. \TODO: s'hauria de cridar també quan obrim una finestra nova?
    void writeSettings();

    /**
     * Fa accessibles les extensions a través dels menús. S'hauria de cridar quan ja
     * tenim un input vàlid i es poden carregar volums
     */
    void enableExtensions();

private slots:
    /**
     * Crea una nova finestra i l'obre. Si li proporcionem dades de pacient, els hi afegeix
     * @param patient El pacient que volem que contingui la nova finestra
     */
    void openNewWindow( Patient *patient = 0 );

    /// mostra el formulari d'about
    void about();

    /// canvia a l'idioma indicat
    void switchToLanguage( int id );

    /// canvia a pantalla completa o no
    void switchFullScreen( bool full );

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

    /// Accions
    QAction *m_pacsAction;

    QAction *m_newAction;
    QAction *m_openAction;
    QAction *m_openDirAction;
    QAction *m_closeAction;
    QAction *m_exitAction;
    QAction *m_aboutAction;
    QAction *m_fullScreenAction;
    QAction *m_logViewerAction;
    QAction *m_catalanAction;
    QAction *m_spanishAction;
    QAction *m_englishAction;

    /// Mapeig de signals
    QSignalMapper *m_signalMapper;
    QList<QAction*> m_actionsList;

    /// Per veure els logs
    QLogViewer *m_logViewer;

    /// El pacient que li correspon a aquesta instància
    Patient *m_patient;
};

}; // fi namespace udg

#endif
