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

#ifndef UDG_SLICEORIENTEDVOLUMEPIXELDATA_H
#define UDG_SLICEORIENTEDVOLUMEPIXELDATA_H

#include "orthogonalplane.h"
#include "vector3.h"

#include <vtkSmartPointer.h>

class vtkMatrix4x4;

namespace udg {

class VolumePixelData;
class Voxel;
class VoxelIndex;

/**
 * @brief The SliceOrientedVolumePixelData class is a proxy class for VolumePixelData that combines it with an orthogonal plane and a pair of transformation
 *        matrices to enable access to its data and metadata in a slice-oriented or pixeldata-oriented fashion.
 */
class SliceOrientedVolumePixelData
{
public:
    SliceOrientedVolumePixelData();

    /// Sets the given volume pixel data to this object and returns the object.
    SliceOrientedVolumePixelData& setVolumePixelData(VolumePixelData *volumePixelData);
    /// Sets the given orthogonal plane to this object and returns the object.
    SliceOrientedVolumePixelData& setOrthogonalPlane(OrthogonalPlane plane);
    /// Sets the given data-to-world matrix and its inverse as world-to-data to this object and returns the object.
    SliceOrientedVolumePixelData& setDataToWorldMatrix(vtkMatrix4x4 *dataToWorldMatrix);

    /// Returns the orthogonal plane that defines the slice orientation with respect to the volume pixel data.
    const OrthogonalPlane& getOrthogonalPlane() const;
    /// Returns the data-to-world matrix that allows to transform from pixel data space to world space.
    vtkMatrix4x4* getDataToWorldMatrix() const;
    /// Returns the world-to-data matrix that allows to transform from world space to pixel data space.
    vtkMatrix4x4* getWorldToDataMatrix() const;

    /// Returns the slice oriented pixel data extent.
    std::array<int, 6> getExtent();
    /// Returns the slice oriented pixel data spacing.
    Vector3 getSpacing();

    /// Returns the slice oriented voxel index corresponding to the given world coordinate.
    VoxelIndex getVoxelIndex(const Vector3 &worldCoordinate);
    /// Returns the voxel value at the given slice oriented voxel index.
    Voxel getVoxelValue(const VoxelIndex &index);
    /// Returns the voxel value at the given world coordinate.
    Voxel getVoxelValue(const Vector3 &worldCoordinate);
    /// Returns the world coordinate corresponding to the given slice oriented voxel index.
    Vector3 getWorldCoordinate(const VoxelIndex &index);

private:
    /// The underlying volume pixel data.
    VolumePixelData *m_volumePixelData;
    /// The plane that defines the slice orientation with respect to the volume pixel data.
    OrthogonalPlane m_orthogonalPlane;
    /// Data-to-world matrix to transform from data space to world space.
    vtkSmartPointer<vtkMatrix4x4> m_dataToWorldMatrix;
    /// Inverse of the data-to-world matrix.
    vtkSmartPointer<vtkMatrix4x4> m_worldToDataMatrix;

};

} // namespace udg

#endif // UDG_SLICEORIENTEDVOLUMEPIXELDATA_H
