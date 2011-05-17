#include "qpacslist.h"

#include <QTreeView>
#include <QList>

#include "pacsdevicemanager.h"
#include "pacsdevice.h"

namespace udg {

QPacsList::QPacsList(QWidget *parent)
 : QWidget(parent)
{
    setupUi(this);

    // La columna PacsId està amagada
    m_PacsTreeView->setColumnHidden(0, true);
    // La columna PACSAddress està amagada
    m_PacsTreeView->setColumnHidden(4, true);
    // Ordenem per la columna AETitle
    m_PacsTreeView->sortByColumn(1, Qt::AscendingOrder);

    m_filterPacsByService = PacsDeviceManager::PacsWithQueryRetrieveServiceEnabled;
    m_showQueryPacsDefaultHighlighted = true;

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

    pacsList = pacsDeviceManager.getPACSList(m_filterPacsByService);

    foreach (PacsDevice pacs, pacsList)
    {
        QTreeWidgetItem *item = new QTreeWidgetItem(m_PacsTreeView);

        item->setText(0, pacs.getID());
        item->setText(1, pacs.getAETitle());
        item->setText(2, pacs.getInstitution());
        item->setText(3, pacs.getDescription());
        item->setText(4, pacs.getAddress());

        if (getShowQueryPacsDefaultHighlighted())
        {
            item->setSelected(pacs.isDefault());
        }
    }
}

QList<PacsDevice> QPacsList::getSelectedPacs()
{
    PacsDeviceManager pacsDeviceManager;
    QList<PacsDevice> selectedPacsList;
    QList<QTreeWidgetItem*> qPacsList(m_PacsTreeView->selectedItems());

    QTreeWidgetItem *item;
    for (int i = 0; i < qPacsList.count(); i++)
    {
        PacsDevice pacs;
        item = qPacsList.at(i);
        // Fem el query per cercar la informació del PACS
        pacs = pacsDeviceManager.getPACSDeviceByID(item->text(0));
        // Inserim a la llista
        selectedPacsList.append(pacs);
    }

    return selectedPacsList;
}

void QPacsList::clearSelection()
{
    m_PacsTreeView->clearSelection();
}

void QPacsList::setFilterPACSByService(PacsDeviceManager::FilterPACSByService filter)
{
    m_filterPacsByService = filter;
}

PacsDeviceManager::FilterPACSByService QPacsList::getFilterPACSByService()
{
    return m_filterPacsByService;
}

void QPacsList::setShowQueryPacsDefaultHighlighted(bool showHighlighted)
{
    m_showQueryPacsDefaultHighlighted = showHighlighted;
}

bool QPacsList::getShowQueryPacsDefaultHighlighted()
{
    return m_showQueryPacsDefaultHighlighted;
}

void QPacsList::setDefaultPACS(QTreeWidgetItem *item)
{
    Q_ASSERT(item);

    PacsDeviceManager pacsDeviceManager;

    PacsDevice pacs;
    pacs = pacsDeviceManager.getPACSDeviceByID(item->text(0));
    pacs.setDefault(item->isSelected());
}

};
