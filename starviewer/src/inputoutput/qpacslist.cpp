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

    m_PacsTreeView->setColumnHidden(0, true); //la columna PacsId està amagada
    m_PacsTreeView->setColumnHidden(4, true); //La columna default està amagada

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

    pacsList = pacsListDB.queryPacsList();

    foreach(PacsParameters pacs, pacsList)
    {
        QTreeWidgetItem* item = new QTreeWidgetItem( m_PacsTreeView );

        item->setText(0, pacs.getPacsID());
        item->setText(1, pacs.getAEPacs());
        item->setText(2, pacs.getInstitution());
        item->setText(3, pacs.getDescription());
        item->setText(4, pacs.getDefault());
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
        if ( item->text(4) == "S" )
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

        pacs = pacsListDB.queryPacs(item->text(0)); //fem el query per cercar la informació del PACS

        selectedPacsList.append( pacs ); //inserim a la llista
    }

    return selectedPacsList;
}

};
