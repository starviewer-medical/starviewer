/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "imageplane.h"
#include <vtkMath.h>

namespace udg {

ImagePlane::ImagePlane()
 : m_rows(1), m_columns(1)
{
    setRowDirectionVector( 1., 0., 0. );
    setColumnDirectionVector( 0., 1., 0. );
}

ImagePlane::~ImagePlane()
{
}

void ImagePlane::setRowDirectionVector( const double vector[3] )
{
    setRowDirectionVector( vector[0], vector[1], vector[2] );
}

void ImagePlane::setRowDirectionVector( double x, double y, double z )
{
    m_rowDirectionVector[0] = x;
    m_rowDirectionVector[1] = y;
    m_rowDirectionVector[2] = z;
    vtkMath::Cross( m_rowDirectionVector, m_columnDirectionVector, m_normal );
}

void ImagePlane::setColumnDirectionVector( const double vector[3] )
{
    setColumnDirectionVector( vector[0], vector[1], vector[2] );
}

void ImagePlane::setColumnDirectionVector( double x, double y, double z )
{
    m_columnDirectionVector[0] = x;
    m_columnDirectionVector[1] = y;
    m_columnDirectionVector[2] = z;
    vtkMath::Cross( m_rowDirectionVector, m_columnDirectionVector, m_normal );
}

void ImagePlane::getRowDirectionVector( double vector[3] )
{
    vector[0] = m_rowDirectionVector[0];
    vector[1] = m_rowDirectionVector[1];
    vector[2] = m_rowDirectionVector[2];
}

void ImagePlane::getColumnDirectionVector( double vector[3] )
{
    vector[0] = m_columnDirectionVector[0];
    vector[1] = m_columnDirectionVector[1];
    vector[2] = m_columnDirectionVector[2];
}

void ImagePlane::getNormalVector( double vector[3] )
{
    vector[0] = m_normal[0];
    vector[1] = m_normal[1];
    vector[2] = m_normal[2];
}

void ImagePlane::setOrigin( double origin[3] )
{
    setOrigin( origin[0], origin[1], origin[2] );
}

void ImagePlane::setOrigin( double x, double y, double z )
{
    m_origin[0] = x;
    m_origin[1] = y;
    m_origin[2] = z;
}

void ImagePlane::getOrigin( double origin[3] )
{
    origin[0] = m_origin[0];
    origin[1] = m_origin[1];
    origin[2] = m_origin[2];
}

void ImagePlane::setSpacing( double spacing[2] )
{
    setSpacing( spacing[0], spacing[1] );
}

void ImagePlane::setSpacing( double x, double y )
{
    m_spacing[0] = x;
    m_spacing[1] = y;
}

void ImagePlane::getSpacing( double spacing[2] )
{
    spacing[0] = m_spacing[0];
    spacing[1] = m_spacing[1];
}

void ImagePlane::setRows( int rows )
{
    m_rows = rows;
}

void ImagePlane::setColumns( int columns )
{
    m_columns = columns;
}

double ImagePlane::getRowLength() const
{
    return m_rows * m_spacing[0];
}

double ImagePlane::getColumnLength() const
{
    return m_columns * m_spacing[1];
}

bool ImagePlane::operator ==(const ImagePlane &imagePlane)
{
    if( m_rowDirectionVector[0] == imagePlane.m_rowDirectionVector[0] &&
        m_rowDirectionVector[1] == imagePlane.m_rowDirectionVector[1] &&
        m_rowDirectionVector[2] == imagePlane.m_rowDirectionVector[2] &&
        m_columnDirectionVector[0] == imagePlane.m_columnDirectionVector[0] &&
        m_columnDirectionVector[1] == imagePlane.m_columnDirectionVector[1] &&
        m_columnDirectionVector[2] == imagePlane.m_columnDirectionVector[2] &&
        m_normal[0] == imagePlane.m_normal[0] &&
        m_normal[1] == imagePlane.m_normal[1] &&
        m_normal[2] == imagePlane.m_normal[2] &&
        m_origin[0] == imagePlane.m_origin[0] &&
        m_origin[1] == imagePlane.m_origin[1]  &&
        m_origin[2] == imagePlane.m_origin[2] &&
        m_spacing[0] == imagePlane.m_spacing[0] &&
        m_spacing[1] == imagePlane.m_spacing[1] &&
        m_rows == imagePlane.m_rows &&
        m_columns == imagePlane.m_columns
    )
        return true;
    else
        return false;
}

bool ImagePlane::operator !=(const ImagePlane &imagePlane)
{
    return !(*this == imagePlane);
}

}
