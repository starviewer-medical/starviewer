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

#ifndef UDGACCUMULATOR_CPP
#define UDGACCUMULATOR_CPP

#include "accumulator.h"

#include "maximumaccumulator.h"
#include "minimumaccumulator.h"
#include "averageaccumulator.h"

namespace udg {

template <class T>
Accumulator<T>* AccumulatorFactory::getAccumulator(AccumulatorType type, unsigned long size)
{
    switch (type)
    {
        case Maximum:
            return new MaximumAccumulator<T>();
        case Minimum:
            return new MinimumAccumulator<T>();
        case Average:
            return new AverageAccumulator<T>(size);
        default:
            return 0;
    }
}

template <class T>
Accumulator<T>* AccumulatorFactory::getAccumulator(const QString &type, unsigned long size)
{
    if (type == "Maximum")
    {
        return new MaximumAccumulator<T>();
    }
    else if (type == "Minimum")
    {
        return new MinimumAccumulator<T>();
    }
    else if (type == "Average")
    {
        return new AverageAccumulator<T>(size);
    }
    else
    {
        return 0;
    }
}

}

#endif
