#include "windowlevelfilter.h"

#include "transferfunction.h"
#include "filteroutput.h"
#include "windowlevel.h"

#include <vtkColorTransferFunction.h>
#include <vtkImageData.h>
#include "vtkImageMapToWindowLevelColors3.h"
#include <vtkLookupTable.h>

namespace udg {

WindowLevelFilter::WindowLevelFilter()
{
    m_filter = vtkImageMapToWindowLevelColors3::New();
}

WindowLevelFilter::~WindowLevelFilter()
{
    m_filter->Delete();
}

void WindowLevelFilter::setInput(vtkImageData *input)
{
    m_filter->SetInput(input);
}

void WindowLevelFilter::setInput(FilterOutput input)
{
    m_filter->SetInputConnection(input.getVtkAlgorithmOutput());
}

void WindowLevelFilter::setWindowLevel(const WindowLevel &windowLevel)
{
    setWindowLevel(windowLevel.getWidth(), windowLevel.getCenter());
}

void WindowLevelFilter::setWindowLevel(double window, double level)
{
    m_filter->SetWindow(window);
    m_filter->SetLevel(level);
}

void WindowLevelFilter::setWindow(double window)
{
    m_filter->SetWindow(window);
}

double WindowLevelFilter::getWindow() const
{
    return m_filter->GetWindow();
}

void WindowLevelFilter::setLevel(double level)
{
    m_filter->SetLevel(level);
}

double WindowLevelFilter::getLevel() const
{
    return m_filter->GetLevel();
}

void WindowLevelFilter::setTransferFunction(const TransferFunction &transferFunction)
{
    m_filter->SetLookupTable(transferFunction.vtkColorTransferFunction());
}

void WindowLevelFilter::clearTransferFunction()
{
    m_filter->SetLookupTable(0);
}

vtkAlgorithm* WindowLevelFilter::getVtkAlgorithm() const
{
    return m_filter;
}

}
