/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "perfusionmapcalculatorthread.h"


namespace udg {


PerfusionMapCalculatorThread::PerfusionMapCalculatorThread( int id, int numberOfThreads, QObject * parent )
    : QThread(parent),
      m_id( id ), m_numberOfThreads( numberOfThreads )
{
}


PerfusionMapCalculatorThread::~PerfusionMapCalculatorThread()
{
}

void PerfusionMapCalculatorThread::run()
{
}

}
