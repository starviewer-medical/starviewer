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

#ifndef UDGACCUMULATOR_H
#define UDGACCUMULATOR_H

#include <QString>

namespace udg {

template <class T> class Accumulator {
public:
    Accumulator() {}
    virtual ~Accumulator() {}

    virtual void initialize() = 0;
    virtual void accumulate(T input) = 0;
    virtual T getValue() const = 0;
};

class AccumulatorFactory {
public:
    enum AccumulatorType { Maximum = 0, Minimum = 1, Average = 2 };
    template <class T> static Accumulator<T>* getAccumulator(AccumulatorType type, unsigned long size);
    template <class T> static Accumulator<T>* getAccumulator(const QString &type, unsigned long size);
};

}

#include "accumulator.cpp"

#endif
