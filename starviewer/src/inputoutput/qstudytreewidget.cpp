/***************************************************************************
 *   Copyright (C) 2005 by marc                                            *
 *   marc@localhost                                                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <QHeaderView>
#include <QContextMenuEvent>
#include <QMessageBox>
#include <QShortcut>

#include "qstudytreewidget.h"
#include "qserieslistwidget.h"
#include "pacslistdb.h"
#include "starviewersettings.h"
#include "studylist.h"
#include "dicomstudy.h"
#include "serieslist.h"
#include "dicomseries.h"
#include "imagelist.h"
#include "dicomimage.h"
#include "status.h"

namespace udg {

QStudyTreeWidget::QStudyTreeWidget( QWidget *parent )
 : QWidget( parent ), m_viewAction(NULL), m_retrieveAction(NULL), m_deleteStudyAction(NULL), m_sendToDICOMDIRListAction(NULL), m_storeStudyAction(NULL)
{
    setupUi( this );

    //la columna de UID AETITLE,type, Image Number i Protocol Name les fem invisibles
    m_studyTreeView->setColumnHidden( 10, true );
    // m_studyTreeView->setColumnHidden( 11, true ); UID
    m_studyTreeView->setColumnHidden( 12, true );
    m_studyTreeView->setColumnHidden( 13, true );
    m_studyTreeView->setColumnHidden( 14 , true );

    //carreguem les imatges que es mostren el QStudyTreeWidget
    m_openFolder = QIcon( ":/images/folderopen.png" );
    m_closeFolder = QIcon( ":/images/folderclose.png" );
    m_iconSeries = QIcon( ":/images/series.png" );

    m_parentName = parent->objectName();//el guardem per saber si es tracta de la llista d'estudis del Pacs o la Cache
    createContextMenu(); //creem el menu contextual
    createConnections();

    setWidthColumns();//s'assigna a les columnes l'amplada definida per l'usuari
}

void QStudyTreeWidget::createConnections()
{
    connect( m_studyTreeView , SIGNAL( itemClicked ( QTreeWidgetItem * , int ) ) , this, SLOT( clicked ( QTreeWidgetItem * , int ) ) );
    connect( m_studyTreeView , SIGNAL( itemDoubleClicked ( QTreeWidgetItem * , int ) ), this , SLOT( doubleClicked( QTreeWidgetItem * , int ) ) );
}

void QStudyTreeWidget::createContextMenu()
{
    //acció veure
    m_viewAction = m_contextMenu.addAction( tr("&View") );
    m_viewAction->setShortcut( tr("Ctrl+V") );
    connect( m_viewAction , SIGNAL( triggered() ) , this , SLOT( viewStudy() ) );

    // només hi ha RETRIEVE en el PACS i DICOMDIR TODO a DICOMDIR s'hauria de dir IMPORT to DATABASE... per ser mes correctes
    // TODO AL TANTO AMB AIXÒ!!!!!!! SI MAI CANVIA EL PARENT QUÈ PASSA???????????? FER SISTEMA PER DETERMINAR EL TIPUS DE TAB QUE ÉS O ALGO
    if( m_parentName == "m_tabPacs" || m_parentName == "m_tabDicomdir" )
    {
        m_retrieveAction = m_contextMenu.addAction( tr("&Retrieve") );
        m_retrieveAction->setShortcut( tr("Ctrl+R") );
        m_retrieveAction->setIcon( QIcon(":/images/retrieve.png") );
        connect( m_retrieveAction , SIGNAL( triggered() ) , this , SLOT( retrieveImages() ) );
    }

    // Al haver un QSplitter el nom del Pare del TabCache és l'splitter TODO molt al tanto amb aquesta guarrada -_-¡
    if( m_parentName == "m_StudyTreeSeriesListQSplitter" )
    {
        // només es pot esborrar a local
        m_deleteStudyAction =  m_contextMenu.addAction( tr("&Delete")) ;
        m_deleteStudyAction->setShortcut( tr("Ctrl+D") );
        connect( m_deleteStudyAction , SIGNAL(triggered()), this, SLOT(deleteStudy()));

        //nomes es pot afegir element a la llista de DICOMDIR desde local
        m_sendToDICOMDIRListAction = m_contextMenu.addAction( tr( "Send to DICOMDIR List" ) );
        m_sendToDICOMDIRListAction->setShortcut( tr( "Ctrl+M" ) );
        connect( m_sendToDICOMDIRListAction , SIGNAL ( triggered() ) , this , SLOT ( createDicomDir() ) );

        // només es pot fer store des de local
        m_storeStudyAction = m_contextMenu.addAction( tr( "Store to PACS" ) );
        m_storeStudyAction->setShortcut( tr( "Ctrl+S" ) );
        m_storeStudyAction->setIcon( QIcon(":/images/store.png") );
        connect( m_storeStudyAction , SIGNAL ( triggered() ) , this , SLOT ( storeStudy() ) );
    }
}

void QStudyTreeWidget::setWidthColumns()
{
    StarviewerSettings settings;

    for ( int i = 0; i < m_studyTreeView->columnCount(); i++ )
    {   //Al haver un QSplitter el nom del Pare del TabCache és l'splitter
        if (m_parentName == "m_StudyTreeSeriesListQSplitter")
        {
            m_studyTreeView->header()->resizeSection( i ,settings.getStudyCacheListColumnWidth(i) );
        }

        //Renombrem segons quin objecte es tracti, si es el que mostra la llista d'estudis del PACS o de la cache
        if (m_parentName == "m_tabPacs")
        {
            m_studyTreeView->header()->resizeSection( i , settings.getStudyPacsListColumnWidth( i ) );
        }

        if (m_parentName == "m_tabDicomdir")
        {
            m_studyTreeView->header()->resizeSection( i , settings.getStudyDicomdirListColumnWidth( i ) );
        }

    }
}

void QStudyTreeWidget::insertStudyList( StudyList *studyList )
{
    m_studyTreeView->clear();

    studyList->firstStudy();

    while ( !studyList->end() )
    {
         insertStudy( &studyList->getStudy() );
         studyList->nextStudy();
    }
}

void QStudyTreeWidget::insertStudy( DICOMStudy *study)
{
    QString text;
    Status state;

    if ( getStudyItem( study->getStudyUID() , study->getPacsAETitle() ) != NULL ) removeStudy( study->getStudyUID() );//si l'estudi ja hi existeix a StudyTreeView l'esborrem

    QTreeWidgetItem* item = new QTreeWidgetItem( m_studyTreeView );
    text.truncate( 0 );
    text.append( tr("Study ") );
    text.append( study->getStudyId() );
    item->setIcon( 0 ,m_closeFolder );
    item->setText( 0 , text );

    item->setText( 1 , study->getPatientId() );
    item->setText( 2 , study->getPatientName() );
    item->setText( 3 , formatAge( study->getPatientAge() ) );
    item->setText( 4 , study->getStudyModality() );
    item->setText( 5 , study->getStudyDescription() );
    item->setText( 6 , formatDate( study->getStudyDate() ) );
    item->setText( 7 , study->getStudyTime() );
    //item->setText( 7 , formatHour( study->getStudyTime() ) );

    if ( study->getInstitutionName().isEmpty() ) //si la informació ve buida l'anem a buscar a la bdd local
    {
        if ( m_oldPacsAETitle != study->getPacsAETitle() ) //comparem que no sigui el mateix pacs que l'anterior, si es el mateix tenim la informacio guardada
        {//si es un pacs diferent busquem la informacio
            PacsListDB pacsList;
            PacsParameters pacs;

            state = pacsList.queryPacs( &pacs , study->getPacsAETitle() );
            if ( state.good() )
            {
                item->setText( 8 , pacs.getInstitution() );
                m_OldInstitution = pacs.getInstitution();
            }
            m_oldPacsAETitle = study->getPacsAETitle();
        }
        else item->setText( 8 , m_OldInstitution );
    }
    else item->setText( 8 , study->getInstitutionName() );

    item->setText( 9 , study->getAccessionNumber() );
    item->setText( 10 , study->getPacsAETitle() );
    item->setText( 11 , study->getStudyUID() );
    item->setText( 12 , "STUDY" );//indiquem de que es tracta d'un estudi
    item->setText( 13 , "" );
    item->setText( 15 , study->getReferringPhysiciansName() );

    m_studyTreeView->clearSelection();
}

void QStudyTreeWidget::insertSeriesList( SeriesList *seriesList )
{
    seriesList->firstSeries();

    while ( !seriesList->end() )
    {
         insertSeries( &seriesList->getSeries() );
         seriesList->nextSeries();
    }
}

void QStudyTreeWidget::insertSeries( DICOMSeries *serie )
{
    QString text, description;
    QTreeWidgetItem* item, *studyItem;

    studyItem = getStudyItem( serie->getStudyUID() , serie->getPacsAETitle() );
    item = new QTreeWidgetItem( studyItem );

    text = tr( "Series %1" ).arg(serie->getSeriesNumber());
    item->setIcon( 0 , m_iconSeries );
    item->setText( 0 , text );
    item->setText( 4 , serie->getSeriesModality() );

    description = serie->getSeriesDescription();
    item->setText( 5 , description.simplified() );//treiem els espaics en blanc del davant i darrera

    //si no tenim data o hora de la sèrie mostrem la de l'estudi
    if ( serie->getSeriesDate().length() != 0 )
    {
        item->setText( 6 , formatDate(serie->getSeriesDate() ) );
    }

    if ( serie->getSeriesTime().length()!= 0 )
    {
        item->setText( 7 , formatHour(serie->getSeriesTime() ) );
    }

    item->setText( 10, serie->getPacsAETitle() );
    item->setText( 11 , serie->getSeriesUID() );
    item->setText( 12 , "SERIES" ); //indiquem que es tracta d'una sèrie

    text.truncate( 0 );
    text.setNum( serie->getImageNumber(), 10 );
    item->setText( 13 , text );
    item->setText( 14 , serie->getProtocolName() );
    item->setText( 16 , serie->getPPSStartDate() );
    item->setText( 17 , serie->getPPStartTime() );
    item->setText( 18 , serie->getRequestedProcedureID() );
    item->setText( 19 , serie->getScheduledProcedureStepID() );

    emit( addSeries(serie) );//afegim serie al SeriesIconView
}

void QStudyTreeWidget::insertImageList( ImageList *imageList )
{
    imageList->firstImage();

    while ( !imageList->end() )
    {
         insertImage( &imageList->getImage() );
         imageList->nextImage();
    }
}

void QStudyTreeWidget::insertImage( DICOMImage * image )
{
    QString text, description;
    char imageNumber[7];
    QTreeWidgetItem* studyItem, *item;
    bool stop = false;
    int index = 0;

    studyItem = getStudyItem( image->getStudyUID() , image->getPacsAETitle() );

    while ( !stop && index < studyItem->childCount() )
    {
        if ( studyItem->child( index )->text( 11 ) == image->getSeriesUID() )
        {
            stop = true;
        }
        else index++;
    }

    item = new QTreeWidgetItem( studyItem->child( index ) );

    //convertim el numero d'imatge a text
    sprintf( imageNumber , "%i" , image->getImageNumber() );
    text.truncate( 0 );
    text.append( tr( "Image " ) );
    text.append( imageNumber );
    item->setIcon( 0 , m_iconSeries );
    item->setText( 0 , text );

    item->setText( 10 , image->getPacsAETitle() );
    item->setText( 11 , image->getSOPInstanceUID() );
    item->setText( 12, "IMAGE" ); //indiquem que es tracta d'una imatge
}

QString QStudyTreeWidget::formatAge( const QString age )
{
    QString text( age );

    if (text.length() > 0)
    {
        if (text.at(0) == '0')
        {//treiem el 0 de davant els anys, el PACS envia per ex: 047Y nosaltes tornem 47Y
            text.replace(0,1," ");
        }
    }

    return text;
}

QString QStudyTreeWidget::formatDate( const QString date )
{
    QString formateDate , originalDate ( date );

    formateDate.insert( 0 , originalDate.mid( 6 , 2 ) ); //dd
    formateDate.append( "/" );
    formateDate.append( originalDate.mid( 4 , 2 ) );
    formateDate.append( "/" );
    formateDate.append( originalDate.mid( 0 , 4 ) );

    return formateDate;
}

QString QStudyTreeWidget::formatHour( const QString hour )
{
    QString formatedHour,originalHour( hour );

    formatedHour.insert( 0 , originalHour.mid( 0 , 2 ) );
    formatedHour.append( ":" );
    formatedHour.append( originalHour.mid( 2 , 2 ) );

    return formatedHour;
}

void QStudyTreeWidget::clear()
{
    m_studyTreeView->clear();
}

void QStudyTreeWidget::setSortColumn( int col )
{
    m_studyTreeView->sortItems( col , Qt::AscendingOrder );
    m_studyTreeView->clearSelection();
}

QString QStudyTreeWidget::getSelectedStudyUID()
{
    if ( m_studyTreeView->currentItem() != NULL )
    {
        if ( m_studyTreeView->currentItem()->text( 12 ) == "STUDY" ) //es un estudi
        {
            return m_studyTreeView->currentItem()->text( 11 );
        }
        else if ( m_studyTreeView->currentItem()->text( 12 ) == "SERIES" )
        {
            return m_studyTreeView->currentItem()->parent()->text( 11 );
        }
        else if ( m_studyTreeView->currentItem()->text( 12 ) == "IMAGE" )
        {
            return m_studyTreeView->currentItem()->parent()->parent()->text( 11 );
        }
        else return "";
    }
    else return "";
}

QString QStudyTreeWidget::getSelectedSeriesUID()
{
    if ( m_studyTreeView->currentItem() != NULL )
    {
        if ( m_studyTreeView->currentItem()->text( 12 ) == "SERIES" )
        {
            return m_studyTreeView->currentItem()->text( 11 );
        }
        else if ( m_studyTreeView->currentItem()->text( 12 ) == "IMAGE" )
        {
            return m_studyTreeView->currentItem()->parent()->text( 11 );
        }
        else return "";
    }
    else return "";
}

QTreeWidgetItem*  QStudyTreeWidget::getStudyItem( QString studyUID , QString AETitle )
{
    int index = 0;
    bool stop = false;
    //busquem l'estudi a la que pertany la sèrie
    QList<QTreeWidgetItem*> qStudyList( m_studyTreeView->findItems(studyUID, Qt::MatchExactly, 11) );

    while ( !stop && index < qStudyList.count() )
    {
        if ( qStudyList.at( index )->text( 10 ) == AETitle )
        {
            stop = true;
        }
        else index++;
    }

    if ( stop )
    {
        return qStudyList.at( index );
    }
    else return NULL;
}

QString QStudyTreeWidget::getSelectedImageUID()
{
    if ( m_studyTreeView->currentItem() != NULL )
    {
        if ( m_studyTreeView->currentItem()->text( 12 ) == "IMAGE" )
        {
            return m_studyTreeView->currentItem()->text( 11 );
        }
        else return "";
    }
    else return "";
}

QString QStudyTreeWidget::getSelectedPacsAETitle()
{
   if ( m_studyTreeView->currentItem() == NULL ) return "";
   else return m_studyTreeView->currentItem()->text( 10 );
}

void QStudyTreeWidget::setSeriesToSeriesListWidget( QTreeWidgetItem *item )
{
    QTreeWidgetItem *child;

    if (item == NULL) return;

    emit( clearSeriesListWidget() ); //es neteja el QSeriesListWidget

    for ( int i = 0; i < item->childCount(); i++ )
    {
        child = item->child( i );
        if ( item != NULL )
        {
            DICOMSeries serie;
            serie.setSeriesUID( child->text( 11 ) );
            serie.setImageNumber( child->text( 13 ).toInt( NULL , 10 ) );
            serie.setSeriesModality( child->text( 4 ) );
            serie.setSeriesNumber( child->text( 0 ).remove( tr("Series") ) );
            serie.setStudyUID( getSelectedStudyUID() );
            serie.setProtocolName( child->text( 14 ) );
            emit( addSeries( &serie ) );
        }
    }
}

void QStudyTreeWidget::removeStudy( QString studyUID )
{
    QList<QTreeWidgetItem *> qStudyList( m_studyTreeView->findItems( studyUID , Qt::MatchExactly, 11 ) );
    QTreeWidgetItem *item;

    //Si l'estudi que anem a esborrar és el que està seleccionat el treiem del SeriesListWidget
    if ( studyUID == getSelectedStudyUID() ) emit( clearSeriesListWidget() ); //es neteja el QSeriesListWidget

    for ( int i = 0; i < qStudyList.count(); i++ )
    {
        item = qStudyList.at( i );
        delete item;
    }
}

void QStudyTreeWidget::deleteStudy()
{
    emit(delStudy()); //aquest signal es recollit per la QueryScreen
}

void QStudyTreeWidget::selectedSeriesIcon( QString seriesUID )
{
    QTreeWidgetItem *item , *current;

    /*busquem el pare (l'estudi principal), per saber quina sèrie hem d'assenyalar, en funcio
     *de quin element està actualment seleccionat haurem de pujar diferents nivells per trobar l'estudi pare*/
    if ( m_studyTreeView->currentItem()->text( 12 ) == "STUDY" )
    {//ja tenim seleccionat l'estudi pare que conté les séries
            current = m_studyTreeView->currentItem();
    }
    else if ( m_studyTreeView->currentItem()->text( 12 ) == "SERIES" )
    {//es tracta d'una sèrie la que està seleccionada en aquests moments, pujem un nivell per trobar l'estudi pare
        current = m_studyTreeView->currentItem()->parent();
    } //es tracta d'una imatge la que està seleccionada en aquests moments, pujem dos nivells per trobar l'estudi pare
    else current = m_studyTreeView->currentItem()->parent()->parent();

    /*Quan tenim l'estudi pare podem començar a recorre'l per assenyar la sèrie que ha estat seleccionada
     *en el QSeriesListWidget
     */
    for ( int i = 0; i < current->childCount(); i++ )
    {
        item = current->child( i );
        if ( item != NULL )
        {
            if (item->text( 11 ) == seriesUID )
            {
                m_studyTreeView->setItemSelected( item , true );
                m_studyTreeView->setCurrentItem( item );
            }
            else m_studyTreeView->setItemSelected( item , false );
        }
    }
}

void QStudyTreeWidget::sort()
{
    //Ordenem per la columna seleccionada
    m_studyTreeView->sortItems( m_studyTreeView->sortColumn() , Qt::AscendingOrder );
}

void QStudyTreeWidget::contextMenuEvent( QContextMenuEvent *event )
{
    m_contextMenu.exec( event->globalPos() );
}

void QStudyTreeWidget::clicked( QTreeWidgetItem *item , int )
{
    if ( item != NULL )
    {
        if ( item->text( 12 ) == "STUDY")
        {
            setSeriesToSeriesListWidget( item );
        }
        else if ( item->text( 12 ) == "SERIES" )
        {
            setSeriesToSeriesListWidget( item->parent() );
        }
        else if ( item->text( 12 ) == "IMAGE" )
        {
            setSeriesToSeriesListWidget( item->parent()->parent() );
        }
    }
}

void QStudyTreeWidget::doubleClicked( QTreeWidgetItem *item , int )
{
    //al fer doblec click al QTreeWidget ja expandeix o amaga automàticament l'objecte
    if ( item == NULL ) return;

    if ( !m_studyTreeView->isItemExpanded( item ) )
    {
        if ( item->childCount() == 0 )
        {
            if ( item->text( 12 ) == "STUDY" ) //volem expandir les series d'un estudi
            {
                item->setIcon( 0 , m_openFolder );
                emit( expandStudy( item->text(11) , item->text(10) ) );
                setSeriesToSeriesListWidget( item ); //en el cas que ja tinguem la informació de la sèrie, per passar la informació al QSeriesListWidget amb la informació de la sèrie cridarem aquest mètode
            }
            else if ( item->text( 12 ) == "SERIES" ) emit( expandSeries( getSelectedStudyUID() , item->text(11) , item->text(10) ) );
        }
    }
    else
    {
        if ( item->text(12)== "STUDY")
        {
            item->setIcon( 0 , m_closeFolder );
            emit( clearSeriesListWidget() );
        }
    }
}

void QStudyTreeWidget::viewStudy()
{
    emit(view());
}

void QStudyTreeWidget::retrieveImages()
{
    emit( retrieve() );
}

void QStudyTreeWidget::createDicomDir()
{
    emit ( convertToDicomDir( getSelectedStudyUID() ) );
}

void QStudyTreeWidget::storeStudy()
{
    emit ( storeStudyToPacs( getSelectedStudyUID() ) );
}

void QStudyTreeWidget::saveColumnsWidth()
{
    StarviewerSettings settings;


    for ( int i = 0; i < m_studyTreeView->columnCount(); i++ )
    {

        if ( m_parentName == "m_tabPacs" )
        {
            settings.setStudyPacsListColumnWidth( i , m_studyTreeView->columnWidth( i ) );
        }

        //Al haver un QSplitter el nom del Pare del TabCache és l'splitter
        if ( m_parentName == "m_StudyTreeSeriesListQSplitter" )
        {
            settings.setStudyCacheListColumnWidth( i , m_studyTreeView->columnWidth( i ) );
        }

        if ( m_parentName == "m_tabDicomdir" )
        {
            settings.setStudyDicomdirListColumnWidth( i , m_studyTreeView->columnWidth( i ) );
        }
    }
}

QStudyTreeWidget::~QStudyTreeWidget()
{
}

};
