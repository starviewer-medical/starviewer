/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "imageplane.h"
#include "image.h"
#include <QString>
#include <vtkMath.h>

namespace udg {

ImagePlane::ImagePlane()
 : m_rows(1), m_columns(1), m_thickness(1.0), m_sliceLocation(0.0)
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

void ImagePlane::setThickness( double thickness )
{
    m_thickness = thickness;
}

double ImagePlane::getThickness() const
{
    return m_thickness;
}

void ImagePlane::setRows( int rows )
{
    m_rows = rows;
}

void ImagePlane::setColumns( int columns )
{
    m_columns = columns;
}

int ImagePlane::getRows() const
{
    return m_rows;
}

int ImagePlane::getColumns() const
{
    return m_columns;
}

double ImagePlane::getRowLength() const
{
    return m_rows * m_spacing[0];
}

double ImagePlane::getColumnLength() const
{
    return m_columns * m_spacing[1];
}

void ImagePlane::setSliceLocation( double location )
{
    m_sliceLocation = location;
}

double ImagePlane::getSliceLocation() const
{
    return m_sliceLocation;
}

bool ImagePlane::fillFromImage( const Image *image )
{
    if( image )
    {
        const double *dirCosines = image->getImageOrientationPatient();

        this->setRowDirectionVector( dirCosines[0], dirCosines[1], dirCosines[2] );
        this->setColumnDirectionVector( dirCosines[3], dirCosines[4], dirCosines[5] );
        this->setSpacing( image->getPixelSpacing()[0], image->getPixelSpacing()[1] );
        this->setThickness( this->getThickness() );
        this->setRows( image->getRows() );
        this->setColumns( image->getColumns() );
        this->setOrigin( image->getImagePositionPatient()[0], image->getImagePositionPatient()[1], image->getImagePositionPatient()[2] );
        QString location = image->getSliceLocation();
        this->setSliceLocation( location.toDouble() );

        return true;
    }
    else
        return false;
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
        m_columns == imagePlane.m_columns &&
        m_thickness == imagePlane.m_thickness &&
        m_sliceLocation == imagePlane.m_sliceLocation
    )
        return true;
    else
        return false;
}

bool ImagePlane::operator !=(const ImagePlane &imagePlane)
{
    return !(*this == imagePlane);
}

QList< QVector<double> > ImagePlane::getBounds( int location )
{
    double factor = 0.0;
    switch( location )
    {
    case 0: // central
        factor = 0.0;
    break;

    case 1: // upper
        factor = m_thickness*0.5;
    break;

    case 2: // lower
        factor = -m_thickness*0.5;
    break;
    }

    QList< QVector<double> > boundsList;
    QVector<double> tlhc, trhc, brhc, blhc;
    for( int i = 0; i<3; i++ )
    {
        tlhc << m_origin[i] + m_normal[i] * factor;
        trhc << m_origin[i] + m_rowDirectionVector[i]*this->getRowLength() + m_normal[i] * factor;
        brhc << m_origin[i] + m_rowDirectionVector[i]*this->getRowLength() + m_columnDirectionVector[i]*this->getColumnLength() + m_normal[i] * factor;
        blhc << m_origin[i] + m_columnDirectionVector[i]*this->getColumnLength() + m_normal[i] * factor;
    }
    boundsList << tlhc << trhc << brhc << blhc;
    return boundsList;
}

QList< QVector<double> > ImagePlane::getCentralBounds()
{
    return getBounds( 0 );
}

QList< QVector<double> > ImagePlane::getUpperBounds()
{
    return getBounds( 1 );
}

QList< QVector<double> > ImagePlane::getLowerBounds()
{
    return getBounds( 2 );
}

QString ImagePlane::toString( bool verbose )
{
    QString result;

    result = QString("Origin: %1, %2, %3").arg(m_origin[0]).arg(m_origin[1]).arg(m_origin[2]);
    result += QString("\nRows: %1  Columns: %2").arg( m_rows ).arg( m_columns );
    result += QString("\nRow Vector: %1, %2, %3").arg(m_rowDirectionVector[0]).arg(m_rowDirectionVector[1]).arg(m_rowDirectionVector[2]);
    result += QString("\nColumn Vector: %1, %2, %3").arg(m_columnDirectionVector[0]).arg(m_columnDirectionVector[1]).arg(m_columnDirectionVector[2]);
    result += QString("\nNormal Vector: %1, %2, %3").arg(m_normal[0]).arg(m_normal[1]).arg(m_normal[2]);
    result += QString("\nSpacing: %1, %2").arg( m_spacing[0] ).arg( m_spacing[1] );
    result += QString("\nThickness: %1").arg(m_thickness);
    result += QString("\nSlice Location: %1").arg(m_sliceLocation);

    if( verbose )
    {
        QList< QVector<double> > bounds = this->getCentralBounds();
        result += QString( "\nTLHC: %1, %2, %3" ).arg( bounds.at(0)[0] ).arg( bounds.at(0)[1] ).arg( bounds.at(0)[2] );
        result += QString( "\nTRHC: %1, %2, %3" ).arg( bounds.at(1)[0] ).arg( bounds.at(1)[1] ).arg( bounds.at(1)[2] );
        result += QString( "\nBRHC: %1, %2, %3" ).arg( bounds.at(2)[0] ).arg( bounds.at(2)[1] ).arg( bounds.at(2)[2] );
        result += QString( "\nBLHC: %1, %2, %3" ).arg( bounds.at(3)[0] ).arg( bounds.at(3)[1] ).arg( bounds.at(3)[2] );
    }

    return result;
}

}
