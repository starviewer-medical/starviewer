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

#include "pacsmanager.h"
#include "pacsparameters.h"
#include "status.h"

namespace udg {

QPacsList::QPacsList( QWidget *parent )
 : QWidget( parent )
{
    setupUi( this );

    m_PacsTreeView->setColumnHidden(0, true); //la columna PacsId està amagada

    refresh();
}

QPacsList::~QPacsList()
{
}

void QPacsList::refresh()
{
    PacsManager pacsManager;
    QList<PacsParameters> pacsList;

    m_PacsTreeView->clear();

    pacsList = pacsManager.queryPacsList();

    foreach(PacsParameters pacs, pacsList)
    {
        QTreeWidgetItem* item = new QTreeWidgetItem( m_PacsTreeView );

        item->setText(0, pacs.getPacsID());
        item->setText(1, pacs.getAEPacs());
        item->setText(2, pacs.getInstitution());
        item->setText(3, pacs.getDescription());

        item->setSelected(pacs.isDefault());
    }
}

QList<PacsParameters> QPacsList::getSelectedPacs()
{
    PacsManager pacsManager;

    QList< QTreeWidgetItem * > qPacsList( m_PacsTreeView->selectedItems() );
    QTreeWidgetItem *item;

    QList<PacsParameters> selectedPacsList;

    for ( int i = 0; i < qPacsList.count(); i++ )
    {
        item = qPacsList.at( i );
        PacsParameters pacs;

        pacs = pacsManager.queryPacs(item->text(0)); //fem el query per cercar la informació del PACS

        selectedPacsList.append( pacs ); //inserim a la llista
    }

    return selectedPacsList;
}

};
