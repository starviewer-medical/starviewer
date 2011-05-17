#ifndef UDGMINIMUMACCUMULATOR_H
#define UDGMINIMUMACCUMULATOR_H

#include "accumulator.h"

#include <QtGlobal>

namespace udg {

template <class T> class MinimumAccumulator : public Accumulator<T> {
public:
    MinimumAccumulator(unsigned long size) : Accumulator<T>() {}
    virtual ~MinimumAccumulator() {}

    inline virtual void initialize()
    {
        m_first = true;
    }
    inline virtual void accumulate(T input)
    {
        if (m_first)
        {
            m_minimum = input;
            m_first = false;
            return;
        }
        m_minimum = qMin(m_minimum, input);
    }
    inline virtual T getValue() const
    {
        return m_minimum;
    }

private:
    T m_minimum;
    bool m_first;
};

}

#endif
