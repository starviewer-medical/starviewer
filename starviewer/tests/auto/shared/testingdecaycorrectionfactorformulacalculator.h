#ifndef TESTINGDECAYCORRECTIONFACTORFORMULACALCULATOR_H
#define TESTINGDECAYCORRECTIONFACTORFORMULACALCULATOR_H

#include "decaycorrectionfactorformulacalculator.h"

using namespace udg;

class TestingDecayCorrectionFactorFormulaCalculator : public DecayCorrectionFactorFormulaCalculator {
public:
    bool m_canCompute;
    double m_computeValue;

public:
    TestingDecayCorrectionFactorFormulaCalculator();

    bool canCompute();

    double compute();
};

#endif // TESTINGDECAYCORRECTIONFACTORFORMULACALCULATOR_H
