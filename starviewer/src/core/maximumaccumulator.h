/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGMAXIMUMACCUMULATOR_H
#define UDGMAXIMUMACCUMULATOR_H

#include "accumulator.h"

#include <QtGlobal>

namespace udg {

/**
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
template <class T> class MaximumAccumulator : public Accumulator<T>
{
public:
    MaximumAccumulator( unsigned long /*size*/ ) : Accumulator<T>() {}
    virtual ~MaximumAccumulator() {}

    inline virtual void initialize() { m_first = true; }
    inline virtual void accumulate( T input )
    {
        if ( m_first )
        {
            m_maximum = input;
            m_first = false;
            return;
        }
        m_maximum = qMax( m_maximum, input );
    }
    inline virtual T getValue() const { return m_maximum; }

private:
    T m_maximum;
    bool m_first;
};


}

#endif
