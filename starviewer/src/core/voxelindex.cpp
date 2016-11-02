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

#include "voxelindex.h"

namespace udg {

VoxelIndex::VoxelIndex()
    : VoxelIndex(-1, -1, -1)
{
}

VoxelIndex::VoxelIndex(int x, int y, int z)
{
    m_index[0] = x;
    m_index[1] = y;
    m_index[2] = z;
}

bool VoxelIndex::isValid() const
{
    return x() >= 0 && y() >= 0 && z() >= 0;
}

int VoxelIndex::x() const
{
    return m_index[0];
}

int VoxelIndex::y() const
{
    return m_index[1];
}

int VoxelIndex::z() const
{
    return m_index[2];
}

int VoxelIndex::operator[](int i) const
{
    return m_index[i];
}

int& VoxelIndex::operator[](int i)
{
    return m_index[i];
}

bool VoxelIndex::operator==(const VoxelIndex &index) const
{
    return this->x() == index.x() && this->y() == index.y() && this->z() == index.z();
}

} // namespace udg
