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

#include "identifier.h"

namespace udg {

Identifier::Identifier()
{
    m_id = -1;
}

Identifier::Identifier(int id)
{
    if (id >= 0)
    {
        m_id = id;
    }
    else
    {
        m_id = -1;
    }
}

Identifier::Identifier(const Identifier &id)
{
    m_id = id.m_id;
}

Identifier::~Identifier()
{
}

void Identifier::setValue(int newId)
{
    if (newId >= 0)
    {
        m_id = newId;
    }
}

bool Identifier::isNull()
{
    if (m_id == -1)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool Identifier::operator !=(const Identifier &identifier)
{
    return !(*this == identifier);
}

bool Identifier::operator ==(const Identifier &id)
{
    if (m_id == id.m_id)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool Identifier::operator <=(const Identifier &id)
{
    if (m_id <= id.m_id)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool Identifier::operator >(const Identifier &id)
{
    return ! (*this <= id);
}

bool Identifier::operator >=(const Identifier &id)
{
    if (m_id >= id.m_id)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool Identifier::operator <(const Identifier &id) const
{
    return m_id < id.m_id;
}

Identifier& Identifier::operator =(const int id)
{
    m_id = id;
    return *this;
}

Identifier& Identifier::operator =(const Identifier &id)
{
    m_id = id.m_id;
    return *this;
}

Identifier Identifier::operator +(const Identifier &id)
{
    Identifier result;
    result.m_id = m_id + id.m_id;
    return result;
}

Identifier Identifier::operator +(const int id)
{
    Identifier result;
    result.m_id = m_id + id;
    return result;
}

Identifier Identifier::operator +=(const Identifier &id)
{
    m_id += id.m_id;
    return *this;
}

Identifier Identifier::operator +=(const int id)
{
    m_id += id;
    return *this;
}

Identifier Identifier::operator -(const Identifier &id)
{
    Identifier result;
    result.m_id = m_id - id.m_id;
    return result;
}

Identifier Identifier::operator -(const int id)
{
    Identifier result;
    result.m_id = m_id - id;
    return result;
}

Identifier Identifier::operator -=(const Identifier &id)
{
    m_id -= id.m_id;
    return *this;
}

Identifier Identifier::operator -=(const int id)
{
    m_id -= id;
    return *this;
}

std::ostream& operator <<(std::ostream &out, const Identifier &id)
{
    return out << id.m_id;
}

bool operator==(const Identifier &id1, const Identifier &id2)
{
    return id1.m_id == id2.m_id;
}

unsigned int qHash(const Identifier &id)
{
    return static_cast<unsigned int>(id.m_id);
}

};
