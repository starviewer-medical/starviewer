/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#ifndef UDGREPOSITORY_CPP
#define UDGREPOSITORY_CPP

#include "repository.h"

namespace udg {

template< typename ItemType >
Repository< ItemType >::~Repository( )
{
    this->cleanUp();
}

template< typename ItemType >
Identifier Repository< ItemType >::addItem( ItemType* item )
{
    Identifier id( m_nextIdentifierNumber++ );
    m_itemList[ id ] = item ;
    return id;
}

template< typename ItemType >
ItemType* Repository< ItemType >::getItem( const Identifier id )
{
    return m_itemList.value(id);
}

template< typename ItemType >
QList<ItemType*> Repository< ItemType >::getItems()
{
    return m_itemList.values();
}


template< typename ItemType >
void Repository< ItemType >::removeItem( const Identifier id )
{
    m_itemList.remove( id );
}

template< typename ItemType >
int Repository< ItemType >::getNumberOfItems()
{
   return m_itemList.size();
}

template< typename ItemType >
QList<Identifier*> Repository< ItemType >::getIdentifiers()
{
    return m_itemList.keys();
}

template< typename ItemType >
void Repository< ItemType >::cleanUp()
{
    //Buida la llista però no elimina els ItemType
    m_itemList.clear();
}

};  // end namespace udg {

#endif
