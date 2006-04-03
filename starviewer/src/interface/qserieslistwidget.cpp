/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/ 
#include "qserieslistwidget.h"
#include <QString>
#include <QIcon>
//Added by qt3to4:
#include "starviewersettings.h"

namespace udg {

/** Constructor de la classe
  */
QSeriesListWidget::QSeriesListWidget(QWidget *parent )
 : QWidget( parent )
{

    setupUi( this );
    QSize size;    

    size.setHeight(100);
    size.setWidth(100);
    m_seriesListWidget->setIconSize(size);
    
    createConnections();
    
}

/** crea les connexions dels signals i slots 
  */
void QSeriesListWidget::createConnections()
{
    connect(m_seriesListWidget,SIGNAL(itemClicked ( QListWidgetItem *)), SLOT(clicked(QListWidgetItem *)));
    connect(m_seriesListWidget,SIGNAL(itemDoubleClicked ( QListWidgetItem *)), SLOT(view(QListWidgetItem *)));
}

/** Insereix l'informació d'una sèrie al ListICon
  *          @param descripció de la sèrie
  */
/* A l'status Tip de cada item es guarda la UID de la serie, ja que aquest camp no el vul mostrar i no tinc
   enlloc per amagar-lo */
void QSeriesListWidget::insertSeries(Series *serie)
{
    QString text,num,pathImage,nameClass;
    StarviewerSettings settings;
    QListWidgetItem *item = new QListWidgetItem( m_seriesListWidget );
    QString statusTip;
    
    
    text.insert(0,tr("Series "));
    text.append(serie->getSeriesNumber().c_str() );
    
    if (serie->getProtocolName().length() > 0)
    {//si hi ha descripció la inserim
        text.append(" ");
        text.append(serie->getProtocolName().c_str() );
        text.append('\n');
    }
    else text.append('\n');
    
    if (serie->getImageNumber() > 0)
    {
        num.setNum(serie->getImageNumber() );
        text.append(num);
        text.append( tr(" images") );
        text.append('\n');
    }
    
    nameClass.insert(0,this->objectName());
    if (nameClass == "m_seriesListWidgetCache")
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

/** slot que s'activa quant es selecciona una serie, emiteix signal a QStudyTreeWidget, perquè selecciona la mateixa serie que el QSeriesListWidget
  *        @param serie Seleccionada
  */
void QSeriesListWidget::clicked(QListWidgetItem *item)
{
     if (item != NULL) emit(selectedSeriesIcon(item->statusTip()));
     
}

/** slot que s'activa quant es fa doblec
  *        @param item de la serie Seleccionada
  */
void QSeriesListWidget::view(QListWidgetItem *item)
{

    if (item != NULL) emit(viewSeriesIcon());
    
}

/** Slot que s'activa quant es selecciona una sèrie des del StudyTreeWidget,selecciona la serie del QStudyTreeWidget en el QSeriesListWidget
  *        @param  UID de la serie seleccionada
  */
void QSeriesListWidget::selectedSeriesList(QString seriesUID)
{
    QList<QListWidgetItem *> qSeriesList(m_seriesListWidget->findItems("*",Qt::MatchWildcard));
    QListWidgetItem *item;
    
    for (int i = 0;i < qSeriesList.count();i++)
    {
        item = qSeriesList.at(i);;
        if (item->statusTip() == seriesUID)
        {
            m_seriesListWidget->setItemSelected(item,true);
            m_seriesListWidget->setCurrentItem(item);
        }
        else m_seriesListWidget->setItemSelected(item,false);
    }  
    
}

/** Neteja el ListWidget de sèries
  */
void QSeriesListWidget::clear()
{
    m_seriesListWidget->clear();
}

/** Slot, que al rebre la senyal addSeries del del QStudyTreeWidget afegeix una sèrie al IconView
  *        @param serie 
  */
void QSeriesListWidget::addSeries(Series *serie)
{
    insertSeries(serie);
}

/** Slot, que al rebre la senya del QStudyTreeWidget neteja el ListWidget
  */
void QSeriesListWidget::clearSeriesListWidget()
{
    clear();
}

/** Destructor de la classe
  */
QSeriesListWidget::~QSeriesListWidget()
{
}


};
