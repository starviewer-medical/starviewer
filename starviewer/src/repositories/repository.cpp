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
Identifier Repository< ItemType >::addItem( ItemType* item )
{
    Identifier id( m_nextIdentifierNumber++ );
    m_itemList[ id ] = item ;
    return id;
}

template< typename ItemType >
ItemType* Repository< ItemType >::getItem( const Identifier id )
{
 
    Identifier listID;
    
    ItemListIteratorType itr;
    itr = m_itemList.begin();
    listID = (*itr).first; // s'ha de fer així perquè sinó dóna un error guarro
    while( itr != m_itemList.end() && listID != id )
    { 
        itr++;
        listID = (*itr).first;
    }
    
    if( listID == id )
    {
        return itr->second;
    }
    else // \TODO aquí es podria generar una excepció
    {
        ItemType* dummy;
        return dummy;
    }
    
}
 
template< typename ItemType >
void Repository< ItemType >::removeItem( const Identifier id )
{
    m_itemList.erase( id );    
}

template< typename ItemType >
int Repository< ItemType >::getNumberOfItems()
{
   return m_itemList.size();
}

template< typename ItemType >
Identifier *Repository< ItemType >::getIdentifierList() 
{
    // declarem la llista d'id's que tindrà tants elements com el map
    Identifier *idList;
    int listSize = m_itemList.size();
    if( listSize == 0 )
    {
        idList = 0;
    }
    else
    {
        idList = new Identifier[ listSize ];
        // creem el corresponent iterador
        ItemListIteratorType itr;
        itr = m_itemList.begin();
        // recorrem amb un iterador el map i omplim al llista
        
        int i = 0;
        while( itr != m_itemList.end()  )
        { 
            idList[ i ] = (*itr).first;
            itr++;
            i++;
        }
    }
    
    return idList;
    
}

};  // end namespace udg {

#endif
