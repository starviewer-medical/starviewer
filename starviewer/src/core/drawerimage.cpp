/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "drawerimage.h"
#include "logging.h"
#include "q2dviewer.h"
#include "mathtools.h"
// vtk
#include <vtkImageActor.h>
#include <vtkImageData.h>
#include <vtkLine.h>
#include <vtkTransform.h>

#include <vtkImageMapToWindowLevelColors.h>
#include <vtkScalarsToColors.h>
#include <vtkWindowLevelLookupTable.h>
#include <vtkImageMask.h>
#include <vtkImageThreshold.h>
// qt
#include <QVector>

namespace udg {

DrawerImage::DrawerImage(QObject *parent)
: DrawerPrimitive(parent), m_vtkImageActor(0), m_vtkImageData(0)
{
    m_vtkImageActor = vtkImageActor::New();
    m_useInsideBounds = false;
}

DrawerImage::~DrawerImage()
{
    emit dying(this);

    if ( m_vtkImageActor )
        m_vtkImageActor->Delete();

    if ( m_vtkImageData )
        m_vtkImageData->Delete();

}

void DrawerImage::addImage( vtkImageData *data )
{
    m_vtkImageData = data;

    vtkImageMapToWindowLevelColors *windowLevelLUTMapper = vtkImageMapToWindowLevelColors::New();
    windowLevelLUTMapper->SetInput( m_vtkImageData );
    windowLevelLUTMapper->SetWindow( m_window );
    windowLevelLUTMapper->SetLevel( m_level );

    //vtkScalarsToColors *scalarsToColors = vtkScalarsToColors::New();
    double *rgba;
    vtkWindowLevelLookupTable * lookupTable = vtkWindowLevelLookupTable::New();
    lookupTable->SetTableRange( m_vtkImageData->GetScalarRange()[0]-1, m_vtkImageData->GetScalarRange()[1] );
    lookupTable->SetWindow( m_window );
    lookupTable->SetLevel( m_level );
    lookupTable->Build();

    int index = lookupTable->GetIndex( m_vtkImageData->GetScalarRange()[0]-1 );
    DEBUG_LOG( QString( "Index: %1" ).arg(index) );
    rgba = lookupTable->GetTableValue( index );
    DEBUG_LOG( QString( "RGBA before: %1, %2, %3, %4" ).arg(rgba[0]).arg(rgba[1]).arg(rgba[2]).arg(rgba[3]) );
    rgba[3] = 0; //alpha value
    //for( int i = 0; i < 256; i++ )
        lookupTable->SetTableValue( index, rgba );
    DEBUG_LOG( QString( "RGBA after: %1, %2, %3, %4" ).arg(rgba[0]).arg(rgba[1]).arg(rgba[2]).arg(rgba[3]) );
    lookupTable->Build();

    //m_windowLevelLUTMapper->Update();
    //scalarsToColors = m_windowLevelLUTMapper->GetLookupTable();
    //if( scalarsToColors == 0 )
        //DEBUG_LOG( "Taula NULL" );


    //DEBUG_LOG( QString().arg().arg() );
    //DEBUG_LOG( QString("Range: %1").arg( scalarsToColors->GetOpacity( 0 ) ) );

    windowLevelLUTMapper->SetLookupTable( lookupTable );

    m_vtkImageActor->SetInput( windowLevelLUTMapper->GetOutput() );

    windowLevelLUTMapper->Delete();

    //m_vtkImageActor->SetInput( m_vtkImageData );
}

void DrawerImage::setBoundingPolyline( QList<double *> points )
{
    if( points.size() > 0 )
    {
        m_pointsList = points;
    }
    else
    {
        m_useInsideBounds = true;
    }
}

void DrawerImage::setView( int view )
{
    m_view = view;
}

void DrawerImage::setWindowLevel( double window, double level )
{
    m_window = window;
    m_level = level;
}

void DrawerImage::setWindowLevelLutMapper( vtkImageMapToWindowLevelColors *windowLevelLUTMapper )
{
    m_windowLevelLUTMapper = windowLevelLUTMapper;
}

void DrawerImage::setOpacity( double opacity )
{
    m_vtkImageActor->SetOpacity( opacity );
}

void DrawerImage::applyMask()
{
    vtkImageThreshold *imageThreshold = vtkImageThreshold::New();
    imageThreshold->SetInput( m_vtkImageData );
    imageThreshold->ThresholdByLower( 0.0 );
    imageThreshold->SetInValue( 1.0 );

    imageThreshold->Update();

    addImage( imageThreshold->GetOutput() );



    /*vtkImageMask *imageMask = vtkImageMask::New();
    imageMask->SetImageInput( m_vtkImageData );
    imageMask->SetMaskAlpha( 0.0 );


    addImage( imageMask->GetOutput() );*/


    /*vtkImageThreshold *imageThreshold = vtkImageThreshold::New();
    imageThreshold->SetInput( m_Volume->getVtkData() );
    imageThreshold->ThresholdBetween( m_lowerThreshold,  m_upperThreshold);
    imageThreshold->SetInValue( m_insideMaskValue-100 );
    imageThreshold->SetOutValue( m_outsideMaskValue );
    DEBUG_LOG( QString("min: %1, mout: %2").arg(m_insideMaskValue).arg(m_outsideMaskValue) );
    imageThreshold->Update();
    vtkImageData* imMask = imageThreshold->GetOutput();*/
    
}

void DrawerImage::move( double *movement )
{
    m_vtkImageActor->AddPosition( movement );
}

void DrawerImage::rotate( vtkTransform *transform )
{
    m_vtkImageActor->SetUserTransform( transform );
}

vtkProp *DrawerImage::getAsVtkProp()
{
    return m_vtkImageActor;
}

void DrawerImage::update()
{
	switch( m_internalRepresentation )
    {
    case VTKRepresentation:
    break;

    case OpenGLRepresentation:
    break;

    }
}

void DrawerImage::updateVtkProp()
{
}

vtkImageData *DrawerImage::getImageData()
{
    return m_vtkImageData;
}

double DrawerImage::getDistanceToPoint( double *point3D )
{
    if( m_useInsideBounds )
    {
        double p0[3], p1[3];
        double bounds[6];
        m_vtkImageActor->GetBounds(bounds);
        p0[0] = bounds[0];
        p0[1] = bounds[2];
        p0[2] = bounds[4];

        p1[0] = bounds[1];
        p1[1] = bounds[3];
        p1[2] = bounds[5];

        if( isPointInsideOfBounds( point3D, p0, p1, m_view ) )
        {
            return 0.0;
        }
    }
    else
    {
        if( isPointInPolygon( point3D ) )
        {
            return 0.0; //Si el punt és "sobre" la imatge considerem que la distància és 0
        }
        else
        {
            double minDistanceLine = VTK_DOUBLE_MAX;
            double distance;
            bool found = false;

            if ( !m_pointsList.isEmpty() )
            {
                //mirem si el polígon conté com a últim punt el primer punt, és a dir, si està tancat o no.
                //ens cal que sigui tancat per a dibuixar tots els segments reals que el formen.
                QList< double* > auxList;
                auxList += m_pointsList;

                if ( auxList.first()[0] != auxList.last()[0] || auxList.first()[1] != auxList.last()[1] || auxList.first()[2] != auxList.last()[2] )
                {
                    //si el primer i últim punt no són iguals, dupliquem el primer punt.
                    auxList << auxList.first();
                }

                for ( int i = 0; ( i < auxList.count() - 1 ) && !found ; i++ )
                {
                    if ( isPointIncludedInLineBounds( point3D, auxList[i], auxList[i+1] ) )
                    {
                        minDistanceLine = 0.0;
                        found = true;
                    }
                    else
                    {
                        distance = vtkLine::DistanceToLine( point3D , auxList[i] , auxList[i+1] );

                        if ( minDistanceLine == VTK_DOUBLE_MAX )
                            minDistanceLine = distance;
                        else if ( distance < minDistanceLine )
                                minDistanceLine = distance;
                    }
                }
            }
            return minDistanceLine;
        }
    }
}

bool DrawerImage::isPointIncludedInLineBounds( double point[3], double *lineP1, double *lineP2 )
{
    double range = 10.0;

    return ( MathTools::getDistance3D( point, lineP1 ) <= range || MathTools::getDistance3D( point, lineP2 ) <= range );
}

bool DrawerImage::isInsideOfBounds( double p1[3], double p2[3], int view )
{
    double minX, maxX, minY, maxY;

    int numberOfPoints = m_pointsList.count();
    bool allPointsAreInside = true;

    //determinem x i y màximes i mínimes segons la vista
    switch( view )
    {
        case Q2DViewer::AxialPlane:
            if ( p1[0] < p2[0] )
            {
                minX = p1[0];
                maxX = p2[0];
            }
            else
            {
                maxX = p1[0];
                minX = p2[0];
            }

            if ( p1[1] < p2[1] )
            {
                minY = p1[1];
                maxY = p2[1];
            }
            else
            {
                maxY = p1[1];
                minY = p2[1];
            }

            for (int i = 0; i < numberOfPoints && allPointsAreInside; i++ )
            {
                double *currentPoint = m_pointsList.at( i );
                if ( currentPoint[0] >= maxX || currentPoint[0] <= minX || currentPoint[1] >= maxY || currentPoint[1] <= minY )
                    allPointsAreInside = false;
            }
            break;
        case Q2DViewer::SagitalPlane:
            if ( p1[2] < p2[2] )
            {
                minX = p1[2];
                maxX = p2[2];
            }
            else
            {
                maxX = p1[2];
                minX = p2[2];
            }

            if ( p1[1] < p2[1] )
            {
                minY = p1[1];
                maxY = p2[1];
            }
            else
            {
                maxY = p1[1];
                minY = p2[1];
            }

            for (int i = 0; i < numberOfPoints && allPointsAreInside; i++ )
            {
                double *currentPoint = m_pointsList.at( i );
                if ( currentPoint[2] >= maxX || currentPoint[2] <= minX || currentPoint[1] >= maxY || currentPoint[1] <= minY )
                    allPointsAreInside = false;
            }
            break;
        case Q2DViewer::CoronalPlane:
            if ( p1[0] < p2[0] )
            {
                minX = p1[0];
                maxX = p2[0];
            }
            else
            {
                maxX = p1[0];
                minX = p2[0];
            }

            if ( p1[2] < p2[2] )
            {
                minY = p1[2];
                maxY = p2[2];
            }
            else
            {
                maxY = p1[2];
                minY = p2[2];
            }
            for (int i = 0; i < numberOfPoints && allPointsAreInside; i++ )
            {
                double *currentPoint = m_pointsList.at( i );
                if ( currentPoint[0] >= maxX || currentPoint[0] <= minX || currentPoint[2] >= maxY || currentPoint[2] <= minY )
                    allPointsAreInside = false;
            }
            break;
    }

    return ( allPointsAreInside );
}

bool DrawerImage::isPointInsideOfBounds( double *point, double p1[3], double p2[3], int view )
{
    double minX, maxX, minY, maxY;

    //determinem x i y màximes i mínimes segons la vista
    switch( view )
    {
        case Q2DViewer::AxialPlane:
            if ( p1[0] < p2[0] )
            {
                minX = p1[0];
                maxX = p2[0];
            }
            else
            {
                maxX = p1[0];
                minX = p2[0];
            }

            if ( p1[1] < p2[1] )
            {
                minY = p1[1];
                maxY = p2[1];
            }
            else
            {
                maxY = p1[1];
                minY = p2[1];
            }

            if( point[0] >= maxX || point[0] <= minX || point[1] >= maxY || point[1] <= minY )
                return false;

            break;
        case Q2DViewer::SagitalPlane:
            if ( p1[2] < p2[2] )
            {
                minX = p1[2];
                maxX = p2[2];
            }
            else
            {
                maxX = p1[2];
                minX = p2[2];
            }

            if ( p1[1] < p2[1] )
            {
                minY = p1[1];
                maxY = p2[1];
            }
            else
            {
                maxY = p1[1];
                minY = p2[1];
            }

            if ( point[2] >= maxX || point[2] <= minX || point[1] >= maxY || point[1] <= minY )
                return false;
            break;
        case Q2DViewer::CoronalPlane:
            if ( p1[0] < p2[0] )
            {
                minX = p1[0];
                maxX = p2[0];
            }
            else
            {
                maxX = p1[0];
                minX = p2[0];
            }

            if ( p1[2] < p2[2] )
            {
                minY = p1[2];
                maxY = p2[2];
            }
            else
            {
                maxY = p1[2];
                minY = p2[2];
            }

            if ( point[0] >= maxX || point[0] <= minX || point[2] >= maxY || point[2] <= minY )
                return false;
            break;
    }

    return true;
}

bool DrawerImage::isPointInPolygon( double *point3D )
{
    int xCoordinate, yCoordinate, zCoordinate;

    switch( m_view )
    {
    case Q2DViewer::Axial:
        xCoordinate = 0;
        yCoordinate = 1;
        zCoordinate = 2;
        break;
    case Q2DViewer::Sagital:
        xCoordinate = 1;
        yCoordinate = 2;
        zCoordinate = 0;
        break;
    case Q2DViewer::Coronal:
        xCoordinate = 0;
        yCoordinate = 2;
        zCoordinate = 1;
        break;
    }

    double newPoint[3], oldPoint[3], p1[3], p2[3];
    bool inside = false;
    int nPoints = m_pointsList.size();

    if( nPoints < 3 )
        return inside; //false

    oldPoint[xCoordinate] = m_pointsList.at( nPoints - 1 )[xCoordinate];
    oldPoint[yCoordinate] = m_pointsList.at( nPoints - 1 )[yCoordinate];

    for( int i = 0; i < nPoints; i++ )
    {
        newPoint[xCoordinate] = m_pointsList.at( i )[xCoordinate];
        newPoint[yCoordinate] = m_pointsList.at( i )[yCoordinate];

        if( newPoint[xCoordinate] > oldPoint[xCoordinate] )
        {
            p1[xCoordinate] = oldPoint[xCoordinate];
            p1[yCoordinate] = oldPoint[yCoordinate];
            p2[xCoordinate] = newPoint[xCoordinate];
            p2[yCoordinate] = newPoint[yCoordinate];
        }
        else
        {
            p1[xCoordinate] = newPoint[xCoordinate];
            p1[yCoordinate] = newPoint[yCoordinate];
            p2[xCoordinate] = oldPoint[xCoordinate];
            p2[yCoordinate] = oldPoint[yCoordinate];
        }

        if ( ( newPoint[xCoordinate] < point3D[xCoordinate] ) == ( point3D[xCoordinate] <= oldPoint[xCoordinate] ) /* edge "open" at one end */
            && ( (long)point3D[yCoordinate] - (long)p1[yCoordinate] ) * (long)( p2[xCoordinate] - p1[xCoordinate] )
            < ( (long)p2[yCoordinate] - (long)p1[yCoordinate] ) * (long)( point3D[xCoordinate] - p1[xCoordinate] ) )
        {
            inside = !inside;
        }

        oldPoint[xCoordinate] = newPoint[xCoordinate];
        oldPoint[yCoordinate] = newPoint[yCoordinate];
    }
    
    return inside;

}

void DrawerImage::getBounds(double bounds[6])
{
    if ( m_vtkImageActor )
        m_vtkImageActor->GetBounds(bounds);
    else
        memset(bounds, 0.0, sizeof(double)*6);
}
}
