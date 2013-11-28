#include "formulacalculator.h"

namespace udg {

FormulaCalculator::FormulaCalculator()
{
    m_imageSource = 0;
    m_tagReaderSource = 0;
}

FormulaCalculator::~FormulaCalculator()
{
}

void FormulaCalculator::setDataSource(Image *image)
{
    m_imageSource = image;
}

void FormulaCalculator::setDataSource(DICOMTagReader *tagReader)
{
    m_tagReaderSource = tagReader;
}

} // End namespace udg
