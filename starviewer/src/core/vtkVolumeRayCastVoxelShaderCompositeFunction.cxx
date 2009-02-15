#include "vtkVolumeRayCastVoxelShaderCompositeFunction.h"

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


vtkCxxRevisionMacro( vtkVolumeRayCastVoxelShaderCompositeFunction, "$Revision: 1.0 $" );
vtkStandardNewMacro( vtkVolumeRayCastVoxelShaderCompositeFunction );

const float vtkVolumeRayCastVoxelShaderCompositeFunction::MINIMUM_REMAINING_OPACITY = 0.02f;

vtkVolumeRayCastVoxelShaderCompositeFunction::vtkVolumeRayCastVoxelShaderCompositeFunction()
{
    m_compositeMethod = ClassifyInterpolate;
    m_interpolator = new TrilinearInterpolator();
}


vtkVolumeRayCastVoxelShaderCompositeFunction::~vtkVolumeRayCastVoxelShaderCompositeFunction()
{
    delete m_interpolator;
}


// We don't need to do any specific initialization here...
void vtkVolumeRayCastVoxelShaderCompositeFunction::SpecificFunctionInitialize( vtkRenderer *vtkNotUsed(renderer), vtkVolume *vtkNotUsed(volume),
                                                                      vtkVolumeRayCastStaticInfo *vtkNotUsed(staticInfo),
                                                                      vtkVolumeRayCastMapper *vtkNotUsed(mapper) )
{
}


void vtkVolumeRayCastVoxelShaderCompositeFunction::PrintSelf( ostream &os, vtkIndent indent )
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


const char* vtkVolumeRayCastVoxelShaderCompositeFunction::GetCompositeMethodAsString() const
{
    switch ( m_compositeMethod )
    {
        case ClassifyInterpolate: return "Classify & Interpolate";
        case InterpolateClassify: return "Interpolate & Classify";
        default: return "Unknown";
    }
}


// This is called from RenderAnImage (in vtkDepthPARCMapper.cxx)
void vtkVolumeRayCastVoxelShaderCompositeFunction::CastRay( vtkVolumeRayCastDynamicInfo *dynamicInfo, vtkVolumeRayCastStaticInfo *staticInfo )
{
    if ( m_voxelShaderList.isEmpty() )  // si no hi ha voxel shaders retornem transparent
    {
        // Set the return pixel value. The depth value is the distance to the center of the volume.
        dynamicInfo->Color[0] = 0.0f;
        dynamicInfo->Color[1] = 0.0f;
        dynamicInfo->Color[2] = 0.0f;
        dynamicInfo->Color[3] = 0.0f;
        dynamicInfo->NumberOfStepsTaken = 0;
        return;
    }

    const bool INTERPOLATION = staticInfo->InterpolationType == VTK_LINEAR_INTERPOLATION;
    const bool CLASSIFY_INTERPOLATE = m_compositeMethod == ClassifyInterpolate;

    // Move the increments into local variables
    const int * const INCREMENTS = staticInfo->DataIncrement;
    const int X_INC = INCREMENTS[0], Y_INC = INCREMENTS[1], Z_INC = INCREMENTS[2];

    // Get the gradient opacity constant. If this number is greater than or equal to 0.0, then the gradient opacity transfer function is a constant at that
    // value, otherwise it is not a constant function.
//    const float GRADIENT_OPACITY_CONSTANT = staticInfo->Volume->GetGradientOpacityConstant();
//    const bool GRADIENT_OPACITY_IS_CONSTANT = GRADIENT_OPACITY_CONSTANT >= 0.0f;

    // Get a pointer to the gradient magnitudes for this volume
//    const unsigned char * const GRADIENT_MAGNITUDES;
//    if ( !GRADIENT_OPACITY_IS_CONSTANT ) GRADIENT_MAGNITUDES = staticInfo->GradientMagnitudes;

    const int N_STEPS = dynamicInfo->NumberOfStepsToTake;
    const float * const RAY_START = dynamicInfo->TransformedStart;
    const float * const A_RAY_INCREMENT = dynamicInfo->TransformedIncrement;
    const Vector3 RAY_INCREMENT( A_RAY_INCREMENT[0], A_RAY_INCREMENT[1], A_RAY_INCREMENT[2] );
    const float * const A_DIRECTION = dynamicInfo->TransformedDirection;
    Vector3 direction( A_DIRECTION[0], A_DIRECTION[1], A_DIRECTION[2] );
    direction.normalize();

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
    float accumulatedRedIntensity = 0.0f, accumulatedGreenIntensity = 0.0f, accumulatedBlueIntensity = 0.0f;
    float remainingOpacity = 1.0f;

    int stepsThisRay = 0, nShaders = m_voxelShaderList.size();

    // For each step along the ray
    for ( int step = 0; step < N_STEPS && remainingOpacity > MINIMUM_REMAINING_OPACITY; step++ )
    {
        // We've taken another step
        stepsThisRay++;

        HdrColor color;

        if ( !INTERPOLATION )
        {
            int offset = voxel[0] * X_INC + voxel[1] * Y_INC + voxel[2] * Z_INC;
            for ( int i = 0; i < nShaders; i++ ) color = m_voxelShaderList.at( i )->shade( rayPosition, offset, direction, remainingOpacity, color );
        }
        else if ( CLASSIFY_INTERPOLATE )
        {
            Vector3 positions[8];
            int offsets[8];
            double weights[8];

            m_interpolator->getPositions( rayPosition, positions );
            m_interpolator->getOffsetsAndWeights( rayPosition, offsets, weights );

            for ( int j = 0; j < 8; j++ )
            {
                HdrColor tempColor;

                for ( int i = 0; i < nShaders; i++ )
                    tempColor = m_voxelShaderList.at( i )->shade( positions[j], offsets[j], direction, remainingOpacity, tempColor );

                tempColor.alpha *= weights[j];
                color += tempColor.multiplyColorBy( tempColor.alpha );
            }
        }
        else //if ( !CLASSIFY_INTERPOLATE )
        {
            for ( int i = 0; i < nShaders; i++ ) color = m_voxelShaderList.at( i )->shade( rayPosition, direction, m_interpolator, remainingOpacity, color );
        }

        float opacity = color.alpha, f;

        if ( !INTERPOLATION || !CLASSIFY_INTERPOLATE ) f = opacity * remainingOpacity;
        else f = remainingOpacity;

        accumulatedRedIntensity += f * color.red;
        accumulatedGreenIntensity += f * color.green;
        accumulatedBlueIntensity += f * color.blue;
        remainingOpacity *= ( 1.0f - opacity );

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
    if ( accumulatedRedIntensity > 1.0f ) accumulatedRedIntensity = 1.0f;
    if ( accumulatedGreenIntensity > 1.0f ) accumulatedGreenIntensity = 1.0f;
    if ( accumulatedBlueIntensity > 1.0f ) accumulatedBlueIntensity = 1.0f;
    if ( remainingOpacity < MINIMUM_REMAINING_OPACITY ) remainingOpacity = 0.0f;

    // Set the return pixel value. The depth value is the distance to the center of the volume.
    dynamicInfo->Color[0] = accumulatedRedIntensity;
    dynamicInfo->Color[1] = accumulatedGreenIntensity;
    dynamicInfo->Color[2] = accumulatedBlueIntensity;
    dynamicInfo->Color[3] = 1.0f - remainingOpacity;
    dynamicInfo->NumberOfStepsTaken = stepsThisRay;
}


float vtkVolumeRayCastVoxelShaderCompositeFunction::GetZeroOpacityThreshold( vtkVolume *volume )
{
    return volume->GetProperty()->GetScalarOpacity()->GetFirstNonZeroValue();
}


void vtkVolumeRayCastVoxelShaderCompositeFunction::AddVoxelShader( VoxelShader *voxelShader )
{
    Q_ASSERT( voxelShader );

    m_voxelShaderList << voxelShader;
}


void vtkVolumeRayCastVoxelShaderCompositeFunction::InsertVoxelShader( int i, VoxelShader *voxelShader )
{
    m_voxelShaderList.insert( i, voxelShader );
}


int vtkVolumeRayCastVoxelShaderCompositeFunction::IndexOfVoxelShader( VoxelShader *voxelShader )
{
    return m_voxelShaderList.indexOf( voxelShader );
}


void vtkVolumeRayCastVoxelShaderCompositeFunction::RemoveVoxelShader( int i )
{
    m_voxelShaderList.removeAt( i );
}


void vtkVolumeRayCastVoxelShaderCompositeFunction::RemoveVoxelShader( VoxelShader *voxelShader )
{
    int index = m_voxelShaderList.indexOf( voxelShader );
    if ( index >= 0 ) m_voxelShaderList.removeAt( index );
}


void vtkVolumeRayCastVoxelShaderCompositeFunction::RemoveAllVoxelShaders()
{
    m_voxelShaderList.clear();
}


}
