/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
 
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "identifier.h"
// QT
#include <QMainWindow>
#include <QStringList>
#include <QMenu>

// Forward declarations
class QAction;
class QToolBar;
class QProgressDialog;
class QCloseEvent;

namespace udg{
// Forward declarations
class VolumeRepository;
class Input;
class Output;
class ExtensionWorkspace;
class ExtensionHandler;

class QApplicationMainWindow : public QMainWindow
{
Q_OBJECT
public:
    ///El constructor del Mainwindow. Aquí es crea tot l'esquelet de l'interfície: Menús, Barres d'eines, toolbox, working area, etc..
    QApplicationMainWindow(QWidget *parent = 0, const char *name = 0);
    
    ~QApplicationMainWindow();
    
    /// Ens retorna l'id del volum que ha obert
    Identifier getVolumeID() const { return m_volumeID; };

    /// [apanyo] Li indiquem quin és el seu id de volum
    void setVolumeID( Identifier id );
    
    /// L'àrea de mini-aplicacions
    ExtensionWorkspace *m_extensionWorkspace;

    /// Mètode de conveniència per sortir del pas de mentres. Es crida quan tenim ja un model obert i volem obri-ne un de nou
    void newAndOpen();
    void newAndOpenDir();
    
    /// Neteja la barra d'eines d'extensions
    void clearExtensionsToolBar();

    /// Retorna la barra d'eines d'extensions
    QToolBar *getExtensionsToolBar() const { return m_extensionsToolBar; }
    
public slots:   
    /// mostra el progrés d'un procés (ara mateix només serà obrir fitxers) amb un diàleg
    void showProgress( int value );

protected:
    ///Aquest event ocurreix quanes tanca la finestra. És el moment en que es realitzen algunes tasques com desar la configuració
    void closeEvent(QCloseEvent *event);

private:
    // :::::::::::::::::::::::::::
    // membres
    // :::::::::::::::::::::::::::
    /// Gestor de serveis i miniaplicacions
    ExtensionHandler *m_extensionHandler;
    
    /// Tipus de fitxer a exportar
    enum ExportFiles{ JpegExport , MetaIOExport , TiffExport, PngExport , BmpExport };

    /// Punter a sí mateix \TODO, no se si és gaire útil ni necessari, pot desaparèixer facilment
    QApplicationMainWindow* m_self;    

    /// Indicarà l'id del volum que posseïm
    udg::Identifier m_volumeID; 

    /// Indica si hi ha hagut modificacions \TODO: és gaire útil i/o necessari això? es podria eliminar
    bool m_modified;

    /// Llista d'arxius recents
    QStringList m_recentFiles;
    enum { MaxRecentFiles = 5 };
    QAction *m_recentFileActions[ MaxRecentFiles ];

    /// Arxiu actual que estem visualitzant
    QString m_currentFile;

    /// Filtres per als diàlegs d'obrir/exportar arxius
    QString m_openFileFilters;
    QString m_exportFileFilters;
    QString m_exportToJpegFilter;
    QString m_exportToMetaIOFilter;
    QString m_exportToTiffFilter;
    QString m_exportToBmpFilter;
    QString m_exportToPngFilter;
    QString m_exportToDicomFilter;

    /// Path actual del directori de treball ( obrir fitxer )
    QString m_workingDirectory;

    ///Path del directori de treball ( exportació a altres formats )
    QString m_exportWorkingDirectory;

    /// Aquesta string indica el path del directori que conté els arxius de traducció de l'aplicació
    QString m_qmPath;

    /// Indica l'idioma per defecte amb el que s'inicialitza l'aplicació, tindríem en_GB, en_US, ca_ES i es_ES
    QString m_defaultLocale;

    /// La llista de idiomes disponibles amb la que omplim el menú d'idiomes
    QStringList m_locales;

    /// Menús
    QMenu *m_fileMenu;
    QMenu *m_databaseMenu;
    QMenu *m_segmentationMenu;
    QMenu *m_clusteringMenu;
    QMenu *m_registrationMenu;
    QMenu *m_visualizationMenu;
    QMenu *m_colorMenu;
    QMenu *m_toolsMenu;
    QMenu *m_optionsMenu;
    QMenu *m_languageMenu;
    QMenu *m_helpMenu;
    QMenu *m_windowMenu;
    
    /// Sub menús
    QMenu *m_importFilesMenu;
    QMenu *m_exportFilesMenu;
    QMenu *m_recentFilesMenu;
    
    /// Barres d'eines
    QToolBar *m_extensionsToolBar;
    QToolBar *m_fileToolBar;
    
    /// Accions
    QAction *m_defaultViewerAction;
    QAction *m_basicViewAction;
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
    QAction *m_exportToJpegAction;
    QAction *m_exportToMetaIOAction;
    QAction *m_exportToTiffAction;
    QAction *m_exportToPngAction;
    QAction *m_exportToBmpAction;

    /// Per indicar el procés al obrir-se un fitxer
    QProgressDialog *m_progressDialog;

    /// El repositori de volums
    udg::VolumeRepository* m_volumeRepository;

    /// Lectura d'arxius
    udg::Input* m_inputReader;

    /// Escriptura/Exportació de formats d'arxius
    udg::Output* m_outputWriter;

    // :::::::::::::::::::::::::::
    // mètodes
    // :::::::::::::::::::::::::::
    
    ///Crea i inicialitza les accions de l'aplicació
    void createActions();

    /// Crea els menús.
    void createMenus();

    /// Crea la barra d'eines.
    void createToolBars();

    /// Crea la barra d'estat de l'aplicació
    void createStatusBar();

    /// Llegeix la configuració inicial amb la que engega el programa
    void readSettings();

    /// Escriu la configuració amb la que s'engegarà el programa el pròxim cop. \TODO: s'hauria de cridar també quan obrim una finestra nova?
    void writeSettings();

    ///Retorna el numero de finestres amb diferents models que tenim obertes, el num. de QApplicationMainWindow
    unsigned int getCountQApplicationMainWindow();

    /// Especifica el fitxer acctual que s'està tractant \TODO \deprecated aquest mètode esdevindrà antiquat en breu i no té gaure sentit desar el nom del fitxer, el metode continua existint perquè s'ha arrastrat des del principi
    void setCurrentFile( const QString &fileName );

    /// actualitza el llistat de noms de fitxers oberts recentment \TODO, s'hauria d'adaptar si cal quqan hi hagi lo del pacs
    void updateRecentFileActions();

    /// Mètode intern per extreure el nom del fitxer tal qual sense el path sencer
    QString strippedName(const QString &fullFileName);

private slots:
    /// Obre una nova finestra
    void newFile();

    /// Slot tancar volum
    void close();

    /// mostra el formulari d'about
    void about();

    /// obre un dels arxius recents::\TODO passar responsabilitat a la mini-app encarregada de fer això
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
    
signals:
    /// senyal emesa que indica si la finestra conté un volum o no
    void containsVolume( bool );
};

}; // fi namespace udg

#endif
