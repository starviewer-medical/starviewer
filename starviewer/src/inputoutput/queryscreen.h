/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGQUERYSCREEN_H
#define UDGQUERYSCREEN_H

#include <QHash>

#include "ui_queryscreenbase.h"
#include "qexecuteoperationthread.h"
#include "multiplequerystudy.h"
#include "pacsparameters.h"
#include "localdatabasemanager.h"
#include "listenrisrequestthread.h"
#include "qpopuprisrequestsscreen.h"

namespace udg {

class Status;
class PacsServer;
class QCreateDicomdir;
class ProcessImageSingleton;
class DicomMask;
class QOperationStateScreen;
class StatsWatcher;

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

#ifndef STARVIEWER_LITE
    /// Mostra la pestanya de PACS. 
    /// TODO Deixar els camps de cerca com estaven, fer un clear o posar valors per defecte?
    /// Es podria passar un paràmetre "bool clear" amb valor per defecte (false, per exemple)
    void showPACSTab();
#endif

    /// Mostra tots els estudis en local i reseteja els camps de cerca
    void showLocalExams();

    /// Neteja els LineEdit del formulari
    void clearTexts();

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

    /** Busca la informació d'una sèrie
     * @param studyUID UID de l'estidi
     */
    void expandStudy( const QString &studyUID);

    /** Busca la informació d'una sèrie
     * @param studyUID UID de l'estidi
     * @param seriesUID
     */
    void expandSeries( const QString &studyUID, const QString &seriesUID);

    /** Al canviar de pàgina del tab hem de canviar alguns paràmetres, com activar el boto Retrieve, etec..
     * @param index del tab al que s'ha canviat
     */
    void refreshTab( int index );

    /** Aquest mètode s'encarrega de començar la descarrega d'un estudi, estableix la màscara, insereix l'estudi i la sèria a la caché, ademés de crear el thread per continuar amb la descàrrega de l'estdui
     * @param view boolea que indica si després de la descarrega s'ha de visualitzar l'estudi
     */
    void retrieve(bool view = false);

    /// Visualitza un estudi, si aquest estudi esta en el pacs el descarrega i posteriorment es visualitza, si es de la cache el carrega a la classe volum i es visualitza
    void view();

    /** Slot que s'activa per la classe qexecuteoperationthread, que quant un estudi ha estat descarregat el visualitzar, si l'usuari així ho ha indicat
     * @param studyUID studyUID de l'estudi descarregat
     * @param seriesUID seriesUID de la sèrie descarregada
     * @param sopInstanceUID de la imatge descarregada
     */
    void studyRetrievedView( QString studyUID , QString seriesUID , QString sopInstanceUID );

    /** Slot que s'activa pel signal de la classe MultimpleQueryStudy, quan s'ha produit un error al connectar amb el pacs
     * @param pacsID ID del pacs a la base de ades local
     */
    void errorConnectingPacs(QString pacsID);

    /** Slot que s'activa pel signal de la classe MultimpleQueryStudy, quan s'ha produit un error al fer una query d'estudis amb el pacs
     * @param id del PACS
     */
    void errorQueringStudiesPacs( QString PacsID );

    /** guarda els estudis seleccionats a m_studyTreeWidgetCache al PACS
     */
    void storeStudiesToPacs();

    /** Cerca les imatges d'una sèrie al PACS
     * @param StudyUID uid de l'estudi
     * @param SeriesUID  uid de la sèrie
     * @param PacsAETitle AETitle del PACS a buscar la sèrie
     */
    void queryImagePacs(QString StudyUID, QString SeriesUID);

    void updateOperationsInProgressMessage();

    /// Mostra/amaga els camps de cerca avançats
    void setAdvancedSearchVisible(bool visible);

    ///Mostra la pantalla QOperationStateScreen
    void showOperationStateScreen();

    ///Ens Mostra un missatge indicant l'error produït a la QExecuteOperationThread, i com es pot solucionar
    void showQExecuteOperationThreadError(QString studyInstanceUID, QString pacsID, QExecuteOperationThread::OperationError error);

    ///Ens mostra un message box indicant l'error produït mentre s'esperaven peticions del RIS
    void showListenRISRequestThreadError(ListenRISRequestThread::ListenRISRequestThreadError);

    ///Passant-li la màscara resultant de parserjar la petició del RIS descarrega l'estudi que el RIS ha sol·licitat
    void retrieveStudyFromRISRequest(DicomMask maskRisRequest);

    void viewPatients(QList<Patient*>);

    ///Fa un refresc del estudis que es mostren al tab de base de dades local
    void refreshLocalDatabaseTab();

private:

    #ifdef STARVIEWER_LITE //Al fer remove de la pestanya del pacs es canvia el index de cada tab, per això hem de redefinir-lo pel cas de StarviewerLite
        enum TabType{ PACSQueryTab = -1, LocalDataBaseTab = 0, DICOMDIRTab = 1 };
    #else 
        enum TabType{ LocalDataBaseTab = 0, PACSQueryTab = 1, DICOMDIRTab = 2 };
    #endif

    ///Connecta els signals i slots pertinents
    void createConnections();

    DicomMask buildSeriesDicomMask(QString);

    /** Construeix la màscara d'entrada pels dicom
     * @return retorna la màscara d'un objecte dicom
     */
    DicomMask buildDicomMask();

    /** construeix la màscara de les dates
     * @return retorna la màscara de les dates
     */
    QString getStudyDatesStringMask();

    /** Descarrega una estudi del pacs
     * @param indica si l'estudi s'ha de visualitzar
     * @param pacsIdToRetrieve indica l'id del Pacs del qual s'ha de descarregar l'estudi
     * @param maskStudyToRetrieve la màscara dels objectes a descarregar
     * @param studyToRetrieve L'estudi al qual pertanyen els objectes ad escarregar
     */
    void retrieveFromPacs(bool view, QString pacsIdToRetrieve, DicomMask mask, Study *studyToRetrieve);

    /// Cerca als pacs seleccionats
    void queryStudyPacs();

    ///Cerca els estudis que compleixen la màscara de cerca als pacs passats per paràmetre
    Status queryMultiplePacs(DicomMask searchMask, QList<PacsParameters> listPacsToQuery, MultipleQueryStudy *multipleQueryStudy);

    /** Busca la informació d'una sèrie en el PACS i la mostra en la interfície
     * @param studyUID UID de l'estidi
     */
    void querySeriesPacs(QString studyUID);

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

    ///Crear el menú contextual del QStudyTreeWidgetPacs
    void CreateContextMenuQStudyTreeWidgetPacs();

    ///Estableix la mida de les columnes de QStudyTreeWidget
    void setQStudyTreeWidgetColumnsWidth();

    /** Donat un AETitle busca les dades del PACS a la configuració i prepara un objecte PACSERVER, per poder
     * connectar al PACS
     * @param AETitlePACS Aetitle del PACS a connectar
     * @return Objecte ParcsServer a punt per connectar amb el PACS
     */
    PacsServer getPacsServerByPacsID(QString pacsID);

    /**
     * Llegeix i aplica dades de configuració
     */
    void readSettings();

    ///Guarda els settings de la QueryScreen
    void writeSettings();

    ///Guarda la mida de les columnes del QStudyTreeView Pacs, dicomdir i cache
    void saveQStudyTreeWidgetColumnsWidth();

    ///Retorna l'ID del pacs al que pertany l'estudi passat per paràmetre, només té en compte els estudis que s'han consultat a la última query
    QString getPacsIDFromQueriedStudies(QString studyInstanceUID);

private:

/// estructura necessària per passar els paràmetres al thread que descarregarrà les imatges
struct retrieveParameters
     {
        QString studyUID;
        PacsParameters pacs;
      };

    retrieveParameters retParam;

    ProcessImageSingleton *m_processImageSingleton;

    /// Ha de ser global, sino l'objecte es destrueix i QT no té temps d'atendre els signals dels threads
    MultipleQueryStudy m_multipleQueryStudy;

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
    QExecuteOperationThread m_qexecuteOperationThread;

    QMenu m_contextMenuQStudyTreeWidgetCache, m_contextMenuQStudyTreeWidgetPacs;

    ListenRISRequestThread *m_listenRISRequestThread;

    QPopUpRisRequestsScreen *m_qpopUpRisRequestsScreen; //Popup que indica que el RIS ha fet una petició per descarregar un estudi

    QHash<QString, QString> m_hashPacsIDOfStudyInstanceUID;

    StatsWatcher *m_statsWatcher;
};

};

#endif
