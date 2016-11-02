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

#ifndef UDG_VOXELINDEX_H
#define UDG_VOXELINDEX_H

namespace udg {

/**
 * @brief The VoxelIndex class represents a 3D index of a voxel inside a volume pixel data.
 */
class VoxelIndex
{
public:
    /// Creates an invalid index.
    VoxelIndex();
    /// Creates an index with the given coordinates.
    VoxelIndex(int x, int y, int z);

    /// Returns true if all three components are positive or zero.
    bool isValid() const;

    /// Returns the x component.
    int x() const;
    /// Returns the y component.
    int y() const;
    /// Returns the z component.
    int z() const;

    /// Allows array-like read-only access to the index.
    int operator[](int i) const;
    /// Allows array-like read-write access to the index.
    int& operator[](int i);

    /// Returns true if both indices are equal and false otherwise.
    bool operator==(const VoxelIndex &index) const;

private:
    int m_index[3];

};

} // namespace udg

#endif // UDG_VOXELINDEX_H
