#include "imageplaneprojectiontooldata.h"
#include "imageplane.h"
#include "volume.h"

namespace udg {

ImagePlaneProjectionToolData::ImagePlaneProjectionToolData(QObject *parent)
 : ToolData(parent)
{
    m_volume = NULL;
}

ImagePlaneProjectionToolData::~ImagePlaneProjectionToolData()
{
}

void ImagePlaneProjectionToolData::setVolume( Volume *volume )
{
    m_volume = volume;
    emit volumeChanged( m_volume );
}

Volume * ImagePlaneProjectionToolData::getVolume()
{
    return m_volume;
}

void ImagePlaneProjectionToolData::setProjectedLineImagePlane( QString projectedLineName, ImagePlane *imagePlane )
{
    m_imagePlanes.insert( projectedLineName, imagePlane );
    emit imagePlaneUpdated( projectedLineName );
}

ImagePlane * ImagePlaneProjectionToolData::getProjectedLineImagePlane( QString projectedLineName )
{
    return m_imagePlanes[ projectedLineName ];
}

}
