/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/ 

#include "qretrievescreen.h"
#include <QString>
#include <iostream.h>
#include <qdatetime.h>
#include "processimagesingleton.h"


namespace udg {

/**
 * 
 * @param parent 
 * @return 
 */
QRetrieveScreen::QRetrieveScreen( QWidget *parent )
 : QDialog( parent )
{
    setupUi( this );
    m_treeRetrieveStudy->setColumnHidden(9,true);//Conte l'UID de l'estudi
    
    createConnections();
    
}

/** Crea les connexions pels signals i slots
  */
void QRetrieveScreen::createConnections()
{
    connect(m_buttonClear, SIGNAL(clicked()), this, SLOT(clearList()));
}

/** Insereix un nou estudi per descarregar
  *            @param study[in]    informació de l'estudi a descarregar
  */
void QRetrieveScreen::insertNewRetrieve(Study *study)
{
    QTreeWidgetItem* item = new QTreeWidgetItem(m_treeRetrieveStudy);
    QTime time = QTime::currentTime();
    QString name;
    QDate date = QDate::currentDate();
    
    deleteStudy( study->getStudyUID().c_str() ); //si l'estudi ja existeix a la llista l'esborrem
    name.insert(0,study->getPatientName().c_str() );
    name.replace("^"," ");
    
    item->setText(0,tr("PENDING"));
    item->setText(1,tr("Local"));
    item->setText(2,study->getPacsAETitle().c_str() );
    item->setText(3,study->getPatientId().c_str() );
    item->setText(4,name);
    item->setText(5,date.toString("dd/MM/yyyy"));
    item->setText(6,time.toString("hh:mm"));
    item->setText(7,"0");
    item->setText(8,"0");
    item->setText(9,study->getStudyUID().c_str() );
    item->setText(10,tr("Started"));

}

/** Neteja la llista d'estudis excepte dels que s'estant descarregant en aquells moments 
  */
void QRetrieveScreen::clearList()
{
    QList<QTreeWidgetItem *> qRetrieveList(m_treeRetrieveStudy->findItems("*",Qt::MatchWildcard,0));
    QTreeWidgetItem *item;
    
    for (int i=0;i<qRetrieveList.count();i++)
    {
        item = qRetrieveList.at(i);
        if (item->text(0) != tr("PENDING") && item->text(0) != tr("RETRIEVING"))
        {
            delete item;
        }
    }
}

/** Esborra l'estudi enviat per parametre
  *    @param UID de l'estudi
  */
void QRetrieveScreen::deleteStudy(QString studyUID)
{
    QList<QTreeWidgetItem *> qRetrieveList(m_treeRetrieveStudy->findItems(studyUID,Qt::MatchExactly,9));
    QTreeWidgetItem *item;
    int i = 0;
    
    while (i < qRetrieveList.count())
    {
        item = qRetrieveList.at(i);
        if (item->text(9) == studyUID)
        {
            delete item;
            break;
        }
        i++;
    }
}

/** Connecta signals d'StarviewerProcessImage amb aquesta classe. Connecta el signal d'imageRetrieved i seriesRetrieved 
  *        @param Objecte StarviewerProcessImage dels que connectarem els signal
  */
void QRetrieveScreen::setConnectSignal(StarviewerProcessImage *process)
{
    connect(process, SIGNAL(imageRetrieved(QString,int)), this, SLOT(imageRetrieved(QString,int)),Qt::QueuedConnection);
    connect(process, SIGNAL(seriesRetrieved(QString )), this, SLOT(setSeriesRetrieved(QString )),Qt::QueuedConnection);
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
  *        @param uid de l'estudi descarregat de l'imatge
  *        @param número d'imatges descarregades
  */
void QRetrieveScreen::imageRetrieved(QString studyUID,int downloadedImages)
{
    
    QString Images;
    QList<QTreeWidgetItem *> qRetrieveList(m_treeRetrieveStudy->findItems(studyUID,Qt::MatchExactly,9));
    QTreeWidgetItem *item;

    if  (!qRetrieveList.isEmpty())
    {
        item = qRetrieveList.at(0);
        Images.setNum(downloadedImages,10);
        item->setText(8,Images);
    }    

}

/** Augmenta en un el nombre de series descarregades
  *        @param UID de l'estudi que s'ha descarregat una sèrie
  */
void QRetrieveScreen::setSeriesRetrieved(QString studyUID)
{
    
    QList<QTreeWidgetItem *> qRetrieveList(m_treeRetrieveStudy->findItems(studyUID,Qt::MatchExactly,9));
    QTreeWidgetItem *item;
    QString series;
    int nSeries = 0;
    bool ok;
    
    if ( !qRetrieveList.isEmpty())
    {
        item = qRetrieveList.at(0);
        nSeries = item->text(7).toInt(&ok,10) +1;
        series.setNum(nSeries,10);
        item->setText(7,series);
    }    
    
}

/**  S'invoca quan es comença a descarregar un estudi, per indicar-ho a la llista que aquell estudi ha començat la   
  *   descarrega
  *     @param  UID de l'estudi que es comença a descarregar
  */
void QRetrieveScreen::setRetrieving(QString studyUID)
{
    QList<QTreeWidgetItem *> qRetrieveList(m_treeRetrieveStudy->findItems(studyUID,Qt::MatchExactly,9));
    QTreeWidgetItem *item;

    
    if (! qRetrieveList.isEmpty())
    {
        item = qRetrieveList.at(0);
        item->setText(0,tr("RETRIEVING")); 
    }
    
}

/**  S'invoca quant s'ha acabat de baixa un estudi. S'indica a la llista que la descarrega de l'estudi ha finalitzat
  *     @param  UID de l'estudi descarregat
  */
void QRetrieveScreen::setRetrievedFinished(QString studyUID)
{
    QList<QTreeWidgetItem *> qRetrieveList(m_treeRetrieveStudy->findItems(studyUID,Qt::MatchExactly,9));
    QTreeWidgetItem *item;

    //hem de cridar al seriesRetrieved, perquè hem d'indicar que s'ha acabat la descarrega de l'última sèrie, ja que el starviewerprocess no sap quant acaba la descarregar de l'última sèrie
    setSeriesRetrieved(studyUID);
    
    if ( !qRetrieveList.isEmpty())
    {
        item = qRetrieveList.at(0);
        if (item->text(8) == "0") //si el número d'imatges descarregat és 0! error!
        {
            item->setText(0,tr("ERROR"));
        }
        else  item->setText(0,tr("RETRIEVED")); 
    }
    m_treeRetrieveStudy->repaint(); 
}

/**  S'invoca quant es produeix algun error al descarregar un estudi
  *     @param studyUID[in] UID de l'estudi descarregat
  */
void QRetrieveScreen::setErrorRetrieving(QString studyUID)
{
    QList<QTreeWidgetItem *> qRetrieveList(m_treeRetrieveStudy->findItems(studyUID,Qt::MatchExactly,9));
    QTreeWidgetItem *item;

    //hem de cridar al seriesRetrieved, perquè hem d'indicar que s'ha acabat la descarrega de l'última sèrie, ja que el starviewerprocess no sap quant acaba la descarregar de l'última sèrie
    
    setSeriesRetrieved(studyUID);
    
    if (!qRetrieveList.isEmpty())
    {
        item = qRetrieveList.at(0);
        item->setText(0,tr("ERROR"));
    }      
    
    m_treeRetrieveStudy->repaint();
}

/** destructor de la classe
  */
QRetrieveScreen::~QRetrieveScreen()
{
}


};
