/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/ 
#ifndef UDGQUERYSCREEN_H
#define UDGQUERYSCREEN_H

#include "ui_queryscreenbase.h"
#include "multiplequerystudy.h"
#include "processimagesingleton.h"
#include "studyvolum.h"
#include "serieslistsingleton.h"
#include "qexecuteoperationthread.h"
#include "qretrievescreen.h"
#include "qcreatedicomdir.h"
#include "readdicomdir.h"

namespace udg {

class SeriesList;
class Status;
class SeriesMask;
class ReadDicomdir;

/** Aquesta classe crea la interfície princial de cerca, i connecta amb el PACS i la bd dades local per donar els resultats finals
@author marc
*/
class QueryScreen : public QWidget , private Ui::QueryScreenBase{
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

    /// Neteja els LineEdit del formulari
    void clearTexts();
    
    /// Fa una cerca dels estudis fets avui
    void searchTodayStudy();
    
    /// Fa una cerca dels estudis d'ahir
    void searchYesterdayStudy();
    
    /// Escull a on fer la cerca, si a nivell local o PACS
    void searchStudy();
    
    /** Busca la informació d'una sèrie
     * @param studyUID UID de l'estidi
     * @param pacsAETItle AEtitle del pacs a buscar la sèrie          
     */
    void searchSeries( QString , QString );
    
    /** Al canviar de pàgina del tab hem de canviar alguns paràmetres, com activar el boto Retrieve, etec..
     * @param index del tab al que s'ha canviat
     */
    void tabChanged( int index );
    
    /** Aquest mètode s'encarrega de començar la descarrega d'un estudi, estableix la màscara, insereix l'estudi i la sèria a la caché, ademés de crear el thread per continuar amb la descàrrega de l'estdui
     * @param view boolea que indica si després de la descarrega s'ha de visualitzar l'estudi
     */
    void retrieve();

    /// Slot que mostra la interfície QRetrieveScreen
    void showRetrieveScreen();
    
    ///Slot que mostra la interfície QCreateDicomdir
    void showCreateDicomdirScreen();

    /// Visualitza un estudi, si aquest estudi esta en el pacs el descarrega i posteriorment es visualitza, si es de la cache el carrega a la classe volum i es visualitza
    void view();
    
    ///Mostran la interficia QConfigurationScreen
    void config();
    
    ///Mostra la llista de Pacs, o l'amaga
    void showPacsList();
    
    ///slot que s'activa al esborrar estudi de la caché
    void deleteStudyCache();
    
    /** Slot que s'activa per la classe qexecuteoperationthread, que quant un estudi ha estat descarregat el visualitzar, si l'usuari així ho ha indicat
     * @param UID de l'estudi descarregat
     */
    void studyRetrievedView( QString );   
    
    /// Slot que activa o desactiva el m_checkAll en funció de si hi ha alguna modalitat d'estudi seleccionada
    void setCheckAll();
    
    /** Quant la data from canvia, amb aquest slot també es canvia la data del TO, per a que vagin sincronitzats
     * @param data 
     */
    void dateFromChanged( const QDate & );
    
    /// Activa o desactiva els text i el label de la data fins
    void setEnabledTextTo( int );
    
    /// Activa o desactiva el text de la data desde
    void setEnabledTextFrom( int );
        
    /// Posa a verdader o fals tots els check modality, i deixa a true el all
    void clearCheckedModality();
   
    /// Slot que s'activa pel signal notEnoughFreeSpace de QExecuteOperationThread, que s'emiteix quan no hi ha prou espai al disc per descarregar nous estudis
    void notEnoughFreeSpace();
    
    /** Slot que s'activa pel signal de la classe MultimpleQueryStudy, quan s'ha produit un error al connectar amb el pacs
     * @param id del PACS
     */
    void errorConnectingPacs( int );

    /** Slot que s'activa pel signal de la classe MultimpleQueryStudy, quan s'ha produit un error al fer una query d'estudis amb el pacs
     * @param id del PACS
     */
    void errorQueringStudiesPacs( int );
    
    ///Slot que s'activa quant s'ha produit un error alliberant espai al disc
    void errorFreeingCacheSpace();

    /** Slot que s'activa quant s'ha descarregat un estudi, prove de la classe QExecuteOperationThread
     * @param studyUID UID de l'estudi descarregat
     */
    void studyRetrieveFinished( QString studyUID );

    /** Afegeix l'estudi a la llista d'estudis per convertir a Dicomdir
     * @param studyUID UID de l'estudi a covnertir a Dicomdir
     */
    void convertToDicomdir( QString studyUID );

    /** Obre un dicomdir
     */
    void openDicomdir(); 

signals :
     
    /** Signal que s'emet quan es vol visualtizar un estudi cap a ExtensionHandler
     * @param Volum de l'estudi a visualitzar 
     */
    void viewStudy( StudyVolum );
    
    /// Signal cap a QSeriesListWidget, que neteja la llista de sèries del Widget
    void clearSeriesListWidget();

protected :
    
    /** Event que s'activa al tancar al rebren un event de tancament
     * @param event de tancament
     */
    void closeEvent( QCloseEvent* ce );
            
private:


//estructura necessària per passar els paràmetres al thread que descarregarrà les imatges
struct retrieveParameters
     {
        std::string studyUID;
        PacsParameters pacs;
      };   

    retrieveParameters retParam; 
    StudyListSingleton *m_studyListSingleton; //aquest es utilitzat per buscar estudis al pacs
    StudyList m_studyListCache;//aquest es utilitzat per buscar estudis a la cache
    SeriesListSingleton *m_seriesListSingleton;
    SeriesList m_seriesListCache;
    ProcessImageSingleton *m_piSingleton;
    
    MultipleQueryStudy multipleQueryStudy;//Ha de ser global, sino l'objecte es destrueix i QT no té temps d'atendre els signals dels threads  
    
    ReadDicomdir m_readDicomdir;// conté la informació del dicomdir obert en aquests instants

    //StudyVolum m_volum;
    bool m_PacsListShow;
    
    QRetrieveScreen *m_retrieveScreen;
    QCreateDicomdir *m_qcreateDicomdir;
    QExecuteOperationThread m_qexecuteOperationThread;   
    
    /** Activa o desactiva els checkbox per buscar per modalitat, en funcio del paràmetre passat
     * @param boolea que indica si s'ha d'activar els checkbox de modalitat o desactivar
     */
    void setEnabledModalityChecks( bool );
    
    ///Connecta els signals i slots pertinents
    void connectSignalsAndSlots();    
    
    /** Activa els labels i text de la data segons el parametre d'entrada
     * @param Indica si s'ha d'activar o desactivar
     */
    void setEnabledDates(bool);

    /** Construeix la màscara de cerca de la sèrie
     * @param UID de l'estudi
     * @return màscara
     */
    SeriesMask buildSeriesMask(QString);
    
    /** valida que la màscara de cerca no estigui buida, en el cas que ho sigui s'haurà d'avisar al usuari, perquè fer una cerca al Pacs sense filtrar potser molt lenta, al haver de mostrar totes les dades
     * @return indica si el filtre de cerca està buit
     */
    bool validateNoEmptyMask();
        
    /** Construeix la màscara d'entrada pels estudis
     * @return retorna la màscara d'un estudi
     */
    StudyMask buildStudyMask();
    
    /** Contrueix el nom del pacient per a crear la màscara, el format del la màscara de pacient ha de ser "*" o "congoms* Nom*"
     * @return retorna la màscara amb el nom del pacient
     */
    QString buildPatientName();
    
    /** construeix la màscara de les dates
     * @return retorna la màscara de les dates
     */
    QString buildStudyDates();
    
    /** Descarrega una estudi del pacs
     * @param indica si l'estudi s'ha de visualitzar
     */
    void retrievePacs( bool view );
    
    /** Carrega un estudi de la cache perque pugui ser visualitzat a la classe Volum i emet una senyal perque sigui visualitzat
     * @param UID de l'estudi
     * @param UID de la serie que s'ha de visualitzar per defecte, si es buit, es posara per defecte la primera serie de l'estudi
    */
    void retrieveCache( QString StudyUID , QString SeriesUID );
    
    /** Insereix un estudi a descarregar a la cache
     * @param estudi a insertat
     * @return retorna si la operacio s'ha realitzat amb èxit
     */
    Status insertStudyCache( Study );
    
    /// Cerca als pacs seleccionats
    void queryStudyPacs();
    
    /// Busca un estudi a la cache local
    void queryStudyCache();
    
    ///Cerca un estudi en el dicomdir obert en aquells moment
    void queryStudyDicomdir();

    /** Busca la informació d'una sèrie en el PACS i la mostra en la interfície
     * @param studyUID UID de l'estidi
     * @param pacsAETItle AEtitle del pacs a buscar la sèrie    
     * @param show Si es verdader mostra les dades de la sèrie per pantalla, pot ser que no les volguem mostrar, per exemple el cas que volem la informació per guardar-la en la caché al descarragar-nos una imatge
     */
    void QuerySeriesPacs( QString , QString , bool );
    
    /** Cerca les sèries d'un estudi a la Cache local
     * @param StudyUID UID de l'estudi a cercar
     */
    void QuerySeriesCache( QString );

    /** Cerca les sèries d'un estudi al Dicomdir
     * @param StudyUID UID de l'estudi a cercar
     */
    void querySeriesDicomdir( QString studyUID);
    
    /// esborra els estudis vells de la cache
    void deleteOldStudies();
    
    /** Tracta els errors de la base de dades
     * @param Estat del mètode de la base de dades
     */
    void databaseError( Status *state );
    
    /// Centra la finestra a la pantalla
    void centerWindow();
    
    /// Assigna la mida al PacsList en funcio del tab en que es trobi i de la variable m_PacsListShow
    void resizePacsList();

    ///inicialitza les variables necessaries, es cridat pel constructor
    void initialize();
	
    /** Fa el log, indicant amb quins parametres es cerquen els estudis
     * @return retorna un QString indicant amb quins paràmetres es fa la cerca d'estudis	
     */
    QString logQueryStudy();
};

};

#endif
