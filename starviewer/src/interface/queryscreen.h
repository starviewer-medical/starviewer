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
#include "qoperationstatescreen.h"
#include "qcreatedicomdir.h"
#include "readdicomdir.h"
#include "imagelistsingleton.h"
#include "dicommask.h"

#include "patientfillerinput.h"

namespace udg {

class SeriesList;
class Status;
class ReadDicomdir;
class PacsServer;

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

    /** Busca la informació d'una sèrie
     * @param studyUID UID de l'estidi
     * @param seriesUID
     * @param pacsAETItle AEtitle del pacs a buscar la sèrie
     */
    void searchImages( QString , QString , QString );

    /** Al canviar de pàgina del tab hem de canviar alguns paràmetres, com activar el boto Retrieve, etec..
     * @param index del tab al que s'ha canviat
     */
    void tabChanged( int index );

    /** Aquest mètode s'encarrega de començar la descarrega d'un estudi, estableix la màscara, insereix l'estudi i la sèria a la caché, ademés de crear el thread per continuar amb la descàrrega de l'estdui
     * @param view boolea que indica si després de la descarrega s'ha de visualitzar l'estudi
     */
    void retrieve();

    ///importa el dicomdir a la nostra base de ades
    void importDicomdir();

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
     * @param studyUID studyUID de l'estudi descarregat
     * @param seriesUID seriesUID de la sèrie descarregada
     * @param sopInstanceUID de la imatge descarregada
     */
    void studyRetrievedView( QString studyUID , QString seriesUID , QString sopInstanceUID );

    /// Slot que activa o desactiva el m_checkAll en funció de si hi ha alguna modalitat d'estudi seleccionada
    void checkedSeriesModality();

    /** Quant la data from canvia, amb aquest slot també es canvia la data del TO, per a que vagin sincronitzats
     * @param data
     */
    void dateFromChanged( const QDate & );

    /// Activa o desactiva els text i el label de la data fins
    void setEnabledTextTo( int );

    /// Activa o desactiva el text de la data desde
    void setEnabledTextFrom( int );

    ///Si la data del m_textDate canviarà la data m_textTo, per si es vol cercar per una data concreta
    void searchStudyfromDateChanged( QDate fromDate );

    /// Posa a verdader o fals tots els check modality, i deixa a true el all
    void clearCheckedModality();

    /// Slot que s'activa pel signal notEnoughFreeSpace de QExecuteOperationThread, que s'emiteix quan no hi ha prou espai al disc per descarregar nous estudis
    void notEnoughFreeSpace();

    /** Slot que s'activa pel signal de la classe MultimpleQueryStudy, quan s'ha produit un error al connectar amb el pacs
     * @param pacsID ID del pacs a la base de ades local
     */
    void errorConnectingPacs( int pacsID );

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

    /** guarda un estudi en el PACS
     * @param studyUID uid de l'estudi a guardar
     */
    void storeStudyToPacs( QString studyUID );

    /** Cerca les imatges d'una sèrie al PACS
     * @param StudyUID uid de l'estudi
     * @param SeriesUID  uid de la sèrie
     * @param PacsAETitle AETitle del PACS a buscar la sèrie
     */
    void queryImagePacs( QString StudyUID , QString SeriesUID , QString PacsAETitle );

    /** Cerca les imatges d'una sèrie a la font indicada (Cache,DICOMDIR)
     * @param studyUID uid de l'estudi
     * @param seriesUID  uid de la sèrie
     * @param source  font de dades on consultar (Cache,DICOMDIR)
     */
    void queryImage(QString studyUID, QString seriesUID, QString source );

    /** Slot que s'activa quan s'ha editat el m_textOtherModality, en cas que el text sigui <> "" deselecciona totes les modalitats, i en cas que sigui = "" selecciona la modalitat checkAll
     */
    void textOtherModalityEdited();

signals :

    /** Signal que s'emet quan es vol visualtizar un estudi cap a ExtensionHandler
     * @param Volum de l'estudi a visualitzar
     */
    void viewStudy( StudyVolum );

    /// Signal similar a viewStudy(), però en aquest cas enviem tota la estructura PatientFillerInput que es continuarà processant per la classe que reculli aquest signal
    void viewPatient( PatientFillerInput input );

    /// Signal cap a QSeriesListWidget, que neteja la llista de sèries del Widget
    void clearSeriesListWidget();

    /// Signal que s'emet quan s'escull veure un Key Image Note. Es passa el path d'aquest
    void viewKeyImageNote( const QString & path);

    /// Signal que s'emet quan s'escull veure un Presentation State. Es passa el path d'aquest
    void viewPresentationState( const QString & path);

protected :

    /** Event que s'activa al tancar al rebren un event de tancament
     * @param event de tancament
     */
    void closeEvent( QCloseEvent* ce );

private:


//estructura necessària per passar els paràmetres al thread que descarregarrà les imatges
struct retrieveParameters
     {
        QString studyUID;
        PacsParameters pacs;
      };

    retrieveParameters retParam;
    StudyListSingleton *m_studyListSingleton; //aquest es utilitzat per buscar estudis al pacs
    SeriesListSingleton *m_seriesListSingleton;
    ImageListSingleton *m_imageListSingleton;
    ProcessImageSingleton *m_piSingleton;

    MultipleQueryStudy multipleQueryStudy;//Ha de ser global, sino l'objecte es destrueix i QT no té temps d'atendre els signals dels threads

    ReadDicomdir m_readDicomdir;// conté la informació del dicomdir obert en aquests instants

    /*A la pestanya de dicomdir no s'ha de mostrar el QPacsList, per tant a la pestany de dicomdir
     * automaticament l'amaguem, i si tornem a la pestanya de la cache o del pacs, si anteriorment
     estava desplagat es mostra el QPacsList, per això utilitzem el m_PacsListShow que guarda si
     per la Cache o el PACS el QPacsList es mostrava. Llavors el m_PacsListIsShowed és utilitzat
     independentment de la pestanya per saber si en aquells moments s'està mostran el QPacsListShow */
    bool m_PacsListShow;
    bool m_pacsListIsShowed;

    QOperationStateScreen *m_OperationStateScreen;
    QCreateDicomdir *m_qcreateDicomdir;
    QExecuteOperationThread m_qexecuteOperationThread;

    QString m_lastQueriedPacs;//Indica quin és l'últim pacs que hem consultat, això es per de cares anar al connectathon, ja que les messatools no retornen el tag indicant a quin pacs pertanyen, per això és necessari guardar quin és l'últim pacs consultat per saber si hem de descarregar l'estudi, consultar, sèrie etc a quin PACS atacar.

    ///Connecta els signals i slots pertinents
    void connectSignalsAndSlots();

    /** Activa els labels i text de la data segons el parametre d'entrada
     * @param Indica si s'ha d'activar o desactivar
     */
    void setEnabledDates(bool);

    DicomMask buildSeriesDicomMask(QString);

    /** valida que la màscara de cerca no estigui buida, en el cas que ho sigui s'haurà d'avisar al usuari, perquè fer una cerca al Pacs sense filtrar potser molt lenta, al haver de mostrar totes les dades
     * @return indica si el filtre de cerca està buit
     */
    bool validateNoEmptyMask();

    /** Construeix la màscara d'entrada pels dicom
     * @return retorna la màscara d'un objecte dicom
     */
    DicomMask buildDicomMask();

    /** construeix la màscara de les dates
     * @return retorna la màscara de les dates
     */
    QString buildStudyDates();

    /** Descarrega una estudi del pacs
     * @param indica si l'estudi s'ha de visualitzar
     */
    void retrievePacs( bool view );

    /** Carrega un estudi del source especificat (Cache,DICOMDIR) la cache perque pugui ser visualitzat a la
     *  classe Volum i emet una senyal perque sigui visualitzat
     * @param studyUID de l'estudi
     * @param seriesUID de la serie que s'ha de visualitzar per defecte, si es buit, es posara per defecte la primera serie de l'estudi
     * @param sopInstanceUID sopInstanceUID de l'imatge a visualitzar
     * @param source font des d'on es volen carregar les dades (Cache i DICOMDIR suportats de moment)
    */
    void retrieve( QString studyUID , QString seriesUID , QString sopInstanceUID, QString source );

    /** Insereix un estudi a descarregar a la cache
     * @param estudi a insertat
     * @return retorna si la operacio s'ha realitzat amb èxit
     */
    Status insertStudyCache( DICOMStudy );

    /// Cerca als pacs seleccionats
    void queryStudyPacs();

    /**
     * Cerca un estudi a la font indicada (Cache,DICOMDIR)
     * @param source la font que volem interrogar
     */
    void queryStudy( QString source );

    /** Busca la informació d'una sèrie en el PACS i la mostra en la interfície
     * @param studyUID UID de l'estidi
     * @param pacsAETItle AEtitle del pacs a buscar la sèrie
     * @param show Si es verdader mostra les dades de la sèrie per pantalla, pot ser que no les volguem mostrar, per exemple el cas que volem la informació per guardar-la en la caché al descarragar-nos una imatge
     */
    void querySeriesPacs( QString , QString , bool );

    /** Cerca les sèries d'un estudi a la font indicada (Cache,DICOMDIR)
     * @param studyUID UID de l'estudi a cercar
     * @param source
     */
    void querySeries( QString studyUID, QString source );

    /// esborra els estudis vells de la cache
    void deleteOldStudies();

    /** Tracta els errors de la base de dades
     * @param Estat del mètode de la base de dades
     */
    void databaseError( Status *state );

    /// Posiciona la pantalla en funció dels valors guardarts a StarviewerSettings
    void setWindowPosition();

    /// Estableix la mida de la pantalla en funció dels valors guardats a StarviewerSettings
    void setWindowSize();

    ///Retorna els Splitter al estat que estaven quan es va tancar l'aplicació
    void setQSplitterState();

    /// Assigna la mida al PacsList en funcio del tab en que es trobi i de la variable m_PacsListShow
    void resizePacsList();

    ///inicialitza les variables necessaries, es cridat pel constructor
    void initialize();

    /** Fa el log, indicant amb quins parametres es cerquen els estudis
     * @return retorna un QString indicant amb quins paràmetres es fa la cerca d'estudis
     */
    QString logQueryStudy();

    /** Afegeix una modalitat a cercar a la màscara d'estudi.
     * @param mask màscara a la que s'ha d'afegir la modalitat
     * @param modality modalitat a afegir
     */
    void addModalityStudyMask( DicomMask* mask, QString modality );

    /** Donat un AETitle busca les dades del PACS a la base de dades i prepara un objecte PACSERVER, per poder
     * connectar al PACS
     * @param AETitlePACS Aetitle del PACS a connectar
     * @return Objecte ParcsServer a punt per connectar amb el PACS
     */
    Status preparePacsServerConnection( QString AETitlePACS , PacsServer *pacsConnection );

    /// Mètode que a partir d'un StudyVolum emetrà el signal correcte depenent de la modalitat que s'intenta obrir
    void emitViewSignal(StudyVolum study);
};

};

#endif
