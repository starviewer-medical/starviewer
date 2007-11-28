/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "toolconfiguration.h"


namespace udg {

ToolConfiguration::ToolConfiguration(QObject *parent)
 : QObject(parent)
{
}


ToolConfiguration::~ToolConfiguration()
{
}


void ToolConfiguration::addAttribute( QString attributeName, QVariant value )
{
    attributeMap.insert( attributeName, value );
}

void ToolConfiguration::setValue( QString attributeName, QVariant value )
{
    attributeMap.insert( attributeName, value );
}

QVariant ToolConfiguration::getValue( QString attributeName )
{
    return attributeMap.value( attributeName );
}

bool ToolConfiguration::containsValue( QString attributeName )
{
    return attributeMap.contains( attributeName );
}

}
