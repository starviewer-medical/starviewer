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

class QProgressDialog;

namespace udg {

// Fordward Declarations
class VolumeRepository;
class QApplicationMainWindow;
class Volume;
class PatientFillerInput;
class Input;

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
    ExtensionHandler( QApplicationMainWindow *mainApp , QObject *parent = 0, QString name = 0);

    ~ExtensionHandler();

public slots:
    /// rep la petició d'un servei/mini-aplicació i fa el que calgui
    void request( int who );
    void request( const QString &who );

    /// aplicació que s'executa per defecte quan carreguem un volum al repositori
    void onVolumeLoaded( Identifier id );

    /// cridat quan l'aplicació mor
    void killBill();

    /**
     * Processa un conjunt d'arxius d'input i els processa per decidir què fer amb aquests, com per exemple
     * crear nous pacient, obrir finestres, afegir les dades al pacient actual, etc
     * @param inputFiles Els arxius a processar, que poden ser del tipus suportat per l'aplicació o no
     * @param defaultStudyUID Estudi que es vol visualitzar per defecte
     * @param defaultSeriesUID Serie que es vol veure per defecte
     * @param defaultImageInstance Imatge que es vol per defecte
     */
    void processInput( QStringList inputFiles, QString defaultStudyUID, QString defaultSeriesUID, QString defaultImageInstance );

    /**
     * Donada una estructura de pacient decideix que fer amb aquesta, com por exemple fusionar les dades si ja tenim
     * dades d'aquest mateix pacient, obrir finestres noves, alertar a l'usuari de les noves dades, etc.
     * @param patient L'estructura de pacient que volem afegir a l'aplicació
     */
    void addPatientData( Patient *patient );

    /**
     * Obrirà l'extensió per defecte. Si no hi ha dades de pacient vàlides, no farà res.
     */
    void openDefaultExtension();

private:
    /// Crea les connexions de signals i slots
    void createConnections();

    /// carrega una nova pestanya amb l'extensió de 2DViewer
    void load2DViewerExtension();

    /// Crea el diàleg de progrés al carregar un Volume.
    QProgressDialog* activateProgressDialog( Input *input );

private slots:
    /// Slot que es crida quan hem canviat d'una extensió a una altre
    void extensionChanged( int index );

private:
    /// Punter a l'aplicació principal
    QApplicationMainWindow *m_mainApp;

    /// L'id del volum amb el que estem treballant TODO tendirà a desaparèixer en breu
    Identifier m_volumeID;

    /// El repository de volums
    VolumeRepository *m_volumeRepository;

    /// Importar models del sistema de fitxers al repositori de volums
    AppImportFile *m_importFileApp;

    /// La pantalla d'accés al pacs
    QueryScreen *m_queryScreen;

    /// Membres de conveniència per saber quin estudi i series es volen veure per defecte
    QString m_defaultStudyUID, m_defaultSeriesUID;
};

};  //  end  namespace udg

#endif
