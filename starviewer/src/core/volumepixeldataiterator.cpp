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

#include "volumepixeldataiterator.h"

#include "logging.h"
#include "volumepixeldata.h"

#include <vtkImageData.h>

// Macro for a single case of the SWITCH_SCALAR_TYPE macro.
#define SWITCH_SCALAR_TYPE_CASE(vtkTypeName, typeName, code) case VTK_ ## vtkTypeName: \
    {                                                                                  \
        typedef typeName ScalarType;                                                   \
        code;                                                                          \
    }                                                                                  \
    break;

// Macro for the default case of the SWITCH_SCALAR_TYPE macro.
#define SWITCH_SCALAR_TYPE_CASE_DEFAULT(defaultCode) default:        \
    {                                                                \
        WARN_LOG(QString("Unsupported type: %1").arg(m_scalarType)); \
        defaultCode;                                                 \
    }                                                                \
    break;

// This macro implements a switch to handle the different scalar types of pixel data.
// For each supported scalar type, the type is typedefed as ScalarType and code is run.
// For the default case, meaning an unsupported scalar type, defaultCode is run.
#define SWITCH_SCALAR_TYPE(code, defaultCode) switch (m_scalarType)  \
{                                                                    \
    SWITCH_SCALAR_TYPE_CASE(CHAR, char, code)                        \
    SWITCH_SCALAR_TYPE_CASE(SIGNED_CHAR, signed char, code)          \
    SWITCH_SCALAR_TYPE_CASE(UNSIGNED_CHAR, unsigned char, code)      \
    SWITCH_SCALAR_TYPE_CASE(SHORT, short, code)                      \
    SWITCH_SCALAR_TYPE_CASE(UNSIGNED_SHORT, unsigned short, code)    \
    SWITCH_SCALAR_TYPE_CASE(INT, int, code)                          \
    SWITCH_SCALAR_TYPE_CASE(UNSIGNED_INT, unsigned int, code)        \
    SWITCH_SCALAR_TYPE_CASE(LONG, long, code)                        \
    SWITCH_SCALAR_TYPE_CASE(UNSIGNED_LONG, unsigned long, code)      \
    SWITCH_SCALAR_TYPE_CASE(FLOAT, float, code)                      \
    SWITCH_SCALAR_TYPE_CASE(DOUBLE, double, code)                    \
    SWITCH_SCALAR_TYPE_CASE_DEFAULT(defaultCode)                     \
}

namespace udg {

VolumePixelDataIterator::VolumePixelDataIterator(VolumePixelData *volumePixelData)
    : m_pointer(volumePixelData->getVtkData()->GetScalarPointerForExtent(volumePixelData->getVtkData()->GetExtent())),
      // ^ HACK: a simple GetScalarPointer() will abort in VTK 6.1 if there is no data allocated, while a GetScalarPointerForExtent() won't.
      m_scalarType(volumePixelData->getVtkData()->GetScalarType())
{
}

VolumePixelDataIterator::VolumePixelDataIterator(VolumePixelData *volumePixelData, int x, int y, int z)
    : m_pointer(volumePixelData->getVtkData()->GetScalarPointer(x, y, z)), m_scalarType(volumePixelData->getVtkData()->GetScalarType())
{
}

bool VolumePixelDataIterator::isNull() const
{
    return m_pointer == NULL;
}

template <class T>
T VolumePixelDataIterator::get() const
{
    SWITCH_SCALAR_TYPE(return *static_cast<ScalarType*>(m_pointer),
                       return T())
}

// Explicit instantiation of get for common types.
template qint8 VolumePixelDataIterator::get() const;
template quint8 VolumePixelDataIterator::get() const;
template qint16 VolumePixelDataIterator::get() const;
template quint16 VolumePixelDataIterator::get() const;
template qint32 VolumePixelDataIterator::get() const;
template quint32 VolumePixelDataIterator::get() const;
template qint64 VolumePixelDataIterator::get() const;
template quint64 VolumePixelDataIterator::get() const;
template float VolumePixelDataIterator::get() const;
template double VolumePixelDataIterator::get() const;

template <class T>
void VolumePixelDataIterator::set(T value)
{
    SWITCH_SCALAR_TYPE(*static_cast<ScalarType*>(m_pointer) = value,
                       /* no default code */)
}

// Explicit instantiation of set for common types.
template void VolumePixelDataIterator::set(qint8);
template void VolumePixelDataIterator::set(quint8);
template void VolumePixelDataIterator::set(qint16);
template void VolumePixelDataIterator::set(quint16);
template void VolumePixelDataIterator::set(qint32);
template void VolumePixelDataIterator::set(quint32);
template void VolumePixelDataIterator::set(qint64);
template void VolumePixelDataIterator::set(quint64);
template void VolumePixelDataIterator::set(float);
template void VolumePixelDataIterator::set(double);

VolumePixelDataIterator VolumePixelDataIterator::operator +(int i) const
{
    return VolumePixelDataIterator(static_cast<char*>(m_pointer) + i * scalarSize(), m_scalarType);
}

VolumePixelDataIterator VolumePixelDataIterator::operator -(int i) const
{
    return VolumePixelDataIterator(static_cast<char*>(m_pointer) - i * scalarSize(), m_scalarType);
}

VolumePixelDataIterator& VolumePixelDataIterator::operator ++()
{
    m_pointer = static_cast<char*>(m_pointer) + scalarSize();
    return *this;
}

VolumePixelDataIterator VolumePixelDataIterator::operator ++(int)
{
    VolumePixelDataIterator old(*this);
    m_pointer = static_cast<char*>(m_pointer) + scalarSize();
    return old;
}

VolumePixelDataIterator& VolumePixelDataIterator::operator --()
{
    m_pointer = static_cast<char*>(m_pointer) - scalarSize();
    return *this;
}

VolumePixelDataIterator VolumePixelDataIterator::operator --(int)
{
    VolumePixelDataIterator old(*this);
    m_pointer = static_cast<char*>(m_pointer) - scalarSize();
    return old;
}

bool VolumePixelDataIterator::operator ==(const VolumePixelDataIterator &it) const
{
    return this->m_pointer == it.m_pointer && this->m_scalarType == it.m_scalarType;
}

bool VolumePixelDataIterator::operator !=(const VolumePixelDataIterator &it) const
{
    return this->m_pointer != it.m_pointer || this->m_scalarType != it.m_scalarType;
}

bool VolumePixelDataIterator::operator >(const VolumePixelDataIterator &it) const
{
    return this->m_pointer > it.m_pointer && this->m_scalarType == it.m_scalarType;
}

bool VolumePixelDataIterator::operator <(const VolumePixelDataIterator &it) const
{
    return this->m_pointer < it.m_pointer && this->m_scalarType == it.m_scalarType;
}

bool VolumePixelDataIterator::operator >=(const VolumePixelDataIterator &it) const
{
    return this->m_pointer >= it.m_pointer && this->m_scalarType == it.m_scalarType;
}

bool VolumePixelDataIterator::operator <=(const VolumePixelDataIterator &it) const
{
    return this->m_pointer <= it.m_pointer && this->m_scalarType == it.m_scalarType;
}

VolumePixelDataIterator& VolumePixelDataIterator::operator +=(int i)
{
    m_pointer = static_cast<char*>(m_pointer) + i * scalarSize();
    return *this;
}

VolumePixelDataIterator& VolumePixelDataIterator::operator -=(int i)
{
    m_pointer = static_cast<char*>(m_pointer) - i * scalarSize();
    return *this;
}

VolumePixelDataIterator::VolumePixelDataIterator(void *pointer, int scalarType) :
    m_pointer(pointer), m_scalarType(scalarType)
{
}

size_t VolumePixelDataIterator::scalarSize() const
{
    SWITCH_SCALAR_TYPE(return sizeof(ScalarType),
                       return 0)
}

} // namespace udg
