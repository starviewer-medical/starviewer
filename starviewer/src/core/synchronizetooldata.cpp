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
    m_level = 0;
    m_window = 0;
    m_zoomFactor = 0;
    m_panVector[0] = 0;
    m_panVector[1] = 1;
    m_panVector[2] = 2;
}


SynchronizeToolData::~SynchronizeToolData()
{
}

void SynchronizeToolData::setIncrement( double value )
{
    m_increment = value;
    emit( sliceChanged() );
}

double SynchronizeToolData::getIncrement()
{
    return m_increment;
}

void SynchronizeToolData::setWindowLevel( double window, double level )
{
    m_level = level;
    m_window = window;
    emit( windowLevelChanged() );
}

double SynchronizeToolData::getWindow()
{
    return m_window;
}

double SynchronizeToolData::getLevel()
{
    return m_level;
}

void SynchronizeToolData::setZoomFactor( double factor )
{
    m_zoomFactor = factor;
    emit( zoomFactorChanged() );
}

double SynchronizeToolData::getZoomFactor()
{
    return m_zoomFactor;
}

void SynchronizeToolData::setPan( double motionVector[3] )
{
    m_panVector[0] = motionVector[0];
    m_panVector[1] = motionVector[1];
    m_panVector[2] = motionVector[2];
    DEBUG_LOG(" DADES PAN CANVIADES ");
    emit( panChanged() );
}

double* SynchronizeToolData::getPan()
{
    return m_panVector;
}

}
