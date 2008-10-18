#include "experimental3dvolume.h"

#include <vtkFiniteDifferenceGradientEstimator.h>
#include <vtkImageCast.h>
#include <vtkImageData.h>
#include <vtkImageShiftScale.h>
#include <vtkPointData.h>
#include <vtkVolume.h>
#include <vtkVolumeProperty.h>
#include <vtkVolumeRayCastCompositeFunction.h>
#include <vtkVolumeRayCastMapper.h>

#include "transferfunction.h"
#include "volume.h"
#include "vtk4DLinearRegressionGradientEstimator.h"


namespace udg {


Experimental3DVolume::Experimental3DVolume( Volume *volume )
 : m_finiteDifferenceGradientEstimator( 0 ), m_4DLinearRegressionGradientEstimator( 0 )
{
    createImage( volume );
    createVolumeRayCastFunction();
    createMapper();
    createProperty();
    createVolume();
}


Experimental3DVolume::~Experimental3DVolume()
{
    m_image->Delete();
    m_normalVolumeRayCastFunction->Delete();
    m_mapper->Delete();
    m_property->Delete();
    m_volume->Delete();

    if ( m_finiteDifferenceGradientEstimator ) m_finiteDifferenceGradientEstimator->Delete();
    if ( m_4DLinearRegressionGradientEstimator ) m_4DLinearRegressionGradientEstimator->Delete();
}


vtkVolume* Experimental3DVolume::getVolume() const
{
    return m_volume;
}


unsigned char Experimental3DVolume::getRangeMin() const
{
    return m_rangeMin;
}


unsigned char Experimental3DVolume::getRangeMax() const
{
    return m_rangeMax;
}


void Experimental3DVolume::setInterpolation( Interpolation interpolation )
{
    switch ( interpolation )
    {
        case NearestNeighbour:
            m_property->SetInterpolationTypeToNearest();
            break;
        case LinearInterpolateClassify:
            m_property->SetInterpolationTypeToLinear();
            m_normalVolumeRayCastFunction->SetCompositeMethodToInterpolateFirst();
            break;
        case LinearClassifyInterpolate:
            m_property->SetInterpolationTypeToLinear();
            m_normalVolumeRayCastFunction->SetCompositeMethodToClassifyFirst();
            break;
    }
}


void Experimental3DVolume::setGradientEstimator( GradientEstimator gradientEstimator )
{
    switch ( gradientEstimator )
    {
        case FiniteDifference:
            if ( !m_finiteDifferenceGradientEstimator )
                m_finiteDifferenceGradientEstimator = vtkFiniteDifferenceGradientEstimator::New();
            m_mapper->SetGradientEstimator( m_finiteDifferenceGradientEstimator );
            break;
        case FourDLInearRegression1:
            if ( !m_4DLinearRegressionGradientEstimator )
                m_4DLinearRegressionGradientEstimator = vtk4DLinearRegressionGradientEstimator::New();
            m_4DLinearRegressionGradientEstimator->SetRadius( 1 );
            m_mapper->SetGradientEstimator( m_4DLinearRegressionGradientEstimator );
            break;
        case FourDLInearRegression2:
            if ( !m_4DLinearRegressionGradientEstimator )
                m_4DLinearRegressionGradientEstimator = vtk4DLinearRegressionGradientEstimator::New();
            m_4DLinearRegressionGradientEstimator->SetRadius( 2 );
            m_mapper->SetGradientEstimator( m_4DLinearRegressionGradientEstimator );
            break;
    }
}


void Experimental3DVolume::setLighting( bool diffuse, bool specular, double specularPower )
{
    if ( diffuse ) m_property->ShadeOn();
    else m_property->ShadeOff();

    m_property->SetSpecular( specular ? 1.0 : 0.0 );
    m_property->SetSpecularPower( specularPower );
}


void Experimental3DVolume::setTransferFunction( const TransferFunction &transferFunction )
{
    m_property->SetColor( transferFunction.getColorTransferFunction() );
    m_property->SetScalarOpacity( transferFunction.getOpacityTransferFunction() );
}


void Experimental3DVolume::createImage( Volume *volume )
{
    // fem un casting a int perquè a vegades hi ha problemes amb l'scalar range
    vtkImageCast *imageCast = vtkImageCast::New();
    imageCast->SetInput( volume->getVtkData() );
    imageCast->SetOutputScalarTypeToInt();
    imageCast->Update();

    vtkImageData *inputImage = imageCast->GetOutput();

    double *range = inputImage->GetScalarRange();
    double min = range[0], max = range[1];
    DEBUG_LOG( QString( "original range: min = %1, max = %2" ).arg( min ).arg( max ) );

    if ( min >= 0.0 && max <= 255.0 )   // si ja està dins del rang que volem només cal fer un cast
    {
        // cal fer el casting perquè ens arriba com a int
        imageCast->SetOutputScalarTypeToUnsignedChar();
        imageCast->Update();

        m_image = imageCast->GetOutput(); m_image->Register( 0 );   // el register és necessari (comprovat)

        m_rangeMin = static_cast<unsigned char>( qRound( min ) );
        m_rangeMax = static_cast<unsigned char>( qRound( max ) );
    }
    else
    {
        double shift = -min;
        double slope = 255.0 / ( max - min );

        vtkImageShiftScale *imageShiftScale = vtkImageShiftScale::New();
        imageShiftScale->SetInput( inputImage );
        imageShiftScale->SetShift( shift );
        imageShiftScale->SetScale( slope );
        imageShiftScale->SetOutputScalarTypeToUnsignedChar();
        imageShiftScale->ClampOverflowOn();
        imageShiftScale->Update();

        m_image = imageShiftScale->GetOutput(); m_image->Register( 0 ); // el register és necessari (comprovat)
        imageShiftScale->Delete();

        m_rangeMin = 0; m_rangeMax = 255;

        double *newRange = m_image->GetScalarRange();
        DEBUG_LOG( QString( "new range: min = %1, max = %2" ).arg( newRange[0] ).arg( newRange[1] ) );
    }

    imageCast->Delete();

    m_data = reinterpret_cast<unsigned char*>( m_image->GetPointData()->GetScalars()->GetVoidPointer( 0 ) );
    m_dataSize = m_image->GetPointData()->GetScalars()->GetSize();
}


void Experimental3DVolume::createVolumeRayCastFunction()
{
    m_normalVolumeRayCastFunction = vtkVolumeRayCastCompositeFunction::New();
}


void Experimental3DVolume::createMapper()
{
    m_mapper = vtkVolumeRayCastMapper::New();
    m_mapper->SetInput( m_image );
    m_mapper->SetVolumeRayCastFunction( m_normalVolumeRayCastFunction );
}


void Experimental3DVolume::createProperty()
{
    m_property = vtkVolumeProperty::New();
}


void Experimental3DVolume::createVolume()
{
    m_volume = vtkVolume::New();
    m_volume->SetMapper( m_mapper );
    m_volume->SetProperty( m_property );

    // Centrem el volum a (0,0,0)
    double *center = m_volume->GetCenter();
    m_volume->AddPosition( -center[0], -center[1], -center[2] );
}


}
