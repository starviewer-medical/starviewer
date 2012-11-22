#include "vtkimagedatacreator.h"

#include <QtGlobal>

#include <vtkImageData.h>

namespace udg {

VtkImageDataCreator::VtkImageDataCreator()
{
    m_origin[0] = m_origin[1] = m_origin[2] = 0.0;
    m_spacing[0] = m_spacing[1] = m_spacing[2] = 1.0;
}

void VtkImageDataCreator::setOrigin(double origin[3])
{
    m_origin[0] = origin[0];
    m_origin[1] = origin[1];
    m_origin[2] = origin[2];
}

void VtkImageDataCreator::setSpacing(double spacing[3])
{
    m_spacing[0] = spacing[0];
    m_spacing[1] = spacing[1];
    m_spacing[2] = spacing[2];
}

template <class T> vtkSmartPointer<vtkImageData> VtkImageDataCreator::createVtkImageData(int width, int height, int depth, const T *data)
{
    Q_ASSERT(width > 0);
    Q_ASSERT(height > 0);
    Q_ASSERT(depth > 0);

    if (!data)
    {
        return vtkSmartPointer<vtkImageData>();
    }

    vtkSmartPointer<vtkImageData> imageData = vtkSmartPointer<vtkImageData>::New();
    imageData->SetOrigin(m_origin);
    imageData->SetSpacing(m_spacing);
    imageData->SetExtent(0, width - 1, 0, height - 1, 0, depth - 1);
    setImageDataScalarType<T>(imageData);
    imageData->SetNumberOfScalarComponents(1);
    imageData->AllocateScalars();
    memcpy(imageData->GetScalarPointer(), data, width * height * depth * sizeof(T));
    return imageData;
}

// Fem que s'instanciï el mètode per tots aquests tipus. Això evita errors d'undefined reference o unresolved external symbol.
template vtkSmartPointer<vtkImageData> VtkImageDataCreator::createVtkImageData(int width, int height, int depth, const char *data);
template vtkSmartPointer<vtkImageData> VtkImageDataCreator::createVtkImageData(int width, int height, int depth, const signed char *data);
template vtkSmartPointer<vtkImageData> VtkImageDataCreator::createVtkImageData(int width, int height, int depth, const unsigned char *data);
template vtkSmartPointer<vtkImageData> VtkImageDataCreator::createVtkImageData(int width, int height, int depth, const short *data);
template vtkSmartPointer<vtkImageData> VtkImageDataCreator::createVtkImageData(int width, int height, int depth, const unsigned short *data);
template vtkSmartPointer<vtkImageData> VtkImageDataCreator::createVtkImageData(int width, int height, int depth, const int *data);
template vtkSmartPointer<vtkImageData> VtkImageDataCreator::createVtkImageData(int width, int height, int depth, const unsigned int *data);
template vtkSmartPointer<vtkImageData> VtkImageDataCreator::createVtkImageData(int width, int height, int depth, const long *data);
template vtkSmartPointer<vtkImageData> VtkImageDataCreator::createVtkImageData(int width, int height, int depth, const unsigned long *data);
template vtkSmartPointer<vtkImageData> VtkImageDataCreator::createVtkImageData(int width, int height, int depth, const float *data);
template vtkSmartPointer<vtkImageData> VtkImageDataCreator::createVtkImageData(int width, int height, int depth, const double *data);

template <> void VtkImageDataCreator::setImageDataScalarType<char>(vtkImageData *imageData)
{
    imageData->SetScalarTypeToChar();
}

template <> void VtkImageDataCreator::setImageDataScalarType<signed char>(vtkImageData *imageData)
{
    imageData->SetScalarTypeToSignedChar();
}

template <> void VtkImageDataCreator::setImageDataScalarType<unsigned char>(vtkImageData *imageData)
{
    imageData->SetScalarTypeToUnsignedChar();
}

template <> void VtkImageDataCreator::setImageDataScalarType<short>(vtkImageData *imageData)
{
    imageData->SetScalarTypeToShort();
}

template <> void VtkImageDataCreator::setImageDataScalarType<unsigned short>(vtkImageData *imageData)
{
    imageData->SetScalarTypeToUnsignedShort();
}

template <> void VtkImageDataCreator::setImageDataScalarType<int>(vtkImageData *imageData)
{
    imageData->SetScalarTypeToInt();
}

template <> void VtkImageDataCreator::setImageDataScalarType<unsigned int>(vtkImageData *imageData)
{
    imageData->SetScalarTypeToUnsignedInt();
}

template <> void VtkImageDataCreator::setImageDataScalarType<long>(vtkImageData *imageData)
{
    imageData->SetScalarTypeToLong();
}

template <> void VtkImageDataCreator::setImageDataScalarType<unsigned long>(vtkImageData *imageData)
{
    imageData->SetScalarTypeToUnsignedLong();
}

template <> void VtkImageDataCreator::setImageDataScalarType<float>(vtkImageData *imageData)
{
    imageData->SetScalarTypeToFloat();
}

template <> void VtkImageDataCreator::setImageDataScalarType<double>(vtkImageData *imageData)
{
    imageData->SetScalarTypeToDouble();
}

} // namespace udg
