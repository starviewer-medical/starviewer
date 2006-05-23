/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/ 
#ifndef UDGQUERYSCREEN_H
#define UDGQUERYSCREEN_H

#include "ui_queryscreenbase.h"

#include "qretrievescreen.h"
#include "multiplequerystudy.h"
#include "processimagesingleton.h"
#include "studyvolum.h"
#include "serieslistsingleton.h"
#include "qexecuteoperationthread.h"

namespace udg {

/** Aquesta classe crea la interfície princial de cerca, i connecta amb el PACS i la bd dades local per donar els resultats finals
@author marc
*/

class SeriesList;
class Status;

class QueryScreen : public QWidget , private Ui::QueryScreenBase{
Q_OBJECT
public:
    QueryScreen( QWidget *parent = 0 );

    ~QueryScreen();

public slots:
    void clearTexts();
    
    void searchToday();
    void searchYesterday();
    void search();
    
    void searchSeries(QString,QString);
    void tabChanged(int index);
    void retrieve();

    void showRetrieveScreen();
    void view();
    void config();
    void showPacsList();
    
    void deleteStudyCache();
    void studyRetrievedView(QString);   
    
    void setCheckAll();
    
    void dateFromChanged(const QDate &);
    
    void setEnabledTextTo(int);
    void setEnabledTextFrom(int);
    
    
    void clearCheckedModality();
   
    void notEnoughFreeSpace();
    void errorFreeingCacheSpace();

    void errorConnectingPacs( int );

signals :
    void viewStudy(StudyVolum );
    void clearSeriesListWidget();

    void prova();

protected :

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
    QRetrieveScreen *m_retrieveScreen;
    MultipleQueryStudy multipleQueryStudy;//Ha de ser global, sino l'objecte es destrueix i QT no té temps d'atendre els signals dels threads  
    QExecuteOperationThread m_qexecuteOperationThread;
    
    //StudyVolum m_volum;
    bool m_PacsListShow;
    

    void setEnabledModalityChecks(bool);
    
    void connectSignalsAndSlots();    
    void setEnabledDates(bool);

    SeriesMask buildSeriesMask(QString);
    bool validateNoEmptyMask();
        
    StudyMask buildMask();
    QString buildPatientName();
    QString buildStudyDates();
    void retrievePacs(bool view);
    void retrieveCache(QString StudyUID,QString SeriesUID);
    
    Status insertStudyCache(Study);
    
    void QListView_InsertStudy();
    void queryStudyPacs();
    void queryStudyCache();
    void QuerySeriesPacs(QString,QString,bool);
    void QuerySeriesCache(QString);
    void deleteOldStudies();
    
    //ERRORS
    void databaseError(Status *state);
    static void *retrieveImages(void *);
    void centerWindow();
    
    void resizePacsList();

	
	/** Fa el log, indicant amb quins parametres es cerquen els estudis
     * @return retorna un QString indicant amb quins paràmetres es fa la cerca d'estudis	
     */
	QString logQueryStudy();
};

};

#endif
