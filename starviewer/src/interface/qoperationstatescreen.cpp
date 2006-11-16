/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/ 

#include "qoperationstatescreen.h"
#include <QString>
#include <iostream.h>
#include <qdatetime.h>
#include "processimagesingleton.h"
#include "operation.h"
#include "const.h"

namespace udg {

QOperationStateScreen::QOperationStateScreen( QWidget *parent )
 : QDialog( parent )
{
    setupUi( this );
    m_treeRetrieveStudy->setColumnHidden( 9 , true );//Conte l'UID de l'estudi
    m_treeRetrieveStudy->setColumnHidden( 10 , true );//Indica quin tipus d'operació és
    
    createConnections();
}

void QOperationStateScreen::createConnections()
{
    connect( m_buttonClear , SIGNAL( clicked() ) , this , SLOT( clearList() ) );
}

void QOperationStateScreen::insertNewOperation( Operation *operation )
{
    QTreeWidgetItem* item = new QTreeWidgetItem( m_treeRetrieveStudy );
    QTime time = QTime::currentTime();
    QString name, operationNumber;
    QDate date = QDate::currentDate();
    
    deleteStudy( operation->getStudyUID() ); //si l'estudi ja existeix a la llista l'esborrem
    name.insert( 0 , operation->getPatientName() );
    name.replace( "^" ,", ");
    
    item->setText( 0 , tr( "PENDING" ) );
    
    if ( operation->getOperation() == operationRetrieve || operation->getOperation() == operationView )
    {
        item->setText( 1 , tr( "Local" ) );
    }
    else item->setText( 1 , tr("Server") );
    
    item->setText( 2 , operation->getPacsParameters().getAEPacs().c_str() );
    item->setText( 3 , operation->getPatientID() );
    item->setText( 4 , name );
    item->setText( 5 , date.toString("dd/MM/yyyy") );
    item->setText( 6 , time.toString("hh:mm") );
    item->setText( 7 , "0" ); // series
    item->setText( 8 , "0"); //imatges
    item->setText( 9 , operation->getStudyUID() );
    operationNumber.setNum( operation->getOperation() , 10 );
    item->setText( 10 , operationNumber ); // indica el tipus d'operació
}


void QOperationStateScreen::clearList()
{
    QList<QTreeWidgetItem *> qRetrieveList( m_treeRetrieveStudy->findItems( "*" , Qt::MatchWildcard, 0 ) );
    QTreeWidgetItem *item;
    
    for ( int i = 0; i < qRetrieveList.count();i++ )
    {
        item = qRetrieveList.at( i );
        if (item->text( 0 ) != tr( "PENDING" ) && item->text( 0 ) != tr( "RETRIEVING" ) )
        {
            delete item;
        }
    }
}

void QOperationStateScreen::deleteStudy( QString studyUID )
{
    QList<QTreeWidgetItem *> qRetrieveList( m_treeRetrieveStudy->findItems( studyUID , Qt::MatchExactly , 9 ) );
    QTreeWidgetItem *item;
    int i = 0;
    
    while ( i < qRetrieveList.count() )
    {
        item = qRetrieveList.at( i );
        if ( item->text(9) == studyUID )
        {
            delete item;
            break;
        }
        i++;
    }
}

void QOperationStateScreen::imageCommit( QString studyUID , int downloadedImages )
{
    QString Images;
    QList<QTreeWidgetItem *> qRetrieveList( m_treeRetrieveStudy->findItems( studyUID , Qt::MatchExactly , 9 ) );
    QTreeWidgetItem *item;

    if  ( !qRetrieveList.isEmpty() )
    {
        item = qRetrieveList.at( 0 );
        Images.setNum( downloadedImages , 10 );
        item->setText( 8 , Images );
    }    
}

void QOperationStateScreen::seriesCommit( QString studyUID )
{
    QList<QTreeWidgetItem *> qRetrieveList( m_treeRetrieveStudy->findItems( studyUID , Qt::MatchExactly , 9 ) );
    QTreeWidgetItem *item;
    QString series;
    int nSeries = 0;
    bool ok;
    
    if ( !qRetrieveList.isEmpty() )
    {
        item = qRetrieveList.at( 0 );
        nSeries = item->text( 7 ).toInt( &ok , 10 ) + 1;
        series.setNum( nSeries, 10 );
        item->setText( 7 , series );
    }    
}

void QOperationStateScreen::setOperating( QString studyUID )
{
    QList<QTreeWidgetItem *> qRetrieveList( m_treeRetrieveStudy->findItems( studyUID , Qt::MatchExactly , 9 ) );
    QTreeWidgetItem *item;
    
    if ( !qRetrieveList.isEmpty() )
    {
        item = qRetrieveList.at( 0 );
        if ( item->text( 10 ).toInt( NULL , 10 ) == operationView || item->text( 10 ).toInt( NULL , 10 ) == operationRetrieve )
        {
            item->setText( 0 , tr( "RETRIEVING" ) ); 
        }
        else if ( item->text( 10 ).toInt( NULL , 10 ) == operationMove ) item->setText( 0 , tr(" STORING ") );
    }
}

void QOperationStateScreen::setOperationFinished( QString studyUID )
{
    QList<QTreeWidgetItem *> qRetrieveList( m_treeRetrieveStudy->findItems( studyUID , Qt::MatchExactly , 9 ) );
    QTreeWidgetItem *item;
   
    if ( !qRetrieveList.isEmpty() )
    {
        item = qRetrieveList.at( 0 );
        if ( item->text( 8 ) == "0" ) //si el número d'imatges processat és 0 error
        {
            item->setText( 0 , tr( "ERROR" ) );
        }
        else
        { 
            if ( item->text( 10 ).toInt( NULL , 10 ) == operationView || item->text( 10 ).toInt( NULL , 10 ) == operationRetrieve )
            {
                item->setText( 0 , tr( "RETRIEVED" ) ); 
            }
            else if ( item->text( 10 ).toInt( NULL , 10 ) == operationMove ) item->setText( 0 , tr(" STORED ") );
        }
    }
    
    m_treeRetrieveStudy->repaint(); 
}

void QOperationStateScreen::setErrorOperation( QString studyUID )
{
    QList<QTreeWidgetItem *> qRetrieveList(m_treeRetrieveStudy->findItems( studyUID , Qt::MatchExactly , 9 ) );
    QTreeWidgetItem *item;

    //hem de cridar al seriesRetrieved, perquè hem d'indicar que s'ha acabat la descarrega de l'última sèrie, ja que el starviewerprocess no sap quant acaba la descarregar de l'última sèrie
    seriesCommit( studyUID );
    
    if ( !qRetrieveList.isEmpty() )
    {
        item = qRetrieveList.at( 0 );
        item->setText( 0 , tr( "ERROR" ) );
    }      
    
    m_treeRetrieveStudy->repaint();
}

QOperationStateScreen::~QOperationStateScreen()
{
}

};
