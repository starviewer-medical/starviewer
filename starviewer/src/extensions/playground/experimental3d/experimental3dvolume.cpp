#include "experimental3dvolume.h"

#include <vtkEncodedGradientShader.h>
#include <vtkFiniteDifferenceGradientEstimator.h>
#include <vtkImageData.h>
#include <vtkImageShiftScale.h>
#include <vtkPointData.h>
#include <vtkVolume.h>
#include <vtkVolumeProperty.h>
#include <vtkVolumeRayCastCompositeFunction.h>
#include <vtkVolumeRayCastMapper.h>

#include "ambientvoxelshader.h"
#include "contourvoxelshader.h"
#include "directilluminationvoxelshader.h"
#include "transferfunction.h"
#include "volume.h"
#include "vtk4DLinearRegressionGradientEstimator.h"
#include "vtkVolumeRayCastVoxelShaderCompositeFunction.h"


namespace udg {


Experimental3DVolume::Experimental3DVolume( Volume *volume )
 : m_finiteDifferenceGradientEstimator( 0 ), m_4DLinearRegressionGradientEstimator( 0 )
{
    createImage( volume );
    createVolumeRayCastFunctions();
    createVoxelShaders();
    createMapper();
    createProperty();
    createVolume();
}


Experimental3DVolume::~Experimental3DVolume()
{
    m_image->Delete();
    m_normalVolumeRayCastFunction->Delete();
    m_shaderVolumeRayCastFunction->Delete();
    delete m_ambientVoxelShader;
    delete m_directIlluminationVoxelShader;
    delete m_contourVoxelShader;
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
    m_gradientEstimator = gradientEstimator;

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
    m_shaderVolumeRayCastFunction->RemoveVoxelShader( 0 );  // el primer sempre és ambient o il·luminació directa

    if ( diffuse )
    {
        m_property->ShadeOn();
        m_shaderVolumeRayCastFunction->InsertVoxelShader( 0, m_directIlluminationVoxelShader );

        m_directIlluminationVoxelShader->setEncodedNormals( m_mapper->GetGradientEstimator()->GetEncodedNormals() );
        vtkEncodedGradientShader *gradientShader = m_mapper->GetGradientShader();
        m_directIlluminationVoxelShader->setDiffuseShadingTables( gradientShader->GetRedDiffuseShadingTable( m_volume ),
                                                                  gradientShader->GetGreenDiffuseShadingTable( m_volume ),
                                                                  gradientShader->GetBlueDiffuseShadingTable( m_volume ) );
        m_directIlluminationVoxelShader->setSpecularShadingTables( gradientShader->GetRedSpecularShadingTable( m_volume ),
                                                                   gradientShader->GetGreenSpecularShadingTable( m_volume ),
                                                                   gradientShader->GetBlueSpecularShadingTable( m_volume ) );
    }
    else
    {
        m_property->ShadeOff();
        m_shaderVolumeRayCastFunction->InsertVoxelShader( 0, m_ambientVoxelShader );
    }

    m_property->SetSpecular( specular ? 1.0 : 0.0 );
    m_property->SetSpecularPower( specularPower );

    m_mapper->SetVolumeRayCastFunction( m_normalVolumeRayCastFunction );
}


void Experimental3DVolume::setContour( bool on, double threshold )
{
    if ( on )
    {
        m_mapper->SetVolumeRayCastFunction( m_shaderVolumeRayCastFunction );
        m_shaderVolumeRayCastFunction->AddVoxelShader( m_contourVoxelShader );
        m_contourVoxelShader->setGradientEstimator( gradientEstimator() );
        m_contourVoxelShader->setThreshold( threshold );
    }
    else
    {
        m_shaderVolumeRayCastFunction->RemoveVoxelShader( m_contourVoxelShader );
    }
}


void Experimental3DVolume::setTransferFunction( const TransferFunction &transferFunction )
{
    m_property->SetColor( transferFunction.getColorTransferFunction() );
    m_property->SetScalarOpacity( transferFunction.getOpacityTransferFunction() );
    m_ambientVoxelShader->setTransferFunction( transferFunction );
    m_directIlluminationVoxelShader->setTransferFunction( transferFunction );
}


void Experimental3DVolume::createImage( Volume *volume )
{
    // sembla que el volum arriba sempre com a short
    // normalment els volums aprofiten només 12 bits com a màxim, per tant no hi hauria d'haver problema
    vtkImageData *inputImage = volume->getVtkData();

    double *range = inputImage->GetScalarRange();
    double min = range[0], max = range[1];
    DEBUG_LOG( QString( "original range: min = %1, max = %2" ).arg( min ).arg( max ) );

    // fem servir directament un vtkImageShiftScale, que permet fer castings també
    vtkImageShiftScale *imageShiftScale = vtkImageShiftScale::New();
    imageShiftScale->SetInput( volume->getVtkData() );
    imageShiftScale->SetOutputScalarTypeToUnsignedChar();

    if ( min >= 0.0 && max <= 255.0 )   // si ja està dins del rang que volem només cal fer un cast
    {
        m_rangeMin = static_cast<unsigned char>( qRound( min ) );
        m_rangeMax = static_cast<unsigned char>( qRound( max ) );
    }
    else    // si està fora del rang cal scalar i desplaçar
    {
        double shift = -min;
        double scale = 255.0 / ( max - min );

        imageShiftScale->SetShift( shift );
        imageShiftScale->SetScale( scale );

        m_rangeMin = 0; m_rangeMax = 255;
    }

    imageShiftScale->Update();

    m_image = imageShiftScale->GetOutput(); m_image->Register( 0 ); // el register és necessari (comprovat)
    m_data = reinterpret_cast<unsigned char*>( m_image->GetPointData()->GetScalars()->GetVoidPointer( 0 ) );
    m_dataSize = m_image->GetPointData()->GetScalars()->GetSize();

    imageShiftScale->Delete();
}


void Experimental3DVolume::createVolumeRayCastFunctions()
{
    m_normalVolumeRayCastFunction = vtkVolumeRayCastCompositeFunction::New();
    m_shaderVolumeRayCastFunction = vtkVolumeRayCastVoxelShaderCompositeFunction::New();
}


void Experimental3DVolume::createVoxelShaders()
{
    m_ambientVoxelShader = new AmbientVoxelShader();
    m_ambientVoxelShader->setData( m_data );
    m_directIlluminationVoxelShader = new DirectIlluminationVoxelShader();
    m_directIlluminationVoxelShader->setData( m_data );
    m_contourVoxelShader = new ContourVoxelShader();
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


vtkEncodedGradientEstimator* Experimental3DVolume::gradientEstimator() const
{
    switch ( m_gradientEstimator )
    {
        case FiniteDifference:
        default:
            return m_finiteDifferenceGradientEstimator;
        case FourDLInearRegression1:
        case FourDLInearRegression2:
            return m_4DLinearRegressionGradientEstimator;
    }
}


}
