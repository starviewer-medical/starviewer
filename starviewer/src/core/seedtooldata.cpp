/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "seedtooldata.h"
#include "logging.h"


namespace udg {

SeedToolData::SeedToolData(QObject *parent)
 : ToolData(parent)
{
    m_position = QVector<double> (3,0);
    DEBUG_LOG("SEED TOOL DATA CREADA ");
    //std::cout<<this<<std::endl;
}


SeedToolData::~SeedToolData()
{
    DEBUG_LOG("DYING SEED TOOL DATA ");
}

void SeedToolData::setSeedPosition(QVector<double> pos)
{
    m_position[0]=pos[0];
    m_position[1]=pos[1];
    m_position[2]=pos[2];
}

QVector<double> SeedToolData::getSeedPosition( )
{
    return m_position;
}

}
