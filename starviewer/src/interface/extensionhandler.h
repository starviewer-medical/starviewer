/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGEXTENSIONHANDLER_H
#define UDGEXTENSIONHANDLER_H

#include <QObject>
#include <QString>
#include "identifier.h"
#include "studyvolum.h"

namespace udg {

// Fordward Declarations
class VolumeRepository;
class Input;
class Output;
class QApplicationMainWindow;

class ResourceCollection;
class ExtensionFactory;

class QTabAxisViewExtensionCreator;
class QMPRExtensionCreator;
class QMPR3DExtensionCreator;
class QMPR3D2DExtensionCreator;

// Espai reservat pels fwd decl de les mini-apps
class AppImportFile;

// Fi de l'Espai reservat pels fwd decl de les mini-apps
class QueryScreen;
/**
Gestor de mini-aplicacions i serveis de l'aplicació principal

@author Grup de Gràfics de Girona  ( GGG )

\TODO: temporalment seguirem el patró 1 aplicació/servei : 1 mètode públic

*/
class ExtensionHandler : public QObject{
Q_OBJECT
public:
    ExtensionHandler( QApplicationMainWindow *mainApp , QObject *parent = 0, const char *name = 0);

    ~ExtensionHandler();

    // :::::::::::::::::::::::::::::::::::::::::
    // Inicialitzacions
    // :::::::::::::::::::::::::::::::::::::::::
    // Aquests mètodes estan destinats a aspectes de inicialitzacions
    /// Presenta les mini-apps a la aplicació principal, posant-les a l'abast. ( el mètode es podria dir també spreadExtensions o algo així)
    void introduceApplications();
    
    // :::::::::::::::::::::::::::::::::::::::::
    // Serveis/Aplicacions
    // :::::::::::::::::::::::::::::::::::::::::
    
    /// Obre un volum a partir d'un fitxer \TODO ara es fa tot a saco aquí dins però potser seria millor que ho fe suna clase externa especialitzada
    bool open( QString fileName );
    /// Miniaplicació per visualitzar el model carregat amb el que s'esta treballant
    void simpleAxisView();

    /// \TODO Això és un "apanyo"
    void setVolumeID( Identifier id ){ m_volumeID = id; }
    
public slots:
    /// rep la petició d'un servei/mini-aplicació i fa el que calgui
    void request( int who );
    void request( const QString &who );

    /// aplicació que s'executa per defecte quan carreguem un volum al repositori
    void onVolumeLoaded( Identifier id );

    /// [apanyo]es crida quan es demana un studi descarregat, es veu la pimera serie
    void viewStudy( StudyVolum study );
private:
    /// Punter a l'aplicació principal
    QApplicationMainWindow *m_mainApp;
    
    /// Factoria d'extensions
    ExtensionFactory *m_extensionFactory;
    
    /// Creadors d'extensions
    QTabAxisViewExtensionCreator *m_qTabAxisViewExtensionCreator;
    QMPRExtensionCreator *m_qMPRExtensionCreator;
    QMPR3DExtensionCreator *m_qMPR3DExtensionCreator;
    QMPR3D2DExtensionCreator *m_qMPR3D2DExtensionCreator;
    
    /// S'encarrega de fer el registre de totes les extensions amb el factory
    void registerExtensions();
    // :::::::::::::::::::::::::::::::::::::::::
    // Recursos
    // :::::::::::::::::::::::::::::::::::::::::

    /// Col·lecció de recursos de l'aplicació
    ResourceCollection *m_resources;
    
    /// L'id del volum amb el que estem treballant
    Identifier m_volumeID;    
    /// El repository de volums
    udg::VolumeRepository* m_volumeRepository;
    /// accés a l'entrada de dades -> això hauria de formar part d'una mini-app, per tant és temporal
    Input *m_inputReader;
    /// accés a la sortida/escriptura de dades -> això hauria de formar part d'una mini-app, per tant és temporal
    Output *m_outputWriter;
    
    // :::::::::::::::::::::::::::::::::::::::::
    // Mini Aplicacions
    // Aquí es declararan totes les mini-aplicacions que es faran servir
    // :::::::::::::::::::::::::::::::::::::::::
    
    /// Importar models del sistema de fitxers al repositori de volums
    AppImportFile *m_importFileApp;
    
    /// La pantalla d'accés al pacs
    QueryScreen *m_queryScreen;
};

};  //  end  namespace udg 

#endif
