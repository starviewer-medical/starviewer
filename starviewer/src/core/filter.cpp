#include "filter.h"

#include "filteroutput.h"

#include <vtkAlgorithm.h>

namespace udg {

Filter::~Filter()
{
}

void Filter::update()
{
    this->getVtkAlgorithm()->Update();
}

FilterOutput Filter::getOutput()
{
    return FilterOutput(this);
}

}
