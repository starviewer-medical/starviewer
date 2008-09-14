/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGQUERYSCREEN_H
#define UDGQUERYSCREEN_H

#include "ui_queryscreenbase.h"
#include "qexecuteoperationthread.h"
#include "dicomdirreader.h"
#include "multiplequerystudy.h"
#include "dicomstudy.h"
#include "pacsparameters.h"
#include "localdatabasemanager.h"

namespace udg {

class Status;
class PacsServer;
class PatientFillerInput;
class QCreateDicomdir;
class ProcessImageSingleton;
class DicomMask;
class QOperationStateScreen;

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

signals:

    ///Signal per quan tenim un pacient carregat a punt per processar-lo per visualitzar
    void selectedPatient( Patient *patientToProcess, QString selectedSeries);

protected :
    /** Event que s'activa al tancar al rebren un event de tancament
     * @param event de tancament
     */
    void closeEvent( QCloseEvent* event );

private slots:

    /// Neteja els LineEdit del formulari
    void clearTexts();

    /// Escull a on fer la cerca, si a nivell local o PACS
    void searchStudy();

    /** Busca la informació d'una sèrie
     * @param studyUID UID de l'estidi
     * @param pacsAETItle AEtitle del pacs a buscar la sèrie
     */
    void expandStudy( QString studyUID , QString pacsAETitle );

    /** Busca la informació d'una sèrie
     * @param studyUID UID de l'estidi
     * @param seriesUID
     * @param pacsAETItle AEtitle del pacs a buscar la sèrie
     */
    void expandSeries( QString , QString , QString );

    /** Al canviar de pàgina del tab hem de canviar alguns paràmetres, com activar el boto Retrieve, etec..
     * @param index del tab al que s'ha canviat
     */
    void refreshTab( int index );

    /** Aquest mètode s'encarrega de començar la descarrega d'un estudi, estableix la màscara, insereix l'estudi i la sèria a la caché, ademés de crear el thread per continuar amb la descàrrega de l'estdui
     * @param view boolea que indica si després de la descarrega s'ha de visualitzar l'estudi
     */
    void retrieve();

    ///importa el dicomdir a la nostra base de ades
    void importDicomdir();

    /// Visualitza un estudi, si aquest estudi esta en el pacs el descarrega i posteriorment es visualitza, si es de la cache el carrega a la classe volum i es visualitza
    void view();

    ///slot que s'activa al esborrar estudi de la caché
    void deleteSelectedStudiesInCache();

    /** Slot que s'activa per la classe qexecuteoperationthread, que quant un estudi ha estat descarregat el visualitzar, si l'usuari així ho ha indicat
     * @param studyUID studyUID de l'estudi descarregat
     * @param seriesUID seriesUID de la sèrie descarregada
     * @param sopInstanceUID de la imatge descarregada
     */
    void studyRetrievedView( QString studyUID , QString seriesUID , QString sopInstanceUID );

    /** Slot que s'activa pel signal de la classe MultimpleQueryStudy, quan s'ha produit un error al connectar amb el pacs
     * @param pacsID ID del pacs a la base de ades local
     */
    void errorConnectingPacs( int pacsID );

    /** Slot que s'activa pel signal de la classe MultimpleQueryStudy, quan s'ha produit un error al fer una query d'estudis amb el pacs
     * @param id del PACS
     */
    void errorQueringStudiesPacs( int );

    /** Slot que s'activa quant s'ha descarregat un estudi, prove de la classe QExecuteOperationThread
     * @param studyUID UID de l'estudi descarregat
     */
    void studyRetrieveFinished( QString studyUID );

    /** Afegeix els estudis a la llista d'estudis per convertir a Dicomdir
     * @param studiesUIDList UID dels estudis a covnertir a Dicomdir
     */
    void convertToDicomdir();

    /** guarda els estudis seleccionats a m_studyTreeWidgetCache al PACS
     */
    void storeStudiesToPacs();

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

    void updateOperationsInProgressMessage();

    /// Mostra/amaga els camps de cerca avançats
    void setAdvancedSearchVisible(bool visible);

    ///S'activa quan seleccionem un estudi del m_qstudyTreeWidgetCache i envia al QSeriesListWidget la informació de les sèries de l'estudi
    void setSeriesToSeriesListWidgetCache();

    ///Mostra la pantalla QOperationStateScreen
    void showOperationStateScreen();

    ///Visualitza la sèrie sobre la que s'ha fet un view des del QSeriesListWidget
    void viewFromQSeriesListWidget();

private:
    enum TabType{ LocalDataBaseTab = 0, PACSQueryTab = 1, DICOMDIRTab = 2 };

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
     */
    void retrievePacs( bool view );

    /**
     * Donada una llista de uid's d'estudi, procedeix a carregar-los desde la font indicada (Cache,DICOMDIR)
     * @param studiesUIDList Llista d'uid's d'estudi
     * @param source font des d'on es volen carregar les dades (Cache i DICOMDIR suportats de moment)
     */
    void loadStudies( QStringList studiesUIDList, QString defaultSeriesUID , QString defaultSOPInstanceUID, QString source );

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
     */
    void querySeriesPacs(QString studyUID, QString pacsAETitle);

    /** Cerca les sèries d'un estudi a la font indicada (Cache,DICOMDIR)
     * @param studyUID UID de l'estudi a cercar
     * @param source
     */
    void querySeries( QString studyUID, QString source );

    /// esborra els estudis vells de la cache
    void deleteOldStudies();

    ///En el cas que l'error que se li passa com a paràmetre realment sigui un error, mostrarà un missatge a l'usuari explicant-lo.
    ///Es retorna true en el cas que hi hagi error, false si no n'hi ha.
    bool showDatabaseManagerError(LocalDatabaseManager::LastError error);

    ///inicialitza les variables necessaries, es cridat pel constructor
    void initialize();

    ///Crear el menú contextual del QStudyTreeWidgetCache
    void CreateContextMenuQStudyTreeWidgetCache();

    ///Crear el menú contextual del QStudyTreeWidgetPacs
    void CreateContextMenuQStudyTreeWidgetPacs();

    ///Crear el menú contextual del QStudyTreeWidgetDicomdir
    void CreateContextMenuQStudyTreeWidgetDicomdir();

    ///Estableix la mida de les columnes de QStudyTreeWidget
    void setQStudyTreeWidgetColumnsWidth();

    /** Construeix un string amb els parametres de cerca, per debug/logs
     * @return retorna un QString indicant amb quins paràmetres es fa la cerca d'estudis
     */
    QString buildQueryParametersString(DicomMask mask);

    ///Ens indica en en quina posició es troba dins la llista dels estudis trobats a la última query del PACS l'estudi amb l'UID passat per paràmetre i l'AETitle del PACS passat per paràmetre
    int getStudyPositionInStudyListQueriedPacs( QString studyUID , QString pacsAETitle );

    /** Donat un AETitle busca les dades del PACS a la base de dades i prepara un objecte PACSERVER, per poder
     * connectar al PACS
     * @param AETitlePACS Aetitle del PACS a connectar
     * @return Objecte ParcsServer a punt per connectar amb el PACS
     */
    Status preparePacsServerConnection( QString AETitlePACS , PacsServer *pacsConnection );

    /**
     * Llegeix i aplica dades de configuració
     */
    void readSettings();

    ///Guarda els settings de la QueryScreen
    void writeSettings();

    ///Guarda la mida de les columnes del QStudyTreeView Pacs, dicomdir i cache
    void saveQStudyTreeWidgetColumnsWidth();

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
    MultipleQueryStudy multipleQueryStudy;

    /// conté la informació del dicomdir obert en aquests instants
    DICOMDIRReader m_readDicomdir;

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

    QMenu m_contextMenuQStudyTreeWidgetCache, m_contextMenuQStudyTreeWidgetPacs, m_contextMenuQStudyTreeWidgetDicomdir;

    QList<DICOMStudy> m_studyListQueriedPacs;//llista dels estudis que s'han trobat a la última query al PACS
};

};

#endif
