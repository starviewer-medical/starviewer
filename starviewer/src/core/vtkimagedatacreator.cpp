/*************************************************************************************
  Copyright (C) 2014 Laboratori de Gràfics i Imatge, Universitat de Girona &
  Institut de Diagnòstic per la Imatge.
  Girona 2014. All rights reserved.
  http://starviewer.udg.edu

  This file is part of the Starviewer (Medical Imaging Software) open source project.
  It is subject to the license terms in the LICENSE file found in the top-level
  directory of this distribution and at http://starviewer.udg.edu/license. No part of
  the Starviewer (Medical Imaging Software) open source project, including this file,
  may be copied, modified, propagated, or distributed except according to the
  terms contained in the LICENSE file.
 *************************************************************************************/

#include "vtkimagedatacreator.h"

#include <QtGlobal>

#include <vtkImageData.h>

namespace {

template <class T> int getDataType();

template <> int getDataType<char>()
{
    return VTK_CHAR;
}

template <> int getDataType<signed char>()
{
    return VTK_SIGNED_CHAR;
}

template <> int getDataType<unsigned char>()
{
    return VTK_UNSIGNED_CHAR;
}

template <> int getDataType<short>()
{
    return VTK_SHORT;
}

template <> int getDataType<unsigned short>()
{
    return VTK_UNSIGNED_SHORT;
}

template <> int getDataType<int>()
{
    return VTK_INT;
}

template <> int getDataType<unsigned int>()
{
    return VTK_UNSIGNED_INT;
}

template <> int getDataType<long>()
{
    return VTK_LONG;
}

template <> int getDataType<unsigned long>()
{
    return VTK_UNSIGNED_LONG;
}

template <> int getDataType<float>()
{
    return VTK_FLOAT;
}

template <> int getDataType<double>()
{
    return VTK_DOUBLE;
}

}

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
    imageData->AllocateScalars(getDataType<T>(), 1);
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

} // namespace udg
