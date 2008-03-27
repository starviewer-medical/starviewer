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
#include <QString>

#include "qstudytreewidget.h"
#include "pacslistdb.h"
#include "starviewersettings.h"
#include "studylist.h"
#include "dicomstudy.h"
#include "serieslist.h"
#include "dicomseries.h"
#include "imagelist.h"
#include "dicomimage.h"
#include "status.h"
#include "logging.h"
#include "pacsparameters.h"

namespace udg {

QStudyTreeWidget::QStudyTreeWidget( QWidget *parent )
 : QWidget( parent )
{
    setupUi( this );

    //la columna de UID AETITLE,type, Image Number i Protocol Name les fem invisibles
    m_studyTreeView->setColumnHidden( PACSAETitle, true );

    // m_studyTreeView->setColumnHidden( UID, true );

    m_studyTreeView->setColumnHidden( Type, true );
//     m_studyTreeView->setColumnHidden( ProtocolName , true );

    //carreguem les imatges que es mostren el QStudyTreeWidget
    m_openFolder = QIcon( ":/images/folderopen.png" );
    m_closeFolder = QIcon( ":/images/folderclose.png" );
    m_iconSeries = QIcon( ":/images/series.png" );

    createConnections();

    m_studyTreeView->setSelectionMode( QAbstractItemView::ExtendedSelection );
}

QStudyTreeWidget::~QStudyTreeWidget()
{
}

void QStudyTreeWidget::createConnections()
{
    connect( m_studyTreeView , SIGNAL( itemDoubleClicked( QTreeWidgetItem * , int ) ), SLOT( doubleClicked( QTreeWidgetItem * , int ) ) );
    connect( m_studyTreeView , SIGNAL( currentItemChanged( QTreeWidgetItem * , QTreeWidgetItem * ) ) , SLOT ( currentItemChanged ( QTreeWidgetItem * , QTreeWidgetItem * ) ) );
    connect( m_studyTreeView, SIGNAL( itemExpanded( QTreeWidgetItem * ) ) , SLOT ( itemExpanded( QTreeWidgetItem * ) ) );
    connect( m_studyTreeView, SIGNAL( itemCollapsed( QTreeWidgetItem * ) ) , SLOT ( itemCollapsed( QTreeWidgetItem * ) ) );
}

void QStudyTreeWidget::setColumnWidth( int columnNumber , int columnWidth )
{
    m_studyTreeView->header()->resizeSection( columnNumber , columnWidth );
}

int QStudyTreeWidget::getColumnWidth( int columnNumber )
{
    return m_studyTreeView->columnWidth( columnNumber );
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
    Status state;
    QTreeWidgetItem* item = new QTreeWidgetItem( m_studyTreeView );
    QTreeWidgetItem* expandableItem = new QTreeWidgetItem( item );

    if ( getStudyItem( study->getStudyUID() , study->getPacsAETitle() ) != NULL )
        removeStudy( study->getStudyUID() ); //si l'estudi ja hi existeix a StudyTreeView l'esborrem

    item->setIcon( ObjectName, m_closeFolder );
    item->setText( ObjectName , study->getPatientName() );
    item->setText( PatientID , study->getPatientId() );
    item->setText( PatientAge , formatAge( study->getPatientAge() ) );
    item->setText( Modality , study->getStudyModality() );
    item->setText( Description , study->getStudyDescription() );
    item->setText( Date , formatDate( study->getStudyDate() ) );
    item->setText( Time , formatHour( study->getStudyTime() ) );
    item->setText( StudyID, tr("Study %1").arg( study->getStudyId() ) );

    if ( study->getInstitutionName().isEmpty() ) //si la informació ve buida l'anem a buscar a la bdd local
    {
        if ( m_oldPacsAETitle != study->getPacsAETitle() ) //comparem que no sigui el mateix pacs que l'anterior, si es el mateix tenim la informacio guardada
        {//si es un pacs diferent busquem la informacio
            PacsListDB pacsList;
            PacsParameters pacs;

            state = pacsList.queryPacs( &pacs , study->getPacsAETitle() );
            if ( state.good() )
            {
                item->setText( Institution, pacs.getInstitution() );
                m_OldInstitution = pacs.getInstitution();
            }
            m_oldPacsAETitle = study->getPacsAETitle();
        }
        else item->setText( Institution , m_OldInstitution );
    }
    else item->setText( Institution , study->getInstitutionName() );

    item->setText( AccNumber , study->getAccessionNumber() );
    item->setText( PACSAETitle , study->getPacsAETitle() );
    item->setText( UID , study->getStudyUID() );
    item->setText( Type , "STUDY" );//indiquem de que es tracta d'un estudi
    item->setText( RefPhysName , study->getReferringPhysiciansName() );

    /* degut que per cada item estudi tenim items fills que són series, i que consultar les series per cada estudi és 
       una operació costosa (per exemple quan es consulta al pacs) només inserirem les sèries per a que les pugui 
       consultar l'usuari quan es facin un expand d'estudi, però per a que apareixi el botó "+" de desplegar l'estudi inserim un item en blanc
     */
    expandableItem->setText( Type , "EXPANDABLE_ITEM" );

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
    QTreeWidgetItem *item, *studyItem , *expandableItem;

    studyItem = getStudyItem( serie->getStudyUID() , serie->getPacsAETitle() );
    item = new QTreeWidgetItem( studyItem );
    expandableItem = new QTreeWidgetItem( item );

    item->setIcon(ObjectName, m_iconSeries);
    item->setText(ObjectName, tr( "Series %1" ).arg(serie->getSeriesNumber()) );
    item->setText(Modality, serie->getSeriesModality() );

    item->setText( Description , serie->getSeriesDescription().simplified() );//treiem els espaics en blanc del davant i darrera

    //si no tenim data o hora de la sèrie mostrem la de l'estudi
    if ( !serie->getSeriesDate().isEmpty() ) item->setText( Date , formatDate(serie->getSeriesDate() ) );

    if ( !serie->getSeriesTime().isEmpty() ) item->setText( Time , formatHour(serie->getSeriesTime() ) );

    item->setText( PACSAETitle, serie->getPacsAETitle() );
    item->setText( UID , serie->getSeriesUID() );
    item->setText( Type , "SERIES" ); //indiquem que es tracta d'una sèrie

    item->setText( ProtocolName , serie->getProtocolName() );
    item->setText( PPStartDate , serie->getPPSStartDate() );
    item->setText( PPStartTime , serie->getPPStartTime() );
    item->setText( ReqProcID , serie->getRequestedProcedureID() );
    item->setText( SchedProcStep , serie->getScheduledProcedureStepID() );

    /* degut que per cada item serie tenim items fills que són imatges, i que consultar les imatges per cada sèrie és 
       una operació costosa (per exemple quan es consulta al pacs) només inserirem les sèries per a que les pugui 
       consultar l'usuari quan es facin un expand de la sèrie, però per a que apareixi el botó "+" de desplegar la sèrie inserim un item en blanc
     */
    expandableItem->setText( Type , "EXPANDABLE_ITEM" );
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
    QTreeWidgetItem* studyItem, *item;
    bool stop = false;
    int index = 0;

    studyItem = getStudyItem( image->getStudyUID() , image->getPacsAETitle() );

    while ( !stop && index < studyItem->childCount() )//cerquem la sèrie de la que depen la imatge
    {
        if ( studyItem->child( index )->text( UID ) == image->getSeriesUID() )
        {
            stop = true;
        }
        else index++;
    }

    item = new QTreeWidgetItem( studyItem->child( index ) );

    item->setIcon( ObjectName, m_iconSeries );
    item->setText( ObjectName , tr( "Image %1" ).arg(image->getImageNumber()) );

    item->setText( PACSAETitle , image->getPacsAETitle() );
    item->setText( UID , image->getSOPInstanceUID() );
    item->setText( Type, "IMAGE" ); //indiquem que es tracta d'una imatge
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

QString QStudyTreeWidget::getCurrentStudyUID()
{
    if ( m_studyTreeView->currentItem() != NULL )
    {
        if ( isItemStudy( m_studyTreeView->currentItem() ) ) //es un estudi
        {
            return m_studyTreeView->currentItem()->text( UID );
        }
        else if ( isItemSeries( m_studyTreeView->currentItem() ) )
        {
            return m_studyTreeView->currentItem()->parent()->text( UID );
        }
        else if ( isItemImage( m_studyTreeView->currentItem() ) )
        {
            return m_studyTreeView->currentItem()->parent()->parent()->text( UID );
        }
        else return "";
    }
    else return "";
}

QStringList QStudyTreeWidget::getSelectedStudiesUID()
{
    QStringList result;
    QList<QTreeWidgetItem *> selectedItems = m_studyTreeView->selectedItems();
    foreach( QTreeWidgetItem *item, selectedItems )
    {
        if( isItemStudy( item ) ) //es un estudi
        {
            if( !result.contains( item->text(UID) ) )
                result << item->text(UID);
        }
        else if( isItemSeries( item ) )
        {
            if( !result.contains( item->parent()->text(UID) ) )
                result << item->parent()->text(UID);
        }
        else if( isItemImage( item ) )
        {
            if( !result.contains( item->parent()->parent()->text(UID) ) )
                result << item->parent()->parent()->text(UID);
        }
        else
        {
            DEBUG_LOG("Texte no esperat: " + item->text(Type) );
        }
    }
    return result;
}

QStringList QStudyTreeWidget::getStudySelectedSeriesUIDFromSelectedStudies( QString studyUID )
{
    QStringList result;
    QList<QTreeWidgetItem *> selectedItems = m_studyTreeView->selectedItems();
    foreach( QTreeWidgetItem *item, selectedItems )
    {
        // només mirem les series
        if( isItemSeries( item ) )
        {
            // si aquesta serie seleccionada pertany a l'estudi que demanem
            if( item->parent()->text( UID ) == studyUID )
            {
                result << item->text(UID); // afegim el SERIES UID
            }
        }
    }
    return result;
}

QStringList QStudyTreeWidget::getStudySelectedImagesUIDFromSelectedStudies( QString studyUID )
{
    QStringList result;
    QList<QTreeWidgetItem *> selectedItems = m_studyTreeView->selectedItems();
    foreach( QTreeWidgetItem *item, selectedItems )
    {
        // només mirem les series
        if( isItemImage( item ) )
        {
            // si aquesta imatge seleccionada pertany a l'estudi que demanem
            if( item->parent()->parent()->text( UID ) == studyUID )
            {
                result << item->text(UID); // afegim l'Image UID (SOPInstance...)
            }
        }

    }
    return result;
}

QString QStudyTreeWidget::getCurrentSeriesUID()
{
    if ( m_studyTreeView->currentItem() != NULL )
    {
        if ( isItemSeries( m_studyTreeView->currentItem() ) )
        {
            return m_studyTreeView->currentItem()->text( UID );
        }
        else if ( isItemImage( m_studyTreeView->currentItem() ) )
        {
            return m_studyTreeView->currentItem()->parent()->text( UID );
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
    QList<QTreeWidgetItem*> qStudyList( m_studyTreeView->findItems(studyUID, Qt::MatchExactly, UID) );

    while ( !stop && index < qStudyList.count() )
    {
        if ( qStudyList.at( index )->text( PACSAETitle ) == AETitle )
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

QString QStudyTreeWidget::getCurrentImageUID()
{
    QString result;

    if ( m_studyTreeView->currentItem() != NULL )
    {
        if ( isItemImage( m_studyTreeView->currentItem()  ) )
        {
            result = m_studyTreeView->currentItem()->text( UID );
        }
    }

    return result;
}

QString QStudyTreeWidget::getStudyPACSAETitleFromSelectedStudies( QString studyUID )
{
    QString result;
    QList<QTreeWidgetItem *> selectedItems = m_studyTreeView->selectedItems();
    foreach( QTreeWidgetItem *item, selectedItems )
    {
        if( item->text(UID) == studyUID )
        {
            result = item->text(PACSAETitle);
            break;
        }
    }
    return result;
}

void QStudyTreeWidget::removeStudy( QString studyUID )
{
    QList<QTreeWidgetItem *> qStudyList( m_studyTreeView->findItems( studyUID , Qt::MatchExactly, UID ) );
    QTreeWidgetItem *item;

    for ( int i = 0; i < qStudyList.count(); i++ )
    {
        item = qStudyList.at( i );
        delete item;
    }
}

void QStudyTreeWidget::setCurrentSeries( QString seriesUID )
{
    //busquem l'estudi a la que pertany la sèrie
    QList<QTreeWidgetItem*> qStudyList( m_studyTreeView->findItems( seriesUID , Qt::MatchRecursive , UID) );

    //Només hauria de trobar una sèrie amb el mateix UID, sinó tindrem una inconsistència de DICOM, el series UID ha de ser únic
    if ( qStudyList.count() > 0) m_studyTreeView->setCurrentItem ( qStudyList.at( 0 ) ); 
}

void QStudyTreeWidget::sort()
{
    //Ordenem per la columna seleccionada
    m_studyTreeView->sortItems( m_studyTreeView->sortColumn() , Qt::AscendingOrder );
}

void QStudyTreeWidget::setContextMenu(QMenu * contextMenu)
{
    m_contextMenu = contextMenu;
}

int QStudyTreeWidget::getNumberOfColumns()
{
    return m_studyTreeView->columnCount();
}

void QStudyTreeWidget::contextMenuEvent( QContextMenuEvent *event )
{
    if ( !m_studyTreeView->selectedItems().isEmpty() )
        m_contextMenu->exec( event->globalPos() );
}

void QStudyTreeWidget::currentItemChanged( QTreeWidgetItem * current, QTreeWidgetItem * )
{
    if ( current != NULL )
    {
        if ( getCurrentStudyUID() != m_oldCurrentStudyUID )
        {
            emit( currentStudyChanged( ) );
            m_oldCurrentStudyUID = getCurrentStudyUID();
        }

        if ( getCurrentSeriesUID() != m_oldCurrentSeriesUID )
        {
            emit( currentSeriesChanged( ) );
            m_oldCurrentSeriesUID = getCurrentSeriesUID();
        }

        emit( currentImageChanged() ); //sempre que canviem d'element segur que canviem d'imatge
    }
}

void QStudyTreeWidget::itemExpanded( QTreeWidgetItem *itemExpanded )
{
    /* El QTreeWidget després de fer doble click expandeix o col·lapsa l'item en funció del seu estat, a nosaltres no ens interessa
     * que es faci això, per aquest motiu en cas d'un signal de collapse o expand, el que fem és comprovar si per aquell item s'acaba 
     *de fer doble click, si és així anul·lem l'acció de col·lapsar o expandir
     */
 
    if ( m_doubleClickedItemUID != itemExpanded->text( UID ) )
    {
        /* Com que inserim un item en blanc per simular fills dels estudis i de les sèries cada vegada que ens fan un expand hem d'eliminar l'item en blanc i 
        * emetem un signal per a que qui el reculli s'encarregui de fer els passos corresponents per expandir l'estudi o imatge amb el seus fills pertinents
        */
        foreach( QTreeWidgetItem * childItem , itemExpanded->takeChildren() )
        {
            delete childItem;
        }
    
        if ( isItemStudy( itemExpanded ) )
        {
            emit ( studyExpanded( itemExpanded->text( UID ) , itemExpanded->text( PACSAETitle ) ) );
            itemExpanded->setIcon( ObjectName, m_openFolder );//canviem la icona per la de carpeta oberta quan l'item està expanded
        }
        else if ( isItemSeries( itemExpanded ) ) emit( seriesExpanded( itemExpanded->parent()->text( UID ) , itemExpanded->text( UID ) , itemExpanded->text( PACSAETitle ) ) );

        m_doubleClickedItemUID = "";
    }
    else
    {//si s'ha fet un doble click a l'item hem d'anul·lar l'acció que ha fet qt de col·lapsar l'item per tan nosaltres el tornem a expandir
        m_doubleClickedItemUID = "";//Molt important fer-lo abans de fer collapseItem, sinó entrariem en bucle pq s'emetria signal de collapseItem
        m_studyTreeView->collapseItem( itemExpanded );
    }

}

void QStudyTreeWidget::itemCollapsed( QTreeWidgetItem *itemCollapsed )
{
    /* El QTreeWidget després de fer doble click expandeix o col·lapsa l'item en funció del seu estat, a nosaltres no ens interessa
     * que es faci això, per aquest motiu en cas d'un signal de collapse o expand, el que fem és comprovar si per aquell item s'acaba 
     *de fer doble click, si és així anul·lem l'acció de col·lapsar o expandir
     */
    if ( m_doubleClickedItemUID != itemCollapsed->text( UID ) ) // si l'item col·lapsat no se li acaba de fer un doble click
    {
        //Si és una estudi està collapsed, canviem la icona per la carpeta tancada
        if ( isItemStudy( itemCollapsed ) ) itemCollapsed->setIcon( ObjectName, m_closeFolder );

        m_doubleClickedItemUID = "";
    }
    else 
    {//si s'ha fet un doble click a l'item hem d'anul·lar l'acció que ha fet qt de col·lapsar l'item per tan nosaltres el tornem a expandir
        m_doubleClickedItemUID = "";//Molt important fer-lo abans de fer expandItem, sinó entrariem en bucle, pq s'emetria signal d'expandItem
        m_studyTreeView->expandItem ( itemCollapsed );
    }
    
}

void QStudyTreeWidget::doubleClicked( QTreeWidgetItem *item , int )
{
    //al fer doblec click al QTreeWidget ja expandeix o amaga automàticament l'objecte
    if ( item == NULL ) return;

    if ( isItemStudy( item ) ) emit( studyDoubleClicked() );
    else if ( isItemSeries( item ) ) emit( seriesDoubleClicked() );
    else if ( isItemImage( item) ) emit( imageDoubleClicked() );

    /*Pel comportament del tree widget quan es fa un un doble click es col·lapsa o expandeix l'item en funció del seu estat, com que 
     *nosaltres pel doble click no volem que s'expendeixi o es col·lapsi, guardem per quin element s'ha fet el doble click, per anul·laro quan es detecti un signal d'expand o collapse item
     */
    m_doubleClickedItemUID = item->text( UID );
}

bool QStudyTreeWidget::isItemStudy( QTreeWidgetItem *item )
{
    return item->text( Type ) == "STUDY";
}

bool QStudyTreeWidget::isItemSeries( QTreeWidgetItem *item )
{
    return item->text( Type ) == "SERIES";
}

bool QStudyTreeWidget::isItemImage( QTreeWidgetItem *item )
{
    return item->text( Type ) == "IMAGE";
}

};
