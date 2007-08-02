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
    DEBUG_LOG( QString("Creacio d'un PatientItem de tipus patient") );
    itemData << "patient";
}

PatientItem::PatientItem( Study *study, PatientItem *parent )
{
    parentItem = parent;
    DEBUG_LOG( QString("Creacio d'un PatientItem de tipus study") );
    itemData << "study";
}

PatientItem::PatientItem( Series *series, PatientItem *parent )
{
    parentItem = parent;
    DEBUG_LOG( QString("Creacio d'un PatientItem de tipus parent") );
    itemData << "serie";
}

PatientItem::~PatientItem()
{
    DEBUG_LOG( QString("Inici mètode destructor de PatientItem") );
    qDeleteAll(childItems);
}

void PatientItem::appendChild(PatientItem *item)
{
    DEBUG_LOG( QString("Inici mètode appendChild") );
    childItems.append(item);
}

PatientItem *PatientItem::child(int row)
{
    DEBUG_LOG( QString("Inici mètode child") );
    return childItems.value(row);
}

int PatientItem::childCount() const
{
    DEBUG_LOG( QString("Inici mètode childCount") );
    return childItems.count();
}

int PatientItem::columnCount() const
{
    DEBUG_LOG( QString("Inici mètode columnCount") );
    return itemData.count();
}

QVariant PatientItem::data(int column) const
{
    DEBUG_LOG( QString("Inici mètode data") );
    return itemData.value(column);
}

PatientItem *PatientItem::parent()
{
    DEBUG_LOG( QString("Inici mètode parent") );
    return parentItem;
}

int PatientItem::row() const
{
    DEBUG_LOG( QString("Inici mètode row") );
    if (parentItem)
        return parentItem->childItems.indexOf(const_cast<PatientItem*>(this));

    return 0;
}

QString PatientItem::toString()
{
    DEBUG_LOG( QString("Inici mètode toString") );

    QString str = "nom";

    for( int i = 0; i < childCount(); i++)
    {
        str+=child(i)->toString();
    }
    return str;
}

}
