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
#include "patient.h"
#include "studyvolum.h"

namespace udg {

// Fordward Declarations
class VolumeRepository;
class Input;
class Output;
class QApplicationMainWindow;
class Volume;
class ExtensionFactory;

class QMPRExtensionCreator;
class QMPR3DExtensionCreator;
class QMPR3D2DExtensionCreator;
class Q2DViewerExtensionCreator;

// Espai reservat pels fwd decl de les mini-apps
class AppImportFile;

// Fi de l'Espai reservat pels fwd decl de les mini-apps
class QueryScreen;
/**
Gestor de mini-aplicacions i serveis de l'aplicació principal

@author Grup de Gràfics de Girona  ( GGG )

*/
class ExtensionHandler : public QObject{
Q_OBJECT
public:
    ExtensionHandler( QApplicationMainWindow *mainApp , QObject *parent = 0, const char *name = 0);

    ~ExtensionHandler();
    
    /// Obre un volum a partir d'un fitxer \TODO ara es fa tot a saco aquí dins però potser seria millor que ho fes una clase externa especialitzada
    bool open( QString fileName );
    
public slots:
    /// rep la petició d'un servei/mini-aplicació i fa el que calgui
    void request( int who );
    void request( const QString &who );

    /// aplicació que s'executa per defecte quan carreguem un volum al repositori
    void onVolumeLoaded( Identifier id );

    /// [apanyo] es crida quan es demana un studi descarregat, es veu la pimera serie
    void viewStudy( StudyVolum study );

    /// [apanyo] Slot que afegeix al 2n visor una sèrie escollida per comparar-> és un mètode moooolt temporal. 
    void viewStudyToCompare( StudyVolum study );

    /// cridat quan l'aplicació mor
    void killBill();
    
private:
    /// Punter a l'aplicació principal
    QApplicationMainWindow *m_mainApp;
    
    /// Factoria d'extensions
    ExtensionFactory *m_extensionFactory;
    
    /// Creadors d'extensions
    QMPRExtensionCreator *m_qMPRExtensionCreator;
    QMPR3DExtensionCreator *m_qMPR3DExtensionCreator;
    QMPR3D2DExtensionCreator *m_qMPR3D2DExtensionCreator;
    Q2DViewerExtensionCreator *m_q2DViewerExtensionCreator;

    /// Entitat pacient que es controlarà des d'aquí
    Patient m_patient;
    
    /// S'encarrega de fer el registre de totes les extensions amb el factory
    void registerExtensions();

    /// Crea les connexions de signals i slots
    void createConnections();
    
    // :::::::::::::::::::::::::::::::::::::::::
    // Recursos
    // :::::::::::::::::::::::::::::::::::::::::

    /// L'id del volum amb el que estem treballant
    Identifier m_volumeID;

    /// \TODO aquest ha de ser temporal
    Identifier m_compareVolumeID;

    /// El repository de volums
    VolumeRepository *m_volumeRepository;

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

private slots:

    /// Slot que es crida quan hem canviat d'una extensió a una altre
    void extensionChanged( int index );

    /// [temporal] Obre una sèrie per comparar en el visor per defecte
    void openSerieToCompare();

signals:
    /// Emet un senyal amb el segon volum per comparar
    void secondInput( Volume* );
};

};  //  end  namespace udg 

#endif
