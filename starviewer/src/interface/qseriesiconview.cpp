/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/ 
#include "qseriesiconview.h"
#include <QString>
#include <QIcon>
//Added by qt3to4:
#include "starviewersettings.h"

namespace udg {

/** Constructor de la classe
  */
QSeriesIconView::QSeriesIconView(QWidget *parent )
 : QWidget( parent )
{

    setupUi( this );
    QSize size;    

    size.setHeight(100);
    size.setWidth(100);
    m_seriesListV->setIconSize(size);
    //indiquem que després d'inserir s'ha d'ordenar l'iconview ascendentment
    
    createConnections();
    
}

/** crea les connexions dels signals i slots 
  */
void QSeriesIconView::createConnections()
{
    connect(m_seriesListV,SIGNAL(itemClicked ( QListWidgetItem *)), SLOT(clicked(QListWidgetItem *)));
    connect(m_seriesListV,SIGNAL(itemDoubleClicked ( QListWidgetItem *)), SLOT(view(QListWidgetItem *)));
}

/** Insereix l'informació d'una sèrie al ListICon
  *          @param descripció de la sèrie
  */
/* A l'status Tip de cada item es guarda la UID de la serie, ja que aquest camp no el vul mostrar i no tinc
   enlloc per amagar-lo */
void QSeriesIconView::insertSeries(Series *serie)
{
    QString text,num,pathImage,nameClass;
    StarviewerSettings settings;
    QListWidgetItem *item = new QListWidgetItem( m_seriesListV );
    QString statusTip;
    
    
    text.insert(0,tr("Series "));
    text.append(serie->getSeriesNumber().c_str() );
    text.append('\n');
    
    if (serie->getImageNumber() > 0)
    {
        num.setNum(serie->getImageNumber() );
        text.append(num);
        text.append(" images");
        text.append('\n');
    }
    
    if (serie->getSeriesDescription().length() > 0)
    {//si hi ha descripció la inserim
        text.append(serie->getSeriesDescription().c_str() );
        text.append('\n');
    }
        
    nameClass.insert(0,this->name());
    if (nameClass == "m_SeriesImViewCache")
    {
        pathImage.insert(0,settings.getCacheImagePath());
        pathImage.append(serie->getStudyUID().c_str());
        pathImage.append("/");
        pathImage.append(serie->getSeriesUID().c_str() );
        pathImage.append("/scaled.jpeg");
    }
    else
    {
        // \TODO lleig!!!!!!!!!!!!!!!!!
        pathImage = "/home/marc/starviewer-pacs/bin/images3.jpeg" ;
    }
    
    QIcon   icon(pathImage);

    item->setText(text);
    item->setIcon(icon);
    item->setStatusTip(serie->getSeriesUID().c_str()); //guardo a l'status tip l'UID de la serie
}

/** slot que s'activa quant es selecciona una serie, emiteix signal a QStudyListView, perquè selecciona la mateixa serie que el QSeriesIconView
  *        @param serie Seleccionada
  */
void QSeriesIconView::clicked(QListWidgetItem *item)
{
     if (item != NULL) emit(selectedSeriesIcon(item->statusTip()));
     
}

/** slot que s'activa quant es fa doblec
  *        @param item de la serie Seleccionada
  */
void QSeriesIconView::view(QListWidgetItem *item)
{

    if (item != NULL) emit(viewSeriesIcon());
    
}

/** Slot que s'activa quant es selecciona una sèrie des del StudyListView,selecciona la serie del QStudyListView en el QSeriesIconView
  *        @param  UID de la serie seleccionada
  */
void QSeriesIconView::selectedSeriesList(QString seriesUID)
{
    QList<QListWidgetItem *> qSeriesList(m_seriesListV->findItems("*",Qt::MatchWildcard));
    QListWidgetItem *item;
    
    for (int i = 0;i < qSeriesList.count();i++)
    {
        item = qSeriesList.at(i);;
        if (item->statusTip() == seriesUID)
        {
            m_seriesListV->setItemSelected(item,true);
            m_seriesListV->setCurrentItem(item);
        }
        else m_seriesListV->setItemSelected(item,false);
    }  
    
}

/** Neteja el Icon View de sèries
  */
void QSeriesIconView::clear()
{
    m_seriesListV->clear();
}

/** Slot, que al rebre la senyal addSeries del del QStudyListView afegeix una sèrie al IconView
  *        @param serie 
  */
void QSeriesIconView::addSeries(Series *serie)
{
    insertSeries(serie);
}

/** Slot, que al rebre la senya del QStudyListView neteja el IconView
  */
void QSeriesIconView::clearIconView()
{
    clear();
}

/** Destructor de la classe
  */
QSeriesIconView::~QSeriesIconView()
{
}


};
