/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#ifndef UDGACCUMULATOR_CPP
#define UDGACCUMULATOR_CPP

#include "accumulator.h"

#include "maximumaccumulator.h"
#include "minimumaccumulator.h"
#include "averageaccumulator.h"

namespace udg {

template <class T>
Accumulator<T> * AccumulatorFactory::getAccumulator( AccumulatorType type, unsigned long size )
{
    switch ( type )
    {
        case Maximum:
            return new MaximumAccumulator<T>( size );
        case Minimum:
            return new MinimumAccumulator<T>( size );
        case Average:
            return new AverageAccumulator<T>( size );
        default:
            return 0;
    }
}

template <class T>
Accumulator<T> * AccumulatorFactory::getAccumulator( const QString &type, unsigned long size )
{
    if( type = "Maximum" )
        return new MaximumAccumulator<T>( size );
    else if( type = "Minimum" )
        return new MinimumAccumulator<T>( size );
    else if( type = "Average" )
        return new AverageAccumulator<T>( size );
    else
        return 0;
}

}

#endif
