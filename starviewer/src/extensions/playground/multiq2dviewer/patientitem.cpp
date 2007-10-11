/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "patientitem.h"
#include "logging.h"
#include <QStringList>

namespace udg {

PatientItem::PatientItem( Patient *patient, PatientItem *parent )
{
    parentItem = parent;
    itemData << "patient";
}

PatientItem::PatientItem( Study *study, PatientItem *parent )
{
    parentItem = parent;
    itemData << "study";
}

PatientItem::PatientItem( Series *series, PatientItem *parent )
{
    parentItem = parent;
    itemData << "serie";
}

PatientItem::~PatientItem()
{
    qDeleteAll(childItems);
}

void PatientItem::appendChild(PatientItem *item)
{
    childItems.append(item);
}

PatientItem *PatientItem::child(int row)
{
    return childItems.value(row);
}

int PatientItem::childCount() const
{
    return childItems.count();
}

int PatientItem::columnCount() const
{
    return itemData.count();
}

QVariant PatientItem::data(int column) const
{
    return itemData.value(column);
}

PatientItem *PatientItem::parent()
{
    return parentItem;
}

int PatientItem::row() const
{
    if (parentItem)
        return parentItem->childItems.indexOf(const_cast<PatientItem*>(this));

    return 0;
}

QString PatientItem::toString()
{
    QString str = "nom";

    for( int i = 0; i < childCount(); i++)
    {
        str+=child(i)->toString();
    }
    return str;
}

}
