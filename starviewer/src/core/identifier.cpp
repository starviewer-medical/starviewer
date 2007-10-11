/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
 
#include "identifier.h"

namespace udg {

Identifier::Identifier()
{
    m_id = -1;
}

Identifier::Identifier( int id )
{
    if( id >= 0 )
        m_id = id;
    else
        m_id = -1;
}

Identifier::Identifier( const Identifier &id )
{
    m_id = id.m_id;
}


Identifier::~Identifier()
{
}

void Identifier::setValue( int newId )
{
    if( newId >= 0 )
    {
        m_id = newId;
    }
}

bool Identifier::isNull()
{
    if( m_id == -1 )
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool Identifier::operator !=( const Identifier &identifier )
{
    return !( *this == identifier );
}

bool Identifier::operator ==( const Identifier &id )
{
    if( m_id == id.m_id )
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool Identifier::operator <=( const Identifier &id )
{
    if( m_id <= id.m_id )
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool Identifier::operator >( const Identifier &id )
{
    return ! ( *this <= id );
}

bool Identifier::operator >=( const Identifier &id )
{
    if( m_id >= id.m_id )
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool Identifier::operator <( const Identifier &id ) const
{
    return m_id < id.m_id;
}

Identifier& Identifier::operator =( const int id )
{
    m_id = id;
    return *this;
}

Identifier& Identifier::operator =( const Identifier & id )
{
    m_id = id.m_id;
    return *this;
}


Identifier Identifier::operator +( const Identifier & id )
{
    Identifier result;
    result.m_id = m_id + id.m_id;
    return result;
}

Identifier Identifier::operator +( const int  id )
{
    Identifier result;
    result.m_id = m_id + id;
    return result;
}

Identifier Identifier::operator +=( const Identifier & id )
{
    m_id += id.m_id;
    return *this;
}

Identifier Identifier::operator +=( const int  id )
{
    m_id += id;
    return *this;
}

Identifier Identifier::operator -( const Identifier & id )
{
    Identifier result;
    result.m_id = m_id - id.m_id;
    return result;
}

Identifier Identifier::operator -( const int  id )
{
    Identifier result;
    result.m_id = m_id - id;
    return result;
}

Identifier Identifier::operator -=( const Identifier & id )
{
    m_id -= id.m_id;
    return *this;
}

Identifier Identifier::operator -=( const int  id )
{
    m_id -= id;
    return *this;
}

std::ostream& operator <<( std::ostream &out, const Identifier &id )
{
    return out << id.m_id;
}

};
