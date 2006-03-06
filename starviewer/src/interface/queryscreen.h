/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/ 
#ifndef UDGQUERYSCREEN_H
#define UDGQUERYSCREEN_H

#include "queryscreenbase.h"
#include "qretrievescreen.h"

#include "mulquerystudy.h"
#include "processimagesingleton.h"
#include "studyvolum.h"
#include "serieslistsingleton.h"
#include "retrievethreadslist.h"


//#include "databaseconnection.h"
//#include "studymask.h"
//#include "seriesmask.h"
//#include "pacsparameters.h"
//#include "pacsconnection.h"

namespace udg {

/** Aquesta classe crea la interfície princial de cerca, i connecta amb el PACS i la bd dades local per donar els resultats finals
@author marc
*/

class SeriesList;
class Status;

class QueryScreen : public QueryScreenBase
{
Q_OBJECT
public:
    QueryScreen(QWidget *parent = 0, const char *name = 0);

    ~QueryScreen();

public slots:
    void clearTexts();
    
    void setEnabledTextTo();
    void setEnabledTextFrom();
    void setEnabledModalityChecks(bool);
    void clearCheckedModality();
    
    void searchToday();
    void searchYesterday();
    void search();
    
    void searchSeries(QString,QString);
    void tabChanged(QWidget *);
    void retrieve();

    void temporal();
    void view();
    void config();
    void showPacsList();
    
    void deleteStudyCache();
    void studyRetrieved(QString);   
    
    void setCheckAll();
    
    void dateFromChanged(const QDate &);
   


signals :
    void viewStudy(StudyVolum );

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
    RetrieveThreadsList *m_threadsList;
    //StudyVolum m_volum;
    bool m_PacsListShowed;
    
    
    void setEnabledDates(bool);

    SeriesMask buildSeriesMask(QString);
    bool validateNoEmptyMask();
        
    StudyMask buildMask();
    QString buildPatientName();
    QString buildStudyDates();
    void retrievePacs(bool view);
    void retrieveCache(QString StudyUID,QString SeriesUID);
    
    bool insertStudyCache(Study);
    bool insertSeriesCache(QString StudyUID);
    
    void QListView_InsertStudy();
    void queryStudyPacs();
    void queryStudyCache();
    void QuerySeriesPacs(QString,QString,bool);
    void QuerySeriesCache(QString);
    
    //ERRORS
    void retrieveErrorInsertStudy(Status *state);
    void retrieveErrorInsertSeries(Status *state);
    void queryCacheError(Status *state);
    void databaseError(Status *state);
    static void *retrieveImages(void *);
    void centerWindow();
    
};

};

#endif
