/*************************************************************************************
  Copyright (C) 2014 Laboratori de Gràfics i Imatge, Universitat de Girona &
  Institut de Diagnòstic per la Imatge.
  Girona 2014. All rights reserved.
  http://starviewer.udg.edu

  This file is part of the Starviewer (Medical Imaging Software) open source project.
  It is subject to the license terms in the LICENSE file found in the top-level
  directory of this distribution and at http://starviewer.udg.edu/license. No part of
  the Starviewer (Medical Imaging Software) open source project, including this file,
  may be copied, modified, propagated, or distributed except according to the
  terms contained in the LICENSE file.
 *************************************************************************************/

#ifndef UDGAVERAGEACCUMULATOR_H
#define UDGAVERAGEACCUMULATOR_H

#include "accumulator.h"

#include <QtGlobal>

namespace udg {

template <class T> class AverageAccumulator : public Accumulator<T> {
public:
    AverageAccumulator(unsigned long size) : Accumulator<T>()
    {
        m_size = size;
    }
    virtual ~AverageAccumulator() {}

    inline virtual void initialize()
    {
        m_average = 0.0;
    }
    inline virtual void accumulate(T input)
    {
        m_average += input / m_size;
    }
    /// \TODO si T és float o double no s'hauria de fer el round
    inline virtual T getValue() const
    {
        return static_cast<T>(qRound(m_average));
    }

private:
    double m_size;
    double m_average;
};

}

#endif
