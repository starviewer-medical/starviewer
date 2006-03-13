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
#include "qstudylistview.h"
//Added by qt3to4:
#include <Q3PopupMenu>
#include "study.h"
#include <q3listview.h>
#include <qpixmap.h>
#include <qimage.h>
#include "qseriesiconview.h"
#include "pacslistdb.h"
#include "cachepacs.h"
#include <qmessagebox.h>


namespace udg {



/** Constructor de la classe
  */
QStudyListView::QStudyListView( QWidget *parent , const char * name )
 : QWidget( parent )
{
    setupUi( this );
    int i;
    
    StudyListV->setRootIsDecorated(false);
    
    //la columna de UID i AETITLE les fem invisibles, i li indiquem que no poden canvia de tamany automàticament
    StudyListV->setColumnWidth(10,0);
    StudyListV->setColumnWidth(11,0);
    StudyListV->setColumnWidth(12,0);
    StudyListV->setColumnWidth(13,0);
    
    //Les columnes del Listview només es poden canviar de mida per si soles
    for (i=0;i<=StudyListV->columns();i++)
    {
        StudyListV->setColumnWidthMode(i,Q3ListView::Manual); 
    }

    m_openFolder = QPixmap(":/images/folderopen.png");
    m_closeFolder = QPixmap(":/images/folderclose.png");
    m_iconSeries = QPixmap(":/images/series.png");
    
    createPopupMenu( name );
}

/** Creem el popup Menu
  *        @param nom de l'objecte 
  */
void QStudyListView::createPopupMenu(QString nom)
{
    int idRet,idDel,idView;
    
    m_popupMenu = new Q3PopupMenu( this );
    idView = m_popupMenu->insertItem(tr("&View"),  this, SLOT(viewStudy()), Qt::CTRL+Qt::Key_W );
    idRet = m_popupMenu->insertItem(tr("&Retrieve"),this, SLOT(retrieveStudy()), Qt::CTRL+Qt::Key_R);
    m_popupMenu->insertSeparator();
    idDel = m_popupMenu->insertItem(tr("&Delete"),this, SLOT(deleteStudy()), Qt::CTRL+Qt::Key_D);
    m_oldPacsAETitle = "";
      
   if (nom=="StudyLViewPacs")
   { 
       m_popupMenu->setItemEnabled(idDel,false);       
       m_popupMenu->setItemEnabled(idView,false);
   }
   
   if (nom=="StudyLViewCache")
   {
       m_popupMenu->setItemEnabled(idRet,false);
   }
}

/** Mostra l'estudi pel ListView que se li passa per paràmetre
  *        @param StudyList a mostrar
  */
void QStudyListView::showStudyList(StudyList *ls)
{

    StudyListV->clear();

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

    Q3ListViewItem* item = new Q3ListViewItem(StudyListV);
    text.truncate(0);
    text.append("Study ");
    text.append(stu->getStudyId().c_str() );
    item->setPixmap(0,m_closeFolder);
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
    StudyListV->clearSelection();

}

/**Insereix una serie d'un estudi, i emiteix un signal al QSeriesIconView per a insereixi també la informació de la sèrie
  *                    @param informació de la serie
  */
void QStudyListView::insertSeries(Series *serie)
{
    
    QString text;
  
    Q3ListViewItem* item = new Q3ListViewItem(StudyListV->currentItem());
    text.truncate(0);
    text.append(tr("Series "));
    text.append(serie->getSeriesNumber().c_str() );
    item->setPixmap(0,m_iconSeries); 
    item->setText(0,text);
    //item->setText(1,formatName(stu.getPatientName()));
    //item->setText(2,formatAge(stu.getPatiFentAge()));
    item->setText(4,serie->getSeriesModality().c_str() );
    
    
    //si no tenim data o hora de la sèrie mostrem la de l'estudi
    if (serie->getSeriesDate().length()!=0)
    {
        item->setText(6,formatDate(serie->getSeriesDate()));
    }
        
    if (serie->getSeriesTime().length()!=0)
    {
        item->setText(7,formatHour(serie->getSeriesTime()));
    }
   // item->setText(6,stu.getInstitutionName());
    //item->setText(7,stu.getPacsAETitle());
    item->setText(11,serie->getSeriesUID().c_str() );   
    item->setText(12,"SERIES"); //indiquem que es tracta d'una sèrie 
    text.truncate(0);
    text.setNum(serie->getImageNumber(),10);
    item->setText(13,text);
    
    emit(addSeries(serie));//afegim serie al SeriesIconView
    
    //item->setSelectable(false);
}

/** formata l'edat per mostrar per pantalla
  *            @param edat
  */
QString QStudyListView::formatAge(const std::string age)
{
    QString text( age.c_str() );
    
    if (text.at(0)== '0') 
    {//treiem el 0 de davant els anys, el PACS envia per ex: 047Y nosaltes tornem 47Y
        text.replace(0,1," ");
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
    StudyListV->clear();
}


/**Ordena per columna
  *             @param Indica la columna per la que s'ha d'ordenar
  */
void QStudyListView::setSortColumn(int col)
{
    StudyListV->setSortColumn(col);
    StudyListV->clearSelection();
}


/** Retorna el UID Study de l'estudi seleccionat
  *            @return UID de l'estudi seleccionat
  */
QString QStudyListView::getSelectedStudyUID()
{
    Q3ListViewItem *item;

    if (StudyListV->currentItem()!=NULL) 
    {
        if (StudyListV->currentItem()->depth()==0) //es un estudi
        {
            return StudyListV->currentItem()->text(11);   
        }
        else
        {
            item = StudyListV->currentItem()->parent();
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

    if (StudyListV->currentItem()!=NULL) 
    {
        if (StudyListV->currentItem()->depth()==1) //es un estudi
        {
            return StudyListV->currentItem()->text(11);   
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
   Q3ListViewItem * item;
   if (StudyListV->currentItem()==NULL) return "";
    
   if (StudyListV->currentItem()->depth()==0)
   {
       return StudyListV->currentItem()->text(10);
   }
   else 
   {
       item = StudyListV->currentItem()->parent();
       return item->text(10);
   }
}

//SLOT CONNECTAT AMB EL SIGNAL D'UN CLICK
/**al fer un click mostra les sèries d'un estudi,
  *            @param Item element del listview
  *            @param point on s'ha fet el click
  *            @param columna a la qual s'ha clickat
  */
void QStudyListView::expand(Q3ListViewItem * item,const QPoint &,int col)
{

    if (item==NULL) return;

    if (col == 0) //aquesta posicio fan click a la carpeta, per tant despleguem o pleguem!
    {
        if (!item->isOpen())
        {    
            if (item->text(12)=="STUDY") //nomes s'expandeix si es tracta d'un estudi
            {
                item->setOpen(true);
                item->setPixmap(0,m_openFolder); 
                if (item->childCount()==0) //si abans hem consultat le seria ja hi tenim la seva informació, evitem d'haver de consultar el pacs cada vegada
                {
                    emit(click(item->text(11),item->text(10)));
                }
                else setSeriesToIconView(item); //en el cas que ja tinguem la informació de la sèrie, per passar la informació al iconView amb la informació de la sèrie cridarem aquest mètode
            }
        }
        else 
        {
            item->setPixmap(0,m_closeFolder); 
            item->setOpen(false);
            emit(clearIconView());
        }
    }

    if (item->childCount() > 0 && item->text(12)=="STUDY" )
    {
        setSeriesToIconView(item);
    }
    if (item->text(12) == "SERIES")
    {
        setSeriesToIconView(item->parent());
    }
       
}


/** Quant es consulten les sèries d'un estudi, es fa un acces al pacs demanant la informació d'aquelles series,si es tornen a consultar una segona vegada
  * les sèries de l'estudi,no cal tornar a accedir al pacs perquè ja tenim la informació de la sèrie al ListView, però s'ha d'actualitzar l'iconView amb 
  * la informació de les sèries de l'estudi, com no tornem a accedir al pacs, la informació de les sèries li hem de passar d'algun mode, per això el que fem
  * és invocar aquest mètode que crea reconstrueix l'objecte series, amb la principal informació de les sèries, i que fa un emit, que és capturat pel 
  * QSeriesInconView, per mostrar la informació de la sèrie (la connexió entre el QStudyListView i QSeriesIconView es fa la constrcutor de la QueryScreen)
  *        @param Apuntador a l'estudi al list view
  */
void QStudyListView::setSeriesToIconView(Q3ListViewItem *item)
{
    Q3ListViewItem  * child = item->firstChild();
    
    emit(clearIconView());
    do
    {
        Series serie;
        serie.setSeriesUID(child->text(11).toStdString() );
        serie.setImageNumber(child->text(13).toInt(NULL,10));
        serie.setSeriesModality(child->text(4).toStdString() );
        serie.setSeriesNumber((child->text(0).remove(tr("Series"))).toStdString());
        serie.setStudyUID(getSelectedStudyUID().toStdString() );
        emit(addSeries(&serie));   
        child = child->nextSibling(); //busquem el seguent germà!
    }
    while (child!=0);
}

/** ensenya popupMenu al fer click al botó de la dreta
  *    @param item
  *    @param punt on s'ha fet el clock
  *    @param columna que s'ha fet el ckcik
  */
void QStudyListView::popupMenuShow(Q3ListViewItem *,const QPoint& point,int)
{
//    expand(item);    
    m_popupMenu->exec(point);
}


/** removes study from the list
  *    @param esbora l'estudi amb StudyUID de la llista
  */
void QStudyListView::removeStudy(QString studyUID)
{
    Q3ListViewItemIterator it(StudyListV );
    
    while ( it.current() ) 
    {
        if ( it.current()->text(11)==studyUID )
        {
           delete it.current();
           break;
        } 
        it++;
    }
}

/** ESborra un estudi de la caché 
  */
void QStudyListView::deleteStudy()
{
    
    switch( QMessageBox::information( this, tr("Starviewer"),
				      tr("Are you sure you want to delete this Study ?"),
				      tr("Yes"), tr("No"),
				      0, 1 ) ) 
    {
    case 0:
        emit(clearIconView());
        emit(delStudy());
    }


}

/** Si es selecciona una serie del QSeriesIconView s'ha seleccionar la mateixa en el QStudyListView, al seleccionar una serie del SeriesIconView, salta aquest slot i selecciona la serie de l'estudi seleccionada al SeriesIconView
  *        @param index de l'icona seleccionada
  */
void QStudyListView::selectedSeriesIcon(int index)
{
    int i=0;    

    //busquem l'estudi (el pare) per recorre tots els fills, per tal de seleccionar la mateixa serie que la del QSeriesIconView
   Q3ListViewItem *parent = NULL;
   
   if (StudyListV->currentItem()==NULL) return;
    
   if (StudyListV->currentItem()->depth()==0)
   {
       parent = StudyListV->currentItem(); 
   }
   else 
   {
       parent = StudyListV->currentItem()->parent();
   }
   
    if (!parent->isOpen()) parent->setOpen(true);
   
   Q3ListViewItem *child = parent->firstChild();
   
   
   //El qseriesIconView ens indica a quina posicio es troba dins la llista la sèrie que hem de seleccionar
   while (i < index && child != 0)
   {
        child =  child->nextSibling();
        i++;
   } 
   
   if (i == index && child !=0 )
   {   
       StudyListV->setCurrentItem(child);
   }
   
//    StudyListV->repaint();
}

/**  Quant seleccionem una serie de la llista, emiteix un signal cap al QSeriesIconView per a que hi seleccioni la serie, seleccionada
  *  també en el QSeriesIconView
  *         @param item sobre el que s'ha fet click
  */
void QStudyListView::clicked(Q3ListViewItem *item)
{

    if (item!=NULL)
    {
        if (item->depth() == 1) //si es tracta d'una serie
        {   //enviem el numero de serie, ja que la key del QSeriesIconView és el número de Serie
            emit(selectedSeriesList(item->text(0).remove(tr("Series"))));
        }
    }
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

/** Destructor de la classe
  */
QStudyListView::~QStudyListView()
{
}


};
