/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/ 
#include "queryscreen.h"
#include <QString>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qdatetime.h>

//#include <qlistview.h>
#include <qmessagebox.h>
#include <qnamespace.h>
#include <qcursor.h>
#include <qtabwidget.h>
#include <qpushbutton.h>
//Added by qt3to4:
#include <QCloseEvent>
#include <string.h>

#include "starviewerprocessimage.h"
#include "processimagesingleton.h"

#include "pacsparameters.h"
#include "pacsconnection.h"
#include "multiplequerystudy.h"
#include "studylist.h"
#include "qstudytreewidget.h"
#include "series.h"
#include "seriesmask.h"
#include "queryseries.h"
#include "pacsparameters.h"
#include "pacsserver.h"
#include "imagemask.h"
#include "queryimagenumber.h"
#include "qserieslistwidget.h"
#include "retrieveimages.h"
#include "cachepacs.h"
#include "qconfigurationscreen.h"
#include "pacslist.h"
#include "qpacslist.h"
#include "constkey.h"
#include "starviewersettings.h"
#include "cachepool.h"
#include "scalestudy.h"
#include <qdesktopwidget.h>
#include <qapplication.h>
#include "qnavigatewindow.h"
#include "queueoperationlist.h"
#include "operation.h"

namespace udg {

/**Constuctor de la classe
 * @param parent 
 * @return 
 */
QueryScreen::QueryScreen( QWidget *parent )
 : QWidget(parent )
{
    setupUi( this );

    Status state;
    QString path;
    StarviewerSettings settings;
           
    CachePacs * localCache= CachePacs::getCachePacs();
    m_retrieveScreen = new udg::QRetrieveScreen;
    
    //connectem signals i slots
    connectSignalsAndSlots();
    setEnabledModalityChecks(true);
    
    state = localCache->delNotRetrievedStudies();//Esborrem els estudis en estat 'PENDING' o 'RETRIEVING'
    if (!state.good()) 
    {
        databaseError(&state);
    }
    
    //indiquem que la llista de Pacs no es mostra
    m_PacsListShow = false;
        
    //carreguem el processImageSingleton    
    m_piSingleton=ProcessImageSingleton::getProcessImageSingleton();
    m_piSingleton->setPath( settings.getCacheImagePath().toStdString() );

    //Instanciem els llistats
    m_seriesListSingleton = SeriesListSingleton::getSeriesListSingleton();
    m_studyListSingleton = StudyListSingleton::getStudyListSingleton();
   
    qPacsList->setMaximumSize(1,1);//amaguem al finestra del QPacsList
    centerWindow(); //centrem la finestra
}

/**
 * 
 */
void QueryScreen::connectSignalsAndSlots()
{
    //connectem els butons
    connect(m_buttonConfig, SIGNAL(clicked()), this, SLOT(config()));
    connect(m_buttonSearch, SIGNAL(clicked()), this, SLOT(search()));
    connect(m_buttonToday, SIGNAL(clicked()), this, SLOT(searchToday()));
    connect(m_buttonYesterday, SIGNAL(clicked()), this, SLOT(searchYesterday()));
    connect(m_buttonClear, SIGNAL(clicked()), this, SLOT(clearTexts()));
    connect(m_buttonRetrieve, SIGNAL(clicked()), this, SLOT(retrieve()));
    connect(m_buttonRetrieveList, SIGNAL(clicked()), this, SLOT(showRetrieveScreen()));
    connect(m_buttonShowPacsList, SIGNAL(clicked()), this, SLOT(showPacsList()));
    connect(m_buttonView, SIGNAL(clicked()), this, SLOT(view()));
    
    //connectem Slots dels StudyList amb la interficie
    connect(m_studyTreeWidgetPacs, SIGNAL(expand(QString,QString)), this, SLOT(searchSeries(QString,QString)));
    connect(m_studyTreeWidgetCache, SIGNAL(expand(QString,QString)), this, SLOT(searchSeries(QString,QString)));
    
    //es canvia de pestanya del TAB
    connect(m_tab, SIGNAL(currentChanged(int)),this, SLOT(tabChanged(int)));
    
    //connectem els checkbox de les dates
    connect(m_checkFrom, SIGNAL(stateChanged(int)), this, SLOT(setEnabledTextFrom(int)));
    connect(m_checkTo, SIGNAL(stateChanged(int)), this, SLOT(setEnabledTextTo(int)));
    
    //lineedit al fer entrer que cerquin    
    connect(m_textPatientID, SIGNAL(returnPressed()), this, SLOT(search()));
    connect(m_textLastName, SIGNAL(returnPressed()), this, SLOT(search()));
    connect(m_textFirstName, SIGNAL(returnPressed()), this, SLOT(search()));
    connect(m_textStudyID, SIGNAL(returnPressed()), this, SLOT(search()));
    connect(m_textAccessionNumber, SIGNAL(returnPressed()), this, SLOT(search()));
    
    //checkbox
    connect(m_checkAll, SIGNAL(clicked()),this, SLOT(clearCheckedModality()));
    connect(m_checkCT, SIGNAL(clicked()),this, SLOT(setCheckAll()));
    connect(m_checkCR, SIGNAL(clicked()),this, SLOT(setCheckAll()));
    connect(m_checkDX, SIGNAL(clicked()),this, SLOT(setCheckAll()));
    connect(m_checkES, SIGNAL(clicked()),this, SLOT(setCheckAll()));
    connect(m_checkMG, SIGNAL(clicked()),this, SLOT(setCheckAll()));
    connect(m_checkMR, SIGNAL(clicked()),this, SLOT(setCheckAll()));
    connect(m_checkNM, SIGNAL(clicked()),this, SLOT(setCheckAll()));
    connect(m_checkDT, SIGNAL(clicked()),this, SLOT(setCheckAll()));
    connect(m_checkPT, SIGNAL(clicked()),this, SLOT(setCheckAll()));
    connect(m_checkRF, SIGNAL(clicked()),this, SLOT(setCheckAll()));
    connect(m_checkSC, SIGNAL(clicked()),this, SLOT(setCheckAll()));
    connect(m_checkUS, SIGNAL(clicked()),this, SLOT(setCheckAll()));
    connect(m_checkXA, SIGNAL(clicked()),this, SLOT(setCheckAll()));
    
    //conectem els sinals dels TreeView
    
    connect(m_studyTreeWidgetCache, SIGNAL(delStudy()),this, SLOT(deleteStudyCache()));
    connect(m_studyTreeWidgetCache, SIGNAL(view()), this, SLOT(view()));
    
    
    //Aquest connect s'utilitzarà perque els threads puguin avisar que han finalitzat de descarregar les imatges   
    connect(m_retrieveScreen, SIGNAL(studyRetrieved(QString)), this, SLOT(studyRetrieved(QString)));
    
    //connectem els QStudyTreeWidget amb els QSeriesListWidget, per poder mantenir la informació de les series actualitzada!
    connect(m_studyTreeWidgetPacs,SIGNAL(addSeries(Series * )),m_seriesListWidgetPacs,SLOT(addSeries(Series *)));
    connect(m_studyTreeWidgetPacs,SIGNAL(clearSeriesListWidget()),m_seriesListWidgetPacs,SLOT(clearSeriesListWidget()));
    connect(m_seriesListWidgetPacs,SIGNAL(selectedSeriesIcon(QString)),m_studyTreeWidgetPacs,SLOT(selectedSeriesIcon(QString))); 
    connect(m_seriesListWidgetPacs,SIGNAL(viewSeriesIcon()),m_studyTreeWidgetPacs,SLOT(viewStudy())); 
    connect(m_studyTreeWidgetPacs,SIGNAL(selectedSeriesList(QString)),m_seriesListWidgetPacs,SLOT(selectedSeriesList(QString))); 
    
    //connectem els signes del SeriesIconView StudyListView
    connect(m_studyTreeWidgetCache,SIGNAL(addSeries(Series * )),m_seriesListWidgetCache,SLOT(addSeries(Series *)));
    connect(m_studyTreeWidgetCache,SIGNAL(clearSeriesListWidget()),m_seriesListWidgetCache,SLOT(clearSeriesListWidget()));    
    connect(m_seriesListWidgetCache,SIGNAL(selectedSeriesIcon(QString)),m_studyTreeWidgetCache,SLOT(selectedSeriesIcon(QString))); 
    connect(m_seriesListWidgetCache,SIGNAL(viewSeriesIcon()),m_studyTreeWidgetCache,SLOT(viewStudy())); 
    connect(m_studyTreeWidgetCache,SIGNAL(selectedSeriesList(QString)),m_seriesListWidgetCache,SLOT(selectedSeriesList(QString))); 
    
    //per netejar la QSeriesIconView quant s'esborrar un estudi
    connect(this,SIGNAL(clearSeriesListWidget()),m_seriesListWidgetCache,SLOT(clearSeriesListWidget()));
    
    //per poder descarregar i veure un estudi amb el menu contextual dels del QStudyList del PACS
    connect(m_studyTreeWidgetPacs,SIGNAL(view()), this,SLOT(view()));
    connect(m_studyTreeWidgetPacs, SIGNAL(retrieve()), this, SLOT(retrieve()));
    
    //connecta el signal que emiteix qexecuteoperationthread, per visualitzar un estudi amb aquesta classe
     QObject::connect(&m_qexecuteOperationThread,SIGNAL(viewStudy(QString)),this,SLOT(studyRetrievedView(QString)),Qt::QueuedConnection);
    
    //connecta els signals el qexecute operation thread amb els de qretrievescreen, per coneixer quant s'ha descarregat una imatge, serie, estudi, si hi ha error, etc..
    connect(&m_qexecuteOperationThread , SIGNAL( setErrorRetrieving( QString ) ) , m_retrieveScreen, SLOT( setErrorRetrieving( QString ) ));
    connect(&m_qexecuteOperationThread , SIGNAL( setStudyRetrieved( QString ) ) , m_retrieveScreen, SLOT( setRetrievedFinished( QString ) ) ); 
    connect(&m_qexecuteOperationThread , SIGNAL( setStudyRetrieving( QString ) ) , m_retrieveScreen, SLOT( setRetrieving( QString ) ));
    connect(&m_qexecuteOperationThread , SIGNAL( imageRetrieved( QString , int) ) , m_retrieveScreen , SLOT( imageRetrieved( QString , int ) ) );
    connect(&m_qexecuteOperationThread , SIGNAL( seriesRetrieved( QString ) ) ,  m_retrieveScreen , SLOT( setSeriesRetrieved( QString ) ) );
    
}

/** Centra la finestra a la pantalla
  */
void QueryScreen::centerWindow()
{

    QDesktopWidget *d = QApplication::desktop();
        
    move((d->width() - frameGeometry().width()) / 2,(d->height() - frameGeometry().height()) / 2);

}

/** Neteja els LineEdit del formulari
  */
void QueryScreen::clearTexts()
{
    
    m_textStudyID->setText("");
    m_textPatientID->setText("");
    m_textFirstName->setText("");
    m_textLastName->setText("");
    m_textAccessionNumber->setText("");
    m_checkTo->setChecked(false);
    m_checkFrom->setChecked(false);
    
    setEnabledTextTo(m_checkTo->isChecked());
    setEnabledTextFrom(m_checkFrom->isChecked());
    
}

/** Activa o desactiva els text i el label de la data fins
  */
void QueryScreen::setEnabledTextTo(int value)
{
    m_textTo->setEnabled(value);
}

/** Activa o desactiva el text de la data desde
  */
void QueryScreen::setEnabledTextFrom(int value)
{
    m_textFrom->setEnabled(value);
}

/** Activa o desactiva els checkbox per buscar per modalitat
  */
void QueryScreen::setEnabledModalityChecks(bool enabled)
{
    m_checkAll->setEnabled(enabled);
    m_checkCR->setEnabled(enabled);
    m_checkCT->setEnabled(enabled);
    m_checkDX->setEnabled(enabled);
    m_checkES->setEnabled(enabled);
    m_checkMG->setEnabled(enabled);
    m_checkMR->setEnabled(enabled);
    m_checkNM->setEnabled(enabled);
    m_checkDT->setEnabled(enabled);
    m_checkPT->setEnabled(enabled);
    m_checkRF->setEnabled(enabled);
    m_checkSC->setEnabled(enabled);
    m_checkUS->setEnabled(enabled);
    m_checkXA->setEnabled(enabled);
    
    m_buttonGroupModality->setEnabled(enabled);
    
    //clearCheckedModality();
}

/** Posa a verdader o fals tots els check modality, i deixa a true el all a true
  */
void QueryScreen::clearCheckedModality()
{
    m_checkAll->setChecked(true);
    m_checkCR->setChecked(false);
    m_checkCT->setChecked(false);
    m_checkDX->setChecked(false);
    m_checkES->setChecked(false);
    m_checkMG->setChecked(false);
    m_checkMR->setChecked(false);
    m_checkNM->setChecked(false);
    m_checkDT->setChecked(false);
    m_checkPT->setChecked(false);
    m_checkRF->setChecked(false);
    m_checkSC->setChecked(false);
    m_checkUS->setChecked(false);
    m_checkXA->setChecked(false);

}

/** Slot que activa o desactiva el m_checkAll en funció de si hi ha alguna modalitat d'estudi seleccionada
  */
void QueryScreen::setCheckAll()
{
    if (m_checkCR->isChecked() ||
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
        m_checkXA->isChecked()   )
     {
         m_checkAll->setChecked(false);
     } 
     else m_checkAll->setChecked(true);
}

/** Quanta la data from canvia, amb aquest slot també es canvia la data del TO
  *        @param data 
  */
void QueryScreen::dateFromChanged(const QDate &data)
{
    m_textTo->setDate(data);
}


/** Activa els labels i text de la data segons el parametre d'entrada
  *                   @param enabled [in]  Indica si s'ha d'activar o desactivar
  */
void QueryScreen::setEnabledDates(bool enabled)
{
    m_textFrom->setEnabled(enabled);
    m_checkFrom->setEnabled(enabled);
    m_checkFrom->setChecked(enabled);
    
    m_textTo->setEnabled(enabled);
    m_checkTo->setEnabled(enabled);
    m_checkTo->setChecked(enabled);
}


/** Fa una cerca dels estudis fets avui
  */
void QueryScreen::searchToday()
{

    QDate today;

    setEnabledDates(true);
        
    m_textTo->setDate(today.currentDate());
    m_textFrom->setDate(today.currentDate());
    search();
}

/** Fa una cerca dels estudis d'ahir
  */
void QueryScreen::searchYesterday()
{
    QDate today;
    QDate yesterday(today.currentDate());
    
    setEnabledDates(true);
    yesterday = yesterday.addDays(-1);//calcula la data d'ahir
    
    m_textFrom->setDate(yesterday);
    m_textTo->setDate(yesterday);
    search();
}

/** Escull a on fer la cerca, si a nivell local o PACS
  */
void QueryScreen::search()
{
    
    this->setCursor(QCursor(Qt::WaitCursor));
    
    if (m_tab->currentIndex()==1)
    {
        //if (!validateNoEmptyMask())
        if (1==2)
        {
            switch( QMessageBox::information( this, tr("Starviewer"),
                                        tr("You have not specified any filter. This query could take a long time. Do you want to continue ?"),
                                        tr("Yes"), tr("No"),
                                        0, 1 ) ) 
            {
                case 0:  queryStudyPacs();
            }    
        }
        else queryStudyPacs();

    }
    else
    {
        queryStudyCache();
    }
    
    this->setCursor(QCursor(Qt::ArrowCursor));

}


/** valida que la màscara de cerca no estigui buida, en el cas que ho sigui s'haurà d'avisar al usuari, perquè fer una cerca al Pacs sense 
  * filtrar potser molt lenta, al haver de mostrar totes les dades
  *        @return indica si el filtre de cerca està buit
  */
bool QueryScreen::validateNoEmptyMask()
{
    if ( m_textPatientID->text().length() == 0 &&
         m_textFirstName->text().length() == 0 &&
         m_textLastName->text().length() == 0 &&
         m_textStudyID->text().length() == 0 &&
         m_textAccessionNumber->text().length() == 0 &&
         !m_checkFrom->isChecked()  &&
         !m_checkTo->isChecked() )
    {
        return false;
    }
    else return true;
}

/** Fa cerca al pacs seleccionats
  */
/**
 * 
 */
void QueryScreen::queryStudyPacs()
{

    PacsList pacsList;
    PacsParameters pa;
    QString result;
    StarviewerSettings settings;
    
    pacsList.clear(); //netejem el pacsLIST
    qPacsList->getSelectedPacs(&pacsList); //Emplemen el pacsList amb les pacs seleccionats al QPacsList
    
    pacsList.firstPacs();
    
    if (pacsList.end()) //es comprova que hi hagi pacs seleccionats
    {
        QMessageBox::warning( this, tr("StarViewer"),tr("Please select a PACS to query"));
        return;      
    }
    
    
    multipleQueryStudy.setPacsList(pacsList); //indiquem a quins Pacs Cercar
    multipleQueryStudy.setMask(buildMask()); //construim la mascara
    if(!multipleQueryStudy.StartQueries().good())  //fem la query
    {
        m_studyTreeWidgetPacs->clear();
        QMessageBox::information( this, tr("StarViewer"),tr("ERROR QUERING!."));
        return;  
    }
     
      
    m_seriesListWidgetPacs->clear(); //netegem el QSeriesIconView dem_studyListSingleton PACS
    m_studyListSingleton->firstStudy();
       
    if (m_studyListSingleton->end())
    {
        m_studyTreeWidgetPacs->clear();        
        QMessageBox::information( this, tr("StarViewer"),tr("No study match found."));
        return;
    }
    m_studyTreeWidgetPacs->insertStudyList(m_studyListSingleton); //fem que es visualitzi l'studyView seleccionat
    m_studyTreeWidgetPacs->setSortColumn(2);//ordenem pel nom
    
    
}

/** Busca un estudi a la cache local
  */
void QueryScreen::queryStudyCache()
{
    CachePacs * localCache;
    Status state;
    
    localCache = CachePacs::getCachePacs();

    m_seriesListWidgetCache->clear();
    
    m_studyListCache.clear();
    
    state=localCache->queryStudy(buildMask(),m_studyListCache); //busquem els estudis a la cache

    if (!state.good()) 
    {
        m_studyTreeWidgetCache->clear();
        databaseError(&state);
        return;
    }
    
    m_studyListCache.firstStudy();

    if (m_studyListCache.end()) //no hi ha estudis
    {
        m_studyTreeWidgetCache->clear();
        QMessageBox::information( this, tr("StarViewer"),tr("No study match found."));
        return;
    }
    
    m_studyTreeWidgetCache->insertStudyList(&m_studyListCache);//es mostra la llista d'estudis
    
    m_studyTreeWidgetCache->setSortColumn(2); //ordenem pel nom
}

/**Busca la informació d'una sèrie
  *            @param studyUID [in] UID de l'estidi
  *            @param pacsAETItle [in] AEtitle del pacs a buscar la sèrie          
  */
/* AQUESTA ACCIO ES CRIDADA DES DEL STUDYLISTVIEW*/
void QueryScreen::searchSeries(QString studyUID,QString pacsAETitle)
{   
  
    this->setCursor(QCursor(Qt::WaitCursor));
    
    m_seriesListWidgetPacs->clear(); //Netegem el QSeriesIconView del Pacs
    
    if (m_tab->currentIndex() == 1) //si estem la pestanya del PACS fem query al Pacs
    {
        QuerySeriesPacs(studyUID,pacsAETitle,true);
    }
    else if (m_tab->currentIndex() == 0) // si estem a la pestanya de la cache
    {
        QuerySeriesCache(studyUID);
    }

    this->setCursor(QCursor(Qt::ArrowCursor));
}

/**Busca la informació d'una sèrie en el PACS i la mostra en la interfície
  *            @param studyUID [in] UID de l'estidi
  *            @param pacsAETItle [in] AEtitle del pacs a buscar la sèrie    
  *            @param show [in]    Si es verdader mostra les dades de la sèrie per pantalla, pot ser que no les volguem mostrar, per exemple el cas que volem la informació per guardar-la en la caché al descarragar-nos una imatge
  */
void QueryScreen::QuerySeriesPacs(QString studyUID,QString pacsAETitle,bool show)
{
    Series serie;
    ImageMask imagem;
    QString text,timeoutPacs;
    PacsParameters pacs;
    PacsListDB pacsListDB;
    Status state;   
    StarviewerSettings settings;
    int nImages = 0;

    
    state = pacsListDB.queryPacs(&pacs,pacsAETitle.toAscii().constData());//cerquem els paràmetres del Pacs al qual s'han de cercar les dades
    if (!state.good())
    {
        databaseError(&state);
        return;
    }

    pacs.setAELocal( settings.getAETitleMachine().toStdString() ); //especifiquem el nostres AE
    pacs.setTimeOut(settings.getTimeout().toInt(NULL,10)); //li especifiquem el TimeOut

    m_seriesListSingleton->clear();
      
    PacsServer pacsConnection(pacs);
    
    if (!pacsConnection.Connect(PacsServer::query,PacsServer::seriesLevel).good())
    {//Error al connectar
        text.insert(0,tr("Error! connecting to PACS : "));
        text.append(pacsAETitle);
        return;         
    }
    
    QuerySeries querySeries(pacsConnection.getConnection(),buildSeriesMask(studyUID));
    
    if (!querySeries.find().good())
    {//Error a la query
        text.insert(0,tr("Error! Can't query studies in PACS : "));
        text.append(pacsAETitle);
        QMessageBox::warning( this, tr("StarViewer"),text);
        return;         
    }
    
    m_seriesListSingleton->firstSeries();
    if (m_seriesListSingleton->end())
    {
        QMessageBox::information( this, tr("StarViewer"),tr("No series match for this study.\n"));
        return;
    }
    
    m_seriesListSingleton->firstSeries();
    
    if (show)  m_seriesListWidgetPacs->clear();
    
    while (!m_seriesListSingleton->end())
    {
        serie = m_seriesListSingleton->getSeries();
        if (settings.getCountImages())
        {
            //preparem per fer la cerca d'imatges

            imagem.setStudyUID(serie.getStudyUID().c_str());
            imagem.setSeriesUID(serie.getSeriesUID().c_str());
            QueryImageNumber qim(pacsConnection.getConnection(),imagem);
        
            qim.count();//contem el nombre d'imatges
        
            nImages=  qim.getImageNumber();
            serie.setImageNumber(nImages);
        }
        if (show)
        {
            m_studyTreeWidgetPacs->insertSeries(&serie);//inserim la informació de les imatges al formulari
 
        }
        m_seriesListSingleton->nextSeries();
    }
    pacsConnection.Disconnect();
    
}

/** Cerca les sèries d'un estudi a la Cache local
  *               @param StudyUID [in] UID de l'estudi a cercar
  */
void QueryScreen::QuerySeriesCache(QString studyUID)
{
    Series serie;
    ImageMask imageMask;
    CachePacs * localCache = CachePacs::getCachePacs();
    int imagesNumber;
    Status state;
    
    m_seriesListCache.clear();//preparem la llista de series
    
    //preparem la mascara i cerquem les series a la cache
    state=localCache->querySeries(buildSeriesMask(studyUID),m_seriesListCache);
    
    if (!state.good())
    {
        databaseError(&state);
        return;
    }
    
    m_seriesListCache.firstSeries();
    if (m_seriesListCache.end())
    {
        QMessageBox::information( this, tr("StarViewer"),tr("No series match for this study.\n"));
        return;
    }
    
    m_seriesListCache.firstSeries();
    m_seriesListWidgetCache->clear();
    
    while (!m_seriesListCache.end())
    {
        serie= m_seriesListCache.getSeries();
        
        //preparem per fer la cerca d'imatges
        imageMask.setStudyUID(serie.getStudyUID().c_str());
        imageMask.setSeriesUID(serie.getSeriesUID().c_str());
        
        state = localCache->countImageNumber(imageMask,imagesNumber);
        serie.setImageNumber(imagesNumber);
        if (!state.good())
        {
            databaseError(&state);
            return;
        }
        m_studyTreeWidgetCache->insertSeries(&serie);//inserim la informació de les imatges al formulari
        m_seriesListCache.nextSeries();
    }   
}


void QueryScreen::retrieve()
{
    switch( QMessageBox::information( this, tr("Starviewer"),
				      tr("Are you sure you want to retrieve this Study ?"),
				      tr("Yes"), tr("No"),
				      0, 1 ) ) 
    {
    case 0:
        retrievePacs(false);
    }

}
   

/** Aquest mètode s'encarrega de començar la descarrega d'un estudi, estableix la màscara, insereix l'estudi i la sèria a la caché,
  * ademés de crear el thread per continuar amb la descàrrega de l'estdui
  *        @param view [in] boolea que indica si després de la descarrega s'ha de visualitzar l'estudi
  */
    
void QueryScreen::retrievePacs(bool view)
{

    StudyMask mask;
    QString studyUID,defaultSeriesUID;
    Status state;
    Operation operation;
    PacsParameters pacs;      
    PacsListDB pacsListDB; 
    StarviewerSettings settings;
    CachePool *pool = CachePool::getCachePool();
    
    this->setCursor(QCursor(Qt::WaitCursor));
    if (m_studyTreeWidgetPacs->getSelectedStudyUID() == "")
    {
        this->setCursor(QCursor(Qt::ArrowCursor));
        if (view)
        {
            QMessageBox::warning( this, tr("StarViewer"),tr("Select a study to view "));
        }
        else QMessageBox::warning( this, tr("StarViewer"),tr("Select a study to download "));
        return;
    }
    studyUID.insert(0,m_studyTreeWidgetPacs->getSelectedStudyUID().toAscii().constData());
    
    if (pool->getFreeTotalSpace()< 1000) //comprovem que tinguem més 1 GB lliure per poder continuar
    {
        this->setCursor(QCursor(Qt::ArrowCursor));
        QMessageBox::warning( this, tr("StarViewer"),tr("Error disk space under 1 Gb. Plese free spaces in your disk "));
        return;    
    }
    
    //Tenim l'informació de l'estudi a descarregar a la llista d'estudis, el busquem a la llista
    if (!m_studyListSingleton->findStudy(studyUID.toAscii().constData()))
    {
        this->setCursor(QCursor(Qt::ArrowCursor));
        QMessageBox::warning( this, tr("StarViewer"),tr("Internal Error : "));
        return;
    }
   
    
    //Inserim l'informació de l'estudi a la caché!
    if (!insertStudyCache(m_studyListSingleton->getStudy()))
    {
        this->setCursor(QCursor(Qt::ArrowCursor));
        return;
    }   
    
    mask.setStudyUID(studyUID.toAscii().constData());//definim la màscara per descarregar l'estudi
    
    //busquem els paràmetres del pacs del qual volem descarregar l'estudi
    state = pacsListDB.queryPacs(&pacs,m_studyTreeWidgetPacs->getSelectedStudyPacsAETitle().toAscii().constData());
    if (!state.good())
    {
        this->setCursor(QCursor(Qt::ArrowCursor));
        databaseError(&state);
        return;
    }

    //inserim a la pantalla de retrieve que iniciem la descarrega
    m_retrieveScreen->insertNewRetrieve(&m_studyListSingleton->getStudy());      

    //emplanem els parametres amb dades del starviewersettings
    pacs.setAELocal(settings.getAETitleMachine().toAscii().constData());
    pacs.setTimeOut(settings.getTimeout().toInt(NULL,10));
    pacs.setLocalPort(settings.getLocalPort().toAscii().constData());
    
    //definim l'operacio
    operation.setPacsParameters(pacs);
    operation.setStudyMask(mask);
    if (view)
    {
        operation.setOperation(operationView);
    }
    else operation.setOperation(operationRetrieve);
    
    m_qexecuteOperationThread.queueOperation(operation);

    this->setCursor(QCursor(Qt::ArrowCursor));
}

/** Insereix un estudi a descarregar a la cache
  *        @param estudi a insertat
  *        @return retorna si la operacio s'ha realitzat amb èxit
  */
bool QueryScreen::insertStudyCache(Study stu)
{

    CachePacs *localCache = CachePacs::getCachePacs();
    std::string absPath;
    Status state;
    Study study = stu;
    StarviewerSettings settings;
       
    //creem el path absolut de l'estudi
    absPath.insert(0,settings.getCacheImagePath().toAscii().constData());
    absPath.append(study.getStudyUID());
    absPath.append("/");
    study.setAbsPath(absPath);
    //inserim l'estudi a la caché
    state = localCache->insertStudy(&study); 
    if (!state.good())
    {   
        if (state.code() == 2019)
        {
            QMessageBox::warning( this, tr("StarViewer"),tr("The study has been retrieved."));
        }
        else databaseError(&state);
        return false;
    }
   
    return true;
}
 
/** Slot que s'activa per la classe qexecuteoperationthread, que quant un estudi ha estat descarregat el visualitzar, si 
l'usuari així ho ha indicat
  *        @param UID de l'estudi descarregat
  */
void QueryScreen::studyRetrievedView(QString studyUID)
{
    QString series = "";
    
    retrieveCache(studyUID,series);
}

/** Al canviar de pàgina del tab hem de canviar alguns paràmetres, com activar el boto Retrieve, etec..
  *        @param QWidget (no es fa servir)
  */
void QueryScreen::tabChanged(int index)
{

    if (index != 0)
    {
        setEnabledModalityChecks(false);//desactivem el grup button de modalitat
        m_buttonRetrieve->setEnabled(true);//activem el boto retrieve
        m_buttonView->setEnabled(true);
        m_buttonShowPacsList->setEnabled(true);//activem el boto d'ensenyar la llista de pacs
    }
    else 
    {
        setEnabledModalityChecks(true);//activem el grup button de motalitat
        m_buttonRetrieve->setEnabled(false);//desactivem el boto retrieve
        m_buttonView->setEnabled(true);
        m_buttonShowPacsList->setEnabled(false);//activem el boto d'ensenyar la llista de pacs
    }
    
    if ( m_PacsListShow ) resizePacsList();

}

/** Visualitza un estudi, si aquest estudi esta en el pacs el descarrega i posteriorment es visualitza, si es de la cache el carrega
  * a la classe volum i es visualitza
  */
void QueryScreen::view()
{
       
    if (m_tab->currentIndex() == 1)
    {
        switch( QMessageBox::information( this, tr("Starviewer"),
				      tr("Are you sure you want to retrieve this Study ?"),
				      tr("Yes"), tr("No"),
				      0, 1 ) ) 
        {
        case 0:
            retrievePacs(true);
        }
    }
    else if (m_tab->currentIndex() == 0)
    {
        
        retrieveCache(m_studyTreeWidgetCache->getSelectedStudyUID(),m_studyTreeWidgetCache->getSelectedSeriesUID());
    }
    

    
}


/** Carrega un estudi de la cache perque pugui ser visualitzat a la classe Volum i emet una senyal perque sigui visualitzat
  *     @param UID de l'estudi
  *     @param UID de la serie que s'ha de visualitzar per defecte, si es buit, es posara per defecte la primera serie de l'estudi
  */
void QueryScreen::retrieveCache(QString studyUID,QString seriesUID)
{

    CachePacs * localCache = CachePacs::getCachePacs(); 
    Status state;
    StudyList stuList;
    Study stu;
    SeriesMask mask;
    SeriesList seriesList;
    SeriesVolum seriesVol;
    Series series;
    ImageMask imageMask;
    ImageList imageList;
    QString absSeriesPath;
    StarviewerSettings settings;
    StudyVolum volum;
        
    if (studyUID == "")
    {
        this->setCursor(QCursor(Qt::ArrowCursor));
        QMessageBox::warning( this, tr("StarViewer"),tr("Select a study to view "));
        return;
    }    
        
    state = localCache->queryStudy(studyUID.toAscii().constData(),stu); //cerquem la informació de l'estudi
    if (!state.good())
    {   
        databaseError(&state);
        return;
    }
    
    volum.setPatientAge(stu.getPatientAge());
    volum.setPatientId(stu.getPatientId());
    volum.setPatientName(stu.getPatientName());
    volum.setStudyDate(stu.getStudyDate());
    volum.setStudyId(stu.getStudyId());
    volum.setStudyTime(stu.getStudyTime());
    volum.setStudyUID(stu.getStudyUID());    

    mask.setStudyUID(stu.getStudyUID().c_str());

    localCache->querySeries(mask,seriesList);
    if (!state.good())
    {
        databaseError(&state);
        return;
    }
        
    seriesList.firstSeries();
    
    //si es buit indiquem que per defecte es visualitza la primera serie
    if (seriesUID == "")
    {
        volum.setDefaultSeriesUID(seriesList.getSeries().getSeriesUID());
    }
    else volum.setDefaultSeriesUID(seriesUID.toStdString() );
    
    while (!seriesList.end())
    {
        series = seriesList.getSeries();
        
        absSeriesPath = settings.getCacheImagePath();
        absSeriesPath += series.getSeriesPath().c_str();
        seriesVol.setSeriesUID(series.getSeriesUID());
        seriesVol.setStudyId(stu.getStudyId());
        seriesVol.setStudyUID(stu.getStudyUID());
        seriesVol.setSeriesPath( absSeriesPath.toStdString() );
        
        imageMask.setSeriesUID(series.getSeriesUID().c_str());
        imageMask.setStudyUID(stu.getStudyUID().c_str());
        
        imageList.clear();
        state= localCache->queryImages(imageMask,imageList);

        if (!state.good())
        {
            databaseError(&state);
            return;
        }       
  
        imageList.firstImage();
        
        while (!imageList.end())
        {
            seriesVol.addImage(imageList.getImage().getImagePath().c_str());
            imageList.nextImage();
        }
        
        volum.addSeriesVolum(seriesVol);
        seriesList.nextSeries();
    }
    
    localCache->updateStudyAccTime(studyUID.toStdString() );
    
    this->close();//s'amaga per poder visualitzar la serie
    emit(viewStudy(volum));
}

/**slot que s'activa al esborrar estudi de la caché
  */
void QueryScreen::deleteStudyCache()
{
    Status state;
    CachePacs * localCache = CachePacs::getCachePacs();
    QString studyUID;
    
    
    studyUID = m_studyTreeWidgetCache->getSelectedStudyUID();
    
    if (studyUID=="")
    {
        QMessageBox::information( this, tr("StarViewer"),tr("Please select a study to delete"));
        return;    
    }
    
    switch( QMessageBox::information( this, tr("Starviewer"),
				      tr("Are you sure you want to delete this Study ?"),
				      tr("Yes"), tr("No"),
				      0, 1 ) ) 
    {
        case 0:
            
            
            state = localCache->delStudy(studyUID.toAscii().constData());   
            if (state.good())
            {
                m_studyTreeWidgetCache->removeStudy(studyUID);
                emit(clearSeriesListWidget());//Aquest signal es recollit per qSeriesIconView
            }
            else
            {
                databaseError(&state);
            }
    }

}

void QueryScreen::closeEvent( QCloseEvent* ce )
{
	//ce->ignore();
	m_studyTreeWidgetPacs->saveColumnsWidth();
	m_studyTreeWidgetCache->saveColumnsWidth();
        ce->accept();        
}

void QueryScreen::showRetrieveScreen()
{
    //el ActiveWindow no funciona, no enfoca la finestra el setWindowState tampoc, és un bug de QT ? a la docu posa que en certes ocasions el Qt::WindowActive pot ser ignorat! Per aixo s'ha de tornar la finestra invisble i tornar-la a fer visible per visualitzar-la, sinó no s'enfoca la finestra
    m_retrieveScreen->setVisible(false);
    m_retrieveScreen->setVisible(true);
}


void QueryScreen::config()
{
    udg::QConfigurationScreen *configScreen = new udg::QConfigurationScreen;
    
//    configScreen()->setModal(true);
    configScreen->setModal(true);
    configScreen->show();
}

/**  Modifica la interfície per mostar el QPacsList
  */
void QueryScreen::showPacsList()
{
        
    if (!m_PacsListShow)
    {   
        m_PacsListShow = true;
        m_buttonShowPacsList->setText(tr("Hide Pacs List"));
    }
    else
    {
        m_PacsListShow = false;
    }
    
    resizePacsList();
}

/** Assigna la mida al PacsList en funcio del tab en que es trobi i de la variable m_PacsListShow
  */
void QueryScreen::resizePacsList()
{
    int mida;
    
    //si es fals i estem al tab del Pacs s'ha de mostrar la llistat de PACS
    if ( m_PacsListShow && m_tab->currentWidget()->objectName() == "m_tabPacs" )
    {   qPacsList->setFixedSize(200,240);
        mida = 200 + 20;
    }
    else
    {
        qPacsList->setFixedSize(1,1);
        mida = -( 200 + 20);
    }
    
   this->resize(this->width() + mida,this->height());
   
}

/** Construeix la màscara de cerca de la sèrie
  *             @param UID de l'estudi
  *             @return màscara
  */
SeriesMask QueryScreen::buildSeriesMask(QString studyUID)
{
    SeriesMask mask;

    mask.setStudyUID(studyUID.toAscii().constData());
    mask.setSeriesDate(NULL);
    mask.setSeriesTime(NULL);
    mask.setSeriesModality(NULL);
    mask.setSeriesNumber(NULL);
    mask.setSeriesBodyPartExaminated(NULL);
    mask.setSeriesUID(NULL);
    
    return mask;
}


/** Contrueix el nom del pacient per a crear la màscara, el format del la màscara de pacient ha de ser "*" o "congoms* Nom*"
  *        @return retorna la màscara amb el nom del pacient
  */
QString QueryScreen::buildPatientName()
{

     QString patientName;
    
     
    if (m_textFirstName->text().length() == 0 && m_textLastName->text().length() == 0)
    {
        patientName.append("*");
    }    
    else
    {
        patientName.append(m_textLastName->text());
        //posem * per si només posem el primer cognom
        patientName.append("* ");//posem * per si no posa el nom complet, i espai per separar del nom
        if (m_textFirstName->text().length() > 0)
        {
            patientName.append(m_textFirstName->text());
            patientName.append("*");//per si no posa el nom complet
        }
        else patientName.append("*");
    }
    
    return patientName;

}

/** construeix la màscara de les dates
  *        @return retorna la màscara de les dates
  */
QString QueryScreen::buildStudyDates()
{
    QString date;
    
    
    if (m_checkFrom->isChecked() && m_checkTo->isChecked())
    {
        date.append(m_textFrom->date().toString("yyyyMMdd"));
        date.append("-");
        date.append(m_textTo->date().toString("yyyyMMdd"));
    }
    else 
    {
        if (m_checkFrom->isChecked())
        {
            date.append(m_textFrom->date().toString("yyyyMMdd"));
            date.append("-"); // indiquem que volem buscar tots els estudis d'aquella data en endavant 
        }
        else if (m_checkTo->isChecked())
        {
            date.append("-"); //indiquem que volem buscar tots els estudis que no superin aquesta data
            date.append(m_textTo->date().toString("yyyyMMdd"));
        }
    }
    
    return date;
}

/** Construeix la màscara d'entrada pels estudis
  *        @return retorna la màscara d'un estudi
  */
StudyMask QueryScreen::buildMask()
{
    /*Per fer cerques entre valors consultat el capítol 4 de DICOM punt C.2.2.2.5*/
    /*Per defecte si passem un valor null o buit a la màscara,farà una cerca per tots els els valor d'aquella camp*/    
    /*En aquí hem de fer un set a tots els camps que volem cercar */
    StudyMask mask;
    QString modalityMask;
    
    mask.setPatientId(m_textPatientID->text().toStdString() );
    mask.setPatientName(buildPatientName().toStdString() );
    mask.setStudyId(m_textStudyID->text().toStdString() );
    mask.setStudyDate(buildStudyDates().toStdString() );
    mask.setStudyDescription("");
    mask.setStudyTime("");
    mask.setStudyUID("");
    mask.setInstitutionName("");
    mask.setStudyModality("");
    mask.setPatientAge("");
    mask.setAccessionNumber( m_textAccessionNumber->text().toStdString() );
    
    /*Aquesta mascara només serveix per la caché, no serveix pel pacs!!!!!!!!!!!!!!!!!!!1*/
    if (!m_checkAll->isChecked())
    { //es crea una sentencia per poder fer un in       
        if (m_checkCT->isChecked())
        {
            modalityMask.append(",'CT'");
        }
        if (m_checkCR->isChecked())
        {
            modalityMask.append(",'CR'");
        }        
        if (m_checkDX->isChecked())
        {
            modalityMask.append(",'DX'");
        }        
        if (m_checkES->isChecked())
        {
            modalityMask.append(",'ES'");
        }        
        if (m_checkMG->isChecked())
        {
            modalityMask.append(",'MG'");
        }        
        if (m_checkMR->isChecked())
        {
            modalityMask.append(",'MR'");
        }        
        if (m_checkNM->isChecked())
        {
            modalityMask.append(",'NM'");
        }        
        if (m_checkDT->isChecked())
        {
            modalityMask.append(",'DT'");
        }        
        if (m_checkPT->isChecked())
        {
            modalityMask.append(",'PT'");
        }        
        if (m_checkRF->isChecked())
        {
            modalityMask.append(",'RF'");
        }
        if (m_checkSC->isChecked())
        {
            modalityMask.append(",'SC'");
        }        
        if (m_checkUS->isChecked())
        {
            modalityMask.append(",'US'");
        }        
        if (m_checkXA->isChecked())
        {
            modalityMask.append(",'XA'");
        }        
        if  (modalityMask.length()> 0) 
        {
            modalityMask = modalityMask.replace(0,1,"(");
            modalityMask.append(")");
            mask.setStudyModality(modalityMask.toAscii().constData());
        }
    }
    
    return mask;
}

QueryScreen::~QueryScreen()
{

}

};

