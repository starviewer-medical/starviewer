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

VolumePixelData* VolumePixelDataReader::getVolumePixelData()
{
    return m_volumePixelData;
}

} // End namespace udg
