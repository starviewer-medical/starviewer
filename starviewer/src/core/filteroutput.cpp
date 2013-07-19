#include "filteroutput.h"

#include "filter.h"

#include <vtkAlgorithm.h>
#include <vtkImageData.h>

namespace udg {

FilterOutput::FilterOutput(Filter *filter) :
    m_filter(filter)
{
}

Filter* FilterOutput::getFilter() const
{
    return m_filter;
}

vtkAlgorithmOutput* FilterOutput::getVtkAlgorithmOutput()
{
    return m_filter->getVtkAlgorithm()->GetOutputPort();
}

vtkImageData* FilterOutput::getVtkImageData()
{
    m_filter->update();
    return vtkImageData::SafeDownCast(m_filter->getVtkAlgorithm()->GetOutputDataObject(0));
}

}
