/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGQUERYSCREEN_H
#define UDGQUERYSCREEN_H

#include "ui_queryscreenbase.h"

namespace udg {

class Status;
class QCreateDicomdir;
class DicomMask;
class QOperationStateScreen;
class StatsWatcher;
class RISRequestManager;

/** Aquesta classe crea la interfície princial de cerca, i connecta amb el PACS i la bd dades local per donar els resultats finals
@author marc
*/
class QueryScreen : public QDialog , private Ui::QueryScreenBase{
Q_OBJECT
public:
    /**Constuctor de la classe
    * @param parent
    * @return
    */
    QueryScreen( QWidget *parent = 0 );

    ///Destructor de la classe
    ~QueryScreen();

public slots:
    /// Obre un dicomdir
    void openDicomdir();

    /// Actualitza la configuració que l'usuari hagi pogut canviar des del diàleg de configuració
    void updateConfiguration(const QString &configuration);

    /// Si la finestra no és visible o està radera d'una altra, la fa visible i la porta al davant de les finestres.
    void bringToFront();

    /// Mostra la pestanya de PACS. 
    /// TODO Deixar els camps de cerca com estaven, fer un clear o posar valors per defecte?
    /// Es podria passar un paràmetre "bool clear" amb valor per defecte (false, per exemple)
    void showPACSTab();

    /// Mostra tots els estudis en local i reseteja els camps de cerca
    void showLocalExams();

    /// Neteja els LineEdit del formulari
    void clearTexts();

    /**Guarda els objectes que compleixien la màscara passada per paràmetres de l'estudi passat per paràmetre al primer PACS 
       que es troba com a PACS per defecte*/
    void storeDicomObjectsToPacs(Study *studyToStore, DicomMask dicomMaskObjectsToStore);

signals:
    /// Signal que ens indica quins pacients s'han seleccionat per visualitzar
    void selectedPatients( QList<Patient *> selectedPatients );

protected :
    /** Event que s'activa al tancar al rebren un event de tancament
     * @param event de tancament
     */
    void closeEvent( QCloseEvent* event );

private slots:

    /// Escull a on fer la cerca, si a nivell local o PACS
    void searchStudy();

    /** Al canviar de pàgina del tab hem de canviar alguns paràmetres, com activar el boto Retrieve, etec..
     * @param index del tab al que s'ha canviat
     */
    void refreshTab( int index );

    void updateOperationsInProgressMessage();

    /// Mostra/amaga els camps de cerca avançats
    void setAdvancedSearchVisible(bool visible);

    ///Mostra la pantalla QOperationStateScreen
    void showOperationStateScreen();

    void viewPatients(QList<Patient*>);

    ///Es comunica amb el widget de la base de dades i visualitzar un estudi descarregat del PACS
    void viewRetrievedStudyFromPacs(QString studyInstanceUID);

    ///Slot que s'activa quan es rep una petició del RIS per descarregar un estudi d'un determinat PACS
    void retrieveStudyFromRISRequest(QString pacsID, Study *study);

private:

    #ifdef STARVIEWER_LITE //Al fer remove de la pestanya del pacs es canvia el index de cada tab, per això hem de redefinir-lo pel cas de StarviewerLite
        enum TabType{ PACSQueryTab = -1, LocalDataBaseTab = 0, DICOMDIRTab = 1 };
    #else 
        enum TabType{ LocalDataBaseTab = 0, PACSQueryTab = 1, DICOMDIRTab = 2 };
    #endif

    ///Connecta els signals i slots pertinents
    void createConnections();

    /** Construeix la màscara d'entrada pels dicom a partir dels widgets de cerca
     * @return retorna la màscara d'un objecte dicom
     */
    DicomMask buildDicomMask();

    ///Comprova els requeriments necessaris per poder utilitzar la QueryScreen
    void checkRequeriments();

    ///Es comprova la integritat de la base de dades i les imatges, comprovant que la última vegada l'starviewer no s'hagués tancat amb un estudi a mig baixar, i si és així esborra l'estudi a mig descarregar i deixa la base de dades en un estat integre
    void checkDatabaseImageIntegrity();

    ///Comprova que el port pel qual es reben els objectes dicom a descarregar no estigui sent utitlitzat per cap altre aplicació, si és aixì donar una missatge d'error
    void checkIncomingConnectionsPacsPortNotInUse();

    ///En el cas que l'error que se li passa com a paràmetre realment sigui un error, mostrarà un missatge a l'usuari explicant-lo.
    ///Es retorna true en el cas que hi hagi error, false si no n'hi ha.
    bool showDatabaseManagerError(LocalDatabaseManager::LastError error, const QString &doingWhat = "");

    ///inicialitza les variables necessaries, es cridat pel constructor
    void initialize();

    /**
     * Llegeix i aplica dades de configuració
     */
    void readSettings();

    ///Guarda els settings de la QueryScreen
    void writeSettings();

private:

    /** A la pestanya de dicomdir no s'ha de mostrar el QPacsList, per tant a la pestany de dicomdir
     * automaticament l'amaguem, i si tornem a la pestanya de la cache o del pacs, si anteriorment
     * estava desplagat es mostra el QPacsList, per això utilitzem el m_PacsListShow que guarda si
     * per la Cache o el PACS el QPacsList es mostrava. Llavors el m_PacsListIsShowed és utilitzat
     * independentment de la pestanya per saber si en aquells moments s'està mostran el QPacsListShow
     */
    bool m_showPACSNodes;
    bool m_pacsListIsShowed;

    QOperationStateScreen *m_operationStateScreen;
    QCreateDicomdir *m_qcreateDicomdir;

    RISRequestManager *m_risRequestManager;

    StatsWatcher *m_statsWatcher;
};

};

#endif
