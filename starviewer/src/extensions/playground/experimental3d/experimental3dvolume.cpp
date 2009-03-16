#include "experimental3dvolume.h"

#include <vtkEncodedGradientShader.h>
#include <vtkFiniteDifferenceGradientEstimator.h>
#include <vtkFloatArray.h>
#include <vtkImageData.h>
#include <vtkImageShiftScale.h>
#include <vtkPointData.h>
#include <vtkVolume.h>
#include <vtkVolumeProperty.h>
#include <vtkVolumeRayCastCompositeFunction.h>
#include <vtkVolumeRayCastMapper.h>

#include "ambientvoxelshader.h"
#include "colorbleedingvoxelshader.h"
#include "contourvoxelshader.h"
#include "directilluminationvoxelshader.h"
#include "obscurance.h"
#include "obscurancevoxelshader.h"
#include "transferfunction.h"
#include "volume.h"
#include "vtk4DLinearRegressionGradientEstimator.h"
#include "vtkVolumeRayCastVoxelShaderCompositeFunction.h"

// VMI
#include "vmivoxelshader1.h"
#include "vmivoxelshader2.h"
#include "vomivoxelshader.h"
#include "voxelsaliencyvoxelshader.h"
#include "colorvomivoxelshader.h"


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
    delete m_obscuranceVoxelShader;
    delete m_colorBleedingVoxelShader;
    delete m_vmiVoxelShader1;
    delete m_vmiVoxelShader2;
    delete m_vomiVoxelShader;
    delete m_voxelSaliencyVoxelShader;
    delete m_colorVomiVoxelShader;
    m_mapper->Delete();
    m_property->Delete();
    m_volume->Delete();

    if ( m_finiteDifferenceGradientEstimator ) m_finiteDifferenceGradientEstimator->Delete();
    if ( m_4DLinearRegressionGradientEstimator ) m_4DLinearRegressionGradientEstimator->Delete();
}


vtkImageData* Experimental3DVolume::getImage() const
{
    return m_image;
}


vtkVolume* Experimental3DVolume::getVolume() const
{
    return m_volume;
}


unsigned short Experimental3DVolume::getRangeMin() const
{
    return m_rangeMin;
}


unsigned short Experimental3DVolume::getRangeMax() const
{
    return m_rangeMax;
}


unsigned int Experimental3DVolume::getSize() const
{
    return m_dataSize;
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
            m_shaderVolumeRayCastFunction->SetCompositeMethodToInterpolateFirst();
            break;
        case LinearClassifyInterpolate:
            m_property->SetInterpolationTypeToLinear();
            m_normalVolumeRayCastFunction->SetCompositeMethodToClassifyFirst();
            m_shaderVolumeRayCastFunction->SetCompositeMethodToClassifyFirst();
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
            m_finiteDifferenceGradientEstimator->SetInput( m_image );
            break;
        case FourDLInearRegression1:
            if ( !m_4DLinearRegressionGradientEstimator )
                m_4DLinearRegressionGradientEstimator = vtk4DLinearRegressionGradientEstimator::New();
            m_4DLinearRegressionGradientEstimator->SetRadius( 1 );
            m_mapper->SetGradientEstimator( m_4DLinearRegressionGradientEstimator );
            m_4DLinearRegressionGradientEstimator->SetInput( m_image );
            break;
        case FourDLInearRegression2:
            if ( !m_4DLinearRegressionGradientEstimator )
                m_4DLinearRegressionGradientEstimator = vtk4DLinearRegressionGradientEstimator::New();
            m_4DLinearRegressionGradientEstimator->SetRadius( 2 );
            m_mapper->SetGradientEstimator( m_4DLinearRegressionGradientEstimator );
            m_4DLinearRegressionGradientEstimator->SetInput( m_image );
            break;
    }
}


void Experimental3DVolume::resetShadingOptions()
{
    m_shaderVolumeRayCastFunction->RemoveAllVoxelShaders();
    m_mapper->SetVolumeRayCastFunction( m_shaderVolumeRayCastFunction );
}


void Experimental3DVolume::addLighting( bool diffuse, bool specular, double specularPower )
{
    if ( !diffuse )
    {
        m_shaderVolumeRayCastFunction->AddVoxelShader( m_ambientVoxelShader );
        if ( m_shaderVolumeRayCastFunction->IndexOfVoxelShader( m_ambientVoxelShader ) == 0 ) m_mapper->SetVolumeRayCastFunction( m_normalVolumeRayCastFunction );
        m_property->ShadeOff();
    }
    else
    {
        m_shaderVolumeRayCastFunction->AddVoxelShader( m_directIlluminationVoxelShader );
        if ( m_shaderVolumeRayCastFunction->IndexOfVoxelShader( m_directIlluminationVoxelShader ) == 0 ) m_mapper->SetVolumeRayCastFunction( m_normalVolumeRayCastFunction );
        m_property->ShadeOn();

        m_directIlluminationVoxelShader->setEncodedNormals( m_mapper->GetGradientEstimator()->GetEncodedNormals() );
        vtkEncodedGradientShader *gradientShader = m_mapper->GetGradientShader();
        m_directIlluminationVoxelShader->setDiffuseShadingTables( gradientShader->GetRedDiffuseShadingTable( m_volume ),
                                                                  gradientShader->GetGreenDiffuseShadingTable( m_volume ),
                                                                  gradientShader->GetBlueDiffuseShadingTable( m_volume ) );
        m_directIlluminationVoxelShader->setSpecularShadingTables( gradientShader->GetRedSpecularShadingTable( m_volume ),
                                                                   gradientShader->GetGreenSpecularShadingTable( m_volume ),
                                                                   gradientShader->GetBlueSpecularShadingTable( m_volume ) );

        if ( specular )
        {
            m_property->SetSpecular( 1.0 );
            m_property->SetSpecularPower( specularPower );
        }
        else m_property->SetSpecular( 0.0 );
    }
}


void Experimental3DVolume::addContour( double threshold )
{
    m_shaderVolumeRayCastFunction->AddVoxelShader( m_contourVoxelShader );
    m_contourVoxelShader->setGradientEstimator( gradientEstimator() );
    m_contourVoxelShader->setThreshold( threshold );
    m_mapper->SetVolumeRayCastFunction( m_shaderVolumeRayCastFunction );
}


void Experimental3DVolume::addObscurance( Obscurance *obscurance, double factor, double filterLow, double filterHigh )
{
    if ( !obscurance->hasColor() )
    {
        m_shaderVolumeRayCastFunction->AddVoxelShader( m_obscuranceVoxelShader );
        m_obscuranceVoxelShader->setObscurance( obscurance );
        m_obscuranceVoxelShader->setFactor( factor );
        m_obscuranceVoxelShader->setFilters( filterLow, filterHigh );
    }
    else
    {
        m_shaderVolumeRayCastFunction->AddVoxelShader( m_colorBleedingVoxelShader );
        m_colorBleedingVoxelShader->setColorBleeding( obscurance );
        m_colorBleedingVoxelShader->setFactor( factor );
    }

    m_mapper->SetVolumeRayCastFunction( m_shaderVolumeRayCastFunction );
}


void Experimental3DVolume::setTransferFunction( const TransferFunction &transferFunction )
{
    m_property->SetColor( transferFunction.getColorTransferFunction() );
    m_property->SetScalarOpacity( transferFunction.getOpacityTransferFunction() );
    m_ambientVoxelShader->setTransferFunction( transferFunction );
    m_directIlluminationVoxelShader->setTransferFunction( transferFunction );
    m_vmiVoxelShader1->setTransferFunction( transferFunction );
    m_vmiVoxelShader2->setTransferFunction( transferFunction );
    m_vomiVoxelShader->setTransferFunction( transferFunction );
    m_voxelSaliencyVoxelShader->setTransferFunction( transferFunction );
    m_colorVomiVoxelShader->setTransferFunction( transferFunction );
}


void Experimental3DVolume::startVmiMode()
{
    m_mapper->SetVolumeRayCastFunction( m_shaderVolumeRayCastFunction );
}


void Experimental3DVolume::startVmiFirstPass()
{
    m_shaderVolumeRayCastFunction->RemoveAllVoxelShaders();
    m_shaderVolumeRayCastFunction->AddVoxelShader( m_vmiVoxelShader1 );
    m_vmiVoxelShader1->initAccumulator();
}


float Experimental3DVolume::finishVmiFirstPass()
{
    const QHash<QThread*, float> &accumulator = m_vmiVoxelShader1->accumulator();
    QHashIterator<QThread*, float> it( accumulator );
    float volume = 0.0f;

    while ( it.hasNext() )
    {
        it.next();
        volume += it.value();
    }

    return volume;
}


void Experimental3DVolume::startVmiSecondPass()
{
    m_shaderVolumeRayCastFunction->RemoveAllVoxelShaders();
    m_shaderVolumeRayCastFunction->AddVoxelShader( m_vmiVoxelShader2 );
}


QVector<float> Experimental3DVolume::finishVmiSecondPass()
{
    return m_vmiVoxelShader2->objectProbabilities();
}


float Experimental3DVolume::viewedVolumeInVmiSecondPass() const
{
    return m_vmiVoxelShader2->viewedVolume();
}


void Experimental3DVolume::addVomi( const QVector<float> &vomi, float maximumVomi, float factor )
{
    if ( m_shaderVolumeRayCastFunction->IndexOfVoxelShader( m_vomiVoxelShader ) < 0 ) m_shaderVolumeRayCastFunction->AddVoxelShader( m_vomiVoxelShader );
    m_vomiVoxelShader->setVomi( vomi, maximumVomi, factor );
    m_vomiVoxelShader->setCombine( m_shaderVolumeRayCastFunction->IndexOfVoxelShader( m_vomiVoxelShader ) != 0 );
    m_mapper->SetVolumeRayCastFunction( m_shaderVolumeRayCastFunction );
}


void Experimental3DVolume::addColorVomi( const QVector<Vector3Float> &colorVomi, float maximumColorVomi, float factor )
{
    if ( m_shaderVolumeRayCastFunction->IndexOfVoxelShader( m_colorVomiVoxelShader ) < 0 ) m_shaderVolumeRayCastFunction->AddVoxelShader( m_colorVomiVoxelShader );
    m_colorVomiVoxelShader->setColorVomi( colorVomi, maximumColorVomi, factor );
    m_colorVomiVoxelShader->setCombine( m_shaderVolumeRayCastFunction->IndexOfVoxelShader( m_colorVomiVoxelShader ) != 0 );
    m_mapper->SetVolumeRayCastFunction( m_shaderVolumeRayCastFunction );
}


void Experimental3DVolume::addVoxelSaliencies( const QVector<float> &voxelSaliencies, float maximumSaliency, float factor )
{
    m_shaderVolumeRayCastFunction->AddVoxelShader( m_voxelSaliencyVoxelShader );
    m_voxelSaliencyVoxelShader->setVoxelSaliencies( voxelSaliencies, maximumSaliency, factor );
    m_mapper->SetVolumeRayCastFunction( m_shaderVolumeRayCastFunction );
}


QVector<float> Experimental3DVolume::computeVomiGradient( const QVector<float> &vomi )
{
    vtkFloatArray *vomiArray = vtkFloatArray::New();
    vomiArray->SetArray( const_cast<float*>( vomi.data() ), m_dataSize, 1 );

    vtkImageData *vomiData = vtkImageData::New();
    vomiData->CopyStructure( m_image );
    vomiData->SetScalarTypeToFloat();

    vtkPointData *vomiPointData = vomiData->GetPointData();
    vomiPointData->SetScalars( vomiArray );

    vtkEncodedGradientEstimator *gradientEstimator = this->gradientEstimator();
    gradientEstimator->SetInput( vomiData );

    unsigned char *gradientMagnitudes = gradientEstimator->GetGradientMagnitudes();

    QVector<float> vomiGradient( m_dataSize );
    float maxVomiGradient = 0.0f;
    for ( unsigned int i = 0; i < m_dataSize; i++ ) if ( gradientMagnitudes[i] > maxVomiGradient ) maxVomiGradient = gradientMagnitudes[i];
    for ( unsigned int i = 0; i < m_dataSize; i++ ) vomiGradient[i] = gradientMagnitudes[i] / maxVomiGradient;

    gradientEstimator->SetInput( m_image );

    vomiArray->Delete();
    vomiData->Delete();

    return vomiGradient;
}


void Experimental3DVolume::createImage( Volume *volume )
{
    // sembla que el volum arriba sempre com a short
    // normalment els volums aprofiten només 12 bits com a màxim, per tant no hi hauria d'haver problema
    vtkImageData *inputImage = volume->getVtkData();

    double *range = inputImage->GetScalarRange();
    double min = range[0], max = range[1];
    DEBUG_LOG( QString( "original range: min = %1, max = %2" ).arg( min ).arg( max ) );

    double shift = -min;

    // fem servir directament un vtkImageShiftScale, que permet fer castings també
    vtkImageShiftScale *imageShiftScale = vtkImageShiftScale::New();
    imageShiftScale->SetInput( volume->getVtkData() );
    imageShiftScale->SetOutputScalarTypeToUnsignedShort();
    imageShiftScale->SetShift( shift );
    imageShiftScale->Update();

    m_rangeMin = 0; m_rangeMax = static_cast<unsigned short>( max + shift );

    m_image = imageShiftScale->GetOutput(); m_image->Register( 0 ); // el register és necessari (comprovat)
    m_data = reinterpret_cast<unsigned short*>( m_image->GetPointData()->GetScalars()->GetVoidPointer( 0 ) );
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
    m_ambientVoxelShader->setData( m_data, m_rangeMax );
    m_directIlluminationVoxelShader = new DirectIlluminationVoxelShader();
    m_directIlluminationVoxelShader->setData( m_data, m_rangeMax );
    m_contourVoxelShader = new ContourVoxelShader();
    m_obscuranceVoxelShader = new ObscuranceVoxelShader();
    m_colorBleedingVoxelShader = new ColorBleedingVoxelShader();
    m_vmiVoxelShader1 = new VmiVoxelShader1();
    m_vmiVoxelShader1->setData( m_data, m_rangeMax );
    m_vmiVoxelShader2 = new VmiVoxelShader2();
    m_vmiVoxelShader2->setData( m_data, m_rangeMax, m_dataSize );
    m_vomiVoxelShader = new VomiVoxelShader();
    m_vomiVoxelShader->setData( m_data, m_rangeMax );
    m_voxelSaliencyVoxelShader = new VoxelSaliencyVoxelShader();
    m_voxelSaliencyVoxelShader->setData( m_data, m_rangeMax );
    m_colorVomiVoxelShader = new ColorVomiVoxelShader();
    m_colorVomiVoxelShader->setData( m_data, m_rangeMax );
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
