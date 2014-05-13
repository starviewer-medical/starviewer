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

#ifndef VOLUMEPIXELDATAITERATOR_H
#define VOLUMEPIXELDATAITERATOR_H

#include <cstddef>

namespace udg {

class VolumePixelData;

/**
    This class provides an iterator for VolumePixelData, allowing to iterate the data without knowing the actual voxel type.
 */
class VolumePixelDataIterator {

public:

    /// Creates a VolumePixelDataIterator for the given VolumePixelData pointing to the first voxel.
    explicit VolumePixelDataIterator(VolumePixelData *volumePixelData);
    /// Creates a VolumePixelDataIterator for the given VolumePixelData pointint to voxel at index [x, y, z].
    explicit VolumePixelDataIterator(VolumePixelData *volumePixelData, int x, int y, int z);

    /// Returns true if the iterator has a null pointer and false otherwise.
    bool isNull() const;

    /// Returns the value of the current voxel casted to the desired type.
    /// \warning It's not checked whether the pointer is valid or not.
    template <class T> T get() const;
    /// Sets the value of the current voxel to the result of casting the given value to the actual pointer type.
    /// \warning It's not checked whether the pointer is valid or not.
    template <class T> void set(T value);

    /// Returns a new iterator pointing \a i voxels after this.
    VolumePixelDataIterator operator +(int i) const;
    /// Returns a new iterator pointing \a i voxels before this.
    VolumePixelDataIterator operator -(int i) const;
    /// Makes this iterator point to the next voxel and returns a reference to this iterator.
    VolumePixelDataIterator& operator ++();
    /// Makes this iterator point to the next voxel and returns a copy of this iterator before the change.
    VolumePixelDataIterator operator ++(int);
    /// Makes this iterator point to the previous voxel and returns a reference to this iterator.
    VolumePixelDataIterator& operator --();
    /// Makes this iterator point to the previous voxel and returns a copy of this iterator before the change.
    VolumePixelDataIterator operator --(int);

    /// Returns true if this iterator and \a it point to the same voxel and have the same scalar type.
    bool operator ==(const VolumePixelDataIterator &it) const;
    /// Returns true if this iterator and \a it point to a different voxel or have a different scalar type.
    bool operator !=(const VolumePixelDataIterator &it) const;
    /// Returns true if this iterator points to a voxel after the one pointed by \a it and both have the same scalar type.
    bool operator >(const VolumePixelDataIterator &it) const;
    /// Returns true if this iterator points to a voxel before the one pointed by \a it and both have the same scalar type.
    bool operator <(const VolumePixelDataIterator &it) const;
    /// Returns true if this iterator points to a voxel not before the one pointed by \a it and both have the same scalar type.
    bool operator >=(const VolumePixelDataIterator &it) const;
    /// Returns true if this iterator points to a voxel not after the one pointed by \a it and both have the same scalar type.
    bool operator <=(const VolumePixelDataIterator &it) const;

    /// Makes this iterator point \a i voxels after the current and returns a reference to this iterator.
    VolumePixelDataIterator& operator +=(int i);
    /// Makes this iterator point \a i voxels before the current and returns a reference to this iterator.
    VolumePixelDataIterator& operator -=(int i);

private:

    /// Creates a VolumePixelDataIterator with the given pointer and scalar type.
    VolumePixelDataIterator(void *pointer, int scalarType);

    /// Returns the size of the scalar type in bytes.
    size_t scalarSize() const;

private:

    /// Pointer to the raw pixel data.
    void *m_pointer;
    /// Scalar type of the pixel data represented as a VTK integer constant.
    const int m_scalarType;

};

} // namespace udg

#endif // VOLUMEPIXELDATAITERATOR_H
