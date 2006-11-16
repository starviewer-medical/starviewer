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
#include <QMenu>

#include "identifier.h"

// Forward declarations
class QAction;
class QToolBar;
class QProgressDialog;
class QCloseEvent;
class QSignalMapper;

namespace udg{
// Forward declarations
class ExtensionWorkspace;
class ExtensionHandler;

class QApplicationMainWindow : public QMainWindow
{
Q_OBJECT
public:
    ///El constructor del Mainwindow. Aquí es crea tot l'esquelet de l'interfície: Menús, Barres d'eines, toolbox, working area, etc..
    QApplicationMainWindow(QWidget *parent = 0, const char *name = 0);
    
    ~QApplicationMainWindow();
    
    /// L'àrea de mini-aplicacions
    ExtensionWorkspace *m_extensionWorkspace;

    /// Mètode de conveniència per sortir del pas de mentres. Es crida quan tenim ja un model obert i volem obri-ne un de nou
    void newAndOpen();
    void newAndOpenDir();
    
    /// Retorna el numero de finestres amb diferents models que tenim obertes, el num. de QApplicationMainWindow
    unsigned int getCountQApplicationMainWindow();
    
public slots:
    /// Fa les tasques que cal quan s'ha carregat un volum
    void onVolumeLoaded( Identifier id );
    
protected:
    /// Aquest event ocurreix quanes tanca la finestra. És el moment en que es realitzen algunes tasques com desar la configuració
    void closeEvent(QCloseEvent *event);

private:
    /// Gestor de serveis i miniaplicacions
    ExtensionHandler *m_extensionHandler;
    
    /// Tipus de fitxer a exportar
    enum ExportFiles{ JpegExport , MetaIOExport , TiffExport, PngExport , BmpExport };

//    /// Llista d'arxius recents
//     QStringList m_recentFiles;
//     enum { MaxRecentFiles = 5 };
//     QAction *m_recentFileActions[ MaxRecentFiles ];

    /// Arxiu actual que estem visualitzant
    QString m_currentFile;

    /// Filtres per als diàlegs d'obrir/exportar arxius
//     QString m_exportFileFilters;
//     QString m_exportToJpegFilter;
//     QString m_exportToMetaIOFilter;
//     QString m_exportToTiffFilter;
//     QString m_exportToBmpFilter;
//     QString m_exportToPngFilter;
//     QString m_exportToDicomFilter;

    /// Path actual del directori de treball ( obrir fitxer )
    QString m_workingDirectory;

    ///Path del directori de treball ( exportació a altres formats )
    QString m_exportWorkingDirectory;

    /// Aquesta string indica el path del directori que conté els arxius de traducció de l'aplicació
    QString m_qmPath;

    /// Menús
    QMenu *m_fileMenu;
    QMenu *m_visualizationMenu;
    QMenu *m_languageMenu;
    QMenu *m_helpMenu;
    
    /// Sub menús
//     QMenu *m_importFilesMenu;
//     QMenu *m_exportFilesMenu;
//     QMenu *m_recentFilesMenu;
    
    /// Barres d'eines \TODO és possible que les fem desaparèixer
    QToolBar *m_fileToolBar;
    
    /// Accions
    QAction *m_2DViewerAction;
    QAction *m_mpr2DAction;
    QAction *m_mpr3DAction;
    QAction *m_mpr3D2DAction;
    QAction *m_pacsAction;
    QAction *m_newAction;
    QAction *m_openAction;
    QAction *m_openDirAction;
    QAction *m_closeAction;
    QAction *m_exitAction;
    QAction *m_aboutAction;
//     QAction *m_exportToJpegAction;
//     QAction *m_exportToMetaIOAction;
//     QAction *m_exportToTiffAction;
//     QAction *m_exportToPngAction;
//     QAction *m_exportToBmpAction;

    QAction *m_catalanAction;
    QAction *m_spanishAction;
    QAction *m_englishAction;

    /// Per indicar el procés al obrir-se un fitxer
    QProgressDialog *m_progressDialog;

    /// Mapeig de signals
    QSignalMapper *m_signalMapper;
    
    // :::::::::::::::::::::::::::
    // mètodes
    // :::::::::::::::::::::::::::
    
    ///Crea i inicialitza les accions de l'aplicació
    void createActions();

    /// Crea els menús.
    void createMenus();

    /// Crea el menú per escollir l'idioma de l'aplicació
    void createLanguageMenu();
    
    /// Crea la barra d'eines.
    void createToolBars();

    /// Crea la barra d'estat de l'aplicació
    void createStatusBar();

    /// Llegeix la configuració inicial amb la que engega el programa
    void readSettings();

    /// Escriu la configuració amb la que s'engegarà el programa el pròxim cop. \TODO: s'hauria de cridar també quan obrim una finestra nova?
    void writeSettings();

/*
    /// Especifica el fitxer acctual que s'està tractant \TODO \deprecated aquest mètode esdevindrà antiquat en breu i no té gaire sentit desar el nom del fitxer, el metode continua existint perquè s'ha arrastrat des del principi
    void setCurrentFile( const QString &fileName );

    /// actualitza el llistat de noms de fitxers oberts recentment \TODO s'hauria d'adaptar, si cal quan hi hagi lo del pacs
    void updateRecentFileActions();
*/
private slots:
    /// Obre una nova finestra
    void newFile();

    /// Slot tancar volum
    void close();

    /// mostra el formulari d'about
    void about();

    /// canvia a l'idioma indicat
    void switchToLanguage( int id );
    
/*
    /// obre un dels arxius recents::\TODO això pot esdevenir \deprecated
    void openRecentFile();

    /// Exporta la imatge a un format especificat
    void exportFile( int type );

    /// Exporta la imatge a JPEG
    void exportToJpeg();

    /// Exporta la imatge a MetaIO
    void exportToMetaIO();

    /// Exporta la imatge a TIFF
    void exportToTiff();

    /// Exporta la imatge a PNG
    void exportToPng();

    /// Exporta la imatge a BMP
    void exportToBmp();
*/
signals:
    /// senyal emesa que indica si la finestra conté un volum o no
    void containsVolume( bool );
};

}; // fi namespace udg

#endif
