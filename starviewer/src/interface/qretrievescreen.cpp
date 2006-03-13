/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/ 
#include "qretrievescreen.h"
#include <q3listview.h>
#include <QString>
#include <iostream.h>
#include <qdatetime.h>
#include <QString>
#include "processimagesingleton.h"
//#include <qobject.h>
//#include <qwidget.h>


namespace udg {

QRetrieveScreen::QRetrieveScreen( QWidget *parent )
 : QDialog( parent )
{
    setupUi( this );
    listRetrieveStudy->setColumnWidth(9,0);//Conte l'UID de l'estudi
    
    for (int i=0;i<10;i++)
    {
        listRetrieveStudy->setColumnWidthMode(i,Q3ListView::Manual);
    }
    
    int init_value=1;//Només un thread alhora pot actualitzar la llista de descarregues, ja que les XLIB no són multithread,si ens trobem dos threads actualitzan a la vegada, les XLIB donen error
    semafor = (sem_t*)malloc(sizeof(sem_t));
    sem_init(semafor,0,init_value);
}


  

/** Insereix un nou estudi per descarregar
  *            @param study[in]    informació de l'estudi a descarregar
  */
void QRetrieveScreen::insertNewRetrieve(Study *study)
{
    Q3ListViewItem* item = new Q3ListViewItem(listRetrieveStudy);
    QTime time = QTime::currentTime();
    QString name;
    QDate date = QDate::currentDate();
    
    deleteStudy( study->getStudyUID().c_str() ); //si l'estudi ja existeix a la llista l'esborrem
    name.insert(0,study->getPatientName().c_str() );
    name.replace("^"," ");
    
    item->setText(0,"PENDING");
    item->setText(1,"Local");
    item->setText(2,study->getPacsAETitle().c_str() );
    item->setText(3,study->getPatientId().c_str() );
    item->setText(4,name);
    item->setText(5,date.toString("dd/MM/yyyy"));
    item->setText(6,time.toString("hh:mm"));
    item->setText(7,"0");
    item->setText(8,"0");
    item->setText(9,study->getStudyUID().c_str() );
    item->setText(10,"Started");

}

/** Neteja la llista d'estudis excepte dels que s'estant descarregant en aquells moments 
  */
void QRetrieveScreen::clearList()
{

    Q3ListViewItemIterator it(listRetrieveStudy);
    QString text("PENDING");
    
    while (it.current())
    {
        if (it.current()->text(0) !=text)
        {    
           listRetrieveStudy->takeItem(it.current());
        }// no avancem si esborrem perquè al esborrar un element l'iterador ja passa a apuntar el següent!
        else it++;
    }
}

/** Esborra l'estudi enviat per parametre
  *    @param UID de l'estudi
  */
void QRetrieveScreen::deleteStudy(QString studyUID)
{
    Q3ListViewItemIterator it(listRetrieveStudy);
    
    while (it.current())
    {
        if (it.current()->text(9) == studyUID)
        {    
           listRetrieveStudy->takeItem(it.current());
        }// no avancem si esborrem perquè al esborrar un element l'iterador ja passa a apuntar el següent!
        else it++;
    }
}

/** Connecta signals d'StarviewerProcessImage amb aquesta classe. Connecta el signal d'imageRetrieved i seriesRetrieved 
  *        @param Objecte StarviewerProcessImage dels que connectarem els signal
  */
void QRetrieveScreen::setConnectSignal(StarviewerProcessImage *process)
{
    connect(process, SIGNAL(imageRetrieved(Image *,int)), this, SLOT(imageRetrieved(Image *,int)));
    connect(process, SIGNAL(seriesRetrieved(QString )), this, SLOT(setSeriesRetrieved(QString )));
}

/** desconnecta els signals imageRetrieved i seriesRetrieved de l'objecte StarviewerProcessImage
  *        @param Objecte StarviewerProcessImage dels que desconnectarem els signal
  */
void QRetrieveScreen::delConnectSignal(StarviewerProcessImage *process)
{
    disconnect(process, SIGNAL(imageRetrieved(Image *,int)), this, 0);
    disconnect(process, SIGNAL(seriesRetrieved(QString)), this, 0);
}

/** slot que s'invoca quant un StarviewerProcessImage emet un signal imageRetrieved
  *        @param imgatge descarregada
  *        @param número d'imatges descarregades
  */
void QRetrieveScreen::imageRetrieved(Image *img,int downloadedImages)
{
    
    Q3ListViewItemIterator it(listRetrieveStudy);
    QString studyUID( img->getStudyUID().c_str() ),Images;
    
    image++;
    
    while (it.current())
    {
        if (it.current()->text(9) != studyUID)
        {
            it++;
        }
        else break;
    }
    

    if (it.current()->text(9) == studyUID)
    {
        Images.setNum(downloadedImages,10);
        it.current()->setText(8,Images);
    }
    

}

/** Augmenta en un el nombre de series descarregades
  *        @param UID de l'estudi que s'ha descarregat una sèrie
  */
void QRetrieveScreen::setSeriesRetrieved(QString studyUID)
{
    
    Q3ListViewItemIterator it(listRetrieveStudy);
    QString series;
    int nSeries;
    bool ok;
    
    image++;
    
    while (it.current())
    {
        if (it.current()->text(9) != studyUID)
        {
            it++;
        }
        else break;
    }
    

    if (it.current()->text(9) == studyUID)
    {
        nSeries = it.current()->text(7).toInt(&ok,10) +1;
        
        series.setNum(nSeries,10);
        it.current()->setText(7,series);
    }
    

}

/**  S'invoca quant s'ha acabat de baixa un estudi. Aquesta accio alhora emiteix un signal per indicar que l'estudi s'ha acabat des descarregar
  *  a la classe queryScreen, ja que des de la queryscreen no ho podem saber quant un estudi s'ha acabat de descarregar
  *     @param  UID de l'estudi descarregat
  */
void QRetrieveScreen::setRetrievedFinished(QString studyUID)
{
    Q3ListViewItemIterator it(listRetrieveStudy);

    //hem de cridar al seriesRetrieved, perquè hem d'indicar que s'ha acabat la descarrega de l'última sèrie, ja que el starviewerprocess no sap quant acaba la descarregar de l'última sèrie
    
    setSeriesRetrieved(studyUID);
    
    while (it.current())
    {
        if (it.current()->text(9) != studyUID)
        {
            it++;
        }
        else break;
    }
    
    
   if (it.current()->text(9) == studyUID)
    {   
        if (it.current()->text(8) == "0") //si el número d'imatges descarregat és 0! error!
        {
            it.current()->setText(0,"ERROR");
        }
        else  it.current()->setText(0,"RETRIEVED"); 
    }
    else cout << studyUID.toStdString()  << " no trobat "<<endl;
    
    emit(studyRetrieved(studyUID));
}

/**  S'invoca quant es produeix algun error al descarregar un estudi
  *     @param studyUID[in] UID de l'estudi descarregat
  */
void QRetrieveScreen::setErrorRetrieving(QString studyUID)
{
    Q3ListViewItemIterator it(listRetrieveStudy);

    while (it.current())
    {
        if (it.current()->text(9) != studyUID)
        {
            it++;
        }
        else break;
    }
        
   if (it.current()->text(9) == studyUID)
    {   
        it.current()->setText(0,"ERROR");
    }
    else cout << studyUID.toStdString() << " no trobat "<<endl;
    
}

QRetrieveScreen::~QRetrieveScreen()
{
}


};
