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

#include "processimagesingleton.h"
#include "serieslistsingleton.h"
#include "imagelistsingleton.h"
#include "pacsparameters.h"
#include "pacsconnection.h"
#include "multiplequerystudy.h"
#include "studylist.h"
#include "qstudytreewidget.h"
#include "dicomseries.h"
#include "querypacs.h"
#include "pacsparameters.h"
#include "pacsserver.h"
#include "qserieslistwidget.h"
#include "retrieveimages.h"
#include "pacslist.h"
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
#include "importdicomdir.h"
#include "patientfillerinput.h"
#include "qcreatedicomdir.h"

namespace udg {

QueryScreen::QueryScreen( QWidget *parent )
 : QDialog(parent )
{
    setupUi( this );

    Status state;
    QString path;
    StarviewerSettings settings;
    CacheStudyDAL cacheStudyDal;
    ReadDicomdir read;

    m_OperationStateScreen = new udg::QOperationStateScreen;
    m_qcreateDicomdir = new udg::QCreateDicomdir( this );

    initialize();//inicialitzem les variables necessàries

    //connectem signals i slots
    createConnections();

    //esborrem els estudis vells de la cache
    deleteOldStudies();

    //carreguem el processImageSingleton
    m_piSingleton = ProcessImageSingleton::getProcessImageSingleton();
    m_piSingleton->setPath( settings.getCacheImagePath() );

    //Instanciem els llistats
    m_seriesListSingleton = SeriesListSingleton::getSeriesListSingleton();
    m_studyListSingleton = StudyListSingleton::getStudyListSingleton();
    m_imageListSingleton = ImageListSingleton::getImageListSingleton();

    setWindowPosition(); //la pantalla sempre es situa en el lloc on estava l'última vegada que es va tancar
    setWindowSize();//la pantalla sempre té les dimensions de l'última vegada que es va tancar

    setQSplitterState(); //posa els splitters en el lloc on estaven l'última vegada que es va tancar l'aplicació

    m_textPatientID->setFocus();

    QMovie *operationAnimation = new QMovie(this);
    operationAnimation->setFileName(":/images/loader.gif");
    m_operationAnimation->setMovie(operationAnimation);
    operationAnimation->start();

    //fem que per defecte mostri els estudis de la cache
    queryStudy("Cache");
    m_pushButtonAdvancedSearch->hide();
    m_qwidgetAdvancedSearch->hide();
    m_operationAnimation->hide();
    m_labelOperation->hide();
}

QueryScreen::~QueryScreen()
{
    StarviewerSettings settings;

    //guardem la posició en que es troba la pantalla
    settings.setQueryScreenWindowPositionX( x() );
    settings.setQueryScreenWindowPositionY( y() );

    //guardem les dimensions de la pantalla
    settings.setQueryScreenWindowHeight( height() );
    settings.setQueryScreenWindowWidth( width() );

    //guardem l'estat del QSplitter que divideix el StudyTree del QSeries
    settings.setQueryScreenStudyTreeSeriesListQSplitterState( m_StudyTreeSeriesListQSplitter->saveState() );
}

void QueryScreen::initialize()
{
    QDate currentDate;

    //indiquem que la llista de Pacs no es mostra
    m_PacsListShow = false;

    qPacsList->setMaximumSize( 1 , 1 );//amaguem al finestra del QPacsList

    m_dateStudyDateFrom->setDate( currentDate.currentDate() );
    m_dateStudyDateTo->setDate( currentDate.currentDate() );
}

void QueryScreen::deleteOldStudies()
{
    Status state;
    CacheLayer cacheLayer;

    state = cacheLayer.deleteOldStudies();

    if ( !state.good() )
    {
        QMessageBox::warning( this , tr( "Starviewer" ) , tr( "Error deleting old studies" ) );
        databaseError( &state );
    }
}

void QueryScreen::updateOperationsInProgressMessage()
{
    if (m_OperationStateScreen->getActiveOperationsCount() > 0)
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
    connect( m_buttonSearch , SIGNAL( clicked() ) , this , SLOT( searchStudy() ) );
    connect( m_buttonToday , SIGNAL( clicked() ) , this , SLOT( searchTodayStudy() ) );
    connect( m_buttonYesterday , SIGNAL( clicked() ) , this , SLOT( searchYesterdayStudy() ) );
    connect( m_buttonClear , SIGNAL( clicked() ) , this , SLOT( clearTexts() ) );
    connect( m_buttonRetrieve , SIGNAL( clicked() ) , this , SLOT( retrieve() ) );
    connect( m_buttonRetrieveList , SIGNAL( clicked() ) , m_OperationStateScreen , SLOT( show() ) );
    connect( m_buttonShowPacsList , SIGNAL( clicked() ) , this , SLOT( showPacsList() ) );
    connect( m_buttonView , SIGNAL( clicked() ) , this , SLOT( view() ) );
    connect( m_buttonCreateDicomdir , SIGNAL ( clicked() ) , m_qcreateDicomdir , SLOT( show() ) );

    //connectem Slots dels StudyTreeWidget amb la interficie
    connect( m_studyTreeWidgetPacs , SIGNAL( expandStudy( QString , QString ) ) , this , SLOT( searchSeries( QString , QString ) ) );
    connect( m_studyTreeWidgetCache,  SIGNAL( expandStudy( QString , QString ) ) , this , SLOT( searchSeries( QString , QString ) ) );
    connect( m_studyTreeWidgetDicomdir,  SIGNAL( expandStudy( QString , QString ) ) , this , SLOT( searchSeries( QString , QString ) ) );
    connect( m_studyTreeWidgetPacs , SIGNAL( expandSeries( QString , QString , QString ) ) , this , SLOT( searchImages( QString , QString , QString ) ) );
    connect( m_studyTreeWidgetDicomdir , SIGNAL( expandSeries( QString , QString , QString ) ) , this , SLOT( searchImages( QString , QString , QString ) ) );
    connect( m_studyTreeWidgetCache , SIGNAL( expandSeries( QString , QString , QString ) ) , this , SLOT( searchImages( QString , QString , QString ) ) );

    //es canvia de pestanya del TAB
    connect( m_tab , SIGNAL( currentChanged( int ) ) , this , SLOT( tabChanged( int ) ) );

    //connectem els checkbox de les dates
    connect( m_checkFrom, SIGNAL( stateChanged( int ) ) , this , SLOT( setEnabledTextFrom( int ) ) );
    connect( m_checkTo, SIGNAL( stateChanged( int ) ) , this , SLOT( setEnabledTextTo( int ) ) );

    //connectem el QDateEdit
    connect( m_dateStudyDateFrom, SIGNAL( dateChanged( QDate ) ) , this , SLOT( searchStudyfromDateChanged( QDate ) ) );

    //checkbox
    connect( m_checkAll, SIGNAL( clicked() ) , this , SLOT( clearCheckedModality() ) );
    connect( m_checkCT, SIGNAL( clicked() ) , this , SLOT( checkedSeriesModality() ) );
    connect( m_checkCR, SIGNAL( clicked() ) , this , SLOT( checkedSeriesModality() ) );
    connect( m_checkDX, SIGNAL( clicked() ) , this , SLOT( checkedSeriesModality() ) );
    connect( m_checkES, SIGNAL( clicked() ) , this , SLOT( checkedSeriesModality() ) );
    connect( m_checkMG, SIGNAL( clicked() ) , this , SLOT( checkedSeriesModality() ) );
    connect( m_checkMR, SIGNAL( clicked() ) , this , SLOT( checkedSeriesModality() ) );
    connect( m_checkNM, SIGNAL( clicked() ) , this , SLOT( checkedSeriesModality() ) );
    connect( m_checkDT, SIGNAL( clicked() ) , this , SLOT( checkedSeriesModality() ) );
    connect( m_checkPT, SIGNAL( clicked() ) , this , SLOT( checkedSeriesModality() ) );
    connect( m_checkRF, SIGNAL( clicked() ) , this , SLOT( checkedSeriesModality() ) );
    connect( m_checkSC, SIGNAL( clicked() ) , this , SLOT( checkedSeriesModality() ) );
    connect( m_checkUS, SIGNAL( clicked() ) , this , SLOT( checkedSeriesModality() ) );
    connect( m_checkXA, SIGNAL( clicked() ) , this , SLOT( checkedSeriesModality() ) );
    connect( m_checkOtherModality, SIGNAL( clicked() ) , this , SLOT( checkedSeriesModality() ) );

    //conectem els signals dels TreeView

    connect( m_studyTreeWidgetCache , SIGNAL( delStudy() ) , this , SLOT( deleteStudyCache() ) );
    connect( m_studyTreeWidgetCache , SIGNAL( view() ) , this , SLOT( view() ) );

    connect( m_studyTreeWidgetDicomdir , SIGNAL( view() ) , this , SLOT( view() ) );//quan fem doble click sobre un estudi o sèrie de la llista d'estudis

    //connectem els signes del SeriesIconView StudyListView
    connect( m_studyTreeWidgetCache , SIGNAL( addSeries(DICOMSeries * ) ) , m_seriesListWidgetCache , SLOT( insertSeries(DICOMSeries *) ) );
    connect( m_studyTreeWidgetCache , SIGNAL( clearSeriesListWidget() ) , m_seriesListWidgetCache , SLOT( clear() ) );
    connect( m_seriesListWidgetCache , SIGNAL( selectedSeriesIcon( QString) ) , m_studyTreeWidgetCache , SLOT( selectedSeriesIcon( QString) ) );
    connect( m_seriesListWidgetCache , SIGNAL( viewSeriesIcon() ) , m_studyTreeWidgetCache , SLOT( viewStudy() ) );
    connect( m_studyTreeWidgetCache , SIGNAL( storeStudyToPacs( QString) ) , this , SLOT( storeStudyToPacs( QString) ) );


    //per netejar la QSeriesIconView quant s'esborrar un estudi
    connect(this , SIGNAL( clearSeriesListWidget() ) , m_seriesListWidgetCache , SLOT( clear() ) );

    //per poder descarregar i veure un estudi amb el menu contextual dels del QStudyList del PACS
    connect( m_studyTreeWidgetPacs , SIGNAL( view() ) , this , SLOT( view() ) );
    connect( m_studyTreeWidgetPacs , SIGNAL( retrieve() ) , this , SLOT( retrieve() ) );

    //slot per importar objecte del dicomdir
    connect( m_studyTreeWidgetDicomdir , SIGNAL( retrieve() ) , this , SLOT( importDicomdir() ) );


    //connecta el signal que emiteix qexecuteoperationthread, per visualitzar un estudi amb aquesta classe
    QObject::connect( &m_qexecuteOperationThread , SIGNAL( viewStudy( QString , QString , QString ) ) , this , SLOT( studyRetrievedView( QString , QString , QString ) ) , Qt::QueuedConnection );

    //connecta els signals el qexecute operation thread amb els de qretrievescreen, per coneixer quant s'ha descarregat una imatge, serie, estudi, si hi ha error, etc..
    connect( &m_qexecuteOperationThread , SIGNAL(  setErrorOperation( QString ) ) , m_OperationStateScreen, SLOT(  setErrorOperation( QString ) ) );
    connect( &m_qexecuteOperationThread , SIGNAL(  setOperationFinished( QString ) ) , m_OperationStateScreen, SLOT(  setOperationFinished( QString ) ) );

    connect( &m_qexecuteOperationThread , SIGNAL(  setOperating( QString ) ) , m_OperationStateScreen, SLOT(  setOperating( QString ) ) );
    connect( &m_qexecuteOperationThread , SIGNAL(  imageCommit( QString , int) ) , m_OperationStateScreen , SLOT(  imageCommit( QString , int ) ) );
    connect( &m_qexecuteOperationThread , SIGNAL(  seriesCommit( QString ) ) ,  m_OperationStateScreen , SLOT(  seriesCommit( QString ) ) );
    connect( &m_qexecuteOperationThread , SIGNAL(  newOperation( Operation * ) ) ,  m_OperationStateScreen , SLOT(  insertNewOperation( Operation *) ) );

    // Label d'informació (cutre-xapussa)
    connect( &m_qexecuteOperationThread, SIGNAL( setErrorOperation(QString) ), this, SLOT( updateOperationsInProgressMessage() ));
    connect( &m_qexecuteOperationThread, SIGNAL( setOperationFinished(QString) ), this, SLOT( updateOperationsInProgressMessage() ));
    connect( &m_qexecuteOperationThread, SIGNAL( newOperation(Operation *) ),  this, SLOT( updateOperationsInProgressMessage() ));

    //connecta el signal de que no hi ha suficient espai de disc
    connect( &m_qexecuteOperationThread , SIGNAL(  notEnoughFreeSpace() ) , this , SLOT(  notEnoughFreeSpace() ) );
    //error alliberant espai de la cache
    connect( &m_qexecuteOperationThread , SIGNAL(  errorFreeingCacheSpace() ) , this , SLOT(  errorFreeingCacheSpace() ) );

    //connect tracta els errors de connexió al PACS
    connect ( &multipleQueryStudy , SIGNAL ( errorConnectingPacs( int ) ) , this , SLOT(  errorConnectingPacs( int ) ) );

    connect ( &multipleQueryStudy , SIGNAL ( errorQueringStudiesPacs( int ) ) , this , SLOT(  errorQueringStudiesPacs( int ) ) );


    //connect tracta els errors de connexió al PACS, al descarregar imatges
    connect ( &m_qexecuteOperationThread , SIGNAL ( errorConnectingPacs( int ) ) , this , SLOT(  errorConnectingPacs( int ) ) );

    connect( &m_qexecuteOperationThread , SIGNAL(  setRetrieveFinished( QString ) ) , this, SLOT(  studyRetrieveFinished ( QString ) ) );

    //connecta l'acció per afegir un estudi a la llista d'estudis a convertir a dicomdir
    connect( m_studyTreeWidgetCache , SIGNAL ( convertToDicomDir( QString ) ) , this , SLOT ( convertToDicomdir( QString ) ) );

    //Amaga o ensenya la cerca avançada
    connect( m_pushButtonAdvancedSearch , SIGNAL( toggled( bool ) ) , m_qwidgetAdvancedSearch , SLOT( setVisible( bool ) ) );

    connect( m_textOtherModality , SIGNAL ( editingFinished () ) , SLOT( textOtherModalityEdited() ) );

}

void QueryScreen::setWindowPosition()
{
    StarviewerSettings settings;

    move( settings.getQueryScreenWindowPositionX() , settings.getQueryScreenWindowPositionX() );
}

void QueryScreen::setWindowSize()
{
    StarviewerSettings settings;

    resize( settings.getQueryScreenWindowWidth() , settings.getQueryScreenWindowHeight() );
}

void QueryScreen::setQSplitterState()
{
    StarviewerSettings settings;

    if ( !settings.getQueryScreenStudyTreeSeriesListQSplitterState().isEmpty() )
    {
        m_StudyTreeSeriesListQSplitter->restoreState( settings.getQueryScreenStudyTreeSeriesListQSplitterState() );
    }
}

void QueryScreen::clearTexts()
{
    m_textStudyID->clear();
    m_textPatientID->clear();
    m_textPatientName->clear();
    m_textAccessionNumber->clear();
    m_checkTo->setChecked( false );
    m_checkFrom->setChecked( false );
    m_textReferringPhysiciansName->clear();
    m_textStudyUID->clear();
    m_textSeriesUID->clear();
    m_textRequestedProcedureID->clear();
    m_textScheduledProcedureStepID->clear();
    m_textSOPInstanceUID->clear();
    m_textInstanceNumber->clear();
    m_textPPStartDate->clear();
    m_textPPStartTime->clear();
    m_textSeriesNumber->clear();
    m_textStudyModality->clear();

    clearCheckedModality();

    setEnabledTextTo( m_checkTo->isChecked() );
    setEnabledTextFrom( m_checkFrom->isChecked() );
}

void QueryScreen::setEnabledTextTo( int value )
{
    m_dateStudyDateTo->setEnabled( value );
}

void QueryScreen::setEnabledTextFrom( int value )
{
    m_dateStudyDateFrom->setEnabled( value );
}

void QueryScreen::searchStudyfromDateChanged( QDate fromDate )
{
    m_dateStudyDateTo->setDate( fromDate );
}

void QueryScreen::clearCheckedModality()
{
    m_checkAll->setChecked( true );
    m_checkCR->setChecked( false );
    m_checkCT->setChecked( false );
    m_checkDX->setChecked( false );
    m_checkES->setChecked( false );
    m_checkMG->setChecked( false );
    m_checkMR->setChecked( false );
    m_checkNM->setChecked( false );
    m_checkDT->setChecked( false );
    m_checkPT->setChecked( false );
    m_checkRF->setChecked( false );
    m_checkSC->setChecked( false );
    m_checkUS->setChecked( false );
    m_checkXA->setChecked( false );

    m_textOtherModality->clear();
    m_textOtherModality->setEnabled( false );
}

void QueryScreen::checkedSeriesModality()
{
    if ( m_checkCR->isChecked() ||
        m_checkCT->isChecked() ||
        m_checkDX->isChecked() ||
        m_checkES->isChecked() ||
        m_checkMG->isChecked() ||
        m_checkMR->isChecked() ||
        m_checkNM->isChecked() ||
        m_checkDT->isChecked() ||
        m_checkPT->isChecked() ||
        m_checkRF->isChecked() ||
        m_checkSC->isChecked() ||
        m_checkUS->isChecked() ||
        m_checkXA->isChecked() ||
        m_checkAll->isChecked() )
    {
        m_textOtherModality->clear();
        m_textOtherModality->setEnabled( false );
    }
    else
    {
        m_textOtherModality->setEnabled( true );
        m_textOtherModality->clear();
    }
}

void QueryScreen::textOtherModalityEdited()
{
    if ( m_textOtherModality->text().isEmpty() )
    {
        clearCheckedModality();
    }
    else
    {
        m_checkAll->setChecked( false );
        m_checkCR->setChecked( false );
        m_checkCT->setChecked( false );
        m_checkDX->setChecked( false );
        m_checkES->setChecked( false );
        m_checkMG->setChecked( false );
        m_checkMR->setChecked( false );
        m_checkNM->setChecked( false );
        m_checkDT->setChecked( false );
        m_checkPT->setChecked( false );
        m_checkRF->setChecked( false );
        m_checkSC->setChecked( false );
        m_checkUS->setChecked( false );
        m_checkXA->setChecked( false );
    }
}

void QueryScreen::dateFromChanged( const QDate &data )
{
    m_dateStudyDateTo->setDate( data );
}

void QueryScreen::setEnabledDates( bool enabled )
{
    m_dateStudyDateFrom->setEnabled( enabled );
    m_checkFrom->setEnabled( enabled );
    m_checkFrom->setChecked( enabled );

    m_dateStudyDateTo->setEnabled( enabled );
    m_checkTo->setEnabled( enabled );
    m_checkTo->setChecked( enabled );
}

void QueryScreen::searchTodayStudy()
{
    QDate today;

    setEnabledDates( true );

    m_dateStudyDateTo->setDate(today.currentDate() );
    m_dateStudyDateFrom->setDate(today.currentDate() );
    searchStudy();
}

void QueryScreen::searchYesterdayStudy()
{
    QDate today;
    QDate yesterday( today.currentDate() );

    setEnabledDates( true );
    yesterday = yesterday.addDays( -1 );//calcula la data d'ahir

    m_dateStudyDateFrom->setDate( yesterday );
    m_dateStudyDateTo->setDate( yesterday );
    searchStudy();
}

void QueryScreen::searchStudy()
{
    switch ( m_tab->currentIndex() )
    {
        case 0:
            queryStudy("Cache");
            break;
        case 1:
            if ( !validateNoEmptyMask() )
            {
                switch( QMessageBox::information( this , tr( "Starviewer" ) ,
                                            tr( "You have not specified any filter. This query could take a long time. Do you want to continue ?" ) ,
                                            tr( "&Yes" ) , tr( "&No" ) ,
                                            0 , 1 ) )
                {
                    case 0:
                            queryStudyPacs();
                            QApplication::restoreOverrideCursor();
                            break;
                }
            }
            else
            {
                queryStudyPacs();
            }
            break;
        case 2:
            queryStudy("DICOMDIR");
            break;
    }

}

bool QueryScreen::validateNoEmptyMask()
{
    if ( m_textPatientID->text().length() == 0 &&
         m_textPatientName->text().length() == 0 &&
         m_textStudyID->text().length() == 0 &&
         m_textAccessionNumber->text().length() == 0 &&
         !m_checkFrom->isChecked()  &&
         !m_checkTo->isChecked() &&
         m_checkAll->isChecked() &&
         !m_textReferringPhysiciansName->text().length() == 0 &&
         !m_textSeriesNumber->text().length() == 0 &&
         m_textStudyUID->text().length() == 0 &&
         m_textSeriesUID->text().length() == 0 &&
         m_textRequestedProcedureID->text().length() == 0 &&
         m_textScheduledProcedureStepID->text().length() == 0 &&
         m_textPPStartDate->text().length() == 0 &&
         m_textPPStartTime->text().length() == 0 &&
         m_textSOPInstanceUID->text().length() == 0 &&
         m_textInstanceNumber->text().length() == 0 &&
         m_textStudyModality->text().length() )
    {
        return false;
    }
    else return true;
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
        databaseError( &state );
        return state;
    }

    pacs.setAELocal( settings.getAETitleMachine() ); //especifiquem el nostres AE
    pacs.setTimeOut( settings.getTimeout().toInt( NULL , 10 ) ); //li especifiquem el TimeOut

    pacsConnection->setPacs( pacs );

    return state;
}

void QueryScreen::queryStudyPacs()
{
    PacsList pacsList;
    PacsParameters pa;
    QString result;
    StarviewerSettings settings;

    QApplication::setOverrideCursor( QCursor( Qt::WaitCursor ) );

    INFO_LOG( "Cerca d'estudis als PACS amb paràmetres " + logQueryStudy() );

    pacsList.clear(); //netejem el pacsLIST
    qPacsList->getSelectedPacs( &pacsList ); //Emplemen el pacsList amb les pacs seleccionats al QPacsList

    pacsList.firstPacs();
    m_seriesListSingleton->clear();
    m_studyListSingleton->clear();
    m_imageListSingleton->clear();
    if ( pacsList.end() ) //es comprova que hi hagi pacs seleccionats
    {
        QApplication::restoreOverrideCursor();
        QMessageBox::warning( this , tr( "Starviewer" ) , tr( "Please select a PACS to query" ) );
        return;
    }

    multipleQueryStudy.setPacsList( pacsList ); //indiquem a quins Pacs Cercar
    multipleQueryStudy.setMask( buildDicomMask() ); //construim la mascara

    pacsList.firstPacs();
    m_lastQueriedPacs = pacsList.getPacs().getAEPacs();

    if ( !multipleQueryStudy.StartQueries().good() )  //fem la query
    {
        m_studyTreeWidgetPacs->clear();
        QApplication::restoreOverrideCursor();
        QMessageBox::information( this , tr( "Starviewer" ) , tr( "ERROR QUERING!." ) );
        return;
    }

    m_studyListSingleton->firstStudy();

    if ( m_studyListSingleton->end() )
    {
        m_studyTreeWidgetPacs->clear();
        QApplication::restoreOverrideCursor();
        QMessageBox::information( this , tr( "Starviewer" ) , tr( "No study match found." ) );
        return;
    }
    m_studyTreeWidgetPacs->insertStudyList( m_studyListSingleton ); //fem que es visualitzi l'studyView seleccionat
    m_studyTreeWidgetPacs->insertSeriesList( m_seriesListSingleton );
    m_studyTreeWidgetPacs->insertImageList( m_imageListSingleton );
    m_studyTreeWidgetPacs->setSortColumn( 2 );//ordenem pel nom

    QApplication::restoreOverrideCursor();
}

void QueryScreen::queryStudy( QString source )
{
    CacheStudyDAL cacheStudyDAL;
    StudyList studyList;
    Status state;

    QApplication::setOverrideCursor( QCursor( Qt::WaitCursor ) );

    INFO_LOG( "Cerca d'estudis a la font" + source + " amb paràmetres " + logQueryStudy() );

    studyList.clear();
    if( source == "Cache" )
    {
        m_seriesListWidgetCache->clear();
        state = cacheStudyDAL.queryStudy( buildDicomMask() , studyList ); //busquem els estudis a la cache
        if ( !state.good() )
        {
            m_studyTreeWidgetCache->clear();
            QApplication::restoreOverrideCursor();
            databaseError( &state );
            return;
        }
    }
    else if( source == "DICOMDIR" )
    {
        state = m_readDicomdir.readStudies( studyList , buildDicomMask() );
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
    studyList.firstStudy();

    /* Aquest mètode a part de ser cridada quan l'usuari fa click al botó search, també es cridada al
     * constructor d'aquesta classe, per a que al engegar l'aplicació ja es mostri la llista d'estudis
     * que hi ha a la base de dades local. Si el mètode no troba cap estudi a la base de dades local
     * es llença el missatge que no s'han trobat estudis, però com que no és idonii, en el cas aquest que es
     * crida des del constructor que es mostri el missatge de que no s'han trobat estudis al engegar l'aplicació, el que
     * es fa és que per llançar el missatge es comprovi que la finestra estigui activa. Si la finestra no està activa
     * vol dir que el mètode ha estat invocat des del constructor
     */
    if ( studyList.end() && isActiveWindow() )
    {
        //no hi ha estudis
        if( source == "Cache" )
            m_studyTreeWidgetCache->clear();
        else if( source == "DICOMDIR" )
            m_studyTreeWidgetDicomdir->clear();

        QMessageBox::information( this , tr( "Starviewer" ) , tr( "No study match found." ) );
    }
    else
    {
        if( source == "Cache" )
        {
            m_studyTreeWidgetCache->insertStudyList( &studyList );//es mostra la llista d'estudis
            m_studyTreeWidgetCache->setSortColumn( 2 ); //ordenem pel nom
        }
        else if( source == "DICOMDIR" )
        {
            m_studyTreeWidgetDicomdir->clear();
            m_studyTreeWidgetDicomdir->insertStudyList( &studyList );
            m_studyTreeWidgetDicomdir->setSortColumn( 2 );//ordenem pel nom
        }
    }
    QApplication::restoreOverrideCursor();
}

/* AQUESTA ACCIO ES CRIDADA DES DEL STUDYLISTVIEW*/
void QueryScreen::searchSeries( QString studyUID , QString pacsAETitle )
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
void QueryScreen::searchImages( QString studyUID , QString seriesUID , QString pacsAETitle )
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

    m_seriesListSingleton->clear();//netegem la llista de sèries

    INFO_LOG( "Cercant informacio de les sèries de l'estudi" + studyUID + " del PACS " + pacsAETitle );

    if ( pacsAETitle.isEmpty() ) pacsAETitle = m_lastQueriedPacs;//necessari per les mesatools no retornen a quin pacs pertany l'estudi

    if ( ! preparePacsServerConnection( pacsAETitle, &pacsConnection ).good() ) return;

    state = pacsConnection.connect(PacsServer::query,PacsServer::seriesLevel);
    if ( !state.good() )
    {//Error al connectar
        ERROR_LOG( "Error al connectar al pacs " + pacsAETitle + ". PACS ERROR : " + state.text() );

        errorConnectingPacs ( pacsConnection.getPacs().getPacsID() );
        return;
    }

    querySeriesPacs.setConnection( pacsConnection.getConnection() );
    state = querySeriesPacs.query( buildSeriesDicomMask( studyUID ) );
    pacsConnection.disconnect();

    if ( !state.good() )
    {//Error a la query
        ERROR_LOG( "QueryScreen::QueryPacs : Error cercant les sèries al PACS " + pacsAETitle + ". PACS ERROR : " + state.text() );

        text.insert( 0 , tr( "Error! Can't query series in PACS : " ) );
        text.append( pacsAETitle );
        QMessageBox::warning( this , tr( "Starviewer" ) , text );
        return;
    }

    m_seriesListSingleton->firstSeries();
    if ( m_seriesListSingleton->end() )
    {
        QMessageBox::information( this , tr( "Starviewer" ) , tr( "No series match for this study.\n" ) );
        return;
    }

    m_studyTreeWidgetPacs->insertSeriesList( m_seriesListSingleton );
}

void QueryScreen::querySeries( QString studyUID, QString source )
{
    DICOMSeries serie;
    SeriesList seriesList;
    CacheSeriesDAL cacheSeriesDAL;
    CacheImageDAL cacheImageDAL;
    int imagesNumber;
    Status state;
    DicomMask mask;

    INFO_LOG( "Cerca de sèries a la font " + source +" de l'estudi " + studyUID );

    seriesList.clear();//preparem la llista de series

    if( source == "Cache" )
    {
        //preparem la mascara i cerquem les series a la cache
        mask.setStudyUID( studyUID );
        state = cacheSeriesDAL.querySeries( mask , seriesList );
        if ( !state.good() )
        {
            databaseError( &state );
            return;
        }
    }
    else if( source == "DICOMDIR" )
    {
        m_readDicomdir.readSeries( studyUID , "" , seriesList ); //"" pq no busquem cap serie en concret
    }
    else
    {
        DEBUG_LOG( "Unrecognised source: " + source );
        return;
    }
    seriesList.firstSeries();

    if ( seriesList.end() )
    {
        QMessageBox::information( this , tr( "Starviewer" ) , tr( "No series match for this study.\n" ) );
        return;
    }

    if( source == "Cache" )
    {
        m_seriesListWidgetCache->clear();

        while ( !seriesList.end() )
        {
            serie= seriesList.getSeries();
            //preparem per fer la cerca d'imatges
            mask.setSeriesUID( serie.getSeriesUID() );
            state = cacheImageDAL.countImageNumber( mask , imagesNumber );
            serie.setImageNumber( imagesNumber );
            if ( !state.good() )
            {
                databaseError( &state );
                return;
            }
            m_studyTreeWidgetCache->insertSeries( &serie );//inserim la informació de les imatges al formulari
            seriesList.nextSeries();
        }
    }
    else if( source == "DICOMDIR" )
        m_studyTreeWidgetDicomdir->insertSeriesList( &seriesList );//inserim la informació de la sèrie al llistat
}

void QueryScreen::queryImagePacs( QString studyUID , QString seriesUID , QString AETitlePACS )
{
    QApplication::setOverrideCursor( QCursor( Qt::WaitCursor ) );

    DICOMSeries serie;
    Status state;
    QString text;
    PacsServer pacsConnection;
    QueryPacs queryImages;
    DicomMask dicomMask;

    if ( AETitlePACS.isEmpty() )
        AETitlePACS = m_lastQueriedPacs;//necessari per les mesatools no retornen a quin pacs pertany l'estudi
    m_imageListSingleton->clear();//netejem la llista de sèries

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

        text.insert( 0 , tr( "Error! Can't query images in PACS : " ) );
        text.append( AETitlePACS );
        QMessageBox::warning( this , tr( "Starviewer" ) , text );
        return;
    }

    pacsConnection.disconnect();

    m_imageListSingleton->firstImage();
    if ( m_imageListSingleton->end() )
    {
        QApplication::restoreOverrideCursor();
        QMessageBox::information( this , tr( "Starviewer" ) , tr( "No images match for this series.\n" ) );
        return;
    }

    m_studyTreeWidgetPacs->insertImageList( m_imageListSingleton );

    QApplication::restoreOverrideCursor();
}

void QueryScreen::retrieve()
{
//     switch( QMessageBox::information( this , tr( "Starviewer" ) ,
// 				      tr( "Are you sure you want to retrieve this Study ?" ) ,
// 				      tr( "&Yes" ) , tr( "&No" ) ,
// 				      0, 1 ) )
//     {
//     case 0:
        retrievePacs( false );
//         break;
//     }
}

void QueryScreen::queryImage(QString studyUID, QString seriesUID, QString source )
{
    CacheImageDAL cacheImageDAL;
    ImageList imageList;
    DicomMask mask;

    INFO_LOG( "Cerca d'imatges a la font " + source + " de l'estudi " + studyUID + " i serie " + seriesUID );

    if( source == "Cache" )
    {
        mask.setStudyUID( studyUID );
        mask.setSeriesUID( seriesUID );
        cacheImageDAL.queryImages( mask , imageList );
    }
    else if( source == "DICOMDIR" )
    {
        m_readDicomdir.readImages( seriesUID , "", imageList );
    }
    else
    {
        DEBUG_LOG( "Unrecognised source: " + source );
        return;
    }

    imageList.firstImage();
    if ( imageList.end() )
    {
        QMessageBox::information( this , tr( "Starviewer" ) , tr( "No images match for this study.\n" ) );
        return;
    }
    if( source == "Cache" )
        m_studyTreeWidgetCache->insertImageList( &imageList );//inserim la informació de la sèrie al llistat
    else if( source == "DICOMDIR" )
        m_studyTreeWidgetDicomdir->insertImageList( &imageList );//inserim la informació de la sèrie al llistat
}

void QueryScreen::retrievePacs( bool view )
{
    DicomMask mask;
    QString studyUID,defaultSeriesUID;
    Status state;
    Operation operation;
    PacsParameters pacs;
    PacsListDB pacsListDB;
    StarviewerSettings settings;
    QString pacsAETitle;
    DICOMStudy studyToRetrieve;

    QApplication::setOverrideCursor( QCursor ( Qt::WaitCursor ) );

    if ( m_studyTreeWidgetPacs->getSelectedStudyUID().isEmpty() )
    {
        QApplication::restoreOverrideCursor();
        if ( view)
        {
            QMessageBox::warning( this , tr( "Starviewer" ) , tr( "Select a study to view " ) );
        }
        else QMessageBox::warning( this , tr( "Starviewer" ) , tr( "Select a study to download " ) );
        return;
    }
    studyUID.insert(0 , m_studyTreeWidgetPacs->getSelectedStudyUID() );

    //Tenim l'informació de l'estudi a descarregar a la llista d'estudis, el busquem a la llista
    if ( !m_studyListSingleton->exists( studyUID , m_studyTreeWidgetPacs->getSelectedPacsAETitle() ) )
    {
        QApplication::restoreOverrideCursor();
        QMessageBox::warning( this , tr( "Starviewer" ) , tr( "Internal Error : " ) );
        return;
    }

    studyToRetrieve = m_studyListSingleton->getStudy();
    if ( m_studyTreeWidgetPacs->getSelectedPacsAETitle().isEmpty() ) //per les mesatools que no retornen a quin PACS pertany l'estudi cercat
    {
        pacsAETitle = m_lastQueriedPacs;
        studyToRetrieve.setPacsAETitle( m_lastQueriedPacs );
    }
    else
        pacsAETitle = m_studyTreeWidgetPacs->getSelectedPacsAETitle();

    //Inserim l'informació de l'estudi a la caché!
    state = insertStudyCache( studyToRetrieve );

    if( !state.good() )
    {
        if ( state.code() != 2019 ) // si hi ha l'error 2019, indica que l'estudi ja existeix a la base de dades, per tant estar parcialment o totalment descarregat, de totes maneres el tornem a descarregar
        {
            QApplication::restoreOverrideCursor();
            databaseError( &state );
            return;
        }
        else // si l'estudi ja existeix actualizem la seva informació també
        {
            CacheStudyDAL cacheStudyDAL; //Actualitzem la informació
            cacheStudyDAL.updateStudy ( studyToRetrieve );
        }
    }

    mask.setStudyUID( studyUID );//definim la màscara per descarregar l'estudi
    if ( !m_studyTreeWidgetPacs->getSelectedSeriesUID().isEmpty() )
        mask.setSeriesUID( m_studyTreeWidgetPacs->getSelectedSeriesUID() );

    if ( !m_studyTreeWidgetPacs->getSelectedImageUID().isEmpty() )
        mask.setSOPInstanceUID(  m_studyTreeWidgetPacs->getSelectedImageUID() );

    //busquem els paràmetres del pacs del qual volem descarregar l'estudi
    state = pacsListDB.queryPacs( &pacs , pacsAETitle );

    if ( !state.good() )
    {
        QApplication::restoreOverrideCursor();
        databaseError( &state );
        return;
    }

    //inserim a la pantalla de retrieve que iniciem la descarrega
    //m_OperationStateScreen->insertNewRetrieve( &m_studyListSingleton->getStudy() );

    //emplanem els parametres amb dades del starviewersettings
    pacs.setAELocal( settings.getAETitleMachine() );
    pacs.setTimeOut( settings.getTimeout().toInt( NULL , 10 ) );
    pacs.setLocalPort( settings.getLocalPort() );

    //definim l'operacio
    operation.setPacsParameters( pacs );
    operation.setDicomMask( mask );
    if ( view )
        operation.setOperation( operationView );
    else
        operation.setOperation( operationRetrieve );

    //emplenem les dades de l'operació
    operation.setPatientName( m_studyListSingleton->getStudy().getPatientName() );
    operation.setPatientID( m_studyListSingleton->getStudy().getPatientId() );
    operation.setStudyID( m_studyListSingleton->getStudy().getStudyId() );
    operation.setStudyUID( m_studyListSingleton->getStudy().getStudyUID() );

    m_qexecuteOperationThread.queueOperation( operation );

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
    absPath.insert( 0 , settings.getCacheImagePath() );
    absPath.append( study.getStudyUID() );
    absPath.append( "/" );
    study.setAbsPath(absPath);
    //inserim l'estudi a la caché
    state = cacheStudyDAL.insertStudy( &study );

    return state;
}

void QueryScreen::studyRetrievedView( QString studyUID , QString seriesUID , QString sopInstanceUID )
{
    retrieve( studyUID , seriesUID , sopInstanceUID, "Cache" );
}

void QueryScreen::tabChanged( int index )
{
    switch ( index )
    {
        case 0: //Database
                m_buttonGroupModality->setEnabled( false );//desactivem el grup button de motalitat
                m_buttonRetrieve->setEnabled( false );//desactivem el boto retrieve
                m_buttonShowPacsList->setEnabled( true );//activem el boto d'ensenyar la llista de pacs
                clearCheckedModality();
                if (  m_PacsListShow ) resizePacsList();
                m_qwidgetAdvancedSearch->hide();//amaguem la cerca avançada
                m_pushButtonAdvancedSearch->hide();
                break;
        case 1: //Pacs
                m_buttonGroupModality->setEnabled( true );;//activem el grup button de modalitat
                m_buttonRetrieve->setEnabled( true );//activem el boto retrieve
                m_buttonShowPacsList->setEnabled( true );//activem el boto d'ensenyar la llista de pacs
                clearCheckedModality();
                if (  m_PacsListShow ) resizePacsList();
                m_pushButtonAdvancedSearch->show();
                if ( m_pushButtonAdvancedSearch->isChecked() ) m_qwidgetAdvancedSearch->show();
                break;
        case 2: //Dicomdir
                m_buttonGroupModality->setEnabled( false );;//desactivem el grup button de modalitat
                m_buttonRetrieve->setEnabled( false );//activem el boto retrieve
                m_buttonShowPacsList->setEnabled( false );//activem el boto d'ensenyar la llista de pacs
                clearCheckedModality();
                if (  m_PacsListShow ) resizePacsList();
                m_qwidgetAdvancedSearch->hide();//amaguem la cerca avançada
                m_pushButtonAdvancedSearch->hide();
                break;
        }
}

void QueryScreen::view()
{

    switch ( m_tab->currentIndex() )
    {
        case 0 :
            retrieve( m_studyTreeWidgetCache->getSelectedStudyUID() , m_studyTreeWidgetCache->getSelectedSeriesUID() , m_studyTreeWidgetCache->getSelectedImageUID(), "Cache" );
            break;
        case 1 :
//             switch( QMessageBox::information( this , tr( "Starviewer" ) ,
//                         tr( "Are you sure you want to view this Study ?" ) ,
//                         tr( "&Yes" ) , tr( "&No" ) ,
//                         0 , 1 ) )
//             {
//                 case 0:
                    retrievePacs( true );
//                     break;
//             }
           break;
        case 2 :
            retrieve( m_studyTreeWidgetDicomdir->getSelectedStudyUID() , m_studyTreeWidgetDicomdir->getSelectedSeriesUID() ,  m_studyTreeWidgetDicomdir->getSelectedImageUID(), "DICOMDIR" );
            break;
        default :
            break;

    }
    if ( m_tab->currentIndex() == 1)
    {

    }
    else if ( m_tab->currentIndex() == 0)
    {


    }
}

void QueryScreen::retrieve( QString studyUID , QString seriesUID , QString sopInstanceUID, QString source )
{
    CacheStudyDAL cacheStudyDAL;

    if ( studyUID.isEmpty() )
    {
        QMessageBox::warning( this , tr( "Starviewer" ) , tr( "Select a study to view " ) );
        return;
    }

    QStringList files;
    if( source == "Cache" )
    {
        files = cacheStudyDAL.getFiles( studyUID );
        cacheStudyDAL.updateStudyAccTime( studyUID );
    }
    else if( source == "DICOMDIR" )
    {
        files = m_readDicomdir.getFiles( studyUID );
    }
    else
    {
        DEBUG_LOG("Unrecognized source: " + source );
        return;
    }

    this->close();//s'amaga per poder visualitzar la serie
    if ( m_OperationStateScreen->isVisible() )
    {
        m_OperationStateScreen->close();//s'amaga per poder visualitzar la serie
    }

    // enviem la informació a processar
    emit processFiles( files, studyUID, seriesUID, sopInstanceUID );
}

void QueryScreen::importDicomdir()
{
    ImportDicomdir importDicom;

    QApplication::setOverrideCursor( QCursor( Qt::WaitCursor ) );

    importDicom.import( m_readDicomdir.getDicomdirPath() , m_studyTreeWidgetDicomdir->getSelectedStudyUID() , m_studyTreeWidgetDicomdir->getSelectedSeriesUID() ,  m_studyTreeWidgetDicomdir->getSelectedImageUID() );

    QApplication::restoreOverrideCursor();
}

void QueryScreen::deleteStudyCache()
{
    Status state;
    CacheStudyDAL cacheStudyDAL;
    QString studyUID;

    studyUID = m_studyTreeWidgetCache->getSelectedStudyUID();

    if ( studyUID.isEmpty() )
    {
        QMessageBox::information( this , tr( "Starviewer" ) , tr( "Please select a study to delete" ) );
        return;
    }

//     switch( QMessageBox::information( this , tr( "Starviewer" ) ,
// 				      tr( "Are you sure you want to delete this Study ?" ) ,
// 				      tr( "&Yes" ) , tr( "&No" ) ,
// 				      0, 1 ) )
//     {
//         case 0:
            INFO_LOG( "S'esborra de la cache l'estudi " + studyUID );

            state = cacheStudyDAL.delStudy( studyUID );

            if ( state.good() )
            {
                m_studyTreeWidgetCache->removeStudy( studyUID );
                emit( clearSeriesListWidget() );//Aquest signal es recollit per qSeriesIconView
            }
            else
            {
                databaseError( &state );
            }
//     }
}

void QueryScreen::studyRetrieveFinished( QString studyUID )
{
    DICOMStudy study;
    CacheStudyDAL cacheStudyDAL;
    Status state;

    state = cacheStudyDAL.queryStudy( studyUID , study );

    if ( state.good() )
    {
        m_studyTreeWidgetCache->insertStudy( &study );
        m_studyTreeWidgetCache->sort();
    }
    else databaseError( &state );

}

void QueryScreen::closeEvent( QCloseEvent* event )
{
	m_studyTreeWidgetPacs->saveColumnsWidth();
	m_studyTreeWidgetCache->saveColumnsWidth();
    m_studyTreeWidgetDicomdir->saveColumnsWidth();

    event->accept();
    m_qcreateDicomdir->clearTemporaryDir();
}

void QueryScreen::showPacsList()
{
    if ( !m_PacsListShow )
    {
        m_PacsListShow = true;
        m_buttonShowPacsList->setText( tr( "Hide Pacs List" ) );
    }
    else
    {
        m_buttonShowPacsList->setText( tr( "Show Pacs List" ) );
        m_PacsListShow = false;
    }

    resizePacsList();
}

void QueryScreen::resizePacsList()
{
    int mida = 0;

    //si es cert i estem al tab del Pacs s'ha de mostrar la llistat de PACS
    if ( m_tab->currentWidget()->objectName() == "m_tabDicomdir" )
    {
        if ( m_PacsListShow && m_pacsListIsShowed )
        {
            qPacsList->setFixedSize( 1 , 1 );
            mida = -( 200 + 20 );
            m_pacsListIsShowed = false;
        }
    }
    else
    {
        if ( m_PacsListShow && !m_pacsListIsShowed )
        {
            qPacsList->setFixedSize( 200 ,240 );
            mida = 200 + 20;
            m_pacsListIsShowed = true;
        }

        if ( !m_PacsListShow && m_pacsListIsShowed )
        {
            qPacsList->setFixedSize( 1 , 1 );
            mida = -( 200 + 20 );
            m_pacsListIsShowed = false;
        }
    }

   this->resize( this->width() + mida, this->height() );
}

void QueryScreen::convertToDicomdir( QString studyUID )
{
    CacheStudyDAL cacheStudyDAL;
    DICOMStudy study;

    //busquem la informació de l'estudi
    cacheStudyDAL.queryStudy( studyUID , study );

    //afegim l'estudi a la llista d'estudis pendents per crear el Dicomdir
    m_qcreateDicomdir->addStudy( study );
}

void QueryScreen::openDicomdir()
{
    QFileDialog *dlg = new QFileDialog( 0 , QFileDialog::tr( "Open" ) , "./" , tr( "Dicomdir" ) );
    QString path, dicomdirPath;

    dlg->setFileMode( QFileDialog::DirectoryOnly );
    Status state;

    if ( dlg->exec() == QDialog::Accepted )
    {
        if ( !dlg->selectedFiles().empty() ) dicomdirPath.insert( 0 , dlg->selectedFiles().takeFirst() );

        state = m_readDicomdir.open ( dicomdirPath );//Obrim el dicomdir

        if ( !state.good() )
        {
            QMessageBox::warning( this , tr( "Starviewer" ) , tr( "Error openning dicomdir" ) );
            ERROR_LOG( "Error al obrir el dicomdir " + dicomdirPath + state.text() );
        }
        else
        {
            INFO_LOG( "Obert el dicomdir " + dicomdirPath );
            this->show();
            m_tab->setCurrentIndex( 2 ); // mostre el tab del dicomdir
        }

        clearTexts();//Netegem el filtre de cerca al obrir el dicomdir
        //cerquem els estudis al dicomdir per a que es mostrin
        queryStudy("DICOMDIR");
    }

    delete dlg;
}

void QueryScreen::storeStudyToPacs( QString studyUID )
{
    CacheStudyDAL cacheStudy;
    PacsListDB pacsListDB;
    PacsParameters pacs;
    StarviewerSettings settings;
    Operation storeStudyOperation;
    DicomMask dicomMask;
    Status state;
    DICOMStudy study;
    PacsList pacsList;

    QApplication::setOverrideCursor( QCursor( Qt::WaitCursor ) );

    pacsList.clear(); //netejem el pacsLIST
    qPacsList->getSelectedPacs( &pacsList ); //Emplemen el pacsList amb les pacs seleccionats al QPacsList

    switch (pacsList.size())
    {
        case  0 :
            QApplication::restoreOverrideCursor();
            QMessageBox::warning( this , tr( "Starviewer" ) , tr( "You have to select a Pacs to store the study" ));
            break;
        case 1 :
            cacheStudy.queryStudy( studyUID, study );

            dicomMask.setStudyUID( studyUID );
            storeStudyOperation.setPatientName( study.getPatientName() );
            storeStudyOperation.setStudyUID( study.getStudyUID() );
            storeStudyOperation.setOperation( operationMove );
            storeStudyOperation.setDicomMask( dicomMask );
            storeStudyOperation.setPatientID( study.getPatientId() );
            storeStudyOperation.setStudyID( study.getStudyId() );

            pacsList.firstPacs();
            state = pacsListDB.queryPacs( &pacs, pacsList.getPacs().getAEPacs() );//cerquem els par�etres del Pacs al qual s'han de cercar les dades
            if ( !state.good() )
            {
                QApplication::restoreOverrideCursor();
                databaseError( &state );
                return;
            }

            pacsList.firstPacs();
            storeStudyOperation.setPacsParameters( pacsList.getPacs() );

            m_qexecuteOperationThread.queueOperation( storeStudyOperation );
            break;
        default :
            QApplication::restoreOverrideCursor();
            QMessageBox::warning( this , tr( "Starviewer" ) , tr( "The study can only be stored at one pacs" ));
            break;
    }

    QApplication::restoreOverrideCursor();
}

void QueryScreen::notEnoughFreeSpace()
{
    QMessageBox::warning( this , tr( "Starviewer" ) , tr( "Not enough space to retrieve studies. Please free space" ) );
}

void QueryScreen::errorFreeingCacheSpace()
{
    QMessageBox::critical( this , tr( "Starviewer" ) , tr( "Error Freeing Space. The study couldn't be retrieved" ) );
}

void QueryScreen::errorConnectingPacs( int IDPacs )
{
    PacsListDB pacsListDB;
    PacsParameters errorPacs;
    QString errorMessage;

    pacsListDB.queryPacs( &errorPacs, IDPacs );


    errorMessage.insert( 0 , tr( " Can't connect to PACS " ) );
    errorMessage.append( errorPacs.getAEPacs() );
    errorMessage.append( tr ( " of " ) );
    errorMessage.append( errorPacs.getInstitution() );
    errorMessage.append( '\n' );
    errorMessage.append( tr( " Be sure that the IP and AETitle of the PACS is correct " ) );

    QMessageBox::critical( this , tr( "Starviewer" ) , errorMessage );
}

void QueryScreen::errorQueringStudiesPacs( int PacsID )
{
    PacsListDB pacsListDB;
    PacsParameters errorPacs;
    QString errorMessage;

    pacsListDB.queryPacs( &errorPacs, PacsID );

    errorMessage.insert( 0 , tr( " Can't query studies to PACS " ) );
    errorMessage.append( errorPacs.getAEPacs() );
    errorMessage.append( tr ( " of " ) );
    errorMessage.append( errorPacs.getInstitution() );
    errorMessage.append( '\n' );
    errorMessage.append( tr( " Be sure that the IP and AETitle of the PACS is correct " ) );

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
    mask.setSeriesBodyPartExaminated( "" );
    mask.setSeriesUID( "" );
    mask.setPPSStartDate( "" );
    mask.setPPStartTime( "" );
    mask.setRequestAttributeSequence( "" , "" );

    return mask;
}

QString QueryScreen::buildStudyDates()
{
    QString date;

    if ( m_checkFrom->isChecked() && m_checkTo->isChecked() )
    {
        if ( m_dateStudyDateFrom->date() == m_dateStudyDateTo->date() )
        {
            date.append( m_dateStudyDateFrom->date().toString( "yyyyMMdd" ) );
        }
        else
        {
            date.append( m_dateStudyDateFrom->date().toString( "yyyyMMdd" ) );
            date.append( "-" );
            date.append( m_dateStudyDateTo->date().toString( "yyyyMMdd" ) );
        }
    }
    else
    {
        if ( m_checkFrom->isChecked() )
        {
            date.append( m_dateStudyDateFrom->date().toString( "yyyyMMdd" ) );
            date.append( "-" ); // indiquem que volem buscar tots els estudis d'aquella data en endavant
        }
        else if ( m_checkTo->isChecked() )
        {
            date.append( "-" ); //indiquem que volem buscar tots els estudis que no superin aquesta data
            date.append( m_dateStudyDateTo->date().toString( "yyyyMMdd" ) );
        }
    }

    return date;
}

DicomMask QueryScreen::buildDicomMask()
{
    /*Per fer cerques entre valors consultat el capítol 4 de DICOM punt C.2.2.2.5*/
    /*Per defecte si passem un valor buit a la màscara,farà una cerca per tots els els valor d'aquella camp*/
    /*En aquí hem de fer un set a tots els camps que volem cercar */
    DicomMask mask;
    QString modalityMask;

    //Sempre anem a buscar nivell d'estudi
    mask.setPatientId( m_textPatientID->text() );
    mask.setPatientName( m_textPatientName->text() );
    mask.setStudyId( m_textStudyID->text()  );
    mask.setStudyDate( buildStudyDates() );
    mask.setStudyDescription( "" );
    mask.setStudyTime( m_textStudyTime->text() );
    mask.setStudyUID( m_textStudyUID->text() );
    mask.setInstitutionName( "" );
    mask.setStudyModality( m_textStudyModality->text() );
    mask.setPatientAge( "" );
    mask.setAccessionNumber( m_textAccessionNumber->text() );
    mask.setReferringPhysiciansName( m_textReferringPhysiciansName->text() );
    mask.setPatientSex( "" );
    mask.setPatientBirth( "" );

    //si hem de filtrar per un camp a nivell d'imatge o serie activem els filtres de serie
    if (!m_textSeriesUID->text().isEmpty() || !m_textScheduledProcedureStepID->text().isEmpty() ||
        !m_textRequestedProcedureID->text().isEmpty() || !m_checkAll->isChecked() ||
        !m_textSOPInstanceUID->text().isEmpty() || !m_textInstanceNumber->text().isEmpty() ||
        !m_textPPStartDate->text().isEmpty() || !m_textPPStartTime->text().isEmpty() ||
        !m_textSeriesNumber->text().isEmpty()
       )
    {
        mask.setSeriesDate( "" );
        mask.setSeriesTime( "" );
        mask.setSeriesModality( "" );
        mask.setSeriesNumber( m_textSeriesNumber->text() );
        mask.setSeriesBodyPartExaminated( "" );
        mask.setSeriesUID( m_textSeriesUID->text() );
        mask.setRequestAttributeSequence( m_textRequestedProcedureID->text() , m_textScheduledProcedureStepID->text() );
        mask.setPPSStartDate( m_textPPStartDate->text() );
        mask.setPPStartTime( m_textPPStartTime->text() );

        if ( m_buttonGroupModality->isEnabled() )
        { //es crea una sentencia per poder fer un in
            if ( m_checkCT->isChecked() )
            {
                mask.setSeriesModality( "CT" );
            }
            else if ( m_checkCR->isChecked() )
            {
                mask.setSeriesModality( "CR" );
            }
            else if ( m_checkDX->isChecked() )
            {
                mask.setSeriesModality( "DX" );
            }
            else if ( m_checkES->isChecked() )
            {
                mask.setSeriesModality( "ES" );
            }
            else if ( m_checkMG->isChecked() )
            {
                mask.setSeriesModality( "MG" );
            }
            else if ( m_checkMR->isChecked() )
            {
                mask.setSeriesModality( "MR" );
            }
            else if ( m_checkNM->isChecked() )
            {
                mask.setSeriesModality( "NM" );
            }
            else if ( m_checkDT->isChecked() )
            {
                mask.setSeriesModality( "DT" );
            }
            else if ( m_checkPT->isChecked() )
            {
                mask.setSeriesModality(  "PT" );
            }
            else if ( m_checkRF->isChecked() )
            {
                mask.setSeriesModality(  "RF" );
            }
            else if ( m_checkSC->isChecked() )
            {
                mask.setSeriesModality(  "SC" );
            }
            else if ( m_checkUS->isChecked() )
            {
                mask.setSeriesModality(  "US" );
            }
            else if ( m_checkXA->isChecked() )
            {
                mask.setSeriesModality(  "XA" );
            }
            else if ( m_checkOtherModality->isChecked() )
            {
                mask.setSeriesModality( m_textOtherModality->text() );
            }
        }

        if ( !m_textSOPInstanceUID->text().isEmpty() || !m_textInstanceNumber->text().isEmpty() )
        {
            mask.setImageNumber( m_textInstanceNumber->text() );
            mask.setSOPInstanceUID( m_textSOPInstanceUID->text() );
        }

    }

    return mask;
}

void QueryScreen::addModalityStudyMask( DicomMask* mask, QString modality )
{
    QString studyModalities;

    if ( mask->getStudyModality().length() > 0 ) // ja hi ha una altra modalitat
    {
        studyModalities.insert( 0 , mask->getStudyModality() );
        studyModalities.append( "," );
        studyModalities.append( modality );
    }
    else studyModalities.insert( 0 , modality );

    mask->setStudyModality( studyModalities );
}

QString QueryScreen::logQueryStudy()
{
	QString logMessage;

    logMessage.insert( 0 , m_textPatientID->text() );
    logMessage.append(  ";" );
    logMessage.append(  m_textPatientName->text() );
    logMessage.append(  ";" );
    logMessage.append(  m_textStudyID->text() );
    logMessage.append(  ";" );
    logMessage.append(  buildStudyDates() );
    logMessage.append(  ";" );
    logMessage.append(  m_textAccessionNumber->text() );
    logMessage.append(  ";" );

    return logMessage;
}

void QueryScreen::databaseError(Status *state)
{
    if( !state->good() )
    {
        QString message;
        QString errorCode = tr("Error Number : %1").arg( state->code() );
        switch(state->code())
        {
            case 2001:
                message = tr("Database is corrupted or SQL syntax error\n") + errorCode;
                break;
            case 2005:
                message = tr("Database is locked\nRestart the user session to solve this error\n") + errorCode;
                break;
            case 2011:
                message = tr("Database is corrupted\n") + errorCode;
                break;
            case 2019:
                message = tr("Duplicated register\n") + errorCode;
                break;
            case 2050:
                message = tr("Not Connected to the database\n") + errorCode;
                break;
            default:
                message = tr("Internal Database error\n") + errorCode;
                break;
        }
        QMessageBox::critical( this, tr("Starviewer"), message );
    }
}

};

