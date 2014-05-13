/*************************************************************************************
  Copyright (C) 2014 Laboratori de Gràfics i Imatge, Universitat de Girona &
  Institut de Diagnòstic per la Imatge.
  Girona 2014. All rights reserved.
  http://starviewer.udg.edu

  This file is part of the Starviewer (Medical Imaging Software) open source project.
  It is subject to the license terms in the LICENSE file found in the top-level
  directory of this distribution and at http://starviewer.udg.edu/license. No part of
  the Starviewer (Medical Imaging Software) open source project, including this file,
  may be copied, modified, propagated, or distributed except according to the
  terms contained in the LICENSE file.
 *************************************************************************************/

#ifndef UDGREPOSITORY_CPP
#define UDGREPOSITORY_CPP

#include "repository.h"

namespace udg {

template< typename ItemType >
Repository<ItemType>::~Repository()
{
    this->cleanUp();
}

template<typename ItemType>
Identifier Repository<ItemType>::addItem(ItemType* item)
{
    Identifier id(m_nextIdentifierNumber++);
    m_itemList[id] = item;
    emit changed();
    return id;
}

template<typename ItemType>
ItemType* Repository<ItemType>::getItem(const Identifier &id) const
{
    return m_itemList.value(id);
}

template<typename ItemType>
QList<ItemType*> Repository<ItemType>::getItems() const
{
    return m_itemList.values();
}

template<typename ItemType>
void Repository<ItemType>::removeItem(const Identifier &id)
{
    m_itemList.remove(id);
    emit changed();
}

template<typename ItemType>
int Repository<ItemType>::getNumberOfItems() const
{
   return m_itemList.size();
}

template<typename ItemType>
QList<Identifier> Repository<ItemType>::getIdentifiers() const
{
    return m_itemList.keys();
}

template<typename ItemType>
void Repository<ItemType>::cleanUp()
{
    foreach (ItemType *item, m_itemList)
    {
        delete item;
    }

    m_itemList.clear();
    emit changed();
}

};  // End namespace udg

#endif
