/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "qserieslistwidget.h"

#include <QString>
#include <QIcon>
#include <QHash>

#include "dicomseries.h"
#include "scalestudy.h"

namespace udg {

QSeriesListWidget::QSeriesListWidget(QWidget *parent )
 : QWidget( parent )
{
    setupUi( this );
    QSize size;

    //Definim la mida de la imatge que mostrem
    size.setHeight( scaledSeriesSizeY );
    size.setWidth( scaledSeriesSizeX );
    m_seriesListWidget->setIconSize( size );

    createConnections();
}

void QSeriesListWidget::createConnections()
{
    connect( m_seriesListWidget , SIGNAL( itemClicked ( QListWidgetItem *) ) , SLOT( clicked( QListWidgetItem * ) ) );
    connect( m_seriesListWidget , SIGNAL( itemDoubleClicked ( QListWidgetItem * ) ), SLOT( view(QListWidgetItem * ) ) );
}

void QSeriesListWidget::insertSeries( DICOMSeries *serie )
{
    QString text,num;
    QListWidgetItem *item = new QListWidgetItem( m_seriesListWidget );
    QString statusTip;

    text = tr( " Series " ) + serie->getSeriesNumber();
    if ( serie->getProtocolName().length() > 0 )
    {//si hi ha descripció la inserim
        text += " " + serie->getProtocolName();
    }
    text +="\n";

    if( serie->getImageNumber() > 0 )
    {
        text += QString::number( serie->getImageNumber() );
        QString modality = serie->getSeriesModality();
        if( modality == "KO" )
            text += tr(" Key Object Note");
        else if( modality == "PR" )
            text += tr(" Presentation State");
        else if( modality == "SR" )
            text += tr(" Structured Report");
        else
            text += tr(" Images");
    }

    QIcon  icon( ScaleStudy::getScaledImagePath(serie) );

    item->setText(text);
    item->setIcon(icon);

    /* A l'status Tip de cada item es guarda la UID de la serie, ja que aquest camp no el vull mostrar i no tinc
   enlloc per amagar-lo, ho utilitzo per identificar la sèrie */
    item->setStatusTip(serie->getSeriesUID());

    m_HashSeriesStudy[serie->getSeriesUID()] = serie->getStudyUID();//Guardem per la sèrie a quin estudi pertany
}

void QSeriesListWidget::setCurrentSeries( const QString &seriesUID )
{
    int index = 0;
    bool stop = false;
    QList<QListWidgetItem *> llistaSeries =  m_seriesListWidget->findItems ( "" , Qt::MatchContains );


    while ( !stop && index < llistaSeries.count() )
    {
        if ( llistaSeries.at( index )->statusTip() == seriesUID )
        {
            stop = true;
        }
        else index++;
    }
    if ( stop ) m_seriesListWidget->setCurrentItem( llistaSeries.at( index ) );
}

void QSeriesListWidget::clicked( QListWidgetItem *item )
{
     if ( item != NULL ) emit( selectedSeriesIcon( item->statusTip() ) );
}

void QSeriesListWidget::view( QListWidgetItem *item )
{
    if ( item != NULL ) emit( viewSeriesIcon() );
}

QString QSeriesListWidget::getCurrentSeriesUID()
{
    return m_seriesListWidget->currentItem()->statusTip();
}

QString QSeriesListWidget::getCurrentStudyUID()
{
    return m_HashSeriesStudy[getCurrentSeriesUID()];
}

void QSeriesListWidget::clear()
{
    m_seriesListWidget->clear();
    m_HashSeriesStudy.clear();
}

QSeriesListWidget::~QSeriesListWidget()
{
}

};
