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
#include "study.h"
#include "series.h"
#include "qserieslistwidget.h"
#include "pacslistdb.h"
#include "starviewersettings.h"
#include "studylist.h"

namespace udg {

QStudyTreeWidget::QStudyTreeWidget( QWidget *parent )
 : QWidget( parent )
{
    setupUi( this );
    
    m_studyTreeView->setRootIsDecorated( false );
    
    //la columna de UID AETITLE,type, Image Number i Protocol Name les fem invisibles
    m_studyTreeView->setColumnHidden( 10, true );
    m_studyTreeView->setColumnHidden( 11, true );
    m_studyTreeView->setColumnHidden( 12, true );
    m_studyTreeView->setColumnHidden( 13, true );
    m_studyTreeView->setColumnHidden( 14 , true );
    
    //carreguem les imatges que es mostren el QStudyTreeWidget, el :/ indica que és un QRecourse, que s'especifica al main.qrc
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
    QAction *view = m_contextMenu.addAction( tr("&View") );
    view->setShortcut( tr("Ctrl+V") );
    //acció descarregar
    QAction *retrieve = m_contextMenu.addAction( tr("&Retrieve") );
    retrieve->setShortcut( tr("Ctrl+R") );
    //acció esborrar
    m_contextMenu.addSeparator();
    QAction *deleteStudy =  m_contextMenu.addAction( tr("&Delete")) ;
    deleteStudy->setShortcut( tr("Ctrl+D") );
      
    connect( view , SIGNAL( triggered() ) , this , SLOT( viewStudy() ) );
    connect( retrieve , SIGNAL( triggered() ) , this , SLOT( retrieveStudy() ) );
    connect( deleteStudy , SIGNAL(triggered()), this, SLOT(deleteStudy()));
      
    /*QT ignora els shortCut, especificats a través de QAction, per això per fer que els shortCut funcionin els haig de fer aquesta xapussa redefini aquí com QShortcut*/
    (void) new QShortcut( deleteStudy->shortcut() , this , SLOT( deleteStudy() ) );  
    (void) new QShortcut( view->shortcut() , this , SLOT( viewStudy() ) );
    (void) new QShortcut( retrieve->shortcut() , this , SLOT( retrieveStudy() ) );
    
    if (m_parentName == "m_tabPacs")
    {   //si el QStudyTreeWidget es el que mostra la llista d'estudis del PACS, la opcio delete desactivada
        deleteStudy->setEnabled(false);       
    }   
   
    if (m_parentName == "m_tabCache")
    {   //si el QStudyTreeWidget es el que mostra la llista d'estudis a la caché, la opció retrieve es desactiva
        retrieve->setEnabled(false);
    }
}

void QStudyTreeWidget::setWidthColumns()
{    
    StarviewerSettings settings;
    
    //Renombrem segons quin objecte es tracti, si es el que mostra la llista d'estudis del PACS o de la cache
    if (m_parentName == "m_tabPacs")
    { 
        for ( int i = 0; i < m_studyTreeView->columnCount(); i++ )
        {
            m_studyTreeView->header()->resizeSection( i , settings.getStudyPacsListColumnWidth( i ) );
        }    
    }   
   
    if (m_parentName == "m_tabCache")
    {
        for ( int i = 0; i < m_studyTreeView->columnCount(); i++ )
        {
            m_studyTreeView->header()->resizeSection( i ,settings.getStudyCacheListColumnWidth(i) );
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

void QStudyTreeWidget::insertStudy( Study *study)
{
    QString text;
    Status state;
    
    QTreeWidgetItem* item = new QTreeWidgetItem( m_studyTreeView );
    text.truncate( 0 );
    text.append( tr("Study") );
    text.append( study->getStudyId().c_str() );
    item->setIcon( 0 ,m_closeFolder );
    item->setText( 0 , text );
    
    item->setText( 1 , study->getPatientId().c_str() );
    item->setText( 2 , formatName( study->getPatientName() ) );
    item->setText( 3 , formatAge( study->getPatientAge() ) );
    item->setText( 4 , study->getStudyModality().c_str() );
    item->setText( 5 , study->getStudyDescription().c_str() );
    item->setText( 6 , formatDate( study->getStudyDate() ) );
    item->setText( 7 , formatHour( study->getStudyTime() ) );
    
    if ( study->getInstitutionName() == "" ) //si la informació ve buida l'anem a buscar a la bdd local
    {
        if ( m_oldPacsAETitle != study->getPacsAETitle().c_str() ) //comparem que no sigui el mateix pacs que l'anterior, si es el mateix tenim la informacio guardada
        {//si es un pacs diferent busquem la informacio
            PacsListDB pacsList;
            PacsParameters pacs;
            
            state = pacsList.queryPacs( &pacs , study->getPacsAETitle() );
            if ( state.good() )
            {
                item->setText( 8 , pacs.getInstitution().c_str() );
                m_OldInstitution = pacs.getInstitution().c_str();
            }
            m_oldPacsAETitle = study->getPacsAETitle().c_str();
        }
        else item->setText( 8 , m_OldInstitution );
    }
    else item->setText( 8 , study->getInstitutionName().c_str() );
    
    item->setText( 9 , study->getAccessionNumber().c_str() );
    item->setText( 10 , study->getPacsAETitle().c_str() );
    item->setText( 11 , study->getStudyUID().c_str() );
    item->setText( 12 , "STUDY" );//indiquem de que es tracta d'un estudi
    item->setText( 13 , "" );
    
    m_studyTreeView->clearSelection();
}

void QStudyTreeWidget::insertSeries( Series *serie )
{
    QString text, description;
  
    QTreeWidgetItem* item = new QTreeWidgetItem( m_studyTreeView->currentItem() );
    text.truncate( 0 );
    text.append( tr( "Series " ) );
    text.append( serie->getSeriesNumber().c_str() );
    item->setIcon( 0 , m_iconSeries ); 
    item->setText( 0 , text );
    item->setText( 4 , serie->getSeriesModality().c_str() );
    
    description = serie->getSeriesDescription().c_str();
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
    
    item->setText( 11 , serie->getSeriesUID().c_str() );   
    item->setText( 12 , "SERIES" ); //indiquem que es tracta d'una sèrie 
    
    text.truncate( 0 );
    text.setNum( serie->getImageNumber(), 10 );
    item->setText( 13 , text );
    item->setText( 14 , serie->getProtocolName().c_str() );
    
    emit( addSeries(serie) );//afegim serie al SeriesIconView
}

QString QStudyTreeWidget::formatAge( const std::string age )
{
    QString text( age.c_str() );
    
    if (text.length() > 0)
    {
        if (text.at(0) == '0') 
        {//treiem el 0 de davant els anys, el PACS envia per ex: 047Y nosaltes tornem 47Y
            text.replace(0,1," ");
        } 
    }
        
    return text;
}

QString QStudyTreeWidget::formatName( const std::string name )
{
    QString text( name.c_str() );
    
    text.replace("^"," ");
    
    return text;
}

QString QStudyTreeWidget::formatDate( const std::string date )
{
    QString formateDate , originalDate ( date.c_str() );
    
    formateDate.insert( 0 , originalDate.mid( 6 , 2 ) ); //dd
    formateDate.append( "/" );
    formateDate.append( originalDate.mid( 4 , 2 ) );
    formateDate.append( "/" );
    formateDate.append( originalDate.mid( 0 , 4 ) );
    
    return formateDate;
}

QString QStudyTreeWidget::formatHour( const std::string hour )
{
    QString formatedHour,originalHour( hour.c_str() );
    
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
    QTreeWidgetItem *item;

    if ( m_studyTreeView->currentItem() != NULL ) 
    {
        if ( m_studyTreeView->currentItem()->parent() == NULL ) //es un estudi
        {
            return m_studyTreeView->currentItem()->text( 11 );   
        }
        else
        {
            item = m_studyTreeView->currentItem()->parent();
            return item->text( 11 );
        }
    }
    else return "";
}

QString QStudyTreeWidget::getSelectedSeriesUID()
{
    if ( m_studyTreeView->currentItem() != NULL ) 
    {
        if ( m_studyTreeView->currentItem()->parent() != NULL ) //es un serie
        {
            return m_studyTreeView->currentItem()->text( 11 );   
        }
        else
        {
            return "";
        }
    }
    else return "";
}

QString QStudyTreeWidget::getSelectedStudyPacsAETitle()
{
   QTreeWidgetItem * item;
   
   if ( m_studyTreeView->currentItem() == NULL ) return "";
    
   if ( m_studyTreeView->currentItem()->parent() == NULL )
   {
       return m_studyTreeView->currentItem()->text( 10 );
   }
   else 
   {  
       item = m_studyTreeView->currentItem()->parent();
       return item->text( 10 );
   }
}

//SLOT CONNECTAT AMB EL SIGNAL D'UN CLICK
void QStudyTreeWidget::expand( QTreeWidgetItem * item )
{
    if ( item == NULL ) return;

    if ( !m_studyTreeView->isItemExpanded( item ) )
    {    
        if ( item->text( 12 ) == "STUDY" ) //nomes s'expandeix si es tracta d'un estudi
        {
            m_studyTreeView->setItemExpanded( item , true );
            item->setIcon( 0 , m_openFolder ); 
            if ( item->childCount() == 0 ) //si abans hem consultat le seria ja hi tenim la seva informació, evitem d'haver de consultar el pacs cada vegada
            {
                emit( expand( item->text(11) , item->text(10) ) );
            }
            else setSeriesToSeriesListWidget( item ); //en el cas que ja tinguem la informació de la sèrie, per passar la informació al QSeriesListWidget amb la informació de la sèrie cridarem aquest mètode
            m_oldStudyUID = getSelectedStudyUID();
        }
    }
    else 
    {
        if ( item->text(12)== "STUDY" ) //nomes s'expandeix si es tracta d'un estudi
        {
            item->setIcon( 0 , m_closeFolder ); 
            m_studyTreeView->setItemExpanded( item , false );
            emit( clearSeriesListWidget() );
            m_oldStudyUID = getSelectedStudyUID();
        }
    }
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
            Series serie;
            serie.setSeriesUID( child->text( 11 ).toAscii().constData() );
            serie.setImageNumber( child->text( 13 ).toInt( NULL , 10 ) );
            serie.setSeriesModality( child->text( 4 ).toAscii().constData() );
            serie.setSeriesNumber( child->text( 0 ).remove( tr("Series") ).toAscii().constData() );  
            serie.setStudyUID( getSelectedStudyUID().toAscii().constData() );
            serie.setProtocolName( child->text( 14 ).toAscii().constData() );
            emit( addSeries( &serie ) );
        }
    }  
}

void QStudyTreeWidget::removeStudy( QString studyUID )
{
    QList<QTreeWidgetItem *> qStudyList( m_studyTreeView->findItems( "*" , Qt::MatchWildcard , 0 ) );
    QTreeWidgetItem *item;
    
    for ( int i = 0; i < qStudyList.count(); i++ )
    {
        item = qStudyList.at( i );
        if ( item->text( 11 ) == studyUID )
        {
            delete item;
        }
    }
}

void QStudyTreeWidget::deleteStudy()
{
    emit(delStudy()); //aquest signal es recollit per la QueryScreen
}

void QStudyTreeWidget::selectedSeriesIcon( QString seriesUID )
{
    QTreeWidgetItem *item , *current;
    
   //busquem el pare (l'estudi principal),ja que pot estar seleccionada una serie
   if ( m_studyTreeView->currentItem()->parent() == NULL )
   {
        current = m_studyTreeView->currentItem();
   }
   else 
   {  
       current = m_studyTreeView->currentItem()->parent();
   }
   
    
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

void QStudyTreeWidget::contextMenuEvent( QContextMenuEvent *event )
{
    clicked( m_studyTreeView->currentItem() ,0 );
    m_contextMenu.exec( event->globalPos() );
}

void QStudyTreeWidget::clicked( QTreeWidgetItem *item , int col )
{
    if ( item != NULL )
    {
        if ( item->parent() != NULL ) //si es tracta d'una serie
        {   //enviem el UID de la serie
            if ( m_oldStudyUID != getSelectedStudyUID() )
            { //si seleccionem una serie pero d'un altre estudi, hem d'actualizar el qserieslistwidget
                QTreeWidgetItem *parent  = item->parent();
                setSeriesToSeriesListWidget( parent );
            }
            //indiquem que el QSeriesListWidget que seleccioni la sèrie amb el UID passa per parametre
            emit( selectedSeriesList( item->text(11) ) );
        }
        else
        {
            if ( col == 0 )
            {
                expand( item );
            }
            
            if (m_oldStudyUID != getSelectedStudyUID())
            { //si seleccionem una estudi diferent el seleccionat abans, hem d'actualizar el qserieslistwidget
                if ( m_studyTreeView->isItemExpanded( item ) )
                {// ha d'estar expendit per visualitzar les series
                    setSeriesToSeriesListWidget( item );
                }
                else emit( clearSeriesListWidget() );
            }   
                 
        }
        m_oldStudyUID = getSelectedStudyUID();
    }
}

void QStudyTreeWidget::doubleClicked( QTreeWidgetItem *item , int )
{
    if ( item != NULL )
    {
        if ( item->parent() != NULL ) //si es tracta d'una serie
        {   //enviem el UID de la serie
            emit( view() );
        }
        else 
        {
            expand( item ); //es tracta d'un estudi
            //QTreeWidget automaticament al fer doble click modifica l'estat de Expanded, llavors tenim el problema que a al metode expand tambe es modifica, i al modificar-lo dos vegades, queda en el seu estat inicial, per això aquí hem de ficar un altre Expanded perquè realment l'expandeixi o amagui en funció de si es mostraven o no les series
            m_studyTreeView->setItemExpanded( item , !m_studyTreeView->isItemExpanded( item ) );
        }
    }
    
    m_oldStudyUID = getSelectedStudyUID();
}

void QStudyTreeWidget::viewStudy()
{
    emit(view());
}

void QStudyTreeWidget::retrieveStudy()
{
    emit( retrieve() );    
}

void QStudyTreeWidget::saveColumnsWidth()
{
    StarviewerSettings settings;

    if ( m_parentName == "m_tabPacs" )
    { 
        for ( int i = 0; i < m_studyTreeView->columnCount(); i++ )
        {
            settings.setStudyPacsListColumnWidth( i , m_studyTreeView->columnWidth( i ) ); 
        }    
    }   
   
    if ( m_parentName == "m_tabCache" )
    {
        for ( int i = 0; i < m_studyTreeView->columnCount(); i++ )
        {
           settings.setStudyCacheListColumnWidth( i , m_studyTreeView->columnWidth( i ) ); 
        }      
    }
}

QStudyTreeWidget::~QStudyTreeWidget()
{
}

};
