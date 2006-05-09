/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/ 
#include "qpacslist.h"

#include <QTreeView>
#include <QList>
#include <QMessageBox>

#include "pacslistdb.h"
#include "pacslist.h"
#include "pacsparameters.h"
#include "status.h"
#include "starviewersettings.h"

namespace udg {

QPacsList::QPacsList( QWidget *parent )
 : QWidget( parent )
{
    setupUi( this );

    m_PacsTreeView->setColumnHidden( 3 , true ); //La columna default està amagada

    refresh();
}

void QPacsList::refresh()
{
    PacsListDB pacsListDB;
    PacsList   pacsList;
    PacsParameters pacs;
    Status state;
    
    m_PacsTreeView->clear();    

    state = pacsListDB.queryPacsList( pacsList );
    
    if ( !state.good() )
    {
        databaseError( &state );
        return;
    }
    
    pacsList.firstPacs();
    
    while ( !pacsList.end() )
    {
        QTreeWidgetItem* item = new QTreeWidgetItem( m_PacsTreeView );
        pacs = pacsList.getPacs();
        item->setText( 0 , pacs.getAEPacs().c_str() );
        item->setText( 1 , pacs.getInstitution().c_str() );
        item->setText( 2 , pacs.getDescription().c_str() );   
        item->setText( 3 , pacs.getDefault().c_str() );
        pacsList.nextPacs();
    }
    
    setSelectedDefaultPacs();
}

void QPacsList::setSelectedDefaultPacs()
{
    QList<QTreeWidgetItem *> qPacsList( m_PacsTreeView->findItems( "*" , Qt::MatchWildcard , 0 ) );
    QTreeWidgetItem *item;
    
    for  (int i = 0; i < qPacsList.count(); i++ )
    {
        item = qPacsList.at( i );
        if ( item->text(3) == "S" )
        {
            m_PacsTreeView->setItemSelected( item,true );
        }
    }
}

Status QPacsList::getSelectedPacs( PacsList *pacsList )
{
    PacsListDB pacsListDB;
    Status state;
    StarviewerSettings settings;
    
    QList< QTreeWidgetItem * > qPacsList( m_PacsTreeView->selectedItems() );
    QTreeWidgetItem *item;
    
    for ( int i = 0; i < qPacsList.count(); i++ )
    {
        item = qPacsList.at( i );
        PacsParameters pacs;
        
        state = pacsListDB.queryPacs( &pacs , item->text( 0 ).toStdString() ); //fem el query per cercar la informació del PACS
            
        if ( state.good() )
        {
            pacs.setAELocal( settings.getAETitleMachine().toStdString() );
            //emplenem amb les dades del registre el timeout
            pacs.setTimeOut( settings.getTimeout().toInt( NULL , 10 ) );
            pacsList->insertPacs( pacs ); //inserim a la llista
        }
        else return state;        
    }
  
  return state;
}

void QPacsList::databaseError( Status *state )
{

    QString text,code;
    if ( !state->good() )
    {
        switch( state->code() )
        {   
            case 2001 : text.insert( 0 , tr( "Database is corrupted or SQL syntax error" ) );
                        text.append( "\n" );
                        text.append( tr( "Error Number : " ) );
                        code.setNum( state->code() , 10 );
                        text.append( code );
                        break;
            case 2005 : text.insert( 0 , tr( "Database is looked" ) );
                        text.append( "\n" );
                        text.append( "To solve this error restart the user session" );
                        text.append( "\n" );
                        text.append( tr( "Error Number : " ) );
                        code.setNum( state->code() , 10 );
                        text.append( code );
                        break;
            case 2011 : text.insert( 0 , tr("Database is corrupted.") );
                        text.append( "\n" );
                        text.append( tr( "Error Number : " ) );
                        code.setNum( state->code() , 10 );
                        text.append( code );
                        break;
            case 2050 : text.insert( 0 , "Not Connected to database" );
                        text.append( "\n" );
                        text.append( tr( "Error Number : " ) );
                        code.setNum( state->code() , 10 );
                        text.append( code );
                        break;            
            default :   text.insert( 0 , tr("Internal Database error") );
                        text.append( "\n" );
                        text.append( tr( "Error Number : " ) );
                        code.setNum( state->code() , 10 );
                        text.append( code );
                        break;
        }
        QMessageBox::critical( this , tr("StarViewer"), text );
    }    
}

QPacsList::~QPacsList()
{
}

};
