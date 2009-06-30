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

#include "pacsdevicemanager.h"
#include "pacsdevice.h"
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
    PacsDeviceManager pacsDeviceManager;
    QList<PacsDevice> pacsList;

    m_PacsTreeView->clear();

    pacsList = pacsDeviceManager.queryPacsList();

    foreach(PacsDevice pacs, pacsList)
    {
        QTreeWidgetItem* item = new QTreeWidgetItem( m_PacsTreeView );

        item->setText(0, pacs.getPacsID());
        item->setText(1, pacs.getAEPacs());
        item->setText(2, pacs.getInstitution());
        item->setText(3, pacs.getDescription());

        item->setSelected(pacs.isDefault());
    }
}

QList<PacsDevice> QPacsList::getSelectedPacs()
{
    PacsDeviceManager pacsDeviceManager;

    QList< QTreeWidgetItem * > qPacsList( m_PacsTreeView->selectedItems() );
    QTreeWidgetItem *item;

    QList<PacsDevice> selectedPacsList;

    for ( int i = 0; i < qPacsList.count(); i++ )
    {
        item = qPacsList.at( i );
        PacsDevice pacs;

        pacs = pacsDeviceManager.queryPacs(item->text(0)); //fem el query per cercar la informació del PACS

        selectedPacsList.append( pacs ); //inserim a la llista
    }

    return selectedPacsList;
}

};
