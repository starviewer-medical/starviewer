#include "dicomreferencedimage.h"

namespace udg {

DICOMReferencedImage::DICOMReferencedImage(): m_frameNumber(0)
{
}

DICOMReferencedImage::~DICOMReferencedImage()
{
}

QString DICOMReferencedImage::getDICOMReferencedImageSOPInstanceUID() const
{
    return m_DICOMReferencedImageSOPInstanceUID;
}

void DICOMReferencedImage::setDICOMReferencedImageSOPInstanceUID(const QString &referencedImageSOPInstanceUID)
{
    m_DICOMReferencedImageSOPInstanceUID = referencedImageSOPInstanceUID;
}

int DICOMReferencedImage::getFrameNumber() const
{
    return m_frameNumber;
}

void DICOMReferencedImage::setFrameNumber(int frameNumber)
{
    m_frameNumber = frameNumber;
}

QString DICOMReferencedImage::getReferenceParentSOPInstanceUID() const
{
    return m_referenceParentSOPInstanceUID;
}

void DICOMReferencedImage::setReferenceParentSOPInstanceUID(const QString &referenceParentSOPInstanceUID)
{
    m_referenceParentSOPInstanceUID = referenceParentSOPInstanceUID;
}

}
