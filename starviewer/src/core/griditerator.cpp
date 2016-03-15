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

#include "griditerator.h"

namespace udg {

GridIterator::GridIterator(int rows, int columns) :
    m_rows(rows), m_columns(columns), m_rowIndex(0), m_columnIndex(0)
{
}

int GridIterator::rows() const
{
    return m_rows;
}

int GridIterator::columns() const
{
    return m_columns;
}

int GridIterator::row() const
{
    return m_rowIndex;
}

int GridIterator::column() const
{
    return m_columnIndex;
}

void GridIterator::next()
{
    m_columnIndex++;

    if (m_columnIndex == m_columns)
    {
        m_rowIndex++;
        m_columnIndex = 0;
    }
}

bool GridIterator::isAtEnd() const
{
    return m_rowIndex >= m_rows;
}

} // namespace udg
