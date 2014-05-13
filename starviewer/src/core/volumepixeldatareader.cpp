#include "volumepixeldatareader.h"
#include <vtkImageData.h>
namespace udg {

VolumePixelDataReader::VolumePixelDataReader(QObject *parent)
: QObject(parent)
{
    m_volumePixelData = NULL;
}

VolumePixelDataReader::~VolumePixelDataReader()
{
}

void VolumePixelDataReader::setFrameNumbers(const QList<int> &frameNumbers)
{
    m_frameNumbers = frameNumbers;
}

VolumePixelData* VolumePixelDataReader::getVolumePixelData()
{
    return m_volumePixelData;
}

} // End namespace udg
