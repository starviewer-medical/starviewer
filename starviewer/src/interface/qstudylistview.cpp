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

#include "qstudylistview.h"
#include "study.h"
#include "qseriesiconview.h"
#include "pacslistdb.h"
#include "cachepacs.h"
#include "starviewersettings.h"



namespace udg {



/** Constructor de la classe
  */
QStudyListView::QStudyListView( QWidget *parent)
 : QWidget( parent )
{
    setupUi( this );
    
    m_studyTreeView->setRootIsDecorated(false);
    
    //la columna de UID i AETITLE les fem invisibles, i li indiquem que no poden canvia de tamany automàticament
    m_studyTreeView->setColumnHidden(10,true);
    m_studyTreeView->setColumnHidden(11,true);
    m_studyTreeView->setColumnHidden(12,true);
    m_studyTreeView->setColumnHidden(13,true);
    
    m_openFolder = QIcon(":/images/folderopen.png");
    m_closeFolder = QIcon(":/images/folderclose.png");
    m_iconSeries = QIcon(":/images/series.png");
    
    m_parentName = parent->name();//el guardem per saber si es tracta de la llista d'estudis del Pacs o la Cache
    
    createPopupMenu();
    createConnections();
   
    setWidthColumns();
    
}

void QStudyListView::createConnections()
{
    connect(m_studyTreeView, SIGNAL(itemClicked ( QTreeWidgetItem *, int )), this, SLOT(expand(QTreeWidgetItem *,int)));
    connect(m_studyTreeView, SIGNAL(itemClicked ( QTreeWidgetItem *, int )), this, SLOT(clicked(QTreeWidgetItem *,int)));
    
}

/** Creem el popup Menu, en funcio de a quin tab pertany activa unes o altres opcions del menu
  */
void QStudyListView::createPopupMenu()
{
    
    QAction *view = m_popUpMenu.addAction(tr("&View"));
    
    view->setShortcut(Qt::CTRL+Qt::Key_W);
    QAction *retrieve = m_popUpMenu.addAction(tr("&Retrieve"));
    retrieve->setShortcut(Qt::CTRL+Qt::Key_R);
    m_popUpMenu.addSeparator();
    QAction *deleteStudy =  m_popUpMenu.addAction(tr("&Delete"));
    deleteStudy->setShortcut(Qt::CTRL+Qt::Key_D);
      
    connect(view, SIGNAL(triggered()), this, SLOT(viewStudy()));
    connect(retrieve, SIGNAL(triggered()), this, SLOT(retrieveStudy()));
    connect(deleteStudy, SIGNAL(triggered()), this, SLOT(deleteStudy()));
      
    if (m_parentName == "m_tabPacs")
    { 
        deleteStudy->setEnabled(false);       
        view->setEnabled(false);
    }   
   
    if (m_parentName == "m_tabCache")
    {
        retrieve->setEnabled(false);
    }
}

/** Assigna l'ampla a les columnes segons els paràmetres guardats a StarviewerSettings
  */
void QStudyListView::setWidthColumns()
{    
    StarviewerSettings settings;

    if (m_parentName == "m_tabPacs")
    { 
        for (int i = 0;i < m_studyTreeView->columnCount();i++)
        {
            m_studyTreeView->header()->resizeSection(i,settings.getStudyPacsListColumnWidth(i));
        }    
    }   
   
    if (m_parentName == "m_tabCache")
    {
        for (int i = 0;i < m_studyTreeView->columnCount();i++)
        {
            m_studyTreeView->header()->resizeSection(i,settings.getStudyCacheListColumnWidth(i));
        }      
    }

}
/** Mostra l'estudi pel ListView que se li passa per paràmetre
  *        @param StudyList a mostrar
  */
void QStudyListView::insertStudyList(StudyList *ls)
{

    m_studyTreeView->clear();

    ls->firstStudy();
    
    while (!ls->end())
    {    
         insertStudy(&ls->getStudy());        
         ls->nextStudy();
    }
}

/** Inseriex la informació d'un estudi
  *                @param Dades de l'estudi
  */
void QStudyListView::insertStudy(Study *stu)
{

    QString text;
    Status state;
    
    QTreeWidgetItem* item = new QTreeWidgetItem(m_studyTreeView);
    text.truncate(0);
    text.append(tr("Study"));
    text.append(stu->getStudyId().c_str() );
    item->setIcon(0,m_closeFolder);
    item->setText(0,text);
    item->setText(1,stu->getPatientId().c_str() );
    item->setText(2,formatName(stu->getPatientName()));
    item->setText(3,formatAge(stu->getPatientAge()));
    item->setText(4,stu->getStudyModality().c_str() );
    item->setText(5,stu->getStudyDescription().c_str() );
    item->setText(6,formatDate(stu->getStudyDate()));
    item->setText(7,formatHour(stu->getStudyTime()));
    if (stu->getInstitutionName() == "") //si la informació ve buida l'anem a buscar a la bdd local
    {
        if (m_oldPacsAETitle != stu->getPacsAETitle()) //comparem que no sigui el mateix pacs que l'anterior, si es el mateix tenim la informacio guardada
        {//si es un pacs diferent busquem la informacio
            PacsListDB pacsList;
            PacsParameters pacs;
            
            state = pacsList.queryPacs(&pacs,stu->getPacsAETitle());
            if (state.good())
            {
                item->setText(8,pacs.getInstitution().c_str() );
                m_OldInstitution = pacs.getInstitution();
            }
            m_oldPacsAETitle = stu->getPacsAETitle();
        }
        else item->setText(8,m_OldInstitution.c_str() );
    }
    else item->setText(8,stu->getInstitutionName().c_str() );
    item->setText(9,stu->getAccessionNumber().c_str() );
    item->setText(10,stu->getPacsAETitle().c_str() );
    item->setText(11,stu->getStudyUID().c_str() );
    item->setText(12,"STUDY");//indiquem de que es tracta d'un estudi
    item->setText(13,"");
    m_studyTreeView->clearSelection();

}

/**Insereix una serie d'un estudi, i emiteix un signal al QSeriesIconView per a insereixi també la informació de la sèrie
  *                    @param informació de la serie
  */
void QStudyListView::insertSeries(Series *serie)
{
    
    QString text;
  
    QTreeWidgetItem* item = new QTreeWidgetItem(m_studyTreeView->currentItem());
    text.truncate(0);
    text.append(tr("Series "));
    text.append(serie->getSeriesNumber().c_str() );
    item->setIcon(0,m_iconSeries); 
    item->setText(0,text);
    item->setText(4,serie->getSeriesModality().c_str() );
    
    //si no tenim data o hora de la sèrie mostrem la de l'estudi
    if (serie->getSeriesDate().length() != 0)
    {
        item->setText(6,formatDate(serie->getSeriesDate()));
    }
        
    if (serie->getSeriesTime().length()!= 0)
    {
        item->setText(7,formatHour(serie->getSeriesTime()));
    }
    
    item->setText(11,serie->getSeriesUID().c_str() );   
    item->setText(12,"SERIES"); //indiquem que es tracta d'una sèrie 
    text.truncate(0);
    text.setNum(serie->getImageNumber(),10);
    item->setText(13,text);
    
    emit(addSeries(serie));//afegim serie al SeriesIconView
    
}

/** formata l'edat per mostrar per pantalla
  *            @param edat
  */
QString QStudyListView::formatAge(const std::string age)
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

/**Formata el nom
  *             @param Nom i cognoms del pacient
  */
QString QStudyListView::formatName(const std::string name)
{
    QString text( name.c_str() );
    
    text.replace("^"," ");
    
    return text;
}

/**Formata la data
  *             @param data de l'estudi
  */
QString QStudyListView::formatDate(const std::string date)
{
    QString text,dateOrig( date.c_str() );
    
    text.insert(0,dateOrig.mid(6,2)); //dd
    text.append("/");
    text.append(dateOrig.mid(4,2));
    text.append("/");
    text.append(dateOrig.mid(0,4));
    
    return text;
}

/**Formata l'hora
  *             @param Hora de l'estudi
  */
QString QStudyListView::formatHour(const std::string hour)
{
    QString text,hourOrig( hour.c_str() );
    
    text.insert(0,hourOrig.mid(0,2));
    text.append(":");
    text.append(hourOrig.mid(2,2));
    
    return text;
}

/** Neteja el listview
  */
void QStudyListView::clear()
{
    m_studyTreeView->clear();
}


/**Ordena per columna
  *             @param Indica la columna per la que s'ha d'ordenar
  */
void QStudyListView::setSortColumn(int col)
{
    m_studyTreeView->sortItems(col,Qt::AscendingOrder);
    m_studyTreeView->clearSelection();
}


/** Retorna el UID Study de l'estudi seleccionat
  *            @return UID de l'estudi seleccionat
  */
QString QStudyListView::getSelectedStudyUID()
{
    QTreeWidgetItem *item;

    if (m_studyTreeView->currentItem() != NULL) 
    {
        if (m_studyTreeView->currentItem()->parent() == NULL) //es un estudi
        {
            return m_studyTreeView->currentItem()->text(11);   
        }
        else
        {
            item = m_studyTreeView->currentItem()->parent();
            return item->text(11);
        }
    }
    else return "";
    
}

/** Retorna el UID Study de la sèrie seleccionada, si en aquell moment no hi ha cap sèrie seleccionada, retorna un QString buit
  *            @return UID de la sèrie seleccionat
  */
QString QStudyListView::getSelectedSeriesUID()
{

    if (m_studyTreeView->currentItem() != NULL) 
    {
        if (m_studyTreeView->currentItem()->parent() != NULL) //es un serie
        {
            return m_studyTreeView->currentItem()->text(11);   
        }
        else
        {
            return "";
        }
    }
    else return "";
    
}

/** Retorna el AETitle del PACS  l'estudi seleccionat
  *            @return AETitle del PACS de l'estudi seleccionat
  */
QString QStudyListView::getSelectedStudyPacsAETitle()
{
   QTreeWidgetItem * item;
   
   if (m_studyTreeView->currentItem() == NULL) return "";
    
   if (m_studyTreeView->currentItem()->parent() == NULL)
   {
       return m_studyTreeView->currentItem()->text(10);
   }
   else 
   {  
       item = m_studyTreeView->currentItem()->parent();
       return item->text(10);
   }
}

//SLOT CONNECTAT AMB EL SIGNAL D'UN CLICK
/**al fer un click mostra les sèries d'un estudi,
  *            @param Item element del listview
  *            @param columna a la qual s'ha clickat
  */
void QStudyListView::expand(QTreeWidgetItem * item,int col)
{

    if (item==NULL) return;

    if (col == 0) //aquesta posicio fan click a la carpeta, per tant despleguem o pleguem!
    {
        if (!m_studyTreeView->isItemExpanded(item))
        {    
            if (item->text(12)=="STUDY") //nomes s'expandeix si es tracta d'un estudi
            {
                m_studyTreeView->setItemExpanded(item,true);
                item->setIcon(0,m_openFolder); 
                if (item->childCount()==0) //si abans hem consultat le seria ja hi tenim la seva informació, evitem d'haver de consultar el pacs cada vegada
                {
                    emit(expand(item->text(11),item->text(10)));
                }
                else setSeriesToIconView(item); //en el cas que ja tinguem la informació de la sèrie, per passar la informació al iconView amb la informació de la sèrie cridarem aquest mètode
            }
        }
        else 
        {
            item->setIcon(0,m_closeFolder); 
            m_studyTreeView->setItemExpanded(item,false);
            emit(clearIconView());
        }
    }

       
}


/** Quant es consulten les sèries d'un estudi, es fa un acces al pacs demanant la informació d'aquelles series,si es tornen a consultar una segona vegada
  * les sèries de l'estudi,no cal tornar a accedir al pacs perquè ja tenim la informació de la sèrie al ListView, però s'ha d'actualitzar l'iconView amb 
  * la informació de les sèries de l'estudi, com no tornem a accedir al pacs, la informació de les sèries li hem de passar d'algun mode, per això el que fem
  * és invocar aquest mètode que crea reconstrueix l'objecte series, amb la principal informació de les sèries, i que fa un emit, que és capturat pel 
  * QSeriesInconView, per mostrar la informació de la sèrie (la connexió entre el QStudyListView i QSeriesIconView es fa la constrcutor de la QueryScreen)
  *        @param Apuntador a l'estudi al list view
  */
void QStudyListView::setSeriesToIconView(QTreeWidgetItem *item)
{
    QTreeWidgetItem *child;
    
    if (item == NULL) return;
    
    emit(clearIconView()); //es neteja el QSeriesIconView
    
    for (int i = 0;i < item->childCount();i++)
    {
        child = item->child(i);
        if (item != NULL)
        {
            Series serie;
            serie.setSeriesUID(child->text(11).ascii());
            serie.setImageNumber(child->text(13).toInt(NULL,10));
            serie.setSeriesModality(child->text(4).ascii());
            serie.setSeriesNumber(child->text(0).remove(tr("Series")).ascii());  
            serie.setStudyUID(getSelectedStudyUID().ascii());
            emit(addSeries(&serie));
        }
    }  
}


/** removes study from the lisk
  *    @param esbora l'estudi amb StudyUID de la llista
  */
void QStudyListView::removeStudy(QString studyUID)
{

    QList<QTreeWidgetItem *> qStudyList(m_studyTreeView->findItems("*",Qt::MatchWildcard,0));
    QTreeWidgetItem *item;
    
    for (int i = 0;i < qStudyList.count();i++)
    {
        item = qStudyList.at(i);
        if (item->text(11) == studyUID)
        {
            delete item;
        }
    }

}

/** ESborra un estudi de la caché 
  */
void QStudyListView::deleteStudy()
{
    
    emit(delStudy()); //aquest signal es recollit per la QueryScreen


}

/** Si es selecciona una serie del QSeriesIconView s'ha seleccionar la mateixa en el QStudyListView, al seleccionar una serie del SeriesIconView, salta aquest slot i selecciona la serie de l'estudi seleccionada al SeriesIconView
  *        @param SeriesUID Uid de la serie seleccionada en QSeriesIconView
  */
void QStudyListView::selectedSeriesIcon(QString seriesUID)
{
    QTreeWidgetItem *item,*current;
    
   //busquem el pare (l'estudi principal),ja que pot estar seleccionada una serie
   if (m_studyTreeView->currentItem()->parent() == NULL)
   {
        current = m_studyTreeView->currentItem();
   }
   else 
   {  
       current = m_studyTreeView->currentItem()->parent();
   }
   
    
    for (int i = 0;i < current->childCount();i++)
    {
        item = current->child(i);
        if (item != NULL)
        {
            if (item->text(11) == seriesUID)
            {
                m_studyTreeView->setItemSelected(item,true);
                m_studyTreeView->setCurrentItem(item);
            }
            else m_studyTreeView->setItemSelected(item,false);
        }
    }  

}

/** Mostra el menu contextual
  *     @param Dades de l'event sol·licitat
  */
void QStudyListView::contextMenuEvent(QContextMenuEvent *event)
{

    m_popUpMenu.exec(event->globalPos());
}


/**  Quant seleccionem una serie de la llista, emiteix un signal cap al QSeriesIconView per a que hi seleccioni la serie, seleccionada, a mes si clickem sobre un estudi expandid, s'ha de tornar a recarregar el QSeriesIconView amb les series   
d'aquell estudi
  *  també en el QSeriesIconView
  *         @param item sobre el que s'ha fet click
  */
void QStudyListView::clicked(QTreeWidgetItem *item,int)
{
    

    if (item != NULL)
    {
        if (item->parent() != NULL) //si es tracta d'una serie
        {   //enviem el UID de la serie
            if (m_oldStudyUID != getSelectedStudyUID())
            { //si seleccionem una serie pero d'un altre estudi, hem d'actualizar el qseriesiconview
                QTreeWidgetItem *parent  = item->parent();
                setSeriesToIconView(parent);
            }
            //indiquem que el QSeriesIconView que seleccioni la sèrie amb el UID passa per parametre
            emit(selectedSeriesList(item->text(11)));
        }
        else
        {
            if (m_oldStudyUID != getSelectedStudyUID())
            { //si seleccionem una estudi diferent el seleccionat abans, hem d'actualizar el qseriesiconview
                if (m_studyTreeView->isItemExpanded(item))
                {// ha d'estar expendit per visualitzar les series
                    setSeriesToIconView(item);
                }
                else emit(clearIconView());
            }   
                 
        }
    }
    
    m_oldStudyUID = getSelectedStudyUID();
}

/** Slot que visualitza l'estudi
  */
void QStudyListView::viewStudy()
{
    emit(view());
}

/** Slot que descarrega un estudi
  */
void QStudyListView::retrieveStudy()
{
    emit(retrieve());    
}

/** guarda de la mida de les columnes
  */
void QStudyListView::saveColumnsWidth()
{
    StarviewerSettings settings;

    if (m_parentName == "m_tabPacs")
    { 
        for (int i = 0;i < m_studyTreeView->columnCount();i++)
        {
            settings.setStudyPacsListColumnWidth(i,m_studyTreeView->columnWidth(i)); 
        }    
    }   
   
    if (m_parentName == "m_tabCache")
    {
        for (int i = 0;i < m_studyTreeView->columnCount();i++)
        {
           settings.setStudyCacheListColumnWidth(i,m_studyTreeView->columnWidth(i)); 
        }      
    }

}

/** Destructor de la classe
  */
QStudyListView::~QStudyListView()
{
}

};
