/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "queryscreen.h"

#include <QMessageBox>
#include <QCloseEvent>
#include <QFileDialog>
#include <QMovie>
#include <QContextMenuEvent>
#include <QShortcut>

#include "processimagesingleton.h"
#include "pacsconnection.h"
#include "qstudytreewidget.h"
#include "dicomseries.h"
#include "querypacs.h"
#include "pacsserver.h"
#include "qserieslistwidget.h"
#include "qpacslist.h"
#include "starviewersettings.h"
#include "cachepool.h"
#include "operation.h"
#include "cachelayer.h"
#include "pacslistdb.h"
#include "logging.h"
#include "status.h"
#include "cachestudydal.h"
#include "cacheseriesdal.h"
#include "cacheimagedal.h"
#include "qchooseoneobjectdialog.h"
#include "dicomdirimporter.h"
#include "patientfillerinput.h"
#include "qcreatedicomdir.h"
#include "dicommask.h"
#include "qoperationstatescreen.h"
#include "localdatabasemanager.h"
#include "patient.h"
#include "testdatabase.h"
#include "testdicomobjects.h"
#include <QTime>

namespace udg {

QueryScreen::QueryScreen( QWidget *parent )
 : QDialog(parent )
{
    setupUi( this );
    setWindowFlags( Qt::Widget );
    initialize();//inicialitzem les variables necessàries
    //connectem signals i slots
    createConnections();
    //esborrem els estudis vells de la cache
    deleteOldStudies();
    readSettings();
    //fem que per defecte mostri els estudis de la cache
    queryStudy("Cache");
}

QueryScreen::~QueryScreen()
{
    /*sinó fem un this.close i tenim la finestra queryscreen oberta al tancar l'starviewer, l'starviewer no finalitza
     *desapareixen les finestres, però el procés continua viu
     */
    this->close();
}

void QueryScreen::initialize()
{
    //indiquem que la llista de Pacs no es mostra
    m_showPACSNodes = false;
    m_PACSNodes->setVisible(false);

    /* Posem com a pare el pare de la queryscreen, d'aquesta manera quan es tanqui el pare de la queryscreen
     * el QOperationStateScreen també es tancarà
     */
    m_operationStateScreen = new udg::QOperationStateScreen( this );

    m_qcreateDicomdir = new udg::QCreateDicomdir( this );
    m_processImageSingleton = ProcessImageSingleton::getProcessImageSingleton();


    QMovie *operationAnimation = new QMovie(this);
    operationAnimation->setFileName(":/images/loader.gif");
    m_operationAnimation->setMovie(operationAnimation);
    operationAnimation->start();

    m_qadvancedSearchWidget->hide();
    m_operationAnimation->hide();
    m_labelOperation->hide();
    refreshTab( LocalDataBaseTab );

    CreateContextMenuQStudyTreeWidgetCache();
    CreateContextMenuQStudyTreeWidgetPacs();
    CreateContextMenuQStudyTreeWidgetDicomdir();

    setQStudyTreeWidgetColumnsWidth();
}

void QueryScreen::CreateContextMenuQStudyTreeWidgetCache()
{
    QAction *action;

    action = m_contextMenuQStudyTreeWidgetCache.addAction( QIcon(":/images/view.png") , tr( "&View" ) , this , SLOT( view() ) , tr("Ctrl+V") );
    (void) new QShortcut( action->shortcut() , this , SLOT( view() ) );

    action = m_contextMenuQStudyTreeWidgetCache.addAction( QIcon(":/images/databaseRemove.png") , tr( "&Delete" ) , this , SLOT( deleteSelectedStudiesInCache() ) , Qt::Key_Delete );
    (void) new QShortcut( action->shortcut() , this , SLOT( deleteSelectedStudiesInCache() ) );

    action = m_contextMenuQStudyTreeWidgetCache.addAction( tr( "Send to DICOMDIR List" ) , this , SLOT( convertToDicomdir() ) , tr( "Ctrl+M" ) );
    (void) new QShortcut( action->shortcut() , this , SLOT( convertToDicomdir() ) );

    action = m_contextMenuQStudyTreeWidgetCache.addAction( QIcon(":/images/store.png") , tr( "Store to PACS" ) , this , SLOT( storeStudiesToPacs() ) , tr( "Ctrl+S" ) );
    (void) new QShortcut( action->shortcut() , this , SLOT( storeStudiesToPacs() ) );

    m_studyTreeWidgetCache->setContextMenu( & m_contextMenuQStudyTreeWidgetCache );//Especifiquem que és el menú per la cache
}

void QueryScreen::CreateContextMenuQStudyTreeWidgetPacs()
{
    QAction *action;

    action = m_contextMenuQStudyTreeWidgetPacs.addAction( QIcon(":/images/view.png") , tr( "&View" ) , this , SLOT( view() ) , tr("Ctrl+V") );
    (void) new QShortcut( action->shortcut() , this , SLOT( view() ) );

    action = m_contextMenuQStudyTreeWidgetPacs.addAction( QIcon(":/images/retrieve.png") , tr("&Retrieve") , this , SLOT( retrieve() ) , tr("Ctrl+R") );
    (void) new QShortcut( action->shortcut() , this , SLOT( retrieve() ) );

    m_studyTreeWidgetPacs->setContextMenu( & m_contextMenuQStudyTreeWidgetPacs ); //Especifiquem que és el menú del PACS
}

void QueryScreen::CreateContextMenuQStudyTreeWidgetDicomdir()
{
    QAction *action;

    action = m_contextMenuQStudyTreeWidgetDicomdir.addAction( QIcon(":/images/view.png") , tr( "&View" ) , this , SLOT( view() ) , tr("Ctrl+V") );
    (void) new QShortcut( action->shortcut() , this , SLOT( view() ) );

    action = m_contextMenuQStudyTreeWidgetDicomdir.addAction( QIcon(":/images/retrieve.png") , tr("&Retrieve") , this , SLOT( importDicomdir() ) , tr("Ctrl+R") );
    (void) new QShortcut( action->shortcut() , this , SLOT( retrieve() ) );

    m_studyTreeWidgetDicomdir->setContextMenu( & m_contextMenuQStudyTreeWidgetDicomdir ); //Especifiquem que es el menu del dicomdir
}

void QueryScreen::setQStudyTreeWidgetColumnsWidth()
{
    StarviewerSettings settings;

    for ( int column = 0; column < m_studyTreeWidgetCache->getNumberOfColumns(); column++)
    {
        m_studyTreeWidgetCache->setColumnWidth( column , settings.getStudyCacheListColumnWidth(column) );
    }

    for ( int column = 0; column < m_studyTreeWidgetPacs->getNumberOfColumns(); column++)
    {
        m_studyTreeWidgetPacs->setColumnWidth( column , settings.getStudyPacsListColumnWidth(column) );
    }

    for ( int column = 0; column < m_studyTreeWidgetDicomdir->getNumberOfColumns(); column++)
    {
        m_studyTreeWidgetDicomdir->setColumnWidth( column , settings.getStudyDicomdirListColumnWidth(column) );
    }
}

void QueryScreen::setSeriesToSeriesListWidgetCache()
{
    QList<DICOMSeries> oldSeriesList;
    QList<Series*> seriesList;
    CacheSeriesDAL cacheSeriesDAL;
    CacheImageDAL cacheImageDAL;
    LocalDatabaseManager localDatabaseManager;
    int imagesNumber;
    Status state;
    DicomMask mask;
    QString studyInstanceUID = m_studyTreeWidgetCache->getCurrentStudyUID();

    INFO_LOG("Cerca de sèries a la cache de l'estudi " + studyInstanceUID);

    //preparem la mascara i cerquem les series a la cache
    mask.setStudyUID(studyInstanceUID);

    seriesList = localDatabaseManager.querySeries(mask);
    if (showDatabaseManagerError( localDatabaseManager.getLastError() ))    return;

    m_seriesListWidgetCache->clear();

    foreach(Series* series, seriesList)
    {
        mask.setSeriesUID(series->getInstanceUID());

        series->setImages(localDatabaseManager.queryImage(mask));
        if (showDatabaseManagerError( localDatabaseManager.getLastError() ))    return;

        m_seriesListWidgetCache->insertSeries(studyInstanceUID, series);
    }
}

void QueryScreen::deleteOldStudies()
{
    //TODO: Per implementar!
}

void QueryScreen::updateOperationsInProgressMessage()
{
    if (m_operationStateScreen->getActiveOperationsCount() > 0)
    {
        m_operationAnimation->show();
        m_labelOperation->show();
    }
    else
    {
        m_operationAnimation->hide();
        m_labelOperation->hide();
    }
}

void QueryScreen::createConnections()
{
    //connectem els butons
    connect( m_searchButton, SIGNAL( clicked() ), SLOT( searchStudy() ) );
    connect( m_clearToolButton, SIGNAL( clicked() ), SLOT( clearTexts() ) );
    connect( m_retrieveButtonPACS, SIGNAL( clicked() ), SLOT( retrieve() ) );
    connect( m_retrieveButtonDICOMDIR, SIGNAL( clicked() ), SLOT( importDicomdir() ) );
    connect( m_operationListToolButton, SIGNAL( clicked() ) , SLOT( showOperationStateScreen() ) );
    connect( m_showPACSNodesToolButton, SIGNAL( toggled(bool) ), m_PACSNodes, SLOT( setVisible(bool) ) );

    connect( m_viewButtonLocal, SIGNAL( clicked() ), SLOT( view() ) );
    connect( m_viewButtonPACS, SIGNAL( clicked() ), SLOT( view() ) );
    connect( m_viewButtonDICOMDIR, SIGNAL( clicked() ), SLOT( view() ) );
    connect( m_createDICOMDIRToolButton, SIGNAL( clicked() ), m_qcreateDicomdir, SLOT( show() ) );

    //connectem Slots dels StudyTreeWidget amb la interficie
    connect( m_studyTreeWidgetPacs, SIGNAL( studyExpanded( QString , QString ) ), SLOT( expandStudy( QString , QString ) ) );
    connect( m_studyTreeWidgetPacs, SIGNAL( seriesExpanded( QString , QString , QString ) ), SLOT( expandSeries( QString , QString , QString ) ) );
    connect( m_studyTreeWidgetPacs, SIGNAL( studyDoubleClicked() ), SLOT( retrieve() ) );
    connect( m_studyTreeWidgetPacs, SIGNAL( seriesDoubleClicked() ), SLOT( retrieve() ) );
    connect( m_studyTreeWidgetPacs, SIGNAL( imageDoubleClicked() ), SLOT( retrieve() ) );

    connect( m_studyTreeWidgetCache, SIGNAL( studyExpanded( QString , QString ) ), SLOT( expandStudy( QString , QString ) ) );
    connect( m_studyTreeWidgetCache, SIGNAL( seriesExpanded( QString , QString , QString ) ), SLOT( expandSeries( QString , QString , QString ) ) );
    connect( m_studyTreeWidgetCache, SIGNAL( studyDoubleClicked() ), SLOT( view() ) );
    connect( m_studyTreeWidgetCache, SIGNAL( seriesDoubleClicked() ), SLOT( view() ) );
    connect( m_studyTreeWidgetCache, SIGNAL( imageDoubleClicked() ), SLOT( view() ) );

    connect( m_studyTreeWidgetDicomdir, SIGNAL( studyExpanded( QString , QString ) ), SLOT( expandStudy( QString , QString ) ) );
    connect( m_studyTreeWidgetDicomdir, SIGNAL( seriesExpanded( QString , QString , QString ) ), SLOT( expandSeries( QString , QString , QString ) ) );
    connect( m_studyTreeWidgetDicomdir, SIGNAL( studyDoubleClicked() ), SLOT( view() ) );
    connect( m_studyTreeWidgetDicomdir, SIGNAL( seriesDoubleClicked() ), SLOT( view() ) );
    connect( m_studyTreeWidgetDicomdir, SIGNAL( imageDoubleClicked() ), SLOT( view() ) );

    //es canvia de pestanya del TAB
    connect( m_tab , SIGNAL( currentChanged( int ) ), SLOT( refreshTab( int ) ) );

    //connectem els signes del SeriesIconView StudyListView
    connect( m_seriesListWidgetCache, SIGNAL( selectedSeriesIcon(QString) ), m_studyTreeWidgetCache, SLOT( setCurrentSeries(QString) ) );
    connect( m_seriesListWidgetCache, SIGNAL( viewSeriesIcon() ), SLOT( viewFromQSeriesListWidget() ) );
    connect( m_studyTreeWidgetCache, SIGNAL( currentStudyChanged() ), SLOT( setSeriesToSeriesListWidgetCache() ) );
    connect( m_studyTreeWidgetCache, SIGNAL( currentSeriesChanged(QString) ), m_seriesListWidgetCache, SLOT( setCurrentSeries(QString) ) );

    //connecta el signal que emiteix qexecuteoperationthread, per visualitzar un estudi amb aquesta classe
    connect( &m_qexecuteOperationThread, SIGNAL( viewStudy( QString , QString , QString ) ), SLOT( studyRetrievedView( QString , QString , QString ) ) , Qt::QueuedConnection );

    //connecta els signals el qexecute operation thread amb els de qretrievescreen, per coneixer quant s'ha descarregat una imatge, serie, estudi, si hi ha error, etc..
    connect( &m_qexecuteOperationThread, SIGNAL( setErrorOperation( QString ) ), m_operationStateScreen, SLOT(  setErrorOperation( QString ) ) );
    connect( &m_qexecuteOperationThread, SIGNAL( setOperationFinished( QString ) ), m_operationStateScreen, SLOT(  setOperationFinished( QString ) ) );

    connect( &m_qexecuteOperationThread, SIGNAL( setOperating( QString ) ), m_operationStateScreen, SLOT(  setOperating( QString ) ) );
    connect( &m_qexecuteOperationThread, SIGNAL( imageCommit( QString , int) ), m_operationStateScreen, SLOT(  imageCommit( QString , int ) ) );
    connect( &m_qexecuteOperationThread, SIGNAL( currentProcessingStudyImagesRetrievedChanged(int)), m_operationStateScreen, SLOT( setRetrievedImagesToCurrentProcessingStudy(int) ));
    connect( &m_qexecuteOperationThread, SIGNAL( seriesCommit( QString ) ), m_operationStateScreen, SLOT(  seriesCommit( QString ) ) );
    connect( &m_qexecuteOperationThread, SIGNAL( newOperation( Operation * ) ), m_operationStateScreen, SLOT(  insertNewOperation( Operation *) ) );

    // Label d'informació (cutre-xapussa)
    connect( &m_qexecuteOperationThread, SIGNAL( setErrorOperation(QString) ), SLOT( updateOperationsInProgressMessage() ));
    connect( &m_qexecuteOperationThread, SIGNAL( setOperationFinished(QString) ), SLOT( updateOperationsInProgressMessage() ));
    connect( &m_qexecuteOperationThread, SIGNAL( newOperation(Operation *) ), SLOT( updateOperationsInProgressMessage() ));

    //connect tracta els errors de connexió al PACS
    connect ( &multipleQueryStudy, SIGNAL( errorConnectingPacs( int ) ), SLOT( errorConnectingPacs( int ) ) );
    connect ( &multipleQueryStudy, SIGNAL( errorQueringStudiesPacs( int ) ), SLOT( errorQueringStudiesPacs( int ) ) );

    //connect tracta els errors de connexió al PACS, al descarregar imatges
    connect ( &m_qexecuteOperationThread, SIGNAL( errorConnectingPacs( int ) ), SLOT( errorConnectingPacs( int ) ) );
    connect( &m_qexecuteOperationThread, SIGNAL( setRetrieveFinished( QString ) ), SLOT( studyRetrieveFinished ( QString ) ) );

    //Amaga o ensenya la cerca avançada
    connect( m_advancedSearchToolButton, SIGNAL( toggled( bool ) ), SLOT( setAdvancedSearchVisible( bool ) ) );
}

void QueryScreen::setAdvancedSearchVisible(bool visible)
{
    m_qadvancedSearchWidget->setVisible(visible);

    if (visible)
    {
        m_advancedSearchToolButton->setText( m_advancedSearchToolButton->text().replace(">>","<<") );
    }
    else
    {
        m_qadvancedSearchWidget->clear();
        m_advancedSearchToolButton->setText( m_advancedSearchToolButton->text().replace("<<",">>") );
    }
}

void QueryScreen::readSettings()
{
    StarviewerSettings settings;
    move( settings.getQueryScreenWindowPositionX() , settings.getQueryScreenWindowPositionY() );
    resize( settings.getQueryScreenWindowWidth() , settings.getQueryScreenWindowHeight() );
    if ( !settings.getQueryScreenStudyTreeSeriesListQSplitterState().isEmpty() )
    {
        m_StudyTreeSeriesListQSplitter->restoreState( settings.getQueryScreenStudyTreeSeriesListQSplitterState() );
    }
    //carreguem el processImageSingleton
    m_processImageSingleton->setPath( settings.getCacheImagePath() );
}

void QueryScreen::clearTexts()
{
    m_qbasicSearchWidget->clear();
    m_qadvancedSearchWidget->clear();
}

void QueryScreen::updateConfiguration(const QString &configuration)
{
    if (configuration == "Pacs/ListChanged")
    {
        m_PACSNodes->refresh();
    }
    else if (configuration == "Pacs/CacheCleared")
    {
        m_studyTreeWidgetCache->clear();
        m_seriesListWidgetCache->clear();
    }
}

void QueryScreen::bringToFront()
{
    this->show();
    this->raise();
    this->activateWindow();
}

void QueryScreen::searchStudy()
{
    switch ( m_tab->currentIndex() )
    {
        case LocalDataBaseTab:
            queryStudy("Cache");
            break;

        case PACSQueryTab:
            queryStudyPacs();
        break;

        case DICOMDIRTab:
            queryStudy("DICOMDIR");
            break;
    }
}

Status QueryScreen::preparePacsServerConnection(QString AETitlePACS, PacsServer *pacsConnection )
{
    PacsParameters pacs;
    PacsListDB pacsListDB;
    Status state;
    StarviewerSettings settings;

    state = pacsListDB.queryPacs( &pacs, AETitlePACS );//cerquem els paràmetres del Pacs al qual s'han de cercar les dades
    if ( !state.good() )
    {
        return state;
    }

    pacs.setAELocal( settings.getAETitleMachine() ); //especifiquem el nostres AE
    pacs.setTimeOut( settings.getTimeout().toInt( NULL , 10 ) ); //li especifiquem el TimeOut

    pacsConnection->setPacs( pacs );

    return state;
}

void QueryScreen::queryStudyPacs()
{
    QList<PacsParameters> selectedPacsList;
    PacsParameters pa;
    QString result;
    StarviewerSettings settings;

    INFO_LOG( "Cerca d'estudis als PACS amb paràmetres " + buildQueryParametersString(buildDicomMask()) );

    m_PACSNodes->getSelectedPacs(selectedPacsList); //Emplemen el pacsList amb les pacs seleccionats al QPacsList

    if (selectedPacsList.isEmpty()) //es comprova que hi hagi pacs seleccionats
    {
        QMessageBox::warning( this , tr( "Starviewer" ) , tr( "Please select a PACS to query" ) );
        return;
    }

    multipleQueryStudy.setPacsList( selectedPacsList ); //indiquem a quins Pacs Cercar

    DicomMask searchMask = buildDicomMask();
    bool stopQuery = false;
    if ( searchMask.isAHeavyQuery() )
    {
        //0 -> Yes; 1->No
        switch( QMessageBox::information( 0 , tr( "Warning" ) , tr( "This query can take a long time.\nDo you want continue?" ), tr( "&Yes" ) , tr( "&No" ) , 0 , 1 ) )
        {
            case 1:
                stopQuery = true;
            break;

            default:
            break;
        }
    }
    if( !stopQuery )
    {
        QApplication::setOverrideCursor( QCursor( Qt::WaitCursor ) );
        multipleQueryStudy.setMask( searchMask ); //assignem la mascara

        Status queryStatus = multipleQueryStudy.StartQueries();

        if( !queryStatus.good() )  //no fem la query
        {
            m_studyTreeWidgetPacs->clear();
            QApplication::restoreOverrideCursor();
            QMessageBox::information( this , tr( "Starviewer" ) , tr( "ERROR QUERING!." ) );
            return;
        }

        if ( multipleQueryStudy.getStudyList().isEmpty() )
        {
            m_studyTreeWidgetPacs->clear();
            QApplication::restoreOverrideCursor();
            QMessageBox::information( this , tr( "Starviewer" ) , tr( "No study match found." ) );
            return;
        }

        m_studyListQueriedPacs = multipleQueryStudy.getStudyList(); //Guardem una còpia en local de la llista d'estudis trobats al PACS

        m_studyTreeWidgetPacs->insertStudyList( multipleQueryStudy.getStudyList() ); //fem que es visualitzi l'studyView seleccionat
        m_studyTreeWidgetPacs->insertSeriesList( multipleQueryStudy.getSeriesList() );
        m_studyTreeWidgetPacs->insertImageList( multipleQueryStudy.getImageList() );
        m_studyTreeWidgetPacs->setSortColumn( QStudyTreeWidget::ObjectName );

        QApplication::restoreOverrideCursor();
    }
}

void QueryScreen::queryStudy( QString source )
{
    CacheStudyDAL cacheStudyDAL;
    LocalDatabaseManager localDatabaseManager;
    QList<DICOMStudy> studyListResultQuery;
    QList<Patient*> patientList;
    Status state;

    INFO_LOG( "Cerca d'estudis a la font" + source + " amb paràmetres " + buildQueryParametersString(buildDicomMask()) );
    QApplication::setOverrideCursor( QCursor( Qt::WaitCursor ) );

    if( source == "Cache" )
    {
        m_seriesListWidgetCache->clear();

        patientList = localDatabaseManager.queryPatientStudy(buildDicomMask());

        if (showDatabaseManagerError( localDatabaseManager.getLastError() ))    return;
    }
    else if( source == "DICOMDIR" )
    {
        state = m_readDicomdir.readStudies( studyListResultQuery , buildDicomMask() );
        if ( !state.good() )
        {
            QApplication::restoreOverrideCursor();
            if ( state.code() == 1302 ) //Aquest és l'error quan no tenim un dicomdir obert l'ig
            {
                QMessageBox::warning( this , tr( "Starviewer" ) , tr( "Error, not opened Dicomdir" ) );
                ERROR_LOG( "No s'ha obert cap directori dicomdir " + state.text() );
            }
            else
            {
                QMessageBox::warning( this , tr( "Starviewer" ) , tr( "Error quering in dicomdir" ) );
                ERROR_LOG( "Error cercant estudis al dicomdir " + state.text() );
            }
            return;
        }
    }
    else
    {
        QApplication::restoreOverrideCursor();
        DEBUG_LOG( "Unrecognised source: " + source );
        return;
    }

    /* Aquest mètode a part de ser cridada quan l'usuari fa click al botó search, també es cridada al
     * constructor d'aquesta classe, per a que al engegar l'aplicació ja es mostri la llista d'estudis
     * que hi ha a la base de dades local. Si el mètode no troba cap estudi a la base de dades local
     * es llença el missatge que no s'han trobat estudis, però com que no és idonii, en el cas aquest que es
     * crida des del constructor que es mostri el missatge de que no s'han trobat estudis al engegar l'aplicació, el que
     * es fa és que per llançar el missatge es comprovi que la finestra estigui activa. Si la finestra no està activa
     * vol dir que el mètode ha estat invocat des del constructor
     */
    if (patientList.isEmpty() && studyListResultQuery.isEmpty() && isActiveWindow() )
    {
        //no hi ha estudis
        if( source == "Cache" )
            m_studyTreeWidgetCache->clear();
        else if( source == "DICOMDIR" )
            m_studyTreeWidgetDicomdir->clear();

        QApplication::restoreOverrideCursor();
        QMessageBox::information( this , tr( "Starviewer" ) , tr( "No study match found." ) );
    }
    else
    {
        if( source == "Cache" )
        {
            m_studyTreeWidgetCache->insertPatientList(patientList);//es mostra la llista d'estudis
            m_studyTreeWidgetCache->setSortColumn( QStudyTreeWidget::ObjectName ); //ordenem pel nom
        }
        else if( source == "DICOMDIR" )
        {
            m_studyTreeWidgetDicomdir->clear();
            m_studyTreeWidgetDicomdir->insertStudyList( studyListResultQuery );
            m_studyTreeWidgetDicomdir->setSortColumn( QStudyTreeWidget::ObjectName );//ordenem pel nom
        }
        QApplication::restoreOverrideCursor();
    }
}

void QueryScreen::expandStudy( QString studyUID , QString pacsAETitle )
{
    QApplication::setOverrideCursor( QCursor( Qt::WaitCursor ) );

    switch ( m_tab->currentIndex() )
    {
        case 0 : // si estem a la pestanya de la cache
            querySeries( studyUID, "Cache" );
            break;
        case 1 :  //si estem la pestanya del PACS fem query al Pacs
            querySeriesPacs(studyUID, pacsAETitle);
            break;
        case 2 : //si estem a la pestanya del dicomdir, fem query al dicomdir
            querySeries( studyUID, "DICOMDIR" );
            break;
    }

    QApplication::restoreOverrideCursor();
}

/* AQUESTA ACCIO ES CRIDADA DES DEL STUDYLISTVIEW*/
void QueryScreen::expandSeries( QString studyUID , QString seriesUID , QString pacsAETitle )
{
    QApplication::setOverrideCursor( QCursor( Qt::WaitCursor ) );

    switch ( m_tab->currentIndex() )
    {
        case 0 : // si estem a la pestanya de la cache
            queryImage( studyUID , seriesUID, "Cache" );
            break;

        case 1 :  //si estem la pestanya del PACS fem query al Pacs
            queryImagePacs( studyUID , seriesUID , pacsAETitle );
            break;

        case 2 : //si estem a la pestanya del dicomdir, fem query al dicomdir
            queryImage( studyUID , seriesUID, "DICOMDIR" );
            break;
    }

    QApplication::restoreOverrideCursor();
}

void QueryScreen::querySeriesPacs(QString studyUID , QString pacsAETitle)
{
    DICOMSeries serie;
    Status state;
    QString text;
    PacsServer pacsConnection;
    QueryPacs querySeriesPacs;

    INFO_LOG( "Cercant informacio de les sèries de l'estudi" + studyUID + " del PACS " + pacsAETitle );

    if ( ! preparePacsServerConnection( pacsAETitle, &pacsConnection ).good() )
        return;

    state = pacsConnection.connect(PacsServer::query,PacsServer::seriesLevel);
    if ( !state.good() )
    {
        //Error al connectar
        ERROR_LOG( "Error al connectar al pacs " + pacsAETitle + ". PACS ERROR : " + state.text() );
        errorConnectingPacs ( pacsConnection.getPacs().getPacsID() );
        return;
    }

    querySeriesPacs.setConnection( pacsConnection.getConnection() );
    state = querySeriesPacs.query( buildSeriesDicomMask( studyUID ) );
    pacsConnection.disconnect();

    if ( !state.good() )
    {
        //Error a la query
        ERROR_LOG( "QueryScreen::QueryPacs : Error cercant les sèries al PACS " + pacsAETitle + ". PACS ERROR : " + state.text() );

        text = tr( "Error! Can't query series to PACS named %1" ).arg( pacsAETitle );
        QMessageBox::warning( this , tr( "Starviewer" ) , text );
        return;
    }

    if ( querySeriesPacs.getQueryResultsAsSeriesList().isEmpty() )
    {
        QMessageBox::information( this , tr( "Starviewer" ) , tr( "No series match for this study.\n" ) );
        return;
    }

    m_studyTreeWidgetPacs->insertSeriesList( querySeriesPacs.getQueryResultsAsSeriesList() );
}

void QueryScreen::querySeries( QString studyUID, QString source )
{
    QList<DICOMSeries> seriesListQueryResults;
    QList<Series*> seriesList;
    LocalDatabaseManager localDatabaseManager;
    CacheSeriesDAL cacheSeriesDAL;
    Status state;
    DicomMask mask;

    INFO_LOG( "Cerca de sèries a la font " + source +" de l'estudi " + studyUID );
    if( source == "Cache" )
    {
        //preparem la mascara i cerquem les series a la cache
        mask.setStudyUID(studyUID);
        seriesList = localDatabaseManager.querySeries(mask);
        if (showDatabaseManagerError( localDatabaseManager.getLastError() ))    return;
    }
    else if( source == "DICOMDIR" )
    {
        m_readDicomdir.readSeries( studyUID , "" , seriesListQueryResults ); //"" pq no busquem cap serie en concret
    }
    else
    {
        DEBUG_LOG( "Unrecognised source: " + source );
        return;
    }

    if ( seriesListQueryResults.isEmpty() && seriesList.isEmpty() )
    {
        QMessageBox::information( this , tr( "Starviewer" ) , tr( "No series match for this study.\n" ) );
        return;
    }

    if( source == "Cache" )
    {
        m_studyTreeWidgetCache->insertSeriesList(studyUID, seriesList); //inserim la informació de les sèries al estudi
    }
    else if( source == "DICOMDIR" )
        m_studyTreeWidgetDicomdir->insertSeriesList( seriesListQueryResults );//inserim la informació de la sèrie al llistat
}

void QueryScreen::queryImagePacs( QString studyUID , QString seriesUID , QString AETitlePACS )
{
    DICOMSeries serie;
    Status state;
    QString text;
    PacsServer pacsConnection;
    QueryPacs queryImages;
    DicomMask dicomMask;

    QApplication::setOverrideCursor( QCursor( Qt::WaitCursor ) );

    INFO_LOG( "Cercant informacio de les imatges de l'estudi" + studyUID + " serie " + seriesUID + " del PACS " + AETitlePACS );

    dicomMask.setStudyUID( studyUID );
    dicomMask.setSeriesUID( seriesUID );
    dicomMask.setImageNumber( "" );
    dicomMask.setSOPInstanceUID( "" );

    if ( ! preparePacsServerConnection( AETitlePACS, &pacsConnection ).good() )
    {
        QApplication::restoreOverrideCursor();
        return;
    }

    state = pacsConnection.connect(PacsServer::query,PacsServer::imageLevel);
    if ( !state.good() )
    {   //Error al connectar
        QApplication::restoreOverrideCursor();
        ERROR_LOG( "Error al connectar al pacs " + AETitlePACS + ". PACS ERROR : " + state.text() );
        errorConnectingPacs ( pacsConnection.getPacs().getPacsID() );
        return;
    }

    queryImages.setConnection( pacsConnection.getConnection() );

    state = queryImages.query( dicomMask );
    if ( !state.good() )
    {
        //Error a la query
        QApplication::restoreOverrideCursor();
        ERROR_LOG( "QueryScreen::QueryPacs : Error cercant les images al PACS " + AETitlePACS + ". PACS ERROR : " + state.text() );

        text = tr( "Error! Can't query images to PACS named %1 " ).arg(AETitlePACS);
        QMessageBox::warning( this , tr( "Starviewer" ) , text );
        return;
    }

    pacsConnection.disconnect();

    if ( queryImages.getQueryResultsAsImageList().isEmpty() )
    {
        QApplication::restoreOverrideCursor();
        QMessageBox::information( this , tr( "Starviewer" ) , tr( "No images match for this series.\n" ) );
        return;
    }

    m_studyTreeWidgetPacs->insertImageList( queryImages.getQueryResultsAsImageList() );

    QApplication::restoreOverrideCursor();
}

void QueryScreen::retrieve()
{
    retrievePacs( false );
}

void QueryScreen::queryImage(QString studyInstanceUID, QString seriesInstanceUID, QString source)
{
    LocalDatabaseManager localDatabaseManager;
    CacheImageDAL cacheImageDAL;
    DicomMask mask;
    QList<Image*> imageList;
    QList<DICOMImage> imageListQueryResults;

    INFO_LOG("Cerca d'imatges a la font " + source + " de l'estudi " + studyInstanceUID + " i serie " + seriesInstanceUID);

    if(source == "Cache")
    {
        mask.setStudyUID(studyInstanceUID);
        mask.setSeriesUID(seriesInstanceUID);
        imageList = localDatabaseManager.queryImage(mask);
        if(showDatabaseManagerError(localDatabaseManager.getLastError()))   return;
    }
    else if (source == "DICOMDIR")
    {
        m_readDicomdir.readImages(seriesInstanceUID, "", imageListQueryResults);
    }
    else
    {
        DEBUG_LOG("Unrecognised source: " + source);
        return;
    }

    if (imageListQueryResults.isEmpty() && imageList.isEmpty())
    {
        QMessageBox::information( this , tr( "Starviewer" ) , tr( "No images match for this study.\n" ) );
        return;
    }

    if( source == "Cache" )
        m_studyTreeWidgetCache->insertImageList(studyInstanceUID, seriesInstanceUID, imageList);
    else if( source == "DICOMDIR" )
        m_studyTreeWidgetDicomdir->insertImageList( imageListQueryResults );//inserim la informació de la sèrie al llistat
}

void QueryScreen::retrievePacs( bool view )
{
    QApplication::setOverrideCursor( QCursor ( Qt::WaitCursor ) );

    QStringList selectedStudiesUIDList = m_studyTreeWidgetPacs->getSelectedStudiesUID();
    if( selectedStudiesUIDList.isEmpty() )
    {
        QApplication::restoreOverrideCursor();
        if( view )
            QMessageBox::warning( this , tr( "Starviewer" ) , tr( "Select a study to view " ) );
        else
            QMessageBox::warning( this , tr( "Starviewer" ) , tr( "Select a study to download " ) );

        return;
    }

    StarviewerSettings settings;
    bool ok;

    foreach( QString currentStudyUID, selectedStudiesUIDList )
    {
        DicomMask mask;
        QString defaultSeriesUID;
        Operation operation;
        PacsParameters pacs;
        QString pacsAETitle;

        //Busquem en quina posició de la llista on guardem els estudis trobats al PACS en quina posició està per poder-lo recuperar 
        //TODO no hauria de tenir la responsabilitat de retornar l'estudi al QStudyTreeView no la pròpia QueryScreen
        int indexStudyInList = getStudyPositionInStudyListQueriedPacs( currentStudyUID , 
        m_studyTreeWidgetPacs->getStudyPACSAETitleFromSelectedItems( currentStudyUID ) );

        //Tenim l'informació de l'estudi a descarregar a la llista d'estudis cercats del pacs, el busquem a la llista a través d'aquest mètode
        if ( indexStudyInList == -1 ) 
        {   //Es comprova que existeixi l'estudi a la llista d'estudis de la última query que s'ha fet al PACS
            //TODO Arreglar missatge d'error
            QApplication::restoreOverrideCursor();
            QMessageBox::warning( this , tr( "Starviewer" ) , tr( "Internal Error : " ) );
        }
        else
        {
            DICOMStudy studyToRetrieve =  m_studyListQueriedPacs.value( indexStudyInList );

            pacsAETitle = m_studyTreeWidgetPacs->getStudyPACSAETitleFromSelectedItems(currentStudyUID);

            mask.setStudyUID( currentStudyUID );//definim la màscara per descarregar l'estudi

            // TODO aquí només tenim en compte l'última sèrie o imatge seleccionada
            // per tant si seleccionem més d'una sèrie/imatge només s'en baixarà una
            // Caldria fer possible que es baixi tants com en seleccionem
            if ( !m_studyTreeWidgetPacs->getCurrentSeriesUID().isEmpty() )
                mask.setSeriesUID( m_studyTreeWidgetPacs->getCurrentSeriesUID() );

            if ( !m_studyTreeWidgetPacs->getCurrentImageUID().isEmpty() )
                mask.setSOPInstanceUID( m_studyTreeWidgetPacs->getCurrentImageUID() );

            //busquem els paràmetres del pacs del qual volem descarregar l'estudi
            PacsListDB pacsListDB;
            pacsListDB.queryPacs(&pacs, pacsAETitle);

            //emplanem els parametres amb dades del starviewersettings
            pacs.setAELocal( settings.getAETitleMachine() );
            pacs.setTimeOut( settings.getTimeout().toInt( NULL, 10 ) );
            pacs.setLocalPort( settings.getLocalPort() );

            //definim l'operacio
            operation.setPacsParameters( pacs );
            operation.setDicomMask( mask );
            if ( view )
            {
                operation.setOperation( Operation::View );
                operation.setPriority( Operation::Medium );//Té priorita mitjà per passar al davant de les operacions de Retrieve
            }
            else
            {
                operation.setOperation( Operation::Retrieve );
                operation.setPriority( Operation::Low );
            }
            //emplenem les dades de l'operació
            operation.setPatientName( studyToRetrieve.getPatientName() );
            operation.setPatientID( studyToRetrieve.getPatientId() );
            operation.setStudyID( studyToRetrieve.getStudyId() );
            operation.setStudyUID( studyToRetrieve.getStudyUID() );

            m_qexecuteOperationThread.queueOperation( operation );
        }
    }
    QApplication::restoreOverrideCursor();
}

Status QueryScreen::insertStudyCache( DICOMStudy stu )
{
    QString absPath;
    Status state;
    DICOMStudy study = stu;
    StarviewerSettings settings;
    CacheStudyDAL cacheStudyDAL;

    //creem el path absolut de l'estudi
    absPath = settings.getCacheImagePath() + study.getStudyUID() + "/";
    //inserim l'estudi a la caché
    study.setAbsPath(absPath);
    state = cacheStudyDAL.insertStudy( &study );

    return state;
}

void QueryScreen::studyRetrievedView( QString studyUID , QString seriesUID , QString sopInstanceUID )
{
    QStringList studyUIDList;
    studyUIDList << studyUID;

    loadStudies(studyUIDList, seriesUID, sopInstanceUID, "Cache");
}

void QueryScreen::refreshTab( int index )
{
    switch ( index )
    {
        case LocalDataBaseTab:
                m_qbasicSearchWidget->setEnabledSeriesModality(false);
                m_qadvancedSearchWidget->setEnabled(false);
                break;

        case PACSQueryTab:
                m_qbasicSearchWidget->setEnabledSeriesModality(true);
                m_qadvancedSearchWidget->setEnabled( true );
                break;

        case DICOMDIRTab:
                m_qbasicSearchWidget->setEnabledSeriesModality(false);
                m_qadvancedSearchWidget->setEnabled(false);
                break;
    }
}

void QueryScreen::view()
{
    switch ( m_tab->currentIndex() )
    {
        case 0 :
            loadStudies( m_studyTreeWidgetCache->getSelectedStudiesUID(), m_studyTreeWidgetCache->getCurrentSeriesUID(), m_studyTreeWidgetCache->getCurrentImageUID(), "Cache" );
            break;

        case 1 :
            retrievePacs( true );
           break;

        case 2 :
            loadStudies( m_studyTreeWidgetDicomdir->getSelectedStudiesUID(), m_studyTreeWidgetDicomdir->getCurrentSeriesUID(), m_studyTreeWidgetDicomdir->getCurrentImageUID(), "DICOMDIR" );
            break;

        default :
            break;
    }
}

void QueryScreen::viewFromQSeriesListWidget()
{
    QStringList studyUIDList;

    studyUIDList << m_seriesListWidgetCache->getCurrentStudyUID();//Agafem l'estudi uid de la sèrie seleccionada
    loadStudies( studyUIDList, m_seriesListWidgetCache->getCurrentSeriesUID(), "", "Cache" );
}

void QueryScreen::loadStudies( QStringList studiesUIDList, QString defaultSeriesUID , QString defaultSOPInstanceUID, QString source )
{
    if( studiesUIDList.isEmpty() )
    {
        QMessageBox::warning( this , tr( "Starviewer" ) , tr( "Select at least one study to view" ) );
        return;
    }

    this->close();//s'amaga per poder visualitzar la serie
    if ( m_operationStateScreen->isVisible() )
    {
        m_operationStateScreen->close();//s'amaga per poder visualitzar la serie
    }

    DicomMask patientToProcessMask;
    Patient *patient;

    QTime time;
    //TODO: Què passa si seleccionem més d'un pacient???? hem de tractar el cas a l'extensionhandler i/o passar una llista de pacients.
    patientToProcessMask.setStudyUID(studiesUIDList.first());
    if( source == "Cache" )
    {
        LocalDatabaseManager localDatabaseManager;

        time.start();
        patient = localDatabaseManager.retrieve(patientToProcessMask);

        DEBUG_LOG( QString("Rehidratar de la bd ha trigat: %1 ").arg( time.elapsed() ));

        if(showDatabaseManagerError(localDatabaseManager.getLastError()))
        {
            return;
        }
    }
    else if(source == "DICOMDIR")
    {
        time.start();
        patient = m_readDicomdir.retrieve(patientToProcessMask);
        DEBUG_LOG( QString("Llegir del DICOMDIR directament (sense importar) ha trigat: %1 ").arg( time.elapsed() ));
    }

    DEBUG_LOG("Fem emit de selectedPatient");
    emit selectedPatient(patient, defaultSeriesUID);

    DEBUG_LOG("Acabem loadStudies");
}

void QueryScreen::importDicomdir()
{
    DICOMDIRImporter importDicom;
    Status state;
    int failedStudies = 0;

    QApplication::setOverrideCursor( QCursor( Qt::WaitCursor ) );

//     importDicom.import( m_readDicomdir.getDicomdirPath() , m_studyTreeWidgetDicomdir->getSelectedStudyUID() , m_studyTreeWidgetDicomdir->getSelectedSeriesUID() ,  m_studyTreeWidgetDicomdir->getSelectedImageUID() );
    // TODO ara només permetrem importar estudis sencers
    foreach( QString studyUID, m_studyTreeWidgetDicomdir->getSelectedStudiesUID() )
    {
        if ( !importDicom.import(m_readDicomdir.getDicomdirFilePath(), studyUID, QString(), QString() ) )
        {
            failedStudies++;
        }
    }

    queryStudy("Cache"); //Actualitzem la llista tenint en compte el criteri de cerca

    QApplication::restoreOverrideCursor();

    if ( failedStudies > 0 ) //si ha fallat algun estudi
    {
        if ( failedStudies == m_studyTreeWidgetDicomdir->getSelectedStudiesUID().count() ) //si han fallat tots els estudis
            QMessageBox::critical( this , tr( "Starviewer" ) , tr( "Error: Can't import selected studies" ) );
        else
            QMessageBox::warning( this , tr( "Starviewer" ) , tr( "Error: Some studies can't be imported" ) );
    }
}

void QueryScreen::deleteSelectedStudiesInCache()
{
    QStringList studiesList = m_studyTreeWidgetCache->getSelectedStudiesUID();
    if( studiesList.isEmpty() )
    {
        QMessageBox::information( this , tr( "Starviewer" ) , tr( "Please select at least one study to delete" ) );
    }
    else
    {
        QMessageBox::StandardButton response = QMessageBox::question(this, tr( "Starviewer" ),
                                                                           tr( "Are you sure you want to delete the selected Studies?" ),
                                                                           QMessageBox::Yes | QMessageBox::No,
                                                                           QMessageBox::No);
        if (response  == QMessageBox::Yes)
        {
            //Posem el cursor en espera
            QApplication::setOverrideCursor(Qt::WaitCursor);

            Status state;
            CacheStudyDAL cacheStudyDAL;
            LocalDatabaseManager localDatabaseManager;

            foreach(QString studyUID, studiesList)
            {
                if( m_qcreateDicomdir->studyExists( studyUID ) )
                {
                    QMessageBox::warning( this , tr( "Starviewer" ) ,
                    tr( "The study with UID: %1 is in use by the DICOMDIR List. If you want to delete this study you should remove it from the DICOMDIR List first." ).arg(studyUID) );
                }
                else
                {
                    INFO_LOG( "S'esborra de la cache l'estudi " + studyUID );

                    localDatabaseManager.del(studyUID);
                    if (showDatabaseManagerError( localDatabaseManager.getLastError() ))    break;

                    m_studyTreeWidgetCache->removeStudy( studyUID );
                    m_seriesListWidgetCache->clear();
                }
            }

            QApplication::restoreOverrideCursor();
        }
    }
}

void QueryScreen::studyRetrieveFinished( QString studyUID )
{
    DICOMStudy study;
    CacheStudyDAL cacheStudyDAL;
    Status state;
    LocalDatabaseManager localDatabaseManager;
    DicomMask studyMask;
    QList<Patient*> patientList;

    //El signal que desperta aquests slot es fa abans que acabi l'insersió a la base de dades, per això posem aquest sleep per assegurar que s'hagi inserit l'estudi a la bd
    sleep(4);

    studyMask.setStudyUID(studyUID);
    patientList = localDatabaseManager.queryPatientStudy(studyMask);
    if( showDatabaseManagerError( localDatabaseManager.getLastError() ))    return;

    if (patientList.count() == 1)
    {
        m_studyTreeWidgetCache->insertPatient(patientList.at(0));
        m_studyTreeWidgetCache->sort();
    }
}

void QueryScreen::closeEvent( QCloseEvent* event )
{
    writeSettings(); // guardem els settings

    m_operationStateScreen->close(); //Tanquem la QOperationStateScreen al tancar la QueryScreen

    event->accept();
    m_qcreateDicomdir->clearTemporaryDir();
}

void QueryScreen::writeSettings()
{
    /* Només guardem els settings quan la interfície ha estat visible, ja que hi ha settings com el QSplitter que si la primera vegada
     * que executem l'starviewer no obrim la QueryScreen retorna un valors incorrecte per això, el que fem és comprova que la QueryScreen
     * hagi estat visible per guardar el settings
     */
    if (this->isVisible())
    {
        StarviewerSettings settings;

        saveQStudyTreeWidgetColumnsWidth();

        //guardem l'estat del QSplitter que divideix el StudyTree del QSeries
        settings.setQueryScreenStudyTreeSeriesListQSplitterState( m_StudyTreeSeriesListQSplitter->saveState() );

        //guardem la posició en que es troba la pantalla
        settings.setQueryScreenWindowPositionX( x() );
        settings.setQueryScreenWindowPositionY( y() );

        //guardem les dimensions de la pantalla
        settings.setQueryScreenWindowHeight( height() );
        settings.setQueryScreenWindowWidth( width() );
    }
}

void QueryScreen::saveQStudyTreeWidgetColumnsWidth()
{
    StarviewerSettings settings;

    for ( int column = 0; column < m_studyTreeWidgetPacs->getNumberOfColumns(); column++ )
    {
        settings.setStudyPacsListColumnWidth( column , m_studyTreeWidgetPacs->getColumnWidth( column ) );
    }

    for ( int column = 0; column < m_studyTreeWidgetCache->getNumberOfColumns(); column++ )
    {
        settings.setStudyCacheListColumnWidth( column , m_studyTreeWidgetCache->getColumnWidth( column ) );
    }

    for ( int column = 0; column < m_studyTreeWidgetDicomdir->getNumberOfColumns(); column++ )
    {
        settings.setStudyDicomdirListColumnWidth( column , m_studyTreeWidgetDicomdir->getColumnWidth( column ) );
    }
}

void QueryScreen::showOperationStateScreen()
{
    if ( !m_operationStateScreen->isVisible() )
    {
        m_operationStateScreen->setVisible( true );
    }
    else
    {
        m_operationStateScreen->raise();
        m_operationStateScreen->activateWindow();
    }
}

void QueryScreen::convertToDicomdir()
{
    CacheStudyDAL cacheStudyDAL;
    DICOMStudy study;
    QStringList studiesUIDList = m_studyTreeWidgetCache->getSelectedStudiesUID();

    DicomMask studyMask;
    LocalDatabaseManager localDatabaseManager;
    QList<Patient*> patientList;

    foreach(QString studyUID, studiesUIDList )
    {
        studyMask.setStudyUID(studyUID);
        patientList = localDatabaseManager.queryPatientStudy(studyMask);
        if( showDatabaseManagerError( localDatabaseManager.getLastError() ))    return;

        // \TODO Això s'ha de fer perquè queryPatientStudy retorna llista de Patients
        // Nosaltres, en realitat, volem llista d'study amb les dades de Patient omplertes.
        if(patientList.size() != 1 && patientList.first()->getNumberOfStudies() != 1)
        {
            showDatabaseManagerError(LocalDatabaseManager::DatabaseCorrupted);
            return;
        }

        m_qcreateDicomdir->addStudy(patientList.first()->getStudies().first());

        delete patientList.first();
   }
}

void QueryScreen::openDicomdir()
{
    StarviewerSettings settings;
    QFileDialog *dlg = new QFileDialog( 0 , QFileDialog::tr( "Open" ) , settings.getLastOpenedDICOMDIRPath(), "DICOMDIR" );
    QString path, dicomdirPath;

    dlg->setFileMode( QFileDialog::ExistingFile );
    Status state;

    if ( dlg->exec() == QDialog::Accepted )
    {
        if ( !dlg->selectedFiles().empty() )
            dicomdirPath = dlg->selectedFiles().takeFirst();

        state = m_readDicomdir.open ( dicomdirPath );//Obrim el dicomdir

        if ( !state.good() )
        {
            QMessageBox::warning( this , tr( "Starviewer" ) , tr( "Error openning dicomdir" ) );
            ERROR_LOG( "Error al obrir el dicomdir " + dicomdirPath + state.text() );
        }
        else
        {
            INFO_LOG( "Obert el dicomdir " + dicomdirPath );
            settings.setLastOpenedDICOMDIRPath( QFileInfo(dicomdirPath).dir().path() );
            this->bringToFront();
            m_tab->setCurrentIndex( 2 ); // mostre el tab del dicomdir
        }

        clearTexts();//Netegem el filtre de cerca al obrir el dicomdir
        //cerquem els estudis al dicomdir per a que es mostrin
        queryStudy("DICOMDIR");
    }

    delete dlg;
}

void QueryScreen::storeStudiesToPacs()
{
    QList<PacsParameters> selectedPacsList;
    QStringList studiesUIDList = m_studyTreeWidgetCache->getSelectedStudiesUID();
    QApplication::setOverrideCursor( QCursor( Qt::WaitCursor ) );

    m_PACSNodes->getSelectedPacs( selectedPacsList ); //Emplemen el pacsList amb les pacs seleccionats al QPacsList

    switch( selectedPacsList.size() )
    {
        case  0 :
            QApplication::restoreOverrideCursor();
            QMessageBox::warning( this , tr( "Starviewer" ) , tr( "You have to select a PACS to store the study in" ));
            break;

        case 1 :
        {
            StarviewerSettings settings;
            foreach( QString studyUID, studiesUIDList )
            {
                CacheStudyDAL cacheStudy;
                PacsListDB pacsListDB;
                PacsParameters pacs;
                Operation storeStudyOperation;
                DicomMask dicomMask;
                Status state;
                DICOMStudy study;

                QMessageBox::critical(this, "Nova BD", "Eiii, que encara falta implementar el gravar al pacs amb la nova bd!");
                return;

                dicomMask.setStudyUID( studyUID );
                storeStudyOperation.setPatientName( study.getPatientName() );
                storeStudyOperation.setStudyUID( study.getStudyUID() );
                storeStudyOperation.setPriority( Operation::Low );
                storeStudyOperation.setOperation( Operation::Move );
                storeStudyOperation.setDicomMask( dicomMask );
                storeStudyOperation.setPatientID( study.getPatientId() );
                storeStudyOperation.setStudyID( study.getStudyId() );

                //cerquem els paràmetres del Pacs al qual s'han de cercar les dades
                state = pacsListDB.queryPacs( &pacs, selectedPacsList.value(0).getAEPacs() );
                if ( state.good() )
                {
                    storeStudyOperation.setPacsParameters( pacs );

                    m_qexecuteOperationThread.queueOperation( storeStudyOperation );
                }
                else
                {
                    QApplication::restoreOverrideCursor();
                    // TODO potser s'haurien de recollir si hi ha hagut errors i al final fer el sumari si calgues
                }
            }
            break;
        }

        default :
            QApplication::restoreOverrideCursor();
            QMessageBox::warning( this , tr( "Starviewer" ) , tr( "The studies can only be stored to one PACS" ));
            break;
    }

    QApplication::restoreOverrideCursor();
}

void QueryScreen::errorConnectingPacs( int IDPacs )
{
    PacsListDB pacsListDB;
    PacsParameters errorPacs;
    QString errorMessage;

    pacsListDB.queryPacs( &errorPacs, IDPacs );

    errorMessage = tr( "Can't connect to PACS %1 from %2\nBe sure that the IP and AETitle of the PACS are correct" )
        .arg( errorPacs.getAEPacs() )
        .arg( errorPacs.getInstitution()
    );

    QMessageBox::critical( this , tr( "Starviewer" ) , errorMessage );
}

void QueryScreen::errorQueringStudiesPacs( int PacsID )
{
    PacsListDB pacsListDB;
    PacsParameters errorPacs;
    QString errorMessage;

    pacsListDB.queryPacs( &errorPacs, PacsID );
    errorMessage = tr( "Can't query PACS %1 from %2\nBe sure that the IP and AETitle of this PACS are correct" )
        .arg( errorPacs.getAEPacs() )
        .arg( errorPacs.getInstitution()
    );

    QMessageBox::critical( this , tr( "Starviewer" ) , errorMessage );
}

DicomMask QueryScreen::buildSeriesDicomMask( QString studyUID )
{
    DicomMask mask;

    mask.setStudyUID( studyUID );
    mask.setSeriesDate( "" );
    mask.setSeriesTime( "" );
    mask.setSeriesModality( "" );
    mask.setSeriesNumber( "" );
    mask.setSeriesUID( "" );
    mask.setPPSStartDate( "" );
    mask.setPPStartTime( "" );
    mask.setRequestAttributeSequence( "" , "" );

    return mask;
}

DicomMask QueryScreen::buildDicomMask()
{
    return m_qbasicSearchWidget->buildDicomMask() + m_qadvancedSearchWidget->buildDicomMask();
}

int QueryScreen::getStudyPositionInStudyListQueriedPacs( QString studyUID , QString pacsAETitle )
{
    int index = 0;
    bool studyUIDisTheSame = false , pacsAETitleIsTheSame = false;

    while ( index < m_studyListQueriedPacs.count() && ( !studyUIDisTheSame || !pacsAETitleIsTheSame ) )
    {
        studyUIDisTheSame = m_studyListQueriedPacs.value( index ).getStudyUID() == studyUID;
        pacsAETitleIsTheSame = m_studyListQueriedPacs.value( index ).getPacsAETitle() == pacsAETitle;

        if (!studyUIDisTheSame || !pacsAETitleIsTheSame ) index++;
    }

    return index < m_studyListQueriedPacs.count() ? index : -1;
}

QString QueryScreen::buildQueryParametersString(DicomMask mask)
{
    QString logMessage;

    logMessage = "PATIENT_ID=[" + mask.getPatientId() + "] "
        + "PATIENT_NAME=[" + mask.getPatientName() + "] "
        + "STUDY_ID=[" + mask.getStudyId() + "] "
        + "DATES_MASK=[" + mask.getStudyDate() + "] "
        + "ACCESSION_NUMBER=[" + mask.getAccessionNumber() + "]";

    return logMessage;
}

bool QueryScreen::showDatabaseManagerError(LocalDatabaseManager::LastError error )
{
    QString message;

    switch(error)
    {
        case LocalDatabaseManager::Ok:
            return false;

        case LocalDatabaseManager::DatabaseLocked:
            message = tr("The Cache is blocked by another Starviewer window."
                         "\nTry to close all the others Starviewer windows and try again."
                         "\n\nIf you want to open different Starviewer's windows always choose the 'New' option from the File menu.");
            break;
        case LocalDatabaseManager::DatabaseCorrupted:
            message = tr("Starviewer database is corrupted."
                         "\n Try to close all Starviewer windows and try again."
                         "\n\nIf the problem persist contact with an administrator");
            break;
        case LocalDatabaseManager::SyntaxErrorSQL:
            message = tr("Starviewer Database manager error."
                         "\n Try to close all Starviewer windows and try again."
                         "\n\nIf the problem persist contact with an administrator");
            break;
        case LocalDatabaseManager::DatabaseError:
            message = tr("An internal error occurs with Starviewer database."
                         "\n Try to close all Starviewer windows and try again."
                         "\n\nIf the problem persist contact with an administrator");
            break;
        case LocalDatabaseManager::DeletingFilesError:
            message = tr("Some files can not be delete."
                         "\n Theses have to be delete manually.");
            break;
        default:
            message = tr("An unknow error has ocurred");
            break;
    }

    QApplication::restoreOverrideCursor();

    QMessageBox::critical( this , tr( "Starviewer" ) , message );

    return true;
}

};

