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

namespace udg {

QStudyTreeWidget::QStudyTreeWidget( QWidget *parent )
 : QWidget( parent ), m_viewAction(NULL), m_retrieveAction(NULL), m_deleteStudyAction(NULL), m_sendToDICOMDIRListAction(NULL), m_storeStudyAction(NULL)
{
    setupUi( this );

    //la columna de UID AETITLE,type, Image Number i Protocol Name les fem invisibles
    m_studyTreeView->setColumnHidden( PACSAETitle, true );

    // m_studyTreeView->setColumnHidden( UID, true );

    m_studyTreeView->setColumnHidden( Type, true );
    m_studyTreeView->setColumnHidden( ImageNumber, true );
//     m_studyTreeView->setColumnHidden( ProtocolName , true );

    //carreguem les imatges que es mostren el QStudyTreeWidget
    m_openFolder = QIcon( ":/images/folderopen.png" );
    m_closeFolder = QIcon( ":/images/folderclose.png" );
    m_iconSeries = QIcon( ":/images/series.png" );

    m_parentName = parent->objectName();//el guardem per saber si es tracta de la llista d'estudis del Pacs o la Cache
    createContextMenu(); //creem el menu contextual
    createConnections();

    setWidthColumns();//s'assigna a les columnes l'amplada definida per l'usuari
    m_studyTreeView->setSelectionMode( QAbstractItemView::ExtendedSelection );
}

QStudyTreeWidget::~QStudyTreeWidget()
{
}

void QStudyTreeWidget::createConnections()
{
    connect( m_studyTreeView , SIGNAL( itemClicked ( QTreeWidgetItem * , int ) ), SLOT( clicked ( QTreeWidgetItem * , int ) ) );
    connect( m_studyTreeView , SIGNAL( itemDoubleClicked ( QTreeWidgetItem * , int ) ), SLOT( doubleClicked( QTreeWidgetItem * , int ) ) );
}

void QStudyTreeWidget::createContextMenu()
{
    //acció veure
    m_viewAction = m_contextMenu.addAction( tr("&View") );
    m_viewAction->setShortcut( tr("Ctrl+V") );
    m_viewAction->setIcon( QIcon(":/images/view.png") );
    connect( m_viewAction , SIGNAL( triggered() ) , SIGNAL( view() ) );
    // TODO per alguna raó desconeguda, QT ignora els shortCut especificats a través de QAction, i fem aquest workaround perquè funcionin. Mirar com solucionar-ho
    (void) new QShortcut( m_viewAction->shortcut() , this , SIGNAL( view() ) );

    // només hi ha RETRIEVE en el PACS i DICOMDIR TODO a DICOMDIR s'hauria de dir IMPORT to DATABASE... per ser mes correctes
    // TODO AL TANTO AMB AIXÒ!!!!!!! SI MAI CANVIA EL PARENT QUÈ PASSA???????????? FER SISTEMA PER DETERMINAR EL TIPUS DE TAB QUE ÉS O ALGO
    if( m_parentName == "m_tabPacs" || m_parentName == "m_tabDicomdir" )
    {
        m_retrieveAction = m_contextMenu.addAction( tr("&Retrieve") );
        m_retrieveAction->setShortcut( tr("Ctrl+R") );
        m_retrieveAction->setIcon( QIcon(":/images/retrieve.png") );
        connect( m_retrieveAction , SIGNAL( triggered() ) , SIGNAL( retrieve() ) );
        // TODO per alguna raó desconeguda, QT ignora els shortCut especificats a través de QAction, i fem aquest workaround perquè funcionin. Mirar com solucionar-ho
        (void) new QShortcut( m_retrieveAction->shortcut() , this , SIGNAL( retrieve() ) );
    }

    // Al haver un QSplitter el nom del Pare del TabCache és l'splitter TODO molt al tanto amb aquesta guarrada -_-¡
    if( m_parentName == "m_StudyTreeSeriesListQSplitter" )
    {
        // només es pot esborrar a local
        m_deleteStudyAction = m_contextMenu.addAction( tr("&Delete")) ;
        m_deleteStudyAction->setShortcut( Qt::Key_Delete );
        m_deleteStudyAction->setIcon( QIcon(":/images/databaseRemove.png") );
        connect( m_deleteStudyAction , SIGNAL( triggered() ), SIGNAL( deleteSelectedStudies() ) );
        (void) new QShortcut( m_deleteStudyAction->shortcut() , this , SIGNAL( deleteSelectedStudies() ) );

        //nomes es pot afegir element a la llista de DICOMDIR desde local
        m_sendToDICOMDIRListAction = m_contextMenu.addAction( tr( "Send to DICOMDIR List" ) );
        m_sendToDICOMDIRListAction->setShortcut( tr( "Ctrl+M" ) );
        connect( m_sendToDICOMDIRListAction , SIGNAL ( triggered() ) , this , SLOT ( createDicomDir() ) );
        (void) new QShortcut( m_sendToDICOMDIRListAction->shortcut() , this , SLOT( createDicomDir() ) );

        // només es pot fer store des de local
        m_storeStudyAction = m_contextMenu.addAction( tr( "Store to PACS" ) );
        m_storeStudyAction->setShortcut( tr( "Ctrl+S" ) );
        m_storeStudyAction->setIcon( QIcon(":/images/store.png") );
        connect( m_storeStudyAction , SIGNAL ( triggered() ) , this , SLOT ( storeStudies() ) );
        (void) new QShortcut( m_storeStudyAction->shortcut() , this , SLOT( storeStudies() ) );

        // TODO per alguna raó desconeguda, QT ignora els shortCut especificats a través de QAction, i fem aquest workaround perquè funcionin. Mirar com solucionar-ho
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
    Status state;

    if ( getStudyItem( study->getStudyUID() , study->getPacsAETitle() ) != NULL )
        removeStudy( study->getStudyUID() ); //si l'estudi ja hi existeix a StudyTreeView l'esborrem

    QTreeWidgetItem* item = new QTreeWidgetItem( m_studyTreeView );

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
        else
            item->setText( Institution , m_OldInstitution );
    }
    else
        item->setText( Institution , study->getInstitutionName() );

    item->setText( AccNumber , study->getAccessionNumber() );
    item->setText( PACSAETitle , study->getPacsAETitle() );
    item->setText( UID , study->getStudyUID() );
    item->setText( Type , "STUDY" );//indiquem de que es tracta d'un estudi
    item->setText( ImageNumber , "" );
    item->setText( RefPhysName , study->getReferringPhysiciansName() );

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
    QTreeWidgetItem *item, *studyItem;

    studyItem = getStudyItem( serie->getStudyUID() , serie->getPacsAETitle() );
    item = new QTreeWidgetItem( studyItem );

    item->setIcon(ObjectName, m_iconSeries);
    item->setText(ObjectName, tr( "Series %1" ).arg(serie->getSeriesNumber()) );
    item->setText(Modality, serie->getSeriesModality() );

    item->setText( Description , serie->getSeriesDescription().simplified() );//treiem els espaics en blanc del davant i darrera

    //si no tenim data o hora de la sèrie mostrem la de l'estudi
    if ( !serie->getSeriesDate().isEmpty() )
    {
        item->setText( Date , formatDate(serie->getSeriesDate() ) );
    }

    if ( !serie->getSeriesTime().isEmpty() )
    {
        item->setText( Time , formatHour(serie->getSeriesTime() ) );
    }

    item->setText( PACSAETitle, serie->getPacsAETitle() );
    item->setText( UID , serie->getSeriesUID() );
    item->setText( Type , "SERIES" ); //indiquem que es tracta d'una sèrie

    item->setText( ImageNumber , QString::number(serie->getImageNumber()) );
    item->setText( ProtocolName , serie->getProtocolName() );
    item->setText( PPStartDate , serie->getPPSStartDate() );
    item->setText( PPStartTime , serie->getPPStartTime() );
    item->setText( ReqProcID , serie->getRequestedProcedureID() );
    item->setText( SchedProcStep , serie->getScheduledProcedureStepID() );

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
    QTreeWidgetItem* studyItem, *item;
    bool stop = false;
    int index = 0;

    studyItem = getStudyItem( image->getStudyUID() , image->getPacsAETitle() );

    while ( !stop && index < studyItem->childCount() )
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

QString QStudyTreeWidget::getSelectedStudyUID()
{
    if ( m_studyTreeView->currentItem() != NULL )
    {
        if ( m_studyTreeView->currentItem()->text( Type ) == "STUDY" ) //es un estudi
        {
            return m_studyTreeView->currentItem()->text( UID );
        }
        else if ( m_studyTreeView->currentItem()->text( Type ) == "SERIES" )
        {
            return m_studyTreeView->currentItem()->parent()->text( UID );
        }
        else if ( m_studyTreeView->currentItem()->text( Type ) == "IMAGE" )
        {
            return m_studyTreeView->currentItem()->parent()->parent()->text( UID );
        }
        else
            return "";
    }
    else
        return "";
}

QStringList QStudyTreeWidget::getSelectedStudiesUID()
{
    QStringList result;
    QList<QTreeWidgetItem *> selectedItems = m_studyTreeView->selectedItems();
    foreach( QTreeWidgetItem *item, selectedItems )
    {
        if( item->text(Type) == "STUDY" ) //es un estudi
        {
            if( !result.contains( item->text(UID) ) )
                result << item->text(UID);
        }
        else if( item->text(Type) == "SERIES" )
        {
            if( !result.contains( item->parent()->text(UID) ) )
                result << item->parent()->text(UID);
        }
        else if( item->text(Type) == "IMAGE" )
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
        if( item->text( Type ) == "SERIES" )
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
        if( item->text( Type ) == "IMAGE" )
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

QString QStudyTreeWidget::getSelectedSeriesUID()
{
    if ( m_studyTreeView->currentItem() != NULL )
    {
        if ( m_studyTreeView->currentItem()->text( Type ) == "SERIES" )
        {
            return m_studyTreeView->currentItem()->text( UID );
        }
        else if ( m_studyTreeView->currentItem()->text( Type ) == "IMAGE" )
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

QString QStudyTreeWidget::getSelectedImageUID()
{
    QString result;

    if ( m_studyTreeView->currentItem() != NULL )
    {
        if ( m_studyTreeView->currentItem()->text( Type ) == "IMAGE" )
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

void QStudyTreeWidget::setSeriesToSeriesListWidget( QTreeWidgetItem *item )
{
    QTreeWidgetItem *child;

    if(item == NULL)
        return;

    emit( clearSeriesListWidget() ); //es neteja el QSeriesListWidget

    for ( int i = 0; i < item->childCount(); i++ )
    {
        child = item->child( i );
        if ( item != NULL )
        {
            DICOMSeries serie;
            serie.setSeriesUID( child->text( UID ) );
            serie.setImageNumber( child->text( ImageNumber ).toInt( NULL , 10 ) );
            serie.setSeriesModality( child->text( Modality ) );
            serie.setSeriesNumber( child->text( ObjectName ).remove( tr("Series") ) );//El numero de serie està dins l'ObjectName
            serie.setStudyUID( getSelectedStudyUID() );
            serie.setProtocolName( child->text( ProtocolName ) );
            emit( addSeries( &serie ) );
        }
    }
}

void QStudyTreeWidget::removeStudy( QString studyUID )
{
    QList<QTreeWidgetItem *> qStudyList( m_studyTreeView->findItems( studyUID , Qt::MatchExactly, UID ) );
    QTreeWidgetItem *item;

    //Si l'estudi que anem a esborrar és el que està seleccionat el treiem del SeriesListWidget
    if ( studyUID == getSelectedStudyUID() ) emit( clearSeriesListWidget() ); //es neteja el QSeriesListWidget

    for ( int i = 0; i < qStudyList.count(); i++ )
    {
        item = qStudyList.at( i );
        delete item;
    }
}

void QStudyTreeWidget::selectedSeriesIcon( QString seriesUID )
{
    QTreeWidgetItem *item , *current;

    /*busquem el pare (l'estudi principal), per saber quina sèrie hem d'assenyalar, en funcio
     *de quin element està actualment seleccionat haurem de pujar diferents nivells per trobar l'estudi pare*/
    if ( m_studyTreeView->currentItem()->text( Type ) == "STUDY" )
    {//ja tenim seleccionat l'estudi pare que conté les séries
            current = m_studyTreeView->currentItem();
    }
    else if ( m_studyTreeView->currentItem()->text( Type ) == "SERIES" )
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
            if (item->text( UID ) == seriesUID )
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
    if ( !m_studyTreeView->selectedItems().isEmpty() )
        m_contextMenu.exec( event->globalPos() );
}

void QStudyTreeWidget::clicked( QTreeWidgetItem *item , int )
{
    if ( item != NULL )
    {
        if ( item->text( Type ) == "STUDY")
        {
            setSeriesToSeriesListWidget( item );
        }
        else if ( item->text( Type ) == "SERIES" )
        {
            setSeriesToSeriesListWidget( item->parent() );
        }
        else if ( item->text( Type ) == "IMAGE" )
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
            if ( item->text( Type ) == "STUDY" ) //volem expandir les series d'un estudi
            {
                item->setIcon( ObjectName , m_openFolder );
                emit( expandStudy( item->text(UID) , item->text(PACSAETitle) ) );
                setSeriesToSeriesListWidget( item ); //en el cas que ja tinguem la informació de la sèrie, per passar la informació al QSeriesListWidget amb la informació de la sèrie cridarem aquest mètode
            }
            else if ( item->text( Type ) == "SERIES" )
                emit( expandSeries( getSelectedStudyUID() , item->text(UID) , item->text(PACSAETitle) ) );
        }
    }
    else
    {
        if ( item->text(Type)== "STUDY")
        {
            item->setIcon(ObjectName, m_closeFolder);
            emit( clearSeriesListWidget() );
        }
    }
}

void QStudyTreeWidget::createDicomDir()
{
    emit ( convertToDicomDir( getSelectedStudiesUID() ) );
}

void QStudyTreeWidget::storeStudies()
{
    emit ( storeStudiesToPacs( getSelectedStudiesUID() ) );
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

};
