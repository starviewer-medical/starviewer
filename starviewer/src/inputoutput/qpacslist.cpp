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
#include "pacsparameters.h"
#include "starviewersettings.h"
#include "status.h"

namespace udg {

QPacsList::QPacsList( QWidget *parent )
 : QWidget( parent )
{
    setupUi( this );

    m_PacsTreeView->setColumnHidden( 3 , true ); //La columna default està amagada

    refresh();
}

QPacsList::~QPacsList()
{
}

void QPacsList::refresh()
{
    PacsListDB pacsListDB;
    QList<PacsParameters> pacsList;

    m_PacsTreeView->clear();

    pacsListDB.queryPacsList( pacsList );

    foreach(PacsParameters pacs, pacsList)
    {
        QTreeWidgetItem* item = new QTreeWidgetItem( m_PacsTreeView );

        item->setText( 0 , pacs.getAEPacs() );
        item->setText( 1 , pacs.getInstitution() );
        item->setText( 2 , pacs.getDescription() );
        item->setText( 3 , pacs.getDefault() );
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

QList<PacsParameters> QPacsList::getSelectedPacs()
{
    PacsListDB pacsListDB;
    StarviewerSettings settings;

    QList< QTreeWidgetItem * > qPacsList( m_PacsTreeView->selectedItems() );
    QTreeWidgetItem *item;

    QList<PacsParameters> selectedPacsList;

    for ( int i = 0; i < qPacsList.count(); i++ )
    {
        item = qPacsList.at( i );
        PacsParameters pacs;

        pacsListDB.queryPacs( &pacs , item->text( 0 ) ); //fem el query per cercar la informació del PACS

        pacs.setAELocal( settings.getAETitleMachine() );
        //emplenem amb les dades del registre el timeout
        pacs.setTimeOut( settings.getTimeout().toInt( NULL , 10 ) );
        selectedPacsList.append( pacs ); //inserim a la llista
    }

    return selectedPacsList;
}

};
