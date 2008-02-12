/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGAVERAGEACCUMULATOR_H
#define UDGAVERAGEACCUMULATOR_H

#include "accumulator.h"

#include <QtGlobal>

namespace udg {

/**
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
template <class T> class AverageAccumulator : public Accumulator<T>
{
public:
    AverageAccumulator( unsigned long size ) : Accumulator<T>() { m_size = size; }
    virtual ~AverageAccumulator() {}

    inline virtual void initialize() { m_average = 0.0; }
    inline virtual void accumulate( T input ) { m_average += input / m_size; }
    /// \todo si T és float o double no s'hauria de fer el round
    inline virtual T getValue() const { return static_cast<T>( round( m_average ) ); }

private:
    double m_size;
    double m_average;
};


}

#endif
