#include "synchronizetooldata.h"
#include "orthogonalplane.h"

namespace udg {

SynchronizeToolData::SynchronizeToolData()
 : ToolData()
{
    m_increment = 0;
    m_incrementView = OrthogonalPlane::XYPlane;
}

SynchronizeToolData::~SynchronizeToolData()
{
}

void SynchronizeToolData::setIncrement(double value, QString view)
{
    m_increment = value;
    m_incrementView = view;
    emit(sliceChanged());
}

double SynchronizeToolData::getIncrement()
{
    return m_increment;
}

QString SynchronizeToolData::getIncrementView()
{
    return m_incrementView;
}

}
