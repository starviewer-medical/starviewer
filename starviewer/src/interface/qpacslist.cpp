/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/ 
#include "qpacslist.h"

#include <qlistview.h>
#include <qmessagebox.h>

#include "pacslistdb.h"
#include "pacslist.h"
#include "pacsparameters.h"
#include "status.h"
#include "starviewersettings.h"

namespace udg {

/** Constructor de la classe
  */
QPacsList::QPacsList(QWidget *parent, const char *name)
 : QPacsListBase(parent, name)
{
    int i;

    m_PacsListView->setColumnWidth(3,0); //La columna default està amagada

    for (i=0;i<=m_PacsListView->columns();i++)
    {
        m_PacsListView->setColumnWidthMode(i,QListView::Manual); 
    }

    refresh();
}


/** Carrega al ListView la Llista de Pacs disponibles
  */
void QPacsList::refresh()
{
    PacsListDB pacsListDB;
    PacsList   pacsList;
    PacsParameters pacs;
    Status state;
    
    m_PacsListView->clear();    

    state = pacsListDB.queryPacsList(pacsList);
    
    if (!state.good())
    {
        databaseError(&state);
        return;
    }
    
    
    pacsList.firstPacs();
    
    while (!pacsList.end())
    {
        QListViewItem* item = new QListViewItem(m_PacsListView);
        pacs = pacsList.getPacs();
        item->setText(0,pacs.getAEPacs());
        item->setText(1,pacs.getInstitution());
        item->setText(2,pacs.getDescription());   
        item->setText(3,pacs.getDefault());
        pacsList.nextPacs();
    
    }
    
    setSelectedDefaultPacs();
}

/** Aquesta accio selecciona en el PacsListView els Pacs que tenen a 'S' a Default. Son els pacs que per defecte l'usuari 
  * te que es realitzin les cerques
  */
void QPacsList::setSelectedDefaultPacs()
{

    QListViewItemIterator it( m_PacsListView );
    
    while ( it.current() ) 
    {
        if ( it.current()->text(3)=="S" )
        {
            it.current()->setSelected(true);
        }
        else it.current()->setSelected(false);
        
        ++it;
    }

}


/** Retorna els pacs seleccionats per l'usuari per a realitzar la cerca
  */
Status QPacsList::getSelectedPacs(PacsList *pacsList)
{
    QListViewItemIterator it( m_PacsListView );
    PacsListDB pacsListDB;
    Status state;
    StarviewerSettings settings;
    
    
    while ( it.current() ) 
    {
        if (it.current()->isSelected() == true) //si el pacs esta seleccionat buquem els seus paràmetres
        {   PacsParameters pacs;
            state =pacsListDB.queryPacs(&pacs,it.current()->text(0)); //fem el query per cercar la informació del PACS
            
            if (state.good())
            {
                pacs.setAELocal(settings.getAETitleMachine());
                //emplenem amb les dades del registre el timeout
                pacs.setTimeOut(settings.getTimeout().toInt(NULL,10));
                pacsList->insertPacs(pacs); //inserim a la llista
            }
            else return state;
        }
        ++it;
  }
  
  return state;
}


/** Tracta els errors que s'han produït durant els accessos a la base dades
  *           @param state [in] Estat de l'acció retrieve
  */
void QPacsList::databaseError(Status *state)
{

    QString text,code;
    if (!state->good())
    {
        switch(state->code())
        {   
            case 2001 : text.insert(0,tr("Database not found."));
                        text.append("\n");
                        text.append(tr("Error Number : "));
                        code.setNum(state->code(),10);
                        text.append(code);
                        QMessageBox::warning( this, tr("StarViewer"),text);
                        break;
            case 2011 : text.insert(0,tr("Database is corrupted."));
                        text.append("\n");
                        text.append(tr("Error Number : "));
                        code.setNum(state->code(),10);
                        text.append(code);
                        QMessageBox::warning( this, tr("StarViewer"),text);
                        break;
            default :   text.insert(0,tr("Internal Database error"));
                        text.append("\n");
                        text.append(tr("Error Number : "));
                        code.setNum(state->code(),10);
                        text.append(code);
                        QMessageBox::warning( this, tr("StarViewer"),text);
                        break;
        }
    }    

}

QPacsList::~QPacsList()
{
}


};
