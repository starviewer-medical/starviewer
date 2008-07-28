#include "vtkVolumeRayCastCompositeFxFunction.h"

#include <vtkObjectFactory.h>
#include <vtkPiecewiseFunction.h>
#include <vtkVolume.h>
#include <vtkVolumeProperty.h>
#include <vtkVolumeRayCastMapper.h>

#include <QColor>

#include "trilinearinterpolator.h"
#include "vector3.h"
#include "voxelshader.h"



namespace udg {


vtkCxxRevisionMacro( vtkVolumeRayCastCompositeFxFunction, "$Revision: 1.0 $" );
vtkStandardNewMacro( vtkVolumeRayCastCompositeFxFunction );

const float vtkVolumeRayCastCompositeFxFunction::REMAINING_OPACITY = 0.02f;

vtkVolumeRayCastCompositeFxFunction::vtkVolumeRayCastCompositeFxFunction()
{
    m_compositeMethod = ClassifyInterpolate;
    m_interpolator = new TrilinearInterpolator();
}


vtkVolumeRayCastCompositeFxFunction::~vtkVolumeRayCastCompositeFxFunction()
{
}


// We don't need to do any specific initialization here...
void vtkVolumeRayCastCompositeFxFunction::SpecificFunctionInitialize( vtkRenderer *vtkNotUsed(renderer), vtkVolume *vtkNotUsed(volume),
                                                                      vtkVolumeRayCastStaticInfo *vtkNotUsed(staticInfo),
                                                                      vtkVolumeRayCastMapper *vtkNotUsed(mapper) )
{
}


void vtkVolumeRayCastCompositeFxFunction::PrintSelf( ostream &os, vtkIndent indent )
{
    this->Superclass::PrintSelf( os, indent );

    os << indent << "Composite Method: " << this->GetCompositeMethodAsString() << "\n";
    os << indent << "Voxel Shaders:\n";

    if ( m_voxelShaderList.isEmpty() ) os << indent << "  (none)\n";
    else
    {
        foreach ( VoxelShader *voxelShader, m_voxelShaderList )
            os << indent << "  " << voxelShader->toString().toStdString() << "\n";
    }

    os << std::flush;
}


const char* vtkVolumeRayCastCompositeFxFunction::GetCompositeMethodAsString() const
{
    switch ( m_compositeMethod )
    {
        case ClassifyInterpolate: return "Classify & Interpolate";
        case InterpolateClassify: return "Interpolate & Classify";
        default: return "Unknown";
    }
}


// This is called from RenderAnImage (in vtkDepthPARCMapper.cxx)
// It uses the integer data type flag that is passed in to
// determine what type of ray needs to be cast (which is handled
// by a templated function.  It also uses the shading and
// interpolation types to determine which templated function
// to call.
void vtkVolumeRayCastCompositeFxFunction::CastRay( vtkVolumeRayCastDynamicInfo *dynamicInfo, vtkVolumeRayCastStaticInfo *staticInfo )
{
    void *data = staticInfo->ScalarDataPointer;

    switch ( staticInfo->ScalarDataType )
    {
        case VTK_UNSIGNED_CHAR: CastRay( static_cast<unsigned char*>( data ), dynamicInfo, staticInfo ); break;
        case VTK_UNSIGNED_SHORT: CastRay( static_cast<unsigned short*>( data ), dynamicInfo, staticInfo ); break;
        default: vtkWarningMacro( << "Unsigned char and unsigned short are the only supported datatypes for rendering" ); break;
    }
}


float vtkVolumeRayCastCompositeFxFunction::GetZeroOpacityThreshold( vtkVolume *volume )
{
    return volume->GetProperty()->GetScalarOpacity()->GetFirstNonZeroValue();
}


void vtkVolumeRayCastCompositeFxFunction::AddVoxelShader( VoxelShader * voxelShader )
{
    Q_CHECK_PTR( voxelShader );

    m_voxelShaderList << voxelShader;
}


void vtkVolumeRayCastCompositeFxFunction::InsertVoxelShader( int i, VoxelShader * voxelShader )
{
    m_voxelShaderList.insert( i, voxelShader );
}


int vtkVolumeRayCastCompositeFxFunction::IndexOfVoxelShader( VoxelShader * voxelShader )
{
    return m_voxelShaderList.indexOf( voxelShader );
}


void vtkVolumeRayCastCompositeFxFunction::RemoveVoxelShader( int i )
{
    m_voxelShaderList.removeAt( i );
}


void vtkVolumeRayCastCompositeFxFunction::RemoveVoxelShader( VoxelShader * voxelShader )
{
    int index = m_voxelShaderList.indexOf( voxelShader );
    if ( index >= 0 ) m_voxelShaderList.removeAt( index );
}


void vtkVolumeRayCastCompositeFxFunction::RemoveAllVoxelShaders()
{
    m_voxelShaderList.clear();
}


template <class T> void vtkVolumeRayCastCompositeFxFunction::CastRay( const T *data,
                                                                      vtkVolumeRayCastDynamicInfo *dynamicInfo,
                                                                      const vtkVolumeRayCastStaticInfo *staticInfo ) const
{
    const bool INTERPOLATION = staticInfo->InterpolationType == VTK_LINEAR_INTERPOLATION;
    const bool CLASSIFY_INTERPOLATE = m_compositeMethod == ClassifyInterpolate;
    const int N_STEPS = dynamicInfo->NumberOfStepsToTake;
    const float * const RAY_START = dynamicInfo->TransformedStart;
    const float * const A_RAY_INCREMENT = dynamicInfo->TransformedIncrement;
    const Vector3 RAY_INCREMENT( A_RAY_INCREMENT[0], A_RAY_INCREMENT[1], A_RAY_INCREMENT[2] );
    const float * const A_DIRECTION = dynamicInfo->TransformedDirection;
    Vector3 direction( A_DIRECTION[0], A_DIRECTION[1], A_DIRECTION[2] );
    direction.normalize();

//     const float * const SCALAR_OPACITY_TRANSFER_FUNCTION = staticInfo->Volume->GetCorrectedScalarOpacityArray();
//     const float * const COLOR_TRANSFER_FUNCTION = staticInfo->Volume->GetRGBArray();
//     const float * const GRAY_TRANSFER_FUNCTION = staticInfo->Volume->GetGrayArray();
//     const float * const GRADIENT_OPACITY_TRANSFER_FUNCTION = staticInfo->Volume->GetGradientOpacityArray();

    // Get the gradient opacity constant. If this number is greater than
    // or equal to 0.0, then the gradient opacity transfer function is
    // a constant at that value, otherwise it is not a constant function
    const float GRADIENT_OPACITY_CONSTANT = staticInfo->Volume->GetGradientOpacityConstant();
    const bool GRADIENT_OPACITY_IS_CONSTANT = GRADIENT_OPACITY_CONSTANT >= 0.0;

    // Move the increments into local variables
    const int * const INCREMENTS = staticInfo->DataIncrement;
    const int X_INC = INCREMENTS[0], Y_INC = INCREMENTS[1], Z_INC = INCREMENTS[2];

    if ( INTERPOLATION ) m_interpolator->setIncrements( X_INC, Y_INC, Z_INC );

    // Initialize the ray position and voxel location
    Vector3 rayPosition( RAY_START[0], RAY_START[1], RAY_START[2] );
    int voxel[3];

    if ( !INTERPOLATION )
    {
        voxel[0] = vtkRoundFuncMacro( rayPosition.x );
        voxel[1] = vtkRoundFuncMacro( rayPosition.y );
        voxel[2] = vtkRoundFuncMacro( rayPosition.z );
    }
    else
    {
        voxel[0] = vtkFloorFuncMacro( rayPosition.x );
        voxel[1] = vtkFloorFuncMacro( rayPosition.y );
        voxel[2] = vtkFloorFuncMacro( rayPosition.z );
    }

    // So far we haven't accumulated anything
    float accumulatedRedIntensity = 0.0, accumulatedGreenIntensity = 0.0, accumulatedBlueIntensity = 0.0;
    float remainingOpacity = 1.0;

    // Get a pointer to the gradient magnitudes for this volume
    const unsigned char * GRADIENT_MAGNITUDES;
    if ( !GRADIENT_OPACITY_IS_CONSTANT ) GRADIENT_MAGNITUDES = staticInfo->GradientMagnitudes;

    // Keep track of previous voxel to know when we step into a new one
    // set it to something invalid to start with so that everything is
    // computed first time through
    int previousVoxel[3];
    previousVoxel[0] = voxel[0] - 1; previousVoxel[1] = voxel[1] - 1; previousVoxel[2] = voxel[2] - 1;

    int stepsThisRay = 0;
    int nShaders = m_voxelShaderList.size();

    // For each step along the ray
    for ( int step = 0; step < N_STEPS && remainingOpacity > REMAINING_OPACITY; step++ )
    {
        // We've taken another step
        stepsThisRay++;

        int offset = voxel[2] * Z_INC + voxel[1] * Y_INC + voxel[0] * X_INC;

        // Access the value at this voxel location
        if ( previousVoxel[0] != voxel[0] || previousVoxel[1] != voxel[1] || previousVoxel[2] != voxel[2] )
        {
            previousVoxel[0] = voxel[0]; previousVoxel[1] = voxel[1]; previousVoxel[2] = voxel[2];
        }

        HdrColor color;

        if ( !INTERPOLATION )
        {
            for ( int i = 0; i < nShaders; i++ ) color = m_voxelShaderList.at( i )->shade( offset, direction, color );
        }
        else if ( CLASSIFY_INTERPOLATE )
        {
            int offsets[8];
            double weights[8];

            m_interpolator->getOffsetsAndWeights( rayPosition, offsets, weights );

            for ( int j = 0; j < 8; j++ )
            {
                HdrColor tempColor;

                for ( int i = 0; i < nShaders; i++ ) tempColor = m_voxelShaderList.at( i )->shade( offsets[j], direction, tempColor );

                tempColor.alpha *= weights[j];
                color += tempColor.multiplyColorBy( tempColor.alpha );
            }
        }
        else //if ( !CLASSIFY_INTERPOLATE )
        {
            for ( int i = 0; i < nShaders; i++ ) color = m_voxelShaderList.at( i )->shade( rayPosition, direction, m_interpolator, color );
        }

        float opacity = color.alpha, f;

        if ( !INTERPOLATION || !CLASSIFY_INTERPOLATE ) f = opacity * remainingOpacity;
        else f = remainingOpacity;

        accumulatedRedIntensity += f * color.red;
        accumulatedGreenIntensity += f * color.green;
        accumulatedBlueIntensity += f * color.blue;
        remainingOpacity *= ( 1.0 - opacity );

        // Increment our position and compute our voxel location
        rayPosition += RAY_INCREMENT;

        if ( !INTERPOLATION )
        {
            voxel[0] = vtkRoundFuncMacro( rayPosition.x );
            voxel[1] = vtkRoundFuncMacro( rayPosition.y );
            voxel[2] = vtkRoundFuncMacro( rayPosition.z );
        }
        else
        {
            voxel[0] = vtkFloorFuncMacro( rayPosition.x );
            voxel[1] = vtkFloorFuncMacro( rayPosition.y );
            voxel[2] = vtkFloorFuncMacro( rayPosition.z );
        }
    }

    // Cap the intensity value at 1.0
    if ( accumulatedRedIntensity > 1.0 ) accumulatedRedIntensity = 1.0;
    if ( accumulatedGreenIntensity > 1.0 ) accumulatedGreenIntensity = 1.0;
    if ( accumulatedBlueIntensity > 1.0 ) accumulatedBlueIntensity = 1.0;
    if ( remainingOpacity < REMAINING_OPACITY ) remainingOpacity = 0.0;

    // Set the return pixel value.  The depth value is the distance to the
    // center of the volume.
    dynamicInfo->Color[0] = accumulatedRedIntensity;
    dynamicInfo->Color[1] = accumulatedGreenIntensity;
    dynamicInfo->Color[2] = accumulatedBlueIntensity;
    dynamicInfo->Color[3] = 1.0 - remainingOpacity;
    dynamicInfo->NumberOfStepsTaken = stepsThisRay;
}


}
