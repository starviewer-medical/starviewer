#include "transdifferencetooldata.h"
#include "q2dviewer.h"
#include "volume.h"
#include "logging.h"

namespace udg {

TransDifferenceToolData::TransDifferenceToolData()
 : ToolData(), m_inputVolume(0), m_differenceVolume(0)
{
}

TransDifferenceToolData::~TransDifferenceToolData()
{
}

void TransDifferenceToolData::setInputVolume(Volume *input)
{
    m_inputVolume = input;
    // Quan canviem l'input cal invalidar el volum diferència
    m_differenceVolume = 0;
    // Quan canviem l'input posem totes les transicions a 0
    m_sliceTranslations = QVector<QPair<int, int> >(m_inputVolume->getDimensions()[2], QPair<int, int>(0, 0));
}

void TransDifferenceToolData::setDifferenceVolume(Volume *input)
{
    m_differenceVolume = input;
}
}
