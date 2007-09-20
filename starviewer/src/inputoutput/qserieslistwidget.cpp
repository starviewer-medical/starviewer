/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "qserieslistwidget.h"

#include <QString>
#include <QIcon>

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

    text.insert( 0 , tr( "Series " ) );
    text.append( serie->getSeriesNumber() );

    if ( serie->getProtocolName().length() > 0 )
    {//si hi ha descripció la inserim
        text.append( " " );
        text.append (serie->getProtocolName() );
        text.append( '\n' );
    }
    else text.append( '\n' );

    if ( serie->getImageNumber() > 0 )
    {
        num.setNum( serie->getImageNumber() );
        text.append( num );
        text.append( tr(" images") );
        text.append( '\n' );
    }

    QIcon   icon( ScaleStudy::getScaledImagePath(serie) );

    item->setText(text);
    item->setIcon(icon);

    /* A l'status Tip de cada item es guarda la UID de la serie, ja que aquest camp no el vull mostrar i no tinc
   enlloc per amagar-lo, ho utilitzo per identificar la sèrie */
    item->setStatusTip(serie->getSeriesUID());
}

void QSeriesListWidget::clicked( QListWidgetItem *item )
{
     if ( item != NULL ) emit( selectedSeriesIcon( item->statusTip() ) );
}

void QSeriesListWidget::view( QListWidgetItem *item )
{
    if ( item != NULL ) emit( viewSeriesIcon() );
}

void QSeriesListWidget::clear()
{
    m_seriesListWidget->clear();
}

QSeriesListWidget::~QSeriesListWidget()
{
}

};
