/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "synchronizetooldata.h"
#include "q2dviewer.h"
#include "logging.h"

namespace udg {

SynchronizeToolData::SynchronizeToolData()
 : ToolData()
{
    m_increment = 0;
}


SynchronizeToolData::~SynchronizeToolData()
{
}

void SynchronizeToolData::setIncrement( int value )
{
    m_increment = value;
    emit( dataChanged() );
}

int SynchronizeToolData::getIncrement()
{
    return m_increment;
}


}
