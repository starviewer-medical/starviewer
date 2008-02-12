/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGMINIMUMACCUMULATOR_H
#define UDGMINIMUMACCUMULATOR_H

#include "accumulator.h"

#include <QtGlobal>

namespace udg {

/**
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
template <class T> class MinimumAccumulator : public Accumulator<T>
{
public:
    MinimumAccumulator( unsigned long /*size*/ ) : Accumulator<T>() {}
    virtual ~MinimumAccumulator() {}

    inline virtual void initialize() { m_first = true; }
    inline virtual void accumulate( T input )
    {
        if ( m_first )
        {
            m_minimum = input;
            m_first = false;
            return;
        }
        m_minimum = qMin( m_minimum, input );
    }
    inline virtual T getValue() const { return m_minimum; }

private:
    T m_minimum;
    bool m_first;
};


}

#endif
