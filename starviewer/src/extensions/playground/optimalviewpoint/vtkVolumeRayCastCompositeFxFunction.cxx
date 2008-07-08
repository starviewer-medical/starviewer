#include "vtkVolumeRayCastCompositeFxFunction.h"

#include <vtkObjectFactory.h>
#include <vtkPiecewiseFunction.h>
#include <vtkVolume.h>
#include <vtkVolumeProperty.h>
#include <vtkVolumeRayCastMapper.h>

// #include "vector3.h"
// #include <vtkEncodedGradientEstimator.h>
// #include <vtkDirectionEncoder.h>

namespace udg {


vtkCxxRevisionMacro( vtkVolumeRayCastCompositeFxFunction, "$Revision: 1.0 $" );
vtkStandardNewMacro( vtkVolumeRayCastCompositeFxFunction );


vtkVolumeRayCastCompositeFxFunction::vtkVolumeRayCastCompositeFxFunction()
{
    m_compositeMethod = ClassifyInterpolate;
    m_voxelShader = 0;
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
    os << indent << "Voxel Shader: " << m_voxelShader << "\n";
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


template <class T> void vtkVolumeRayCastCompositeFxFunction::CastRay( T *data,
                                                                      vtkVolumeRayCastDynamicInfo *dynamicInfo,
                                                                      vtkVolumeRayCastStaticInfo *staticInfo ) const
{
    int nSteps = dynamicInfo->NumberOfStepsToTake;
    float *rayStart = dynamicInfo->TransformedStart;
    float *rayIncrement = dynamicInfo->TransformedIncrement;

    float *scalarOpacityTransferFunction = staticInfo->Volume->GetCorrectedScalarOpacityArray();
    float *colorTransferFunction = staticInfo->Volume->GetRGBArray();
    float *grayTransferFunction = staticInfo->Volume->GetGrayArray();
    float *gradientOpacityTransferFunction = staticInfo->Volume->GetGradientOpacityArray();

    // Get the gradient opacity constant. If this number is greater than
    // or equal to 0.0, then the gradient opacity transfer function is
    // a constant at that value, otherwise it is not a constant function
    float gradientOpacityConstant = staticInfo->Volume->GetGradientOpacityConstant();
    bool gradientOpacityIsConstant = gradientOpacityConstant >= 0.0;

    // Move the increments into local variables
    int xInc = staticInfo->DataIncrement[0], yInc = staticInfo->DataIncrement[1], zInc = staticInfo->DataIncrement[2];

    // Initialize the ray position and voxel location
    float rayPosition[3];
    rayPosition[0] = rayStart[0]; rayPosition[1] = rayStart[1]; rayPosition[2] = rayStart[2];
    int voxel[3];
    voxel[0] = vtkRoundFuncMacro( rayPosition[0] );
    voxel[1] = vtkRoundFuncMacro( rayPosition[1] );
    voxel[2] = vtkRoundFuncMacro( rayPosition[2] );

    // So far we haven't accumulated anything
    float accumulatedRedIntensity = 0.0, accumulatedGreenIntensity = 0.0, accumulatedBlueIntensity = 0.0;
    float remainingOpacity = 1.0;

    // Get a pointer to the gradient magnitudes for this volume
    unsigned char *gradientMagnitudes;
    if ( !gradientOpacityIsConstant ) gradientMagnitudes = staticInfo->GradientMagnitudes;

    // Keep track of previous voxel to know when we step into a new one
    // set it to something invalid to start with so that everything is
    // computed first time through
    int previousVoxel[3];
    previousVoxel[0] = voxel[0] - 1; previousVoxel[1] = voxel[1] - 1; previousVoxel[2] = voxel[2] - 1;

    int stepsThisRay = 0;

    // For each step along the ray
    for ( int step = 0; step < nSteps && remainingOpacity > REMAINING_OPACITY; step++ )
    {
        // We've taken another step
        stepsThisRay++;

        int offset = voxel[2] * zInc + voxel[1] * yInc + voxel[0] * xInc;

        // Access the value at this voxel location
        if ( previousVoxel[0] != voxel[0] || previousVoxel[1] != voxel[1] || previousVoxel[2] != voxel[2] )
        {
            previousVoxel[0] = voxel[0]; previousVoxel[1] = voxel[1]; previousVoxel[2] = voxel[2];
        }

        // aquí es faria la crida al voxel shader
        accumulatedRedIntensity += static_cast<float>( qrand() ) / RAND_MAX;
        accumulatedGreenIntensity += static_cast<float>( qrand() ) / RAND_MAX;
        accumulatedBlueIntensity += static_cast<float>( qrand() ) / RAND_MAX;
        float opacity = static_cast<float>( qrand() ) / RAND_MAX;

        remainingOpacity *= (1.0 - opacity);

        // Increment our position and compute our voxel location
        rayPosition[0] += rayIncrement[0]; rayPosition[1] += rayIncrement[1]; rayPosition[2] += rayIncrement[2];
        voxel[0] = vtkRoundFuncMacro( rayPosition[0] );
        voxel[1] = vtkRoundFuncMacro( rayPosition[1] );
        voxel[2] = vtkRoundFuncMacro( rayPosition[2] );
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
