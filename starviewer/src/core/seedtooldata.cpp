/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "seedtooldata.h"
#include "logging.h"
#include "drawerpoint.h"
#include "volume.h"


namespace udg {

SeedToolData::SeedToolData(QObject *parent)
 : ToolData(parent)
{
    m_position = QVector<double> (3,0);
    m_point = NULL;
    //DEBUG_LOG("SEED TOOL DATA CREADA ");
}


SeedToolData::~SeedToolData()
{
    if ( !m_point )
    {
        delete m_point;
    }
    //DEBUG_LOG("DYING SEED TOOL DATA ");
}

void SeedToolData::setSeedPosition(QVector<double> pos)
{
    m_position[0]=pos[0];
    m_position[1]=pos[1];
    m_position[2]=pos[2];

    if ( !m_point )
    {
        //DEBUG_LOG("Creem un nou drawerPoint");
        m_point = new DrawerPoint;
        QColor color( 217, 33, 66 );
        m_point->setColor( color );
    }
    m_point->setPosition(m_position);

}

QVector<double> SeedToolData::getSeedPosition( )
{
    return m_position;
}

DrawerPoint* SeedToolData::getPoint( )
{
    return m_point;
}

void SeedToolData::setPoint( DrawerPoint* p )
{
    m_point = p;
}

Volume* SeedToolData::getVolume( )
{
    return m_volume;
}

void SeedToolData::setVolume( Volume* vol )
{
    m_volume = vol;
}

}
