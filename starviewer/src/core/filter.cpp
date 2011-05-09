#include "filter.h"
#include "volume.h"

namespace udg {

Filter::Filter()
{
    m_filterName = "";
}

Filter::~Filter()
{
    delete m_outputVolume;
    delete m_inputVolume;
}

void Filter::setInput(Volume *input)
{
    m_inputVolume = input;
}

Volume* Filter::getOutput()
{
    return m_outputVolume;
}

void Filter::setFilterName(QString name)
{
    m_filterName = name;
}

QString Filter::getFilterName()
{
    return m_filterName;

}
}
