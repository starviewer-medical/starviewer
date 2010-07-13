#include "volumepixeldatareader.h"
#include <vtkImageData.h>
namespace udg {

VolumePixelDataReader::VolumePixelDataReader(QObject *parent)
: QObject(parent), m_vtkImageData(0)
{
}

VolumePixelDataReader::~VolumePixelDataReader()
{
}

vtkImageData* VolumePixelDataReader::getVTKData()
{
    return m_vtkImageData;
}

} // End namespace udg
