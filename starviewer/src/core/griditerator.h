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

#ifndef UDG_GRIDITERATOR_H
#define UDG_GRIDITERATOR_H

namespace udg {

/**
 * @brief The GridIterator class allows easy iteration over the cells of a grid.
 *
 * Given the grid size in rows and columns, it starts at the first cell (0,0) and iterates from left to right and from top to bottom.
 */
class GridIterator
{
public:
    /// Creates an iterator for a grid with the given number of rows and columns, and pointing to the first cell (0,0).
    GridIterator(int rows, int columns);

    /// Returns the number of rows.
    int rows() const;
    /// Returns the number of columns.
    int columns() const;

    /// Returns the current row.
    int row() const;
    /// Returns the current column.
    int column() const;

    /// Advances the iterator to the next cell.
    void next();

    /// Returns true if the iterator has reached the end, i.e. if it is pointing to an invalid cell.
    bool isAtEnd() const;

private:
    /// Number of rows.
    int m_rows;
    /// Number of columns.
    int m_columns;
    /// Current row index.
    int m_rowIndex;
    /// Current column index.
    int m_columnIndex;

};

} // namespace udg

#endif // UDG_GRIDITERATOR_H
