/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "qoperationstatescreen.h"

#include <QCloseEvent>
#include "operation.h"
#include "logging.h"
#include "inputoutputsettings.h"

namespace udg {

QOperationStateScreen::QOperationStateScreen( QWidget *parent )
 : QDialog( parent )
{
    setupUi( this );
    setWindowFlags( (this->windowFlags() | Qt::WindowMaximizeButtonHint)  ^ Qt::WindowContextHelpButtonHint  );
    m_treeRetrieveStudy->setColumnHidden( 9 , true );//Conte l'UID de l'estudi
    m_treeRetrieveStudy->setColumnHidden( 10 , true );//Indica quin tipus d'operació és

    createConnections();

    Settings settings;
    settings.restoreColumnsWidths(InputOutputSettings::OperationStateColumnsWidth, m_treeRetrieveStudy);
}

QOperationStateScreen::~QOperationStateScreen()
{
}

unsigned int QOperationStateScreen::getActiveOperationsCount()
{
    QList<QTreeWidgetItem *> retrieveList( m_treeRetrieveStudy->findItems( "*" , Qt::MatchWildcard, 0 ) );
    QTreeWidgetItem *item;
    unsigned int finalizedItems = 0;

    for ( int i = 0; i < retrieveList.count();i++ )
    {
        item = retrieveList.at( i );
        if ( ! isOperationFinalized(item->text(0)) )
        {
            ++finalizedItems;
        }
    }
    return finalizedItems;
}

void QOperationStateScreen::createConnections()
{
    connect( m_buttonClear , SIGNAL( clicked() ) , this , SLOT( clearList() ) );
}

void QOperationStateScreen::insertNewOperation( Operation *operation )
{
    bool canInsert = true;
    // busquem si l'ítem ja es troba a la llista
    QTreeWidgetItem *duplicateItem = operationExists( operation->getStudyUID() );
    if( duplicateItem )
    {
        // si l'operació està finalitzada el treiem de la llista
        if( isOperationFinalized( duplicateItem->text(0) ) )
        {
            // TODO estaria bé que en el cas que l'element aparegui com a descarregat, és a dir, l'operació 
            // va acabar amb èxit, comprovar quant fa que es va inserir aquesta operació i advertir a l'usuari 
            // si realment vol tornar a baixar-se l'estudi, per exemple
            deleteStudy( operation->getStudyUID() ); //si l'estudi ja existeix a la llista l'esborrem
        }
        else // això vol dir que està o pendent o descarregant, per tant hem de parar i no insertar la operació
        {
            canInsert = false;
        }
    }

    if( !canInsert )
    {
        DEBUG_LOG("L'operació amb UID: " + operation->getStudyUID() + " està pendent/descarregant per tant no s'afegeix de nou a la finestra");
    }
    else
    {
        QTreeWidgetItem* item = new QTreeWidgetItem();
        QTime time = QTime::currentTime();
        QString name, operationNumber;
        QDate date = QDate::currentDate();

        name.insert( 0 , operation->getPatientName() );
        name.replace( "^" ,", ");

        item->setText( 0 , tr( "PENDING" ) );

        if ( operation->getOperation() == Operation::Retrieve || operation->getOperation() == Operation::View )
        {
            item->setText( 1 , tr( "Local" ) );
        }
        else item->setText( 1 , tr("Server") );

        item->setText( 2 , operation->getPacsDevice().getAETitle() );
        item->setText( 3 , operation->getPatientID() );
        item->setText( 4 , name );
        item->setText( 5 , date.toString("dd/MM/yyyy") );
        item->setText( 6 , time.toString("hh:mm") );
        item->setText( 7 , "0" ); // series
        item->setText( 8 , "0"); //imatges
        item->setText( 9 , operation->getStudyUID() );
        operationNumber.setNum( operation->getOperation() , 10 );
        item->setText( 10 , operationNumber ); // indica el tipus d'operació

        m_treeRetrieveStudy->addTopLevelItem(item);
    }
}


void QOperationStateScreen::clearList()
{
    // seleccionem els elements que volem esborrar
    QList<QTreeWidgetItem *> clearableItems;
    clearableItems = m_treeRetrieveStudy->findItems( tr("RETRIEVED"), Qt::MatchExactly, 0 );
    clearableItems += m_treeRetrieveStudy->findItems( tr("STORED"), Qt::MatchExactly, 0 );
    clearableItems += m_treeRetrieveStudy->findItems( tr("ERROR"), Qt::MatchExactly, 0 );
    clearableItems += m_treeRetrieveStudy->findItems( tr("CANCELLED"), Qt::MatchExactly, 0 );
    // els eliminem de la llista
    foreach( QTreeWidgetItem *itemToClear, clearableItems )
    {
        m_treeRetrieveStudy->invisibleRootItem()->takeChild( m_treeRetrieveStudy->invisibleRootItem()->indexOfChild(itemToClear) );
    }
}

void QOperationStateScreen::deleteStudy( QString studyUID )
{
    QList<QTreeWidgetItem *> qTreeWidgetItemsToDelete( m_treeRetrieveStudy->findItems( studyUID , Qt::MatchExactly , 9 ) );

    foreach(QTreeWidgetItem *itemToDelete, qTreeWidgetItemsToDelete)
    {
        m_treeRetrieveStudy->invisibleRootItem()->takeChild( m_treeRetrieveStudy->invisibleRootItem()->indexOfChild(itemToDelete) );
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

void QOperationStateScreen::setRetrievedImagesToCurrentProcessingStudy(int numberOfImages)
{
    QString Images;
    QList<QTreeWidgetItem *> qRetrieveList( m_treeRetrieveStudy->findItems(m_currentProcessingStudyUID, Qt::MatchExactly, 9) );
    QTreeWidgetItem *item;

    if  ( !qRetrieveList.isEmpty() )
    {
        item = qRetrieveList.at( 0 );
        Images.setNum( numberOfImages , 10 );
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
    m_currentProcessingStudyUID = studyUID;

    QList<QTreeWidgetItem *> qRetrieveList( m_treeRetrieveStudy->findItems( studyUID , Qt::MatchExactly , 9 ) );
    QTreeWidgetItem *item;

    if ( !qRetrieveList.isEmpty() )
    {
        item = qRetrieveList.at( 0 );
        if ( item->text( 10 ).toInt( NULL , 10 ) == Operation::View || item->text( 10 ).toInt( NULL , 10 ) == Operation::Retrieve )
        {
            item->setText( 0 , tr( "RETRIEVING" ) );
        }
        else if ( item->text( 10 ).toInt( NULL , 10 ) == Operation::Move ) item->setText( 0 , tr( "STORING" ) );
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
            if ( item->text( 10 ).toInt( NULL , 10 ) == Operation::View || item->text( 10 ).toInt( NULL , 10 ) == Operation::Retrieve )
            {
                item->setText( 0 , tr( "RETRIEVED" ) );
            }
            else if ( item->text( 10 ).toInt( NULL , 10 ) == Operation::Move ) item->setText( 0 , tr( "STORED" ) );
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

void QOperationStateScreen::setCancelledOperation(QString studyInstanceUID)
{
    QList<QTreeWidgetItem *> qRetrieveList(m_treeRetrieveStudy->findItems(studyInstanceUID, Qt::MatchExactly, 9));
    QTreeWidgetItem *item;

    if (!qRetrieveList.isEmpty())
    {
        item = qRetrieveList.at(0);
        item->setText(0, tr("CANCELLED"));
    }

    m_treeRetrieveStudy->repaint();
}

void QOperationStateScreen::closeEvent( QCloseEvent* ce )
{
    Settings settings;
    settings.saveColumnsWidths(InputOutputSettings::OperationStateColumnsWidth, m_treeRetrieveStudy);
    ce->accept();
}

bool QOperationStateScreen::isOperationFinalized(const QString &message)
{
    return  message == tr("RETRIEVED") || message == tr("STORED") || message == tr("ERROR") || message == tr("CANCELLED");
}

QTreeWidgetItem *QOperationStateScreen::operationExists( const QString &studyUID )
{
    QTreeWidgetItem *result = NULL;
    QList<QTreeWidgetItem *> matchingStudies( m_treeRetrieveStudy->findItems( studyUID , Qt::MatchExactly , 9 ) );

    // hauríem de tenir únicament 1 sol estudi
    switch( matchingStudies.count() )
    {
    case 0:
        // ok, no n'hi ha cap
        break;

    case 1: // ok, lo normal
        result = matchingStudies.first();
        break;
         
    default: // oops, alguna problema hi ha aquí!! però de totes maneres tornem el primer element
        DEBUG_LOG( QString("Nombre d'estudis trobats erroni! :: %1").arg( matchingStudies.count() ) );
        result = matchingStudies.first();
        break;
    }
    
    return result;
}

};
