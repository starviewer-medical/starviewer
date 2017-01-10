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

#include "sliceorientedvolumepixeldata.h"

#include "logging.h"
#include "volumepixeldata.h"
#include "voxel.h"
#include "voxelindex.h"

#include <vtkMatrix4x4.h>

namespace udg {

namespace {

// Computes the index that corresponds to each axis to transform from data space to slice space.
void getDataToSliceIndices(const OrthogonalPlane &plane, int &x, int &y, int &z)
{
    plane.getXYZIndexes(x, y, z);
}

// Computes the index that corresponds to each axis to transform from slice space to data space.
void getSliceToDataIndices(const OrthogonalPlane &plane, int &x, int &y, int &z)
{
    plane.getXYZIndexes(x, y, z);
    QVarLengthArray<int> dts{x, y, z};
    x = dts.indexOf(0);
    y = dts.indexOf(1);
    z = dts.indexOf(2);
}

}

SliceOrientedVolumePixelData::SliceOrientedVolumePixelData()
    : m_volumePixelData(nullptr)
{
    m_dataToWorldMatrix = vtkSmartPointer<vtkMatrix4x4>::New();    // identity
    m_worldToDataMatrix = vtkSmartPointer<vtkMatrix4x4>::New();    // identity
}

SliceOrientedVolumePixelData& SliceOrientedVolumePixelData::setVolumePixelData(VolumePixelData *volumePixelData)
{
    m_volumePixelData = volumePixelData;
    return *this;
}

SliceOrientedVolumePixelData& SliceOrientedVolumePixelData::setOrthogonalPlane(OrthogonalPlane plane)
{
    m_orthogonalPlane = std::move(plane);
    return *this;
}

SliceOrientedVolumePixelData& SliceOrientedVolumePixelData::setDataToWorldMatrix(vtkMatrix4x4 *dataToWorldMatrix)
{
    m_dataToWorldMatrix->DeepCopy(dataToWorldMatrix);
    vtkMatrix4x4::Invert(m_dataToWorldMatrix, m_worldToDataMatrix);
    return *this;
}

const OrthogonalPlane& SliceOrientedVolumePixelData::getOrthogonalPlane() const
{
    return m_orthogonalPlane;
}

vtkMatrix4x4* SliceOrientedVolumePixelData::getDataToWorldMatrix() const
{
    return m_dataToWorldMatrix;
}

vtkMatrix4x4* SliceOrientedVolumePixelData::getWorldToDataMatrix() const
{
    return m_worldToDataMatrix;
}

std::array<int, 6> SliceOrientedVolumePixelData::getExtent()
{
    int extent[6];
    m_volumePixelData->getExtent(extent);
    int x, y, z;
    getDataToSliceIndices(m_orthogonalPlane, x, y, z);
    return {{extent[2*x], extent[2*x+1], extent[2*y], extent[2*y+1], extent[2*z], extent[2*z+1]}};
}

Vector3 SliceOrientedVolumePixelData::getSpacing()
{
    double spacing[3];
    m_volumePixelData->getSpacing(spacing);
    int x, y, z;
    getDataToSliceIndices(m_orthogonalPlane, x, y, z);
    return Vector3(spacing[x], spacing[y], spacing[z]);
}

VoxelIndex SliceOrientedVolumePixelData::getVoxelIndex(const Vector3 &wc)
{
    if (!m_volumePixelData->getVtkData())
    {
        DEBUG_LOG("No data");
        return VoxelIndex();
    }

    double worldCoordinate[4] = { wc.x, wc.y, wc.z, 1.0 };
    double pixelDataCoordinate[4];
    m_worldToDataMatrix->MultiplyPoint(worldCoordinate, pixelDataCoordinate);

    double *origin = m_volumePixelData->getVtkData()->GetOrigin();
    double *spacing = m_volumePixelData->getVtkData()->GetSpacing();
    int index[3];

    for (int i = 0; i < 3; i++)
    {
        index[i] = qRound((pixelDataCoordinate[i] - origin[i]) / (spacing[i] == 0 ? 1 : spacing[i]));
    }

    int *extent = m_volumePixelData->getVtkData()->GetExtent();

    if (index[0] >= extent[0] && index[0] <= extent[1] &&
        index[1] >= extent[2] && index[1] <= extent[3] &&
        index[2] >= extent[4] && index[2] <= extent[5])
    {
        int x, y, z;
        getDataToSliceIndices(m_orthogonalPlane, x, y, z);
        return VoxelIndex(index[x], index[y], index[z]);
    }
    else
    {
        return VoxelIndex();
    }
}

Voxel SliceOrientedVolumePixelData::getVoxelValue(const VoxelIndex &index)
{
    int x, y, z;
    getSliceToDataIndices(m_orthogonalPlane, x, y, z);
    int dataIndex[3] = { index[x], index[y], index[z] };
    return m_volumePixelData->getVoxelValue(dataIndex);
}

Voxel SliceOrientedVolumePixelData::getVoxelValue(const Vector3 &worldCoordinate)
{
    return getVoxelValue(getVoxelIndex(worldCoordinate));
}

Vector3 SliceOrientedVolumePixelData::getWorldCoordinate(const VoxelIndex &index)
{
    if (!m_volumePixelData->getVtkData())
    {
        DEBUG_LOG("No data");
        return Vector3();
    }

    int x, y, z;
    getSliceToDataIndices(m_orthogonalPlane, x, y, z);
    VoxelIndex trueIndex(index[x], index[y], index[z]);
    int *extent = m_volumePixelData->getVtkData()->GetExtent();

    if (trueIndex[0] >= extent[0] && trueIndex[0] <= extent[1] &&
        trueIndex[1] >= extent[2] && trueIndex[1] <= extent[3] &&
        trueIndex[2] >= extent[4] && trueIndex[2] <= extent[5])
    {
        double *origin = m_volumePixelData->getVtkData()->GetOrigin();
        double *spacing = m_volumePixelData->getVtkData()->GetSpacing();
        double coordinate[4];

        for (int i = 0; i < 3; i++)
        {
            coordinate[i] = origin[i] + trueIndex[i] * spacing[i];
        }

        coordinate[3] = 1.0;
        double worldCoordinate[4];
        m_dataToWorldMatrix->MultiplyPoint(coordinate, worldCoordinate);

        return Vector3(worldCoordinate[0], worldCoordinate[1], worldCoordinate[2]);
    }
    else
    {
        return Vector3();
    }
}

} // namespace udg
