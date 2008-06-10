/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "pacslist.h"

#include <QString>

namespace udg {

PacsList::PacsList()
{
}

void PacsList::insertPacs( PacsParameters pacs )
{
    m_listPacs.push_back( pacs );
}

void PacsList::firstPacs()
{
    i = m_listPacs.begin();
}

void PacsList::nextPacs()
{
    i++;
}

bool PacsList::findPacs( QString AETitle )
{

    i = m_listPacs.begin();
    if ( i == m_listPacs.end() ) return false;


    while ( i != m_listPacs.end() )
    {
        if ( (*i).getAEPacs() == AETitle ) break;
        else i++;
    }
    return ( i != m_listPacs.end() );
}

int PacsList::size()
{
    return m_listPacs.size();
}

bool PacsList::end()
{
    return i == m_listPacs.end();
}

PacsParameters PacsList::getPacs()
{
    return ( *i );
}

void PacsList::clear()
{
    m_listPacs.clear();
}

PacsList::~PacsList()
{
}

};
