/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkVolumeRayCastCompositeFunctionFx.cxx,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkVolumeRayCastCompositeFunctionFx.h"

#include "vtkObjectFactory.h"
#include "vtkPiecewiseFunction.h"
#include "vtkVolume.h"
#include "vtkVolumeProperty.h"
#include "vtkVolumeRayCastMapper.h"

#include "vector3.h"
#include <vtkEncodedGradientEstimator.h>
#include <vtkDirectionEncoder.h>

vtkCxxRevisionMacro(vtkVolumeRayCastCompositeFunctionFx, "$Revision: 1.1 $");
vtkStandardNewMacro(vtkVolumeRayCastCompositeFunctionFx);

#define VTK_REMAINING_OPACITY           0.02

// This is the templated function that actually casts a ray and computes
// The composite value. This version uses nearest neighbor interpolation
// and does not perform shading.
template <class T>
void vtkCastRay_NN_Unshaded( T *data_ptr, vtkVolumeRayCastDynamicInfo *dynamicInfo,
                          vtkVolumeRayCastStaticInfo *staticInfo,
                          double * aObscurance, double aObscuranceFactor, double aObscuranceFilterLow, double aObscuranceFilterHigh,
                          bool aFxObscurance, double aFxContour, bool aFxSaliency, double * aSaliency )
{
  int             value=0;
  unsigned char   *grad_mag_ptr = NULL;
  float           accum_red_intensity;
  float           accum_green_intensity;
  float           accum_blue_intensity;
  float           remaining_opacity;
  float           opacity=0.0;
  float           gradient_opacity;
  int             loop;
  int             xinc, yinc, zinc;
  int             voxel[3];
  float           ray_position[3];
  int             prev_voxel[3];
  float           *SOTF;
  float           *CTF;
  float           *GTF;
  float           *GOTF;
  int             offset;
  int             steps_this_ray = 0;
  int             grad_op_is_constant;
  float           gradient_opacity_constant;
  int             num_steps;
  float           *ray_start, *ray_increment;

  num_steps = dynamicInfo->NumberOfStepsToTake;
  ray_start = dynamicInfo->TransformedStart;
  ray_increment = dynamicInfo->TransformedIncrement;
 
  SOTF =  staticInfo->Volume->GetCorrectedScalarOpacityArray();
  CTF  =  staticInfo->Volume->GetRGBArray();
  GTF  =  staticInfo->Volume->GetGrayArray();
  GOTF =  staticInfo->Volume->GetGradientOpacityArray();

  // Get the gradient opacity constant. If this number is greater than
  // or equal to 0.0, then the gradient opacity transfer function is
  // a constant at that value, otherwise it is not a constant function
  gradient_opacity_constant = staticInfo->Volume->GetGradientOpacityConstant();
  grad_op_is_constant = ( gradient_opacity_constant >= 0.0 );

  // Move the increments into local variables
  xinc = staticInfo->DataIncrement[0];
  yinc = staticInfo->DataIncrement[1];
  zinc = staticInfo->DataIncrement[2];

  // Initialize the ray position and voxel location
  ray_position[0] = ray_start[0];
  ray_position[1] = ray_start[1];
  ray_position[2] = ray_start[2];
  voxel[0] = vtkRoundFuncMacro( ray_position[0] );
  voxel[1] = vtkRoundFuncMacro( ray_position[1] );
  voxel[2] = vtkRoundFuncMacro( ray_position[2] );

  // So far we haven't accumulated anything
  accum_red_intensity     = 0.0;
  accum_green_intensity   = 0.0;
  accum_blue_intensity    = 0.0;
  remaining_opacity       = 1.0;

  // Get a pointer to the gradient magnitudes for this volume
  if ( !grad_op_is_constant )
    {
    grad_mag_ptr = staticInfo->GradientMagnitudes;
    }

  
  // Keep track of previous voxel to know when we step into a new one
  // set it to something invalid to start with so that everything is
  // computed first time through
  prev_voxel[0] = voxel[0]-1;
  prev_voxel[1] = voxel[1]-1;
  prev_voxel[2] = voxel[2]-1;

  //////////////////////////////////////////////////////////////////////////////////////////////////
  udg::Vector3 fxDirection( dynamicInfo->TransformedDirection[0], dynamicInfo->TransformedDirection[1], dynamicInfo->TransformedDirection[2] );
  fxDirection.normalize();
  vtkVolumeRayCastMapper * fxMapper = vtkVolumeRayCastMapper::SafeDownCast( staticInfo->Volume->GetMapper() );
  vtkEncodedGradientEstimator * fxGradientEstimator = fxMapper->GetGradientEstimator();
  unsigned short * fxEncodedNormals = fxGradientEstimator->GetEncodedNormals();
  vtkDirectionEncoder * fxDirectionEncoder = fxGradientEstimator->GetDirectionEncoder();
  //////////////////////////////////////////////////////////////////////////////////////////////////

  // Two cases - we are working with a gray or RGB transfer
  // function - break them up to make it more efficient
  if ( staticInfo->ColorChannels == 1 ) 
    {
    // For each step along the ray
    for ( loop = 0; 
          loop < num_steps && remaining_opacity > VTK_REMAINING_OPACITY; 
          loop++ )
      {     
      // We've taken another step
      steps_this_ray++;
      
      // Access the value at this voxel location
      if ( prev_voxel[0] != voxel[0] ||
           prev_voxel[1] != voxel[1] ||
           prev_voxel[2] != voxel[2] )
        {
        offset = voxel[2] * zinc + voxel[1] * yinc + voxel[0] * xinc;
        value = *(data_ptr + offset);
        opacity = SOTF[value];

        if ( opacity )
          {
          if ( grad_op_is_constant )
            {
            gradient_opacity = gradient_opacity_constant;
            }
          else 
            {
            gradient_opacity = GOTF[*(grad_mag_ptr + offset)];
            }
          opacity *= gradient_opacity;
          }

        prev_voxel[0] = voxel[0];
        prev_voxel[1] = voxel[1];
        prev_voxel[2] = voxel[2];
        }
        
      // Accumulate some light intensity and opacity
      //////////////////////////////////////////////////////////////////////////////////////////////
//       accum_red_intensity   += ( opacity * remaining_opacity * 
//                                  GTF[(value)] );

      if ( aFxSaliency )
      {
        opacity *= aSaliency[offset];
      }

      double fx = 1.0;

      // contour
      if ( aFxContour > 0.0 )
      {
        float * fxGradient = fxDirectionEncoder->GetDecodedGradient( fxEncodedNormals[offset] );
        udg::Vector3 fxNormal( fxGradient[0], fxGradient[1], fxGradient[2] );
        double fxDotProduct = fxDirection * fxNormal;
        if ( fxDotProduct < 0.0 ) fxDotProduct = -fxDotProduct;
        fx *= fxDotProduct < aFxContour ? 0.0 : 1.0;
      }

      // obscurance
      if ( aFxObscurance && fx > 0.0 )
      {
        double fxObscurance = aObscurance[offset];
        if ( fxObscurance < aObscuranceFilterLow ) fxObscurance = 0.0;
        else if ( fxObscurance > aObscuranceFilterHigh ) fxObscurance = 1.0;
        fx *= aObscuranceFactor * fxObscurance;
      }

      accum_red_intensity   += ( opacity * remaining_opacity * 
                                 GTF[(value)] ) * fx;
      //////////////////////////////////////////////////////////////////////////////////////////////
      remaining_opacity *= (1.0 - opacity);
      
      // Increment our position and compute our voxel location
      ray_position[0] += ray_increment[0];
      ray_position[1] += ray_increment[1];
      ray_position[2] += ray_increment[2];
      voxel[0] = vtkRoundFuncMacro( ray_position[0] );
      voxel[1] = vtkRoundFuncMacro( ray_position[1] );
      voxel[2] = vtkRoundFuncMacro( ray_position[2] );
      }
    accum_green_intensity = accum_red_intensity;
    accum_blue_intensity = accum_red_intensity;
    }
  else if ( staticInfo->ColorChannels == 3 )
    {
    // For each step along the ray
    for ( loop = 0; 
          loop < num_steps && remaining_opacity > VTK_REMAINING_OPACITY; 
          loop++ )
      {     
      // We've taken another step
      steps_this_ray++;
      
      // Access the value at this voxel location
      if ( prev_voxel[0] != voxel[0] ||
           prev_voxel[1] != voxel[1] ||
           prev_voxel[2] != voxel[2] )
        {
        offset = voxel[2] * zinc + voxel[1] * yinc + voxel[0] * xinc;
        value = *(data_ptr + offset);
        opacity = SOTF[value];

        if ( opacity )
          {
          if ( grad_op_is_constant )
            {
            gradient_opacity = gradient_opacity_constant;
            }
          else 
            {
            gradient_opacity = GOTF[*(grad_mag_ptr + offset)];  
            }
          opacity *= gradient_opacity;
          }

        prev_voxel[0] = voxel[0];
        prev_voxel[1] = voxel[1];
        prev_voxel[2] = voxel[2];
        }


      // Accumulate some light intensity and opacity
      //////////////////////////////////////////////////////////////////////////////////////////////
//       accum_red_intensity   += ( opacity * remaining_opacity * 
//                                  CTF[(value)*3] );
//       accum_green_intensity += ( opacity * remaining_opacity * 
//                                  CTF[(value)*3 + 1] );
//       accum_blue_intensity  += ( opacity * remaining_opacity * 
//                                  CTF[(value)*3 + 2] );

      if ( aFxSaliency )
      {
        opacity *= aSaliency[offset];
      }

      double fx = 1.0;

      // contour
      if ( aFxContour > 0.0 )
      {
        float * fxGradient = fxDirectionEncoder->GetDecodedGradient( fxEncodedNormals[offset] );
        udg::Vector3 fxNormal( fxGradient[0], fxGradient[1], fxGradient[2] );
        double fxDotProduct = fxDirection * fxNormal;
        if ( fxDotProduct < 0.0 ) fxDotProduct = -fxDotProduct;
        fx *= fxDotProduct < aFxContour ? 0.0 : 1.0;
      }

      // obscurance
      if ( aFxObscurance && fx > 0.0 )
      {
        double fxObscurance = aObscurance[offset];
        if ( fxObscurance < aObscuranceFilterLow ) fxObscurance = 0.0;
        else if ( fxObscurance > aObscuranceFilterHigh ) fxObscurance = 1.0;
        fx *= aObscuranceFactor * fxObscurance;
      }

      accum_red_intensity   += ( opacity * remaining_opacity * 
                                 CTF[(value)*3] ) * fx;
      accum_green_intensity += ( opacity * remaining_opacity * 
                                 CTF[(value)*3 + 1] ) * fx;
      accum_blue_intensity  += ( opacity * remaining_opacity * 
                                 CTF[(value)*3 + 2] ) * fx;
      //////////////////////////////////////////////////////////////////////////////////////////////
      remaining_opacity *= (1.0 - opacity);
      
      // Increment our position and compute our voxel location
      ray_position[0] += ray_increment[0];
      ray_position[1] += ray_increment[1];
      ray_position[2] += ray_increment[2];
      voxel[0] = vtkRoundFuncMacro( ray_position[0] );
      voxel[1] = vtkRoundFuncMacro( ray_position[1] );
      voxel[2] = vtkRoundFuncMacro( ray_position[2] );
      }
    }

  // Cap the intensity value at 1.0
  if ( accum_red_intensity > 1.0 )
    {
    accum_red_intensity = 1.0;
    }
  if ( accum_green_intensity > 1.0 )
    {
    accum_green_intensity = 1.0;
    }
  if ( accum_blue_intensity > 1.0 )
    {
    accum_blue_intensity = 1.0;
    }
  
  if( remaining_opacity < VTK_REMAINING_OPACITY )
    {
    remaining_opacity = 0.0;
    }

  // Set the return pixel value.  The depth value is the distance to the
  // center of the volume.
  dynamicInfo->Color[0] = accum_red_intensity;
  dynamicInfo->Color[1] = accum_green_intensity;
  dynamicInfo->Color[2] = accum_blue_intensity;
  dynamicInfo->Color[3] = 1.0 - remaining_opacity;
  dynamicInfo->NumberOfStepsTaken = steps_this_ray;
  
}


// This is the templated function that actually casts a ray and computes
// The composite value. This version uses nearest neighbor interpolation
// and does not perform shading.
template <class T>
void vtkCastRay_NN_Unshaded( T *data_ptr, vtkVolumeRayCastDynamicInfo *dynamicInfo,
                          vtkVolumeRayCastStaticInfo *staticInfo, udg::Vector3 * aColorBleeding, double aObscuranceFactor )
{
  int             value=0;
  unsigned char   *grad_mag_ptr = NULL;
  float           accum_red_intensity;
  float           accum_green_intensity;
  float           accum_blue_intensity;
  float           remaining_opacity;
  float           opacity=0.0;
  float           gradient_opacity;
  int             loop;
  int             xinc, yinc, zinc;
  int             voxel[3];
  float           ray_position[3];
  int             prev_voxel[3];
  float           *SOTF;
  float           *CTF;
  float           *GTF;
  float           *GOTF;
  int             offset;
  int             steps_this_ray = 0;
  int             grad_op_is_constant;
  float           gradient_opacity_constant;
  int             num_steps;
  float           *ray_start, *ray_increment;

  num_steps = dynamicInfo->NumberOfStepsToTake;
  ray_start = dynamicInfo->TransformedStart;
  ray_increment = dynamicInfo->TransformedIncrement;
 
  SOTF =  staticInfo->Volume->GetCorrectedScalarOpacityArray();
  CTF  =  staticInfo->Volume->GetRGBArray();
  GTF  =  staticInfo->Volume->GetGrayArray();
  GOTF =  staticInfo->Volume->GetGradientOpacityArray();

  // Get the gradient opacity constant. If this number is greater than
  // or equal to 0.0, then the gradient opacity transfer function is
  // a constant at that value, otherwise it is not a constant function
  gradient_opacity_constant = staticInfo->Volume->GetGradientOpacityConstant();
  grad_op_is_constant = ( gradient_opacity_constant >= 0.0 );

  // Move the increments into local variables
  xinc = staticInfo->DataIncrement[0];
  yinc = staticInfo->DataIncrement[1];
  zinc = staticInfo->DataIncrement[2];

  // Initialize the ray position and voxel location
  ray_position[0] = ray_start[0];
  ray_position[1] = ray_start[1];
  ray_position[2] = ray_start[2];
  voxel[0] = vtkRoundFuncMacro( ray_position[0] );
  voxel[1] = vtkRoundFuncMacro( ray_position[1] );
  voxel[2] = vtkRoundFuncMacro( ray_position[2] );

  // So far we haven't accumulated anything
  accum_red_intensity     = 0.0;
  accum_green_intensity   = 0.0;
  accum_blue_intensity    = 0.0;
  remaining_opacity       = 1.0;

  // Get a pointer to the gradient magnitudes for this volume
  if ( !grad_op_is_constant )
    {
    grad_mag_ptr = staticInfo->GradientMagnitudes;
    }

  
  // Keep track of previous voxel to know when we step into a new one
  // set it to something invalid to start with so that everything is
  // computed first time through
  prev_voxel[0] = voxel[0]-1;
  prev_voxel[1] = voxel[1]-1;
  prev_voxel[2] = voxel[2]-1;
  
  // Two cases - we are working with a gray or RGB transfer
  // function - break them up to make it more efficient
  if ( staticInfo->ColorChannels == 1 ) 
    {
    // For each step along the ray
    for ( loop = 0; 
          loop < num_steps && remaining_opacity > VTK_REMAINING_OPACITY; 
          loop++ )
      {     
      // We've taken another step
      steps_this_ray++;
      
      // Access the value at this voxel location
      if ( prev_voxel[0] != voxel[0] ||
           prev_voxel[1] != voxel[1] ||
           prev_voxel[2] != voxel[2] )
        {
        offset = voxel[2] * zinc + voxel[1] * yinc + voxel[0] * xinc;
        value = *(data_ptr + offset);
        opacity = SOTF[value];

        if ( opacity )
          {
          if ( grad_op_is_constant )
            {
            gradient_opacity = gradient_opacity_constant;
            }
          else 
            {
            gradient_opacity = GOTF[*(grad_mag_ptr + offset)];
            }
          opacity *= gradient_opacity;
          }

        prev_voxel[0] = voxel[0];
        prev_voxel[1] = voxel[1];
        prev_voxel[2] = voxel[2];
        }
        
      // Accumulate some light intensity and opacity
      //////////////////////////////////////////////////////////////////////////////////////////////
//       accum_red_intensity   += ( opacity * remaining_opacity * 
//                                  GTF[(value)] );
      udg::Vector3 vColor = aColorBleeding[offset];
      accum_red_intensity   += ( opacity * remaining_opacity * 
                                 GTF[(value)] ) * aObscuranceFactor * ( vColor.x + vColor.y + vColor.z ) / 3.0;
      //////////////////////////////////////////////////////////////////////////////////////////////
      remaining_opacity *= (1.0 - opacity);
      
      // Increment our position and compute our voxel location
      ray_position[0] += ray_increment[0];
      ray_position[1] += ray_increment[1];
      ray_position[2] += ray_increment[2];
      voxel[0] = vtkRoundFuncMacro( ray_position[0] );
      voxel[1] = vtkRoundFuncMacro( ray_position[1] );
      voxel[2] = vtkRoundFuncMacro( ray_position[2] );
      }
    accum_green_intensity = accum_red_intensity;
    accum_blue_intensity = accum_red_intensity;
    }
  else if ( staticInfo->ColorChannels == 3 )
    {
    // For each step along the ray
    for ( loop = 0; 
          loop < num_steps && remaining_opacity > VTK_REMAINING_OPACITY; 
          loop++ )
      {     
      // We've taken another step
      steps_this_ray++;
      
      // Access the value at this voxel location
      if ( prev_voxel[0] != voxel[0] ||
           prev_voxel[1] != voxel[1] ||
           prev_voxel[2] != voxel[2] )
        {
        offset = voxel[2] * zinc + voxel[1] * yinc + voxel[0] * xinc;
        value = *(data_ptr + offset);
        opacity = SOTF[value];

        if ( opacity )
          {
          if ( grad_op_is_constant )
            {
            gradient_opacity = gradient_opacity_constant;
            }
          else 
            {
            gradient_opacity = GOTF[*(grad_mag_ptr + offset)];  
            }
          opacity *= gradient_opacity;
          }

        prev_voxel[0] = voxel[0];
        prev_voxel[1] = voxel[1];
        prev_voxel[2] = voxel[2];
        }


      // Accumulate some light intensity and opacity
      //////////////////////////////////////////////////////////////////////////////////////////////
//       accum_red_intensity   += ( opacity * remaining_opacity * 
//                                  CTF[(value)*3] );
//       accum_green_intensity += ( opacity * remaining_opacity * 
//                                  CTF[(value)*3 + 1] );
//       accum_blue_intensity  += ( opacity * remaining_opacity * 
//                                  CTF[(value)*3 + 2] );
      udg::Vector3 vColor = aColorBleeding[offset];
      accum_red_intensity   += ( opacity * remaining_opacity * 
                                 CTF[(value)*3] ) * aObscuranceFactor * vColor.x;
      accum_green_intensity += ( opacity * remaining_opacity * 
                                 CTF[(value)*3 + 1] ) * aObscuranceFactor * vColor.y;
      accum_blue_intensity  += ( opacity * remaining_opacity * 
                                 CTF[(value)*3 + 2] ) * aObscuranceFactor * vColor.z;
      //////////////////////////////////////////////////////////////////////////////////////////////
      remaining_opacity *= (1.0 - opacity);
      
      // Increment our position and compute our voxel location
      ray_position[0] += ray_increment[0];
      ray_position[1] += ray_increment[1];
      ray_position[2] += ray_increment[2];
      voxel[0] = vtkRoundFuncMacro( ray_position[0] );
      voxel[1] = vtkRoundFuncMacro( ray_position[1] );
      voxel[2] = vtkRoundFuncMacro( ray_position[2] );
      }
    }

  // Cap the intensity value at 1.0
  if ( accum_red_intensity > 1.0 )
    {
    accum_red_intensity = 1.0;
    }
  if ( accum_green_intensity > 1.0 )
    {
    accum_green_intensity = 1.0;
    }
  if ( accum_blue_intensity > 1.0 )
    {
    accum_blue_intensity = 1.0;
    }
  
  if( remaining_opacity < VTK_REMAINING_OPACITY )
    {
    remaining_opacity = 0.0;
    }

  // Set the return pixel value.  The depth value is the distance to the
  // center of the volume.
  dynamicInfo->Color[0] = accum_red_intensity;
  dynamicInfo->Color[1] = accum_green_intensity;
  dynamicInfo->Color[2] = accum_blue_intensity;
  dynamicInfo->Color[3] = 1.0 - remaining_opacity;
  dynamicInfo->NumberOfStepsTaken = steps_this_ray;
  
}


// This is the templated function that actually casts a ray and computes
// the composite value. This version uses nearest neighbor and does
// perform shading.
template <class T>
void vtkCastRay_NN_Shaded( T *data_ptr, vtkVolumeRayCastDynamicInfo *dynamicInfo,
                           vtkVolumeRayCastStaticInfo *staticInfo,
                           double * aObscurance, double aObscuranceFactor, double aObscuranceFilterLow, double aObscuranceFilterHigh,
                           bool aFxObscurance, double aFxContour )
{
  int             value;
  unsigned char   *grad_mag_ptr = NULL;
  float           accum_red_intensity;
  float           accum_green_intensity;
  float           accum_blue_intensity;
  float           remaining_opacity;
  float           opacity = 0.0;
  float           gradient_opacity;
  int             loop;
  int             xinc, yinc, zinc;
  int             voxel[3];
  float           ray_position[3];
  int             prev_voxel[3];
  float           *SOTF;
  float           *CTF;
  float           *GTF;
  float           *GOTF;
  float           *red_d_shade, *green_d_shade, *blue_d_shade;
  float           *red_s_shade, *green_s_shade, *blue_s_shade;
  unsigned short  *encoded_normals;
  float           red_shaded_value   = 0.0;
  float           green_shaded_value = 0.0;
  float           blue_shaded_value  = 0.0;
  int             offset;
  int             steps_this_ray = 0;
  int             grad_op_is_constant;
  float           gradient_opacity_constant;
  int             num_steps;
  float           *ray_start, *ray_increment;

  num_steps = dynamicInfo->NumberOfStepsToTake;
  ray_start = dynamicInfo->TransformedStart;
  ray_increment = dynamicInfo->TransformedIncrement;
 
  // Get diffuse shading table pointers
  red_d_shade = staticInfo->RedDiffuseShadingTable;
  green_d_shade = staticInfo->GreenDiffuseShadingTable;
  blue_d_shade = staticInfo->BlueDiffuseShadingTable;

  // Get specular shading table pointers
  red_s_shade = staticInfo->RedSpecularShadingTable;
  green_s_shade = staticInfo->GreenSpecularShadingTable;
  blue_s_shade = staticInfo->BlueSpecularShadingTable;

  // Get a pointer to the encoded normals for this volume
  encoded_normals = staticInfo->EncodedNormals;

  // Get the scalar opacity transfer function for this volume (which maps
  // scalar input values to opacities)
  SOTF =  staticInfo->Volume->GetCorrectedScalarOpacityArray();

  // Get the color transfer function for this volume (which maps
  // scalar input values to RGB values)
  CTF =  staticInfo->Volume->GetRGBArray();
  GTF =  staticInfo->Volume->GetGrayArray();

  // Get the gradient opacity transfer function for this volume (which maps
  // gradient magnitudes to opacities)
  GOTF =  staticInfo->Volume->GetGradientOpacityArray();

  // Get the gradient opacity constant. If this number is greater than
  // or equal to 0.0, then the gradient opacity transfer function is
  // a constant at that value, otherwise it is not a constant function
  gradient_opacity_constant = staticInfo->Volume->GetGradientOpacityConstant();
  grad_op_is_constant = ( gradient_opacity_constant >= 0.0 );

  // Get a pointer to the gradient magnitudes for this volume
  if ( !grad_op_is_constant )
    {
    grad_mag_ptr = staticInfo->GradientMagnitudes;
    }

  // Move the increments into local variables
  xinc = staticInfo->DataIncrement[0];
  yinc = staticInfo->DataIncrement[1];
  zinc = staticInfo->DataIncrement[2];

  // Initialize the ray position and voxel location
  ray_position[0] = ray_start[0];
  ray_position[1] = ray_start[1];
  ray_position[2] = ray_start[2];

  voxel[0] = vtkRoundFuncMacro( ray_position[0] );
  voxel[1] = vtkRoundFuncMacro( ray_position[1] );
  voxel[2] = vtkRoundFuncMacro( ray_position[2] );

  // So far we haven't accumulated anything
  accum_red_intensity     = 0.0;
  accum_green_intensity   = 0.0;
  accum_blue_intensity    = 0.0;
  remaining_opacity       = 1.0;

  // Keep track of previous voxel to know when we step into a new one  
  prev_voxel[0] = voxel[0]-1;
  prev_voxel[1] = voxel[1]-1;
  prev_voxel[2] = voxel[2]-1;

  //////////////////////////////////////////////////////////////////////////////////////////////////
  udg::Vector3 fxDirection( dynamicInfo->TransformedDirection[0], dynamicInfo->TransformedDirection[1], dynamicInfo->TransformedDirection[2] );
  fxDirection.normalize();
  vtkVolumeRayCastMapper * fxMapper = vtkVolumeRayCastMapper::SafeDownCast( staticInfo->Volume->GetMapper() );
  vtkEncodedGradientEstimator * fxGradientEstimator = fxMapper->GetGradientEstimator();
  unsigned short * fxEncodedNormals = fxGradientEstimator->GetEncodedNormals();
  vtkDirectionEncoder * fxDirectionEncoder = fxGradientEstimator->GetDirectionEncoder();
  //////////////////////////////////////////////////////////////////////////////////////////////////

  // Two cases - we are working with a gray or RGB transfer
  // function - break them up to make it more efficient
  if ( staticInfo->ColorChannels == 1 ) 
    {
    // For each step along the ray
    for ( loop = 0; 
          loop < num_steps && remaining_opacity > VTK_REMAINING_OPACITY; 
          loop++ )
      {     
      // We've taken another step
      steps_this_ray++;
      
      // Access the value at this voxel location and compute
      // opacity and shaded value
      if ( prev_voxel[0] != voxel[0] ||
           prev_voxel[1] != voxel[1] ||
           prev_voxel[2] != voxel[2] )
        {
        offset = voxel[2] * zinc + voxel[1] * yinc + voxel[0] * xinc;
        value = *(data_ptr + offset);
      
        // Get the opacity contributed by the scalar opacity transfer function
        opacity = SOTF[value];

        // Multiply by the opacity contributed by the gradient magnitude
        // transfer function (don't both if opacity is already 0)
        if ( opacity )
          {
          if ( grad_op_is_constant )
            {
            gradient_opacity = gradient_opacity_constant;
            }
          else 
            {
            gradient_opacity = GOTF[*(grad_mag_ptr + offset)];
            }
          
          opacity *= gradient_opacity;
          
          }

        // Compute the red shaded value (only if there is some opacity)
        // This is grey-scale so green and blue are the same as red
        if ( opacity )
          {
          red_shaded_value = opacity * remaining_opacity *
            ( red_d_shade[*(encoded_normals + offset)] * GTF[value] +
              red_s_shade[*(encoded_normals + offset)] );
          }
        else
          {
          red_shaded_value = 0.0;
          }

        prev_voxel[0] = voxel[0];
        prev_voxel[1] = voxel[1];
        prev_voxel[2] = voxel[2];
        }
    
    
      // Accumulate the shaded intensity and opacity of this sample
      //////////////////////////////////////////////////////////////////////////////////////////////
//       accum_red_intensity += red_shaded_value;

      double fx = 1.0;

      // contour
      if ( aFxContour > 0.0 )
      {
        float * fxGradient = fxDirectionEncoder->GetDecodedGradient( fxEncodedNormals[offset] );
        udg::Vector3 fxNormal( fxGradient[0], fxGradient[1], fxGradient[2] );
        double fxDotProduct = fxDirection * fxNormal;
        if ( fxDotProduct < 0.0 ) fxDotProduct = -fxDotProduct;
        fx *= fxDotProduct < aFxContour ? 0.0 : 1.0;
      }

      // obscurance
      if ( aFxObscurance && fx > 0.0 )
      {
        double fxObscurance = aObscurance[offset];
        if ( fxObscurance < aObscuranceFilterLow ) fxObscurance = 0.0;
        else if ( fxObscurance > aObscuranceFilterHigh ) fxObscurance = 1.0;
        fx *= aObscuranceFactor * fxObscurance;
      }

      accum_red_intensity += red_shaded_value * fx;
      //////////////////////////////////////////////////////////////////////////////////////////////
      remaining_opacity *= (1.0 - opacity);
    
      // Increment our position and compute our voxel location
      ray_position[0] += ray_increment[0];
      ray_position[1] += ray_increment[1];
      ray_position[2] += ray_increment[2];
      voxel[0] = vtkRoundFuncMacro( ray_position[0] );
      voxel[1] = vtkRoundFuncMacro( ray_position[1] );
      voxel[2] = vtkRoundFuncMacro( ray_position[2] );
      }
    accum_green_intensity = accum_red_intensity;
    accum_blue_intensity = accum_red_intensity;
    }
  else if ( staticInfo->ColorChannels == 3 )
    {
    // For each step along the ray
    for ( loop = 0; 
          loop < num_steps && remaining_opacity > VTK_REMAINING_OPACITY; loop++ )
      {     
      // We've taken another step
      steps_this_ray++;
      
      // Access the value at this voxel location and compute
      // opacity and shaded value
      if ( prev_voxel[0] != voxel[0] ||
           prev_voxel[1] != voxel[1] ||
           prev_voxel[2] != voxel[2] )
        {
        offset = voxel[2] * zinc + voxel[1] * yinc + voxel[0] * xinc;
        value = *(data_ptr + offset);
      
        // Get the opacity contributed by the scalar opacity transfer function
        opacity = SOTF[value];

        // Multiply by the opacity contributed by the gradient magnitude
        // transfer function (don't both if opacity is already 0)
        if ( opacity )
          {
          if ( grad_op_is_constant )
            {
            gradient_opacity = gradient_opacity_constant;
            }
          else 
            {
            gradient_opacity = GOTF[*(grad_mag_ptr + offset)];
            }
          
          opacity *= gradient_opacity;    
          }     

        // Compute the red, green, and blue shaded value (only if there
        // is some opacity)
        if ( opacity )
          {
          red_shaded_value = opacity *  remaining_opacity *
            ( red_d_shade[*(encoded_normals + offset)] * CTF[value*3] +
              red_s_shade[*(encoded_normals + offset)] );
          green_shaded_value = opacity *  remaining_opacity *
            ( green_d_shade[*(encoded_normals + offset)] * CTF[value*3 + 1] +
              green_s_shade[*(encoded_normals + offset)] );
          blue_shaded_value = opacity *  remaining_opacity *
            ( blue_d_shade[*(encoded_normals + offset)] * CTF[value*3 + 2] +
              blue_s_shade[*(encoded_normals + offset)] );
          }
        else
          {
          red_shaded_value = 0.0;
          green_shaded_value = 0.0;
          blue_shaded_value = 0.0;
          }

        prev_voxel[0] = voxel[0];
        prev_voxel[1] = voxel[1];
        prev_voxel[2] = voxel[2];
        }
    
    
      // Accumulate the shaded intensity and opacity of this sample
      //////////////////////////////////////////////////////////////////////////////////////////////
//       accum_red_intensity += red_shaded_value;
//       accum_green_intensity += green_shaded_value;
//       accum_blue_intensity += blue_shaded_value;

      double fx = 1.0;

      // contour
      if ( aFxContour > 0.0 )
      {
        float * fxGradient = fxDirectionEncoder->GetDecodedGradient( fxEncodedNormals[offset] );
        udg::Vector3 fxNormal( fxGradient[0], fxGradient[1], fxGradient[2] );
        double fxDotProduct = fxDirection * fxNormal;
        if ( fxDotProduct < 0.0 ) fxDotProduct = -fxDotProduct;
        fx *= fxDotProduct < aFxContour ? 0.0 : 1.0;
      }

      // obscurance
      if ( aFxObscurance && fx > 0.0 )
      {
        double fxObscurance = aObscurance[offset];
        if ( fxObscurance < aObscuranceFilterLow ) fxObscurance = 0.0;
        else if ( fxObscurance > aObscuranceFilterHigh ) fxObscurance = 1.0;
        fx *= aObscuranceFactor * fxObscurance;
      }

      accum_red_intensity += red_shaded_value * fx;
      accum_green_intensity += green_shaded_value * fx;
      accum_blue_intensity += blue_shaded_value * fx;

      /*
      double shade;
      if (dotProduct < 0.5) shade = 0.5;
      else shade = 1.0;

      accum_red_intensity += opacity * remaining_opacity * (CTF[value*3] < 0.3 ? 0.0 : (CTF[value*3] < 0.7 ? 0.5 : 1.0)) * shade * factor;
      accum_green_intensity += opacity * remaining_opacity * (CTF[value*3+1] < 0.3 ? 0.0 : (CTF[value*3+1] < 0.7 ? 0.5 : 1.0)) * shade * factor;
      accum_blue_intensity += opacity * remaining_opacity * (CTF[value*3+2] < 0.3 ? 0.0 : (CTF[value*3+2] < 0.7 ? 0.5 : 1.0)) * shade * factor;
      */
      //////////////////////////////////////////////////////////////////////////////////////////////
      remaining_opacity *= (1.0 - opacity);
    
      // Increment our position and compute our voxel location
      ray_position[0] += ray_increment[0];
      ray_position[1] += ray_increment[1];
      ray_position[2] += ray_increment[2];
      voxel[0] = vtkRoundFuncMacro( ray_position[0] );
      voxel[1] = vtkRoundFuncMacro( ray_position[1] );
      voxel[2] = vtkRoundFuncMacro( ray_position[2] );
      }
    }
  
  // Cap the intensities at 1.0
  if ( accum_red_intensity > 1.0 )
    {
    accum_red_intensity = 1.0;
    }
  if ( accum_green_intensity > 1.0 )
    {
    accum_green_intensity = 1.0;
    }
  if ( accum_blue_intensity > 1.0 )
    {
    accum_blue_intensity = 1.0;
    }
  
  if( remaining_opacity < VTK_REMAINING_OPACITY )
    {
    remaining_opacity = 0.0;
    }
  
  // Set the return pixel value.  The depth value is the distance to the
  // center of the volume.
  dynamicInfo->Color[0] = accum_red_intensity;
  dynamicInfo->Color[1] = accum_green_intensity;
  dynamicInfo->Color[2] = accum_blue_intensity;
  dynamicInfo->Color[3] = 1.0 - remaining_opacity;
  dynamicInfo->NumberOfStepsTaken = steps_this_ray;
  
}


// This is the templated function that actually casts a ray and computes
// the composite value. This version uses nearest neighbor and does
// perform shading.
template <class T>
void vtkCastRay_NN_Shaded( T *data_ptr, vtkVolumeRayCastDynamicInfo *dynamicInfo,
                           vtkVolumeRayCastStaticInfo *staticInfo, udg::Vector3 * aColorBleeding, double aObscuranceFactor )
{
  int             value;
  unsigned char   *grad_mag_ptr = NULL;
  float           accum_red_intensity;
  float           accum_green_intensity;
  float           accum_blue_intensity;
  float           remaining_opacity;
  float           opacity = 0.0;
  float           gradient_opacity;
  int             loop;
  int             xinc, yinc, zinc;
  int             voxel[3];
  float           ray_position[3];
  int             prev_voxel[3];
  float           *SOTF;
  float           *CTF;
  float           *GTF;
  float           *GOTF;
  float           *red_d_shade, *green_d_shade, *blue_d_shade;
  float           *red_s_shade, *green_s_shade, *blue_s_shade;
  unsigned short  *encoded_normals;
  float           red_shaded_value   = 0.0;
  float           green_shaded_value = 0.0;
  float           blue_shaded_value  = 0.0;
  int             offset;
  int             steps_this_ray = 0;
  int             grad_op_is_constant;
  float           gradient_opacity_constant;
  int             num_steps;
  float           *ray_start, *ray_increment;

  num_steps = dynamicInfo->NumberOfStepsToTake;
  ray_start = dynamicInfo->TransformedStart;
  ray_increment = dynamicInfo->TransformedIncrement;
 
  // Get diffuse shading table pointers
  red_d_shade = staticInfo->RedDiffuseShadingTable;
  green_d_shade = staticInfo->GreenDiffuseShadingTable;
  blue_d_shade = staticInfo->BlueDiffuseShadingTable;

  // Get specular shading table pointers
  red_s_shade = staticInfo->RedSpecularShadingTable;
  green_s_shade = staticInfo->GreenSpecularShadingTable;
  blue_s_shade = staticInfo->BlueSpecularShadingTable;

  // Get a pointer to the encoded normals for this volume
  encoded_normals = staticInfo->EncodedNormals;

  // Get the scalar opacity transfer function for this volume (which maps
  // scalar input values to opacities)
  SOTF =  staticInfo->Volume->GetCorrectedScalarOpacityArray();

  // Get the color transfer function for this volume (which maps
  // scalar input values to RGB values)
  CTF =  staticInfo->Volume->GetRGBArray();
  GTF =  staticInfo->Volume->GetGrayArray();

  // Get the gradient opacity transfer function for this volume (which maps
  // gradient magnitudes to opacities)
  GOTF =  staticInfo->Volume->GetGradientOpacityArray();

  // Get the gradient opacity constant. If this number is greater than
  // or equal to 0.0, then the gradient opacity transfer function is
  // a constant at that value, otherwise it is not a constant function
  gradient_opacity_constant = staticInfo->Volume->GetGradientOpacityConstant();
  grad_op_is_constant = ( gradient_opacity_constant >= 0.0 );

  // Get a pointer to the gradient magnitudes for this volume
  if ( !grad_op_is_constant )
    {
    grad_mag_ptr = staticInfo->GradientMagnitudes;
    }

  // Move the increments into local variables
  xinc = staticInfo->DataIncrement[0];
  yinc = staticInfo->DataIncrement[1];
  zinc = staticInfo->DataIncrement[2];

  // Initialize the ray position and voxel location
  ray_position[0] = ray_start[0];
  ray_position[1] = ray_start[1];
  ray_position[2] = ray_start[2];

  voxel[0] = vtkRoundFuncMacro( ray_position[0] );
  voxel[1] = vtkRoundFuncMacro( ray_position[1] );
  voxel[2] = vtkRoundFuncMacro( ray_position[2] );

  // So far we haven't accumulated anything
  accum_red_intensity     = 0.0;
  accum_green_intensity   = 0.0;
  accum_blue_intensity    = 0.0;
  remaining_opacity       = 1.0;

  // Keep track of previous voxel to know when we step into a new one  
  prev_voxel[0] = voxel[0]-1;
  prev_voxel[1] = voxel[1]-1;
  prev_voxel[2] = voxel[2]-1;
  
  // Two cases - we are working with a gray or RGB transfer
  // function - break them up to make it more efficient
  if ( staticInfo->ColorChannels == 1 ) 
    {
    // For each step along the ray
    for ( loop = 0; 
          loop < num_steps && remaining_opacity > VTK_REMAINING_OPACITY; 
          loop++ )
      {     
      // We've taken another step
      steps_this_ray++;
      
      // Access the value at this voxel location and compute
      // opacity and shaded value
      if ( prev_voxel[0] != voxel[0] ||
           prev_voxel[1] != voxel[1] ||
           prev_voxel[2] != voxel[2] )
        {
        offset = voxel[2] * zinc + voxel[1] * yinc + voxel[0] * xinc;
        value = *(data_ptr + offset);
      
        // Get the opacity contributed by the scalar opacity transfer function
        opacity = SOTF[value];

        // Multiply by the opacity contributed by the gradient magnitude
        // transfer function (don't both if opacity is already 0)
        if ( opacity )
          {
          if ( grad_op_is_constant )
            {
            gradient_opacity = gradient_opacity_constant;
            }
          else 
            {
            gradient_opacity = GOTF[*(grad_mag_ptr + offset)];
            }
          
          opacity *= gradient_opacity;
          
          }

        // Compute the red shaded value (only if there is some opacity)
        // This is grey-scale so green and blue are the same as red
        if ( opacity )
          {
          red_shaded_value = opacity * remaining_opacity *
            ( red_d_shade[*(encoded_normals + offset)] * GTF[value] +
              red_s_shade[*(encoded_normals + offset)] );
          }
        else
          {
          red_shaded_value = 0.0;
          }

        prev_voxel[0] = voxel[0];
        prev_voxel[1] = voxel[1];
        prev_voxel[2] = voxel[2];
        }
    
    
      // Accumulate the shaded intensity and opacity of this sample
      //////////////////////////////////////////////////////////////////////////////////////////////
//       accum_red_intensity += red_shaded_value;
      udg::Vector3 vColor = aColorBleeding[offset];
      accum_red_intensity += red_shaded_value * aObscuranceFactor * ( vColor.x + vColor.y + vColor.z ) / 3.0;
      //////////////////////////////////////////////////////////////////////////////////////////////
      remaining_opacity *= (1.0 - opacity);
    
      // Increment our position and compute our voxel location
      ray_position[0] += ray_increment[0];
      ray_position[1] += ray_increment[1];
      ray_position[2] += ray_increment[2];
      voxel[0] = vtkRoundFuncMacro( ray_position[0] );
      voxel[1] = vtkRoundFuncMacro( ray_position[1] );
      voxel[2] = vtkRoundFuncMacro( ray_position[2] );
      }
    accum_green_intensity = accum_red_intensity;
    accum_blue_intensity = accum_red_intensity;
    }
  else if ( staticInfo->ColorChannels == 3 )
    {
    // For each step along the ray
    for ( loop = 0; 
          loop < num_steps && remaining_opacity > VTK_REMAINING_OPACITY; loop++ )
      {     
      // We've taken another step
      steps_this_ray++;
      
      // Access the value at this voxel location and compute
      // opacity and shaded value
      if ( prev_voxel[0] != voxel[0] ||
           prev_voxel[1] != voxel[1] ||
           prev_voxel[2] != voxel[2] )
        {
        offset = voxel[2] * zinc + voxel[1] * yinc + voxel[0] * xinc;
        value = *(data_ptr + offset);
      
        // Get the opacity contributed by the scalar opacity transfer function
        opacity = SOTF[value];

        // Multiply by the opacity contributed by the gradient magnitude
        // transfer function (don't both if opacity is already 0)
        if ( opacity )
          {
          if ( grad_op_is_constant )
            {
            gradient_opacity = gradient_opacity_constant;
            }
          else 
            {
            gradient_opacity = GOTF[*(grad_mag_ptr + offset)];
            }
          
          opacity *= gradient_opacity;    
          }     

        // Compute the red, green, and blue shaded value (only if there
        // is some opacity)
        if ( opacity )
          {
          red_shaded_value = opacity *  remaining_opacity *
            ( red_d_shade[*(encoded_normals + offset)] * CTF[value*3] +
              red_s_shade[*(encoded_normals + offset)] );
          green_shaded_value = opacity *  remaining_opacity *
            ( green_d_shade[*(encoded_normals + offset)] * CTF[value*3 + 1] +
              green_s_shade[*(encoded_normals + offset)] );
          blue_shaded_value = opacity *  remaining_opacity *
            ( blue_d_shade[*(encoded_normals + offset)] * CTF[value*3 + 2] +
              blue_s_shade[*(encoded_normals + offset)] );
          }
        else
          {
          red_shaded_value = 0.0;
          green_shaded_value = 0.0;
          blue_shaded_value = 0.0;
          }

        prev_voxel[0] = voxel[0];
        prev_voxel[1] = voxel[1];
        prev_voxel[2] = voxel[2];
        }
    
    
      // Accumulate the shaded intensity and opacity of this sample
      //////////////////////////////////////////////////////////////////////////////////////////////
//       accum_red_intensity += red_shaded_value;
//       accum_green_intensity += green_shaded_value;
//       accum_blue_intensity += blue_shaded_value;
      udg::Vector3 vColor = aColorBleeding[offset];
      accum_red_intensity += red_shaded_value * aObscuranceFactor * vColor.x;
      accum_green_intensity += green_shaded_value * aObscuranceFactor * vColor.y;
      accum_blue_intensity += blue_shaded_value * aObscuranceFactor * vColor.z;
      //////////////////////////////////////////////////////////////////////////////////////////////
      remaining_opacity *= (1.0 - opacity);
    
      // Increment our position and compute our voxel location
      ray_position[0] += ray_increment[0];
      ray_position[1] += ray_increment[1];
      ray_position[2] += ray_increment[2];
      voxel[0] = vtkRoundFuncMacro( ray_position[0] );
      voxel[1] = vtkRoundFuncMacro( ray_position[1] );
      voxel[2] = vtkRoundFuncMacro( ray_position[2] );
      }
    }
  
  // Cap the intensities at 1.0
  if ( accum_red_intensity > 1.0 )
    {
    accum_red_intensity = 1.0;
    }
  if ( accum_green_intensity > 1.0 )
    {
    accum_green_intensity = 1.0;
    }
  if ( accum_blue_intensity > 1.0 )
    {
    accum_blue_intensity = 1.0;
    }
  
  if( remaining_opacity < VTK_REMAINING_OPACITY )
    {
    remaining_opacity = 0.0;
    }
  
  // Set the return pixel value.  The depth value is the distance to the
  // center of the volume.
  dynamicInfo->Color[0] = accum_red_intensity;
  dynamicInfo->Color[1] = accum_green_intensity;
  dynamicInfo->Color[2] = accum_blue_intensity;
  dynamicInfo->Color[3] = 1.0 - remaining_opacity;
  dynamicInfo->NumberOfStepsTaken = steps_this_ray;
  
}


// This is the templated function that actually casts a ray and computes
// the composite value.  This version uses trilinear interpolation and
// does not compute shading
template <class T>
void vtkCastRay_TrilinSample_Unshaded( T *data_ptr, vtkVolumeRayCastDynamicInfo *dynamicInfo,
                                       vtkVolumeRayCastStaticInfo *staticInfo,
                                       double * aObscurance, double aObscuranceFactor, double aObscuranceFilterLow, double aObscuranceFilterHigh,
                                       bool aFxObscurance, double aFxContour )
{
  unsigned char   *grad_mag_ptr = NULL;
  unsigned char   *gmptr;
  float           accum_red_intensity;
  float           accum_green_intensity;
  float           accum_blue_intensity;
  float           remaining_opacity;
  float           red_value, green_value, blue_value;
  float           opacity;
  int             loop;
  int             xinc, yinc, zinc;
  int             voxel[3];
  float           ray_position[3];
  float           A, B, C, D, E, F, G, H;
  int             Binc, Cinc, Dinc, Einc, Finc, Ginc, Hinc;
  T               *dptr;
  float           *SOTF;
  float           *CTF;
  float           *GTF;
  float           *GOTF;
  float           x, y, z, t1, t2, t3;
  int             offset;
  int             steps_this_ray = 0;
  float           gradient_value;
  float           scalar_value;
  int             grad_op_is_constant;
  float           gradient_opacity_constant;
  int             num_steps;
  float           *ray_start, *ray_increment;

  num_steps = dynamicInfo->NumberOfStepsToTake;
  ray_start = dynamicInfo->TransformedStart;
  ray_increment = dynamicInfo->TransformedIncrement;

  // Get the scalar opacity transfer function which maps scalar input values
  // to opacities
  SOTF =  staticInfo->Volume->GetCorrectedScalarOpacityArray();

  // Get the color transfer function which maps scalar input values
  // to RGB colors
  CTF =  staticInfo->Volume->GetRGBArray();
  GTF =  staticInfo->Volume->GetGrayArray();

  // Get the gradient opacity transfer function for this volume (which maps
  // gradient magnitudes to opacities)
  GOTF =  staticInfo->Volume->GetGradientOpacityArray();

  // Get the gradient opacity constant. If this number is greater than
  // or equal to 0.0, then the gradient opacity transfer function is
  // a constant at that value, otherwise it is not a constant function
  gradient_opacity_constant = staticInfo->Volume->GetGradientOpacityConstant();
  grad_op_is_constant = ( gradient_opacity_constant >= 0.0 );

  // Get a pointer to the gradient magnitudes for this volume
  if ( !grad_op_is_constant )
    {
    grad_mag_ptr = staticInfo->GradientMagnitudes;
    }

  // Move the increments into local variables
  xinc = staticInfo->DataIncrement[0];
  yinc = staticInfo->DataIncrement[1];
  zinc = staticInfo->DataIncrement[2];

  // Initialize the ray position and voxel location
  ray_position[0] = ray_start[0];
  ray_position[1] = ray_start[1];
  ray_position[2] = ray_start[2];
  voxel[0] = vtkFloorFuncMacro( ray_position[0] );
  voxel[1] = vtkFloorFuncMacro( ray_position[1] );
  voxel[2] = vtkFloorFuncMacro( ray_position[2] );

  // So far we have not accumulated anything
  accum_red_intensity     = 0.0;
  accum_green_intensity   = 0.0;
  accum_blue_intensity    = 0.0;
  remaining_opacity       = 1.0;

  // Compute the increments to get to the other 7 voxel vertices from A
  Binc = xinc;
  Cinc = yinc;
  Dinc = xinc + yinc;
  Einc = zinc;
  Finc = zinc + xinc;
  Ginc = zinc + yinc;
  Hinc = zinc + xinc + yinc;

  //////////////////////////////////////////////////////////////////////////////////////////////////
  udg::Vector3 fxDirection( dynamicInfo->TransformedDirection[0], dynamicInfo->TransformedDirection[1], dynamicInfo->TransformedDirection[2] );
  fxDirection.normalize();
  vtkVolumeRayCastMapper * fxMapper = vtkVolumeRayCastMapper::SafeDownCast( staticInfo->Volume->GetMapper() );
  vtkEncodedGradientEstimator * fxGradientEstimator = fxMapper->GetGradientEstimator();
  unsigned short * fxEncodedNormals = fxGradientEstimator->GetEncodedNormals();
  vtkDirectionEncoder * fxDirectionEncoder = fxGradientEstimator->GetDirectionEncoder();
  //////////////////////////////////////////////////////////////////////////////////////////////////

  // Two cases - we are working with a gray or RGB transfer
  // function - break them up to make it more efficient
  if ( staticInfo->ColorChannels == 1 ) 
    {
    // For each step along the ray
    for ( loop = 0; 
          loop < num_steps && remaining_opacity > VTK_REMAINING_OPACITY; 
          loop++ )
      {     
      // We've taken another step
      steps_this_ray++;
      
      offset = voxel[2] * zinc + voxel[1] * yinc + voxel[0];
      dptr = data_ptr + offset;
      
      A = *(dptr);
      B = *(dptr + Binc);
      C = *(dptr + Cinc);
      D = *(dptr + Dinc);
      E = *(dptr + Einc);
      F = *(dptr + Finc);
      G = *(dptr + Ginc);
      H = *(dptr + Hinc);
      
      // Compute our offset in the voxel, and use that to trilinearly
      // interpolate the value
      x = ray_position[0] - (float) voxel[0];
      y = ray_position[1] - (float) voxel[1];
      z = ray_position[2] - (float) voxel[2];
      
      t1 = 1.0 - x;
      t2 = 1.0 - y;
      t3 = 1.0 - z;
      
      scalar_value = 
        A * t1 * t2 * t3 +
        B *  x * t2 * t3 +
        C * t1 *  y * t3 + 
        D *  x *  y * t3 +
        E * t1 * t2 *  z + 
        F *  x * t2 *  z + 
        G * t1 *  y *  z + 
        H *  x *  y *  z;

      //////////////////////////////////////////////////////////////////////////////////////////////
      double tA = t1 * t2 * t3;
      double tB =  x * t2 * t3;
      double tC = t1 *  y * t3;
      double tD =  x *  y * t3;
      double tE = t1 * t2 *  z;
      double tF =  x * t2 *  z;
      double tG = t1 *  y *  z;
      double tH =  x *  y *  z;

      double fx = 1.0;

      // contour
      if ( aFxContour > 0.0 )
      {
        float * fxGradientA = fxDirectionEncoder->GetDecodedGradient( fxEncodedNormals[offset] );
        float * fxGradientB = fxDirectionEncoder->GetDecodedGradient( fxEncodedNormals[offset + Binc] );
        float * fxGradientC = fxDirectionEncoder->GetDecodedGradient( fxEncodedNormals[offset + Cinc] );
        float * fxGradientD = fxDirectionEncoder->GetDecodedGradient( fxEncodedNormals[offset + Dinc] );
        float * fxGradientE = fxDirectionEncoder->GetDecodedGradient( fxEncodedNormals[offset + Einc] );
        float * fxGradientF = fxDirectionEncoder->GetDecodedGradient( fxEncodedNormals[offset + Finc] );
        float * fxGradientG = fxDirectionEncoder->GetDecodedGradient( fxEncodedNormals[offset + Ginc] );
        float * fxGradientH = fxDirectionEncoder->GetDecodedGradient( fxEncodedNormals[offset + Hinc] );
        udg::Vector3 fxNormalA( fxGradientA[0], fxGradientA[1], fxGradientA[2] );
        udg::Vector3 fxNormalB( fxGradientB[0], fxGradientB[1], fxGradientB[2] );
        udg::Vector3 fxNormalC( fxGradientC[0], fxGradientC[1], fxGradientC[2] );
        udg::Vector3 fxNormalD( fxGradientD[0], fxGradientD[1], fxGradientD[2] );
        udg::Vector3 fxNormalE( fxGradientE[0], fxGradientE[1], fxGradientE[2] );
        udg::Vector3 fxNormalF( fxGradientF[0], fxGradientF[1], fxGradientF[2] );
        udg::Vector3 fxNormalG( fxGradientG[0], fxGradientG[1], fxGradientG[2] );
        udg::Vector3 fxNormalH( fxGradientH[0], fxGradientH[1], fxGradientH[2] );
        udg::Vector3 fxNormal = fxNormalA * tA
                              + fxNormalB * tB
                              + fxNormalC * tC
                              + fxNormalD * tD
                              + fxNormalE * tE
                              + fxNormalF * tF
                              + fxNormalG * tG
                              + fxNormalH * tH;
        double fxDotProduct = fxDirection * fxNormal;
        if ( fxDotProduct < 0.0 ) fxDotProduct = -fxDotProduct;
        fx *= fxDotProduct < aFxContour ? 0.0 : 1.0;
      }

      // obscurance
      if ( aFxObscurance && fx > 0.0 )
      {
        double fxObscurance = aObscurance[offset       ] * tA
                            + aObscurance[offset + Binc] * tB
                            + aObscurance[offset + Cinc] * tC
                            + aObscurance[offset + Dinc] * tD
                            + aObscurance[offset + Einc] * tE
                            + aObscurance[offset + Finc] * tF
                            + aObscurance[offset + Ginc] * tG
                            + aObscurance[offset + Hinc] * tH;
        if ( fxObscurance < aObscuranceFilterLow ) fxObscurance = 0.0;
        else if ( fxObscurance > aObscuranceFilterHigh ) fxObscurance = 1.0;
        fx *= aObscuranceFactor * fxObscurance;
      }
      //////////////////////////////////////////////////////////////////////////////////////////////
      
      if ( scalar_value < 0.0 ) 
        {
        scalar_value = 0.0;
        }
      else if ( scalar_value > staticInfo->Volume->GetArraySize() - 1 )
        {
        scalar_value = staticInfo->Volume->GetArraySize() - 1;
        }
      
      opacity = SOTF[(int)(scalar_value)];
      
      if ( opacity )
        {
        if ( !grad_op_is_constant )
          {
          gmptr = grad_mag_ptr + offset;
      
          A = *(gmptr);
          B = *(gmptr + Binc);
          C = *(gmptr + Cinc);
          D = *(gmptr + Dinc);
          E = *(gmptr + Einc);
          F = *(gmptr + Finc);
          G = *(gmptr + Ginc);
          H = *(gmptr + Hinc);
          
          gradient_value = 
            A * t1 * t2 * t3 +
            B *  x * t2 * t3 +
            C * t1 *  y * t3 + 
            D *  x *  y * t3 +
            E * t1 * t2 *  z + 
            F *  x * t2 *  z + 
            G * t1 *  y *  z + 
            H *  x *  y *  z;
      
          if ( gradient_value < 0.0 )
            {
            gradient_value = 0.0;
            }
          else if ( gradient_value > 255.0 )
            {
            gradient_value = 255.0;
            }

          opacity *= GOTF[(int)(gradient_value)];
          }
        else
          {
          opacity *= gradient_opacity_constant;
          }
        red_value   = opacity * GTF[(int)(scalar_value)];
        
        // Accumulate intensity and opacity for this sample location
        ////////////////////////////////////////////////////////////////////////////////////////////
//         accum_red_intensity   += remaining_opacity * red_value;
        accum_red_intensity   += remaining_opacity * red_value * fx;
        ////////////////////////////////////////////////////////////////////////////////////////////
        remaining_opacity *= (1.0 - opacity);
        }
    
      // Increment our position and compute our voxel location
      ray_position[0] += ray_increment[0];
      ray_position[1] += ray_increment[1];
      ray_position[2] += ray_increment[2];      
      voxel[0] = vtkFloorFuncMacro( ray_position[0] );
      voxel[1] = vtkFloorFuncMacro( ray_position[1] );
      voxel[2] = vtkFloorFuncMacro( ray_position[2] );
      }
    accum_green_intensity = accum_red_intensity;
    accum_blue_intensity = accum_red_intensity;
    }
  else if ( staticInfo->ColorChannels == 3 )
    {
    // For each step along the ray
    for ( loop = 0; 
          loop < num_steps && remaining_opacity > VTK_REMAINING_OPACITY; 
          loop++ )
      {     
      // We've taken another step
      steps_this_ray++;
      
      offset = voxel[2] * zinc + voxel[1] * yinc + voxel[0];
      dptr = data_ptr + offset;
      
      A = *(dptr);
      B = *(dptr + Binc);
      C = *(dptr + Cinc);
      D = *(dptr + Dinc);
      E = *(dptr + Einc);
      F = *(dptr + Finc);
      G = *(dptr + Ginc);
      H = *(dptr + Hinc);
      
      // Compute our offset in the voxel, and use that to trilinearly
      // interpolate the value
      x = ray_position[0] - (float) voxel[0];
      y = ray_position[1] - (float) voxel[1];
      z = ray_position[2] - (float) voxel[2];
      
      t1 = 1.0 - x;
      t2 = 1.0 - y;
      t3 = 1.0 - z;
      
      scalar_value = 
        A * t1 * t2 * t3 +
        B *  x * t2 * t3 +
        C * t1 *  y * t3 + 
        D *  x *  y * t3 +
        E * t1 * t2 *  z + 
        F *  x * t2 *  z + 
        G * t1 *  y *  z + 
        H *  x *  y *  z;

      //////////////////////////////////////////////////////////////////////////////////////////////
      double tA = t1 * t2 * t3;
      double tB =  x * t2 * t3;
      double tC = t1 *  y * t3;
      double tD =  x *  y * t3;
      double tE = t1 * t2 *  z;
      double tF =  x * t2 *  z;
      double tG = t1 *  y *  z;
      double tH =  x *  y *  z;

      double fx = 1.0;

      // contour
      if ( aFxContour > 0.0 )
      {
        float * fxGradientA = fxDirectionEncoder->GetDecodedGradient( fxEncodedNormals[offset] );
        float * fxGradientB = fxDirectionEncoder->GetDecodedGradient( fxEncodedNormals[offset + Binc] );
        float * fxGradientC = fxDirectionEncoder->GetDecodedGradient( fxEncodedNormals[offset + Cinc] );
        float * fxGradientD = fxDirectionEncoder->GetDecodedGradient( fxEncodedNormals[offset + Dinc] );
        float * fxGradientE = fxDirectionEncoder->GetDecodedGradient( fxEncodedNormals[offset + Einc] );
        float * fxGradientF = fxDirectionEncoder->GetDecodedGradient( fxEncodedNormals[offset + Finc] );
        float * fxGradientG = fxDirectionEncoder->GetDecodedGradient( fxEncodedNormals[offset + Ginc] );
        float * fxGradientH = fxDirectionEncoder->GetDecodedGradient( fxEncodedNormals[offset + Hinc] );
        udg::Vector3 fxNormalA( fxGradientA[0], fxGradientA[1], fxGradientA[2] );
        udg::Vector3 fxNormalB( fxGradientB[0], fxGradientB[1], fxGradientB[2] );
        udg::Vector3 fxNormalC( fxGradientC[0], fxGradientC[1], fxGradientC[2] );
        udg::Vector3 fxNormalD( fxGradientD[0], fxGradientD[1], fxGradientD[2] );
        udg::Vector3 fxNormalE( fxGradientE[0], fxGradientE[1], fxGradientE[2] );
        udg::Vector3 fxNormalF( fxGradientF[0], fxGradientF[1], fxGradientF[2] );
        udg::Vector3 fxNormalG( fxGradientG[0], fxGradientG[1], fxGradientG[2] );
        udg::Vector3 fxNormalH( fxGradientH[0], fxGradientH[1], fxGradientH[2] );
        udg::Vector3 fxNormal = fxNormalA * tA
                              + fxNormalB * tB
                              + fxNormalC * tC
                              + fxNormalD * tD
                              + fxNormalE * tE
                              + fxNormalF * tF
                              + fxNormalG * tG
                              + fxNormalH * tH;
        double fxDotProduct = fxDirection * fxNormal;
        if ( fxDotProduct < 0.0 ) fxDotProduct = -fxDotProduct;
        fx *= fxDotProduct < aFxContour ? 0.0 : 1.0;
      }

      // obscurance
      if ( aFxObscurance && fx > 0.0 )
      {
        double fxObscurance = aObscurance[offset       ] * tA
                            + aObscurance[offset + Binc] * tB
                            + aObscurance[offset + Cinc] * tC
                            + aObscurance[offset + Dinc] * tD
                            + aObscurance[offset + Einc] * tE
                            + aObscurance[offset + Finc] * tF
                            + aObscurance[offset + Ginc] * tG
                            + aObscurance[offset + Hinc] * tH;
        if ( fxObscurance < aObscuranceFilterLow ) fxObscurance = 0.0;
        else if ( fxObscurance > aObscuranceFilterHigh ) fxObscurance = 1.0;
        fx *= aObscuranceFactor * fxObscurance;
      }
      //////////////////////////////////////////////////////////////////////////////////////////////
      
      if ( scalar_value < 0.0 ) 
        {
        scalar_value = 0.0;
        }
      else if ( scalar_value > staticInfo->Volume->GetArraySize() - 1 )
        {
        scalar_value = staticInfo->Volume->GetArraySize() - 1;
        }
      
      opacity = SOTF[(int)(scalar_value)];
      
      if ( opacity )
        {
        if ( !grad_op_is_constant )
          {
          gmptr = grad_mag_ptr + offset;
          
          A = *(gmptr);
          B = *(gmptr + Binc);
          C = *(gmptr + Cinc);
          D = *(gmptr + Dinc);
          E = *(gmptr + Einc);
          F = *(gmptr + Finc);
          G = *(gmptr + Ginc);
          H = *(gmptr + Hinc);
          
          gradient_value = 
            A * t1 * t2 * t3 +
            B *  x * t2 * t3 +
            C * t1 *  y * t3 + 
            D *  x *  y * t3 +
            E * t1 * t2 *  z + 
            F *  x * t2 *  z + 
            G * t1 *  y *  z + 
            H *  x *  y *  z;
          
          if ( gradient_value < 0.0 )
            {
            gradient_value = 0.0;
            }
          else if ( gradient_value > 255.0 )
            {
            gradient_value = 255.0;
            }

          opacity *= GOTF[(int)(gradient_value)];
          }
        else
          {
          opacity *= gradient_opacity_constant;
          }

        red_value   = opacity * CTF[(int)(scalar_value) * 3    ];
        green_value = opacity * CTF[(int)(scalar_value) * 3 + 1];
        blue_value  = opacity * CTF[(int)(scalar_value) * 3 + 2];
        
        // Accumulate intensity and opacity for this sample location
        ////////////////////////////////////////////////////////////////////////////////////////////
//         accum_red_intensity   += remaining_opacity * red_value;
//         accum_green_intensity += remaining_opacity * green_value;
//         accum_blue_intensity  += remaining_opacity * blue_value;
        accum_red_intensity   += remaining_opacity * red_value * fx;
        accum_green_intensity += remaining_opacity * green_value * fx;
        accum_blue_intensity  += remaining_opacity * blue_value * fx;
        ////////////////////////////////////////////////////////////////////////////////////////////
        remaining_opacity *= (1.0 - opacity);
        }
    
      // Increment our position and compute our voxel location
      ray_position[0] += ray_increment[0];
      ray_position[1] += ray_increment[1];
      ray_position[2] += ray_increment[2];      
      voxel[0] = vtkFloorFuncMacro( ray_position[0] );
      voxel[1] = vtkFloorFuncMacro( ray_position[1] );
      voxel[2] = vtkFloorFuncMacro( ray_position[2] );
      }
    }

  // Cap the intensity value at 1.0
  if ( accum_red_intensity > 1.0 )
    {
    accum_red_intensity = 1.0;
    }
  if ( accum_green_intensity > 1.0 )
    {
    accum_green_intensity = 1.0;
    }
  if ( accum_blue_intensity > 1.0 )
    {
    accum_blue_intensity = 1.0;
    }
  
  if( remaining_opacity < VTK_REMAINING_OPACITY )
    {
    remaining_opacity = 0.0;
    }

  // Set the return pixel value.  The depth value is the distance to the
  // center of the volume.
  dynamicInfo->Color[0] = accum_red_intensity;
  dynamicInfo->Color[1] = accum_green_intensity;
  dynamicInfo->Color[2] = accum_blue_intensity;
  dynamicInfo->Color[3] = 1.0 - remaining_opacity;
  dynamicInfo->NumberOfStepsTaken = steps_this_ray;


}


// This is the templated function that actually casts a ray and computes
// the composite value.  This version uses trilinear interpolation and
// does not compute shading
template <class T>
void vtkCastRay_TrilinSample_Unshaded( T *data_ptr, vtkVolumeRayCastDynamicInfo *dynamicInfo,
                                       vtkVolumeRayCastStaticInfo *staticInfo, udg::Vector3 * aColorBleeding, double aObscuranceFactor )
{
  unsigned char   *grad_mag_ptr = NULL;
  unsigned char   *gmptr;
  float           accum_red_intensity;
  float           accum_green_intensity;
  float           accum_blue_intensity;
  float           remaining_opacity;
  float           red_value, green_value, blue_value;
  float           opacity;
  int             loop;
  int             xinc, yinc, zinc;
  int             voxel[3];
  float           ray_position[3];
  float           A, B, C, D, E, F, G, H;
  int             Binc, Cinc, Dinc, Einc, Finc, Ginc, Hinc;
  T               *dptr;
  float           *SOTF;
  float           *CTF;
  float           *GTF;
  float           *GOTF;
  float           x, y, z, t1, t2, t3;
  int             offset;
  int             steps_this_ray = 0;
  float           gradient_value;
  float           scalar_value;
  int             grad_op_is_constant;
  float           gradient_opacity_constant;
  int             num_steps;
  float           *ray_start, *ray_increment;

  num_steps = dynamicInfo->NumberOfStepsToTake;
  ray_start = dynamicInfo->TransformedStart;
  ray_increment = dynamicInfo->TransformedIncrement;

  // Get the scalar opacity transfer function which maps scalar input values
  // to opacities
  SOTF =  staticInfo->Volume->GetCorrectedScalarOpacityArray();

  // Get the color transfer function which maps scalar input values
  // to RGB colors
  CTF =  staticInfo->Volume->GetRGBArray();
  GTF =  staticInfo->Volume->GetGrayArray();

  // Get the gradient opacity transfer function for this volume (which maps
  // gradient magnitudes to opacities)
  GOTF =  staticInfo->Volume->GetGradientOpacityArray();

  // Get the gradient opacity constant. If this number is greater than
  // or equal to 0.0, then the gradient opacity transfer function is
  // a constant at that value, otherwise it is not a constant function
  gradient_opacity_constant = staticInfo->Volume->GetGradientOpacityConstant();
  grad_op_is_constant = ( gradient_opacity_constant >= 0.0 );

  // Get a pointer to the gradient magnitudes for this volume
  if ( !grad_op_is_constant )
    {
    grad_mag_ptr = staticInfo->GradientMagnitudes;
    }

  // Move the increments into local variables
  xinc = staticInfo->DataIncrement[0];
  yinc = staticInfo->DataIncrement[1];
  zinc = staticInfo->DataIncrement[2];

  // Initialize the ray position and voxel location
  ray_position[0] = ray_start[0];
  ray_position[1] = ray_start[1];
  ray_position[2] = ray_start[2];
  voxel[0] = vtkFloorFuncMacro( ray_position[0] );
  voxel[1] = vtkFloorFuncMacro( ray_position[1] );
  voxel[2] = vtkFloorFuncMacro( ray_position[2] );

  // So far we have not accumulated anything
  accum_red_intensity     = 0.0;
  accum_green_intensity   = 0.0;
  accum_blue_intensity    = 0.0;
  remaining_opacity       = 1.0;

  // Compute the increments to get to the other 7 voxel vertices from A
  Binc = xinc;
  Cinc = yinc;
  Dinc = xinc + yinc;
  Einc = zinc;
  Finc = zinc + xinc;
  Ginc = zinc + yinc;
  Hinc = zinc + xinc + yinc;
  
  // Two cases - we are working with a gray or RGB transfer
  // function - break them up to make it more efficient
  if ( staticInfo->ColorChannels == 1 ) 
    {
    // For each step along the ray
    for ( loop = 0; 
          loop < num_steps && remaining_opacity > VTK_REMAINING_OPACITY; 
          loop++ )
      {     
      // We've taken another step
      steps_this_ray++;
      
      offset = voxel[2] * zinc + voxel[1] * yinc + voxel[0];
      dptr = data_ptr + offset;
      
      A = *(dptr);
      B = *(dptr + Binc);
      C = *(dptr + Cinc);
      D = *(dptr + Dinc);
      E = *(dptr + Einc);
      F = *(dptr + Finc);
      G = *(dptr + Ginc);
      H = *(dptr + Hinc);
      
      // Compute our offset in the voxel, and use that to trilinearly
      // interpolate the value
      x = ray_position[0] - (float) voxel[0];
      y = ray_position[1] - (float) voxel[1];
      z = ray_position[2] - (float) voxel[2];
      
      t1 = 1.0 - x;
      t2 = 1.0 - y;
      t3 = 1.0 - z;
      
      scalar_value = 
        A * t1 * t2 * t3 +
        B *  x * t2 * t3 +
        C * t1 *  y * t3 + 
        D *  x *  y * t3 +
        E * t1 * t2 *  z + 
        F *  x * t2 *  z + 
        G * t1 *  y *  z + 
        H *  x *  y *  z;

      //////////////////////////////////////////////////////////////////////////////////////////////
      udg::Vector3 vColor = aColorBleeding[offset       ] * t1 * t2 * t3
                          + aColorBleeding[offset + Binc] *  x * t2 * t3
                          + aColorBleeding[offset + Cinc] * t1 *  y * t3
                          + aColorBleeding[offset + Dinc] *  x *  y * t3
                          + aColorBleeding[offset + Einc] * t1 * t2 *  z
                          + aColorBleeding[offset + Finc] *  x * t2 *  z
                          + aColorBleeding[offset + Ginc] * t1 *  y *  z
                          + aColorBleeding[offset + Hinc] *  x *  y *  z;
      //////////////////////////////////////////////////////////////////////////////////////////////
      
      if ( scalar_value < 0.0 ) 
        {
        scalar_value = 0.0;
        }
      else if ( scalar_value > staticInfo->Volume->GetArraySize() - 1 )
        {
        scalar_value = staticInfo->Volume->GetArraySize() - 1;
        }
      
      opacity = SOTF[(int)(scalar_value)];
      
      if ( opacity )
        {
        if ( !grad_op_is_constant )
          {
          gmptr = grad_mag_ptr + offset;
      
          A = *(gmptr);
          B = *(gmptr + Binc);
          C = *(gmptr + Cinc);
          D = *(gmptr + Dinc);
          E = *(gmptr + Einc);
          F = *(gmptr + Finc);
          G = *(gmptr + Ginc);
          H = *(gmptr + Hinc);
          
          gradient_value = 
            A * t1 * t2 * t3 +
            B *  x * t2 * t3 +
            C * t1 *  y * t3 + 
            D *  x *  y * t3 +
            E * t1 * t2 *  z + 
            F *  x * t2 *  z + 
            G * t1 *  y *  z + 
            H *  x *  y *  z;
      
          if ( gradient_value < 0.0 )
            {
            gradient_value = 0.0;
            }
          else if ( gradient_value > 255.0 )
            {
            gradient_value = 255.0;
            }

          opacity *= GOTF[(int)(gradient_value)];
          }
        else
          {
          opacity *= gradient_opacity_constant;
          }
        red_value   = opacity * GTF[(int)(scalar_value)];
        
        // Accumulate intensity and opacity for this sample location
        ////////////////////////////////////////////////////////////////////////////////////////////
//         accum_red_intensity   += remaining_opacity * red_value;
        accum_red_intensity   += remaining_opacity * red_value * aObscuranceFactor * ( vColor.x + vColor.y + vColor.z ) / 3.0;
        ////////////////////////////////////////////////////////////////////////////////////////////
        remaining_opacity *= (1.0 - opacity);
        }
    
      // Increment our position and compute our voxel location
      ray_position[0] += ray_increment[0];
      ray_position[1] += ray_increment[1];
      ray_position[2] += ray_increment[2];      
      voxel[0] = vtkFloorFuncMacro( ray_position[0] );
      voxel[1] = vtkFloorFuncMacro( ray_position[1] );
      voxel[2] = vtkFloorFuncMacro( ray_position[2] );
      }
    accum_green_intensity = accum_red_intensity;
    accum_blue_intensity = accum_red_intensity;
    }
  else if ( staticInfo->ColorChannels == 3 )
    {
    // For each step along the ray
    for ( loop = 0; 
          loop < num_steps && remaining_opacity > VTK_REMAINING_OPACITY; 
          loop++ )
      {     
      // We've taken another step
      steps_this_ray++;
      
      offset = voxel[2] * zinc + voxel[1] * yinc + voxel[0];
      dptr = data_ptr + offset;
      
      A = *(dptr);
      B = *(dptr + Binc);
      C = *(dptr + Cinc);
      D = *(dptr + Dinc);
      E = *(dptr + Einc);
      F = *(dptr + Finc);
      G = *(dptr + Ginc);
      H = *(dptr + Hinc);
      
      // Compute our offset in the voxel, and use that to trilinearly
      // interpolate the value
      x = ray_position[0] - (float) voxel[0];
      y = ray_position[1] - (float) voxel[1];
      z = ray_position[2] - (float) voxel[2];
      
      t1 = 1.0 - x;
      t2 = 1.0 - y;
      t3 = 1.0 - z;
      
      scalar_value = 
        A * t1 * t2 * t3 +
        B *  x * t2 * t3 +
        C * t1 *  y * t3 + 
        D *  x *  y * t3 +
        E * t1 * t2 *  z + 
        F *  x * t2 *  z + 
        G * t1 *  y *  z + 
        H *  x *  y *  z;

      //////////////////////////////////////////////////////////////////////////////////////////////
      udg::Vector3 vColor = aColorBleeding[offset       ] * t1 * t2 * t3
                          + aColorBleeding[offset + Binc] *  x * t2 * t3
                          + aColorBleeding[offset + Cinc] * t1 *  y * t3
                          + aColorBleeding[offset + Dinc] *  x *  y * t3
                          + aColorBleeding[offset + Einc] * t1 * t2 *  z
                          + aColorBleeding[offset + Finc] *  x * t2 *  z
                          + aColorBleeding[offset + Ginc] * t1 *  y *  z
                          + aColorBleeding[offset + Hinc] *  x *  y *  z;
      //////////////////////////////////////////////////////////////////////////////////////////////
      
      if ( scalar_value < 0.0 ) 
        {
        scalar_value = 0.0;
        }
      else if ( scalar_value > staticInfo->Volume->GetArraySize() - 1 )
        {
        scalar_value = staticInfo->Volume->GetArraySize() - 1;
        }
      
      opacity = SOTF[(int)(scalar_value)];
      
      if ( opacity )
        {
        if ( !grad_op_is_constant )
          {
          gmptr = grad_mag_ptr + offset;
          
          A = *(gmptr);
          B = *(gmptr + Binc);
          C = *(gmptr + Cinc);
          D = *(gmptr + Dinc);
          E = *(gmptr + Einc);
          F = *(gmptr + Finc);
          G = *(gmptr + Ginc);
          H = *(gmptr + Hinc);
          
          gradient_value = 
            A * t1 * t2 * t3 +
            B *  x * t2 * t3 +
            C * t1 *  y * t3 + 
            D *  x *  y * t3 +
            E * t1 * t2 *  z + 
            F *  x * t2 *  z + 
            G * t1 *  y *  z + 
            H *  x *  y *  z;
          
          if ( gradient_value < 0.0 )
            {
            gradient_value = 0.0;
            }
          else if ( gradient_value > 255.0 )
            {
            gradient_value = 255.0;
            }

          opacity *= GOTF[(int)(gradient_value)];
          }
        else
          {
          opacity *= gradient_opacity_constant;
          }

        red_value   = opacity * CTF[(int)(scalar_value) * 3    ];
        green_value = opacity * CTF[(int)(scalar_value) * 3 + 1];
        blue_value  = opacity * CTF[(int)(scalar_value) * 3 + 2];
        
        // Accumulate intensity and opacity for this sample location
        ////////////////////////////////////////////////////////////////////////////////////////////
//         accum_red_intensity   += remaining_opacity * red_value;
//         accum_green_intensity += remaining_opacity * green_value;
//         accum_blue_intensity  += remaining_opacity * blue_value;
        accum_red_intensity   += remaining_opacity * red_value * aObscuranceFactor * vColor.x;
        accum_green_intensity += remaining_opacity * green_value * aObscuranceFactor * vColor.y;
        accum_blue_intensity  += remaining_opacity * blue_value * aObscuranceFactor * vColor.z;
        ////////////////////////////////////////////////////////////////////////////////////////////
        remaining_opacity *= (1.0 - opacity);
        }
    
      // Increment our position and compute our voxel location
      ray_position[0] += ray_increment[0];
      ray_position[1] += ray_increment[1];
      ray_position[2] += ray_increment[2];      
      voxel[0] = vtkFloorFuncMacro( ray_position[0] );
      voxel[1] = vtkFloorFuncMacro( ray_position[1] );
      voxel[2] = vtkFloorFuncMacro( ray_position[2] );
      }
    }

  // Cap the intensity value at 1.0
  if ( accum_red_intensity > 1.0 )
    {
    accum_red_intensity = 1.0;
    }
  if ( accum_green_intensity > 1.0 )
    {
    accum_green_intensity = 1.0;
    }
  if ( accum_blue_intensity > 1.0 )
    {
    accum_blue_intensity = 1.0;
    }
  
  if( remaining_opacity < VTK_REMAINING_OPACITY )
    {
    remaining_opacity = 0.0;
    }

  // Set the return pixel value.  The depth value is the distance to the
  // center of the volume.
  dynamicInfo->Color[0] = accum_red_intensity;
  dynamicInfo->Color[1] = accum_green_intensity;
  dynamicInfo->Color[2] = accum_blue_intensity;
  dynamicInfo->Color[3] = 1.0 - remaining_opacity;
  dynamicInfo->NumberOfStepsTaken = steps_this_ray;


}


// This is the templated function that actually casts a ray and computes
// the composite value.  This version uses trilinear interpolation, and
// does perform shading.
template <class T>
void vtkCastRay_TrilinSample_Shaded( T *data_ptr, vtkVolumeRayCastDynamicInfo *dynamicInfo,
                                     vtkVolumeRayCastStaticInfo *staticInfo,
                                     double * aObscurance, double aObscuranceFactor, double aObscuranceFilterLow, double aObscuranceFilterHigh,
                                     bool aFxObscurance, double aFxContour )
{
  unsigned char   *grad_mag_ptr = NULL;
  unsigned char   *gmptr;
  float           accum_red_intensity;
  float           accum_green_intensity;
  float           accum_blue_intensity;
  float           remaining_opacity;
  float           opacity;
  int             loop;
  int             xinc, yinc, zinc;
  int             voxel[3];
  float           ray_position[3];
  float           A, B, C, D, E, F, G, H;
  int             A_n, B_n, C_n, D_n, E_n, F_n, G_n, H_n;
  float           final_rd, final_gd, final_bd;
  float           final_rs, final_gs, final_bs;
  int             Binc, Cinc, Dinc, Einc, Finc, Ginc, Hinc;
  T               *dptr;
  float           *SOTF;
  float           *CTF;
  float           *GTF;
  float           *GOTF;
  float           x, y, z, t1, t2, t3;
  float           tA, tB, tC, tD, tE, tF, tG, tH;
  float           *red_d_shade, *green_d_shade, *blue_d_shade;
  float           *red_s_shade, *green_s_shade, *blue_s_shade;
  unsigned short  *encoded_normals, *nptr;
  float           red_shaded_value, green_shaded_value, blue_shaded_value;
  int             offset;
  int             steps_this_ray = 0;
  int             gradient_value;
  int             scalar_value;
  float           r, g, b;
  int             grad_op_is_constant;
  float           gradient_opacity_constant;
  int             num_steps;
  float           *ray_start, *ray_increment;


  num_steps = dynamicInfo->NumberOfStepsToTake;
  ray_start = dynamicInfo->TransformedStart;
  ray_increment = dynamicInfo->TransformedIncrement;

  // Get diffuse shading table pointers
  red_d_shade = staticInfo->RedDiffuseShadingTable;
  green_d_shade = staticInfo->GreenDiffuseShadingTable;
  blue_d_shade = staticInfo->BlueDiffuseShadingTable;


  // Get diffuse shading table pointers
  red_s_shade = staticInfo->RedSpecularShadingTable;
  green_s_shade = staticInfo->GreenSpecularShadingTable;
  blue_s_shade = staticInfo->BlueSpecularShadingTable;

  // Get a pointer to the encoded normals for this volume
  encoded_normals = staticInfo->EncodedNormals;

  // Get the scalar opacity transfer function which maps scalar input values
  // to opacities
  SOTF =  staticInfo->Volume->GetCorrectedScalarOpacityArray();

  // Get the color transfer function which maps scalar input values
  // to RGB values
  CTF =  staticInfo->Volume->GetRGBArray();
  GTF =  staticInfo->Volume->GetGrayArray();

  // Get the gradient opacity transfer function for this volume (which maps
  // gradient magnitudes to opacities)
  GOTF =  staticInfo->Volume->GetGradientOpacityArray();

  // Get the gradient opacity constant. If this number is greater than
  // or equal to 0.0, then the gradient opacity transfer function is
  // a constant at that value, otherwise it is not a constant function
  gradient_opacity_constant = staticInfo->Volume->GetGradientOpacityConstant();
  grad_op_is_constant = ( gradient_opacity_constant >= 0.0 );

  // Get a pointer to the gradient magnitudes for this volume
  if ( !grad_op_is_constant )
    {
    grad_mag_ptr = staticInfo->GradientMagnitudes;
    }

  // Move the increments into local variables
  xinc = staticInfo->DataIncrement[0];
  yinc = staticInfo->DataIncrement[1];
  zinc = staticInfo->DataIncrement[2];

  // Initialize the ray position and voxel location
  ray_position[0] = ray_start[0];
  ray_position[1] = ray_start[1];
  ray_position[2] = ray_start[2];
  voxel[0] = vtkFloorFuncMacro( ray_position[0] );
  voxel[1] = vtkFloorFuncMacro( ray_position[1] );
  voxel[2] = vtkFloorFuncMacro( ray_position[2] );

  // So far we haven't accumulated anything
  accum_red_intensity   = 0.0;
  accum_green_intensity = 0.0;
  accum_blue_intensity  = 0.0;
  remaining_opacity     = 1.0;

  // Compute the increments to get to the other 7 voxel vertices from A
  Binc = xinc;
  Cinc = yinc;
  Dinc = xinc + yinc;
  Einc = zinc;
  Finc = zinc + xinc;
  Ginc = zinc + yinc;
  Hinc = zinc + xinc + yinc;

  //////////////////////////////////////////////////////////////////////////////////////////////////
  udg::Vector3 fxDirection( dynamicInfo->TransformedDirection[0], dynamicInfo->TransformedDirection[1], dynamicInfo->TransformedDirection[2] );
  fxDirection.normalize();
  vtkVolumeRayCastMapper * fxMapper = vtkVolumeRayCastMapper::SafeDownCast( staticInfo->Volume->GetMapper() );
  vtkEncodedGradientEstimator * fxGradientEstimator = fxMapper->GetGradientEstimator();
  unsigned short * fxEncodedNormals = fxGradientEstimator->GetEncodedNormals();
  vtkDirectionEncoder * fxDirectionEncoder = fxGradientEstimator->GetDirectionEncoder();
  //////////////////////////////////////////////////////////////////////////////////////////////////

  // Two cases - we are working with a gray or RGB transfer
  // function - break them up to make it more efficient
  if ( staticInfo->ColorChannels == 1 ) 
    {
    // For each step along the ray
    for ( loop = 0; 
          loop < num_steps && remaining_opacity > VTK_REMAINING_OPACITY; 
          loop++ )
      {     
      // We've taken another step
      steps_this_ray++;
      
      offset = voxel[2] * zinc + voxel[1] * yinc + voxel[0];
      dptr = data_ptr + offset;
      nptr = encoded_normals + offset;
    
      A = *(dptr);
      B = *(dptr + Binc);
      C = *(dptr + Cinc);
      D = *(dptr + Dinc);
      E = *(dptr + Einc);
      F = *(dptr + Finc);
      G = *(dptr + Ginc);
      H = *(dptr + Hinc);
      
      // Compute our offset in the voxel, and use that to trilinearly
      // interpolate a value
      x = ray_position[0] - (float) voxel[0];
      y = ray_position[1] - (float) voxel[1];
      z = ray_position[2] - (float) voxel[2];
      
      t1 = 1.0 - x;
      t2 = 1.0 - y;
      t3 = 1.0 - z;
      
      tA = t1 * t2 * t3;
      tB =  x * t2 * t3;
      tC = t1 *  y * t3;
      tD =  x *  y * t3;
      tE = t1 * t2 *  z;
      tF =  x * t2 *  z;
      tG = t1 *  y *  z;
      tH =  x *  y *  z;
      
      scalar_value = (int) (
                            A * tA + B * tB + C * tC + D * tD + 
                            E * tE + F * tF + G * tG + H * tH );

      //////////////////////////////////////////////////////////////////////////////////////////////
      double fx = 1.0;

      // contour
      if ( aFxContour > 0.0 )
      {
        float * fxGradientA = fxDirectionEncoder->GetDecodedGradient( fxEncodedNormals[offset] );
        float * fxGradientB = fxDirectionEncoder->GetDecodedGradient( fxEncodedNormals[offset + Binc] );
        float * fxGradientC = fxDirectionEncoder->GetDecodedGradient( fxEncodedNormals[offset + Cinc] );
        float * fxGradientD = fxDirectionEncoder->GetDecodedGradient( fxEncodedNormals[offset + Dinc] );
        float * fxGradientE = fxDirectionEncoder->GetDecodedGradient( fxEncodedNormals[offset + Einc] );
        float * fxGradientF = fxDirectionEncoder->GetDecodedGradient( fxEncodedNormals[offset + Finc] );
        float * fxGradientG = fxDirectionEncoder->GetDecodedGradient( fxEncodedNormals[offset + Ginc] );
        float * fxGradientH = fxDirectionEncoder->GetDecodedGradient( fxEncodedNormals[offset + Hinc] );
        udg::Vector3 fxNormalA( fxGradientA[0], fxGradientA[1], fxGradientA[2] );
        udg::Vector3 fxNormalB( fxGradientB[0], fxGradientB[1], fxGradientB[2] );
        udg::Vector3 fxNormalC( fxGradientC[0], fxGradientC[1], fxGradientC[2] );
        udg::Vector3 fxNormalD( fxGradientD[0], fxGradientD[1], fxGradientD[2] );
        udg::Vector3 fxNormalE( fxGradientE[0], fxGradientE[1], fxGradientE[2] );
        udg::Vector3 fxNormalF( fxGradientF[0], fxGradientF[1], fxGradientF[2] );
        udg::Vector3 fxNormalG( fxGradientG[0], fxGradientG[1], fxGradientG[2] );
        udg::Vector3 fxNormalH( fxGradientH[0], fxGradientH[1], fxGradientH[2] );
        udg::Vector3 fxNormal = fxNormalA * tA
                              + fxNormalB * tB
                              + fxNormalC * tC
                              + fxNormalD * tD
                              + fxNormalE * tE
                              + fxNormalF * tF
                              + fxNormalG * tG
                              + fxNormalH * tH;
        double fxDotProduct = fxDirection * fxNormal;
        if ( fxDotProduct < 0.0 ) fxDotProduct = -fxDotProduct;
        fx *= fxDotProduct < aFxContour ? 0.0 : 1.0;
      }

      // obscurance
      if ( aFxObscurance && fx > 0.0 )
      {
        double fxObscurance = aObscurance[offset       ] * tA
                            + aObscurance[offset + Binc] * tB
                            + aObscurance[offset + Cinc] * tC
                            + aObscurance[offset + Dinc] * tD
                            + aObscurance[offset + Einc] * tE
                            + aObscurance[offset + Finc] * tF
                            + aObscurance[offset + Ginc] * tG
                            + aObscurance[offset + Hinc] * tH;
        if ( fxObscurance < aObscuranceFilterLow ) fxObscurance = 0.0;
        else if ( fxObscurance > aObscuranceFilterHigh ) fxObscurance = 1.0;
        fx *= aObscuranceFactor * fxObscurance;
      }
      //////////////////////////////////////////////////////////////////////////////////////////////
      
      if ( scalar_value < 0 ) 
        {
        scalar_value = 0;
        }
      else if ( scalar_value > staticInfo->Volume->GetArraySize() - 1 )
        {
        scalar_value = (int)(staticInfo->Volume->GetArraySize() - 1);
        }
      
      opacity = SOTF[scalar_value];

      // If we have some opacity based on the scalar value transfer function,
      // then multiply by the opacity from the gradient magnitude transfer
      // function
      if ( opacity )
        {
        if ( !grad_op_is_constant )
          {
          gmptr = grad_mag_ptr + offset;
      
          A = *(gmptr);
          B = *(gmptr + Binc);
          C = *(gmptr + Cinc);
          D = *(gmptr + Dinc);
          E = *(gmptr + Einc);
          F = *(gmptr + Finc);
          G = *(gmptr + Ginc);
          H = *(gmptr + Hinc);
          
          gradient_value = (int) (
                                  A * tA + B * tB + C * tC + D * tD + 
                                  E * tE + F * tF + G * tG + H * tH );
          if ( gradient_value < 0 )
            {
            gradient_value = 0;
            }
          else if ( gradient_value > 255 )
            {
            gradient_value = 255;
            }
          
          opacity *= GOTF[gradient_value];
          }
        else
          {
          opacity *= gradient_opacity_constant;
          }
        }

      // If we have a combined opacity value, then compute the shading
      if ( opacity )
        {
        A_n = *(nptr);
        B_n = *(nptr + Binc);
        C_n = *(nptr + Cinc);
        D_n = *(nptr + Dinc);
        E_n = *(nptr + Einc);
        F_n = *(nptr + Finc);
        G_n = *(nptr + Ginc);
        H_n = *(nptr + Hinc);

        final_rd = 
          red_d_shade[ A_n ] * tA + red_d_shade[ B_n ] * tB +   
          red_d_shade[ C_n ] * tC + red_d_shade[ D_n ] * tD + 
          red_d_shade[ E_n ] * tE + red_d_shade[ F_n ] * tF +   
          red_d_shade[ G_n ] * tG + red_d_shade[ H_n ] * tH;
        
        final_rs = 
          red_s_shade[ A_n ] * tA + red_s_shade[ B_n ] * tB +   
          red_s_shade[ C_n ] * tC + red_s_shade[ D_n ] * tD + 
          red_s_shade[ E_n ] * tE + red_s_shade[ F_n ] * tF +   
          red_s_shade[ G_n ] * tG + red_s_shade[ H_n ] * tH;
        
        r = GTF[(scalar_value)];

        // For this sample we have do not yet have any opacity or
        // shaded intensity yet
        red_shaded_value   = opacity * ( final_rd * r + final_rs );
        
        // Accumulate intensity and opacity for this sample location
        ////////////////////////////////////////////////////////////////////////////////////////////
//         accum_red_intensity   += red_shaded_value * remaining_opacity;
        accum_red_intensity   += red_shaded_value * remaining_opacity * fx;
        ////////////////////////////////////////////////////////////////////////////////////////////
        remaining_opacity *= (1.0 - opacity);
        }

      // Increment our position and compute our voxel location
      ray_position[0] += ray_increment[0];
      ray_position[1] += ray_increment[1];
      ray_position[2] += ray_increment[2];      
      voxel[0] = vtkFloorFuncMacro( ray_position[0] );
      voxel[1] = vtkFloorFuncMacro( ray_position[1] );
      voxel[2] = vtkFloorFuncMacro( ray_position[2] );
      }
    accum_green_intensity = accum_red_intensity;
    accum_blue_intensity = accum_red_intensity;
    }
  else if ( staticInfo->ColorChannels == 3 )
    {
    // For each step along the ray
    for ( loop = 0; 
          loop < num_steps && remaining_opacity > VTK_REMAINING_OPACITY; 
          loop++ )
      {     
      // We've taken another step
      steps_this_ray++;
      
      offset = voxel[2] * zinc + voxel[1] * yinc + voxel[0];
      dptr = data_ptr + offset;
      nptr = encoded_normals + offset;
    
      A = *(dptr);
      B = *(dptr + Binc);
      C = *(dptr + Cinc);
      D = *(dptr + Dinc);
      E = *(dptr + Einc);
      F = *(dptr + Finc);
      G = *(dptr + Ginc);
      H = *(dptr + Hinc);
      
      // Compute our offset in the voxel, and use that to trilinearly
      // interpolate a value
      x = ray_position[0] - (float) voxel[0];
      y = ray_position[1] - (float) voxel[1];
      z = ray_position[2] - (float) voxel[2];
      
      t1 = 1.0 - x;
      t2 = 1.0 - y;
      t3 = 1.0 - z;
      
      tA = t1 * t2 * t3;
      tB =  x * t2 * t3;
      tC = t1 *  y * t3;
      tD =  x *  y * t3;
      tE = t1 * t2 *  z;
      tF =  x * t2 *  z;
      tG = t1 *  y *  z;
      tH =  x *  y *  z;
      
      scalar_value = (int) (
                            A * tA + B * tB + C * tC + D * tD + 
                            E * tE + F * tF + G * tG + H * tH );

      //////////////////////////////////////////////////////////////////////////////////////////////
      double fx = 1.0;

      // contour
      if ( aFxContour > 0.0 )
      {
        float * fxGradientA = fxDirectionEncoder->GetDecodedGradient( fxEncodedNormals[offset] );
        float * fxGradientB = fxDirectionEncoder->GetDecodedGradient( fxEncodedNormals[offset + Binc] );
        float * fxGradientC = fxDirectionEncoder->GetDecodedGradient( fxEncodedNormals[offset + Cinc] );
        float * fxGradientD = fxDirectionEncoder->GetDecodedGradient( fxEncodedNormals[offset + Dinc] );
        float * fxGradientE = fxDirectionEncoder->GetDecodedGradient( fxEncodedNormals[offset + Einc] );
        float * fxGradientF = fxDirectionEncoder->GetDecodedGradient( fxEncodedNormals[offset + Finc] );
        float * fxGradientG = fxDirectionEncoder->GetDecodedGradient( fxEncodedNormals[offset + Ginc] );
        float * fxGradientH = fxDirectionEncoder->GetDecodedGradient( fxEncodedNormals[offset + Hinc] );
        udg::Vector3 fxNormalA( fxGradientA[0], fxGradientA[1], fxGradientA[2] );
        udg::Vector3 fxNormalB( fxGradientB[0], fxGradientB[1], fxGradientB[2] );
        udg::Vector3 fxNormalC( fxGradientC[0], fxGradientC[1], fxGradientC[2] );
        udg::Vector3 fxNormalD( fxGradientD[0], fxGradientD[1], fxGradientD[2] );
        udg::Vector3 fxNormalE( fxGradientE[0], fxGradientE[1], fxGradientE[2] );
        udg::Vector3 fxNormalF( fxGradientF[0], fxGradientF[1], fxGradientF[2] );
        udg::Vector3 fxNormalG( fxGradientG[0], fxGradientG[1], fxGradientG[2] );
        udg::Vector3 fxNormalH( fxGradientH[0], fxGradientH[1], fxGradientH[2] );
        udg::Vector3 fxNormal = fxNormalA * tA
                              + fxNormalB * tB
                              + fxNormalC * tC
                              + fxNormalD * tD
                              + fxNormalE * tE
                              + fxNormalF * tF
                              + fxNormalG * tG
                              + fxNormalH * tH;
        double fxDotProduct = fxDirection * fxNormal;
        if ( fxDotProduct < 0.0 ) fxDotProduct = -fxDotProduct;
        fx *= fxDotProduct < aFxContour ? 0.0 : 1.0;
      }

      // obscurance
      if ( aFxObscurance && fx > 0.0 )
      {
        double fxObscurance = aObscurance[offset       ] * tA
                            + aObscurance[offset + Binc] * tB
                            + aObscurance[offset + Cinc] * tC
                            + aObscurance[offset + Dinc] * tD
                            + aObscurance[offset + Einc] * tE
                            + aObscurance[offset + Finc] * tF
                            + aObscurance[offset + Ginc] * tG
                            + aObscurance[offset + Hinc] * tH;
        if ( fxObscurance < aObscuranceFilterLow ) fxObscurance = 0.0;
        else if ( fxObscurance > aObscuranceFilterHigh ) fxObscurance = 1.0;
        fx *= aObscuranceFactor * fxObscurance;
      }
      //////////////////////////////////////////////////////////////////////////////////////////////
      
      if ( scalar_value < 0 ) 
        {
        scalar_value = 0;
        }
      else if ( scalar_value > staticInfo->Volume->GetArraySize() - 1 )
        {
        scalar_value = (int)(staticInfo->Volume->GetArraySize() - 1);
        }
      
      opacity = SOTF[scalar_value];
      
      if ( opacity )
        {
          if ( !grad_op_is_constant )
            {
            gmptr = grad_mag_ptr + offset;
      
            A = *(gmptr);
            B = *(gmptr + Binc);
            C = *(gmptr + Cinc);
            D = *(gmptr + Dinc);
            E = *(gmptr + Einc);
            F = *(gmptr + Finc);
            G = *(gmptr + Ginc);
            H = *(gmptr + Hinc);
            
            gradient_value = (int) (
                                    A * tA + B * tB + C * tC + D * tD + 
                                    E * tE + F * tF + G * tG + H * tH );
            if ( gradient_value < 0 )
              {
              gradient_value = 0;
              }
            else if ( gradient_value > 255 )
              {
              gradient_value = 255;
              }

            opacity *= GOTF[gradient_value];
            }
          else
            {
            opacity *= gradient_opacity_constant;
            }
        }

      // If we have a combined opacity value, then compute the shading
      if ( opacity )
        {
        A_n = *(nptr);
        B_n = *(nptr + Binc);
        C_n = *(nptr + Cinc);
        D_n = *(nptr + Dinc);
        E_n = *(nptr + Einc);
        F_n = *(nptr + Finc);
        G_n = *(nptr + Ginc);
        H_n = *(nptr + Hinc);
        
        final_rd = 
          red_d_shade[ A_n ] * tA + red_d_shade[ B_n ] * tB +   
          red_d_shade[ C_n ] * tC + red_d_shade[ D_n ] * tD + 
          red_d_shade[ E_n ] * tE + red_d_shade[ F_n ] * tF +   
          red_d_shade[ G_n ] * tG + red_d_shade[ H_n ] * tH;
        
        final_gd = 
          green_d_shade[ A_n ] * tA + green_d_shade[ B_n ] * tB +       
          green_d_shade[ C_n ] * tC + green_d_shade[ D_n ] * tD + 
          green_d_shade[ E_n ] * tE + green_d_shade[ F_n ] * tF +       
          green_d_shade[ G_n ] * tG + green_d_shade[ H_n ] * tH;
        
        final_bd = 
          blue_d_shade[ A_n ] * tA + blue_d_shade[ B_n ] * tB +         
          blue_d_shade[ C_n ] * tC + blue_d_shade[ D_n ] * tD + 
          blue_d_shade[ E_n ] * tE + blue_d_shade[ F_n ] * tF + 
          blue_d_shade[ G_n ] * tG + blue_d_shade[ H_n ] * tH;
        
        final_rs = 
          red_s_shade[ A_n ] * tA + red_s_shade[ B_n ] * tB +   
          red_s_shade[ C_n ] * tC + red_s_shade[ D_n ] * tD + 
          red_s_shade[ E_n ] * tE + red_s_shade[ F_n ] * tF +   
          red_s_shade[ G_n ] * tG + red_s_shade[ H_n ] * tH;
        
        final_gs = 
          green_s_shade[ A_n ] * tA + green_s_shade[ B_n ] * tB +       
          green_s_shade[ C_n ] * tC + green_s_shade[ D_n ] * tD + 
          green_s_shade[ E_n ] * tE + green_s_shade[ F_n ] * tF +       
          green_s_shade[ G_n ] * tG + green_s_shade[ H_n ] * tH;
        
        final_bs = 
          blue_s_shade[ A_n ] * tA + blue_s_shade[ B_n ] * tB +         
          blue_s_shade[ C_n ] * tC + blue_s_shade[ D_n ] * tD + 
          blue_s_shade[ E_n ] * tE + blue_s_shade[ F_n ] * tF + 
          blue_s_shade[ G_n ] * tG + blue_s_shade[ H_n ] * tH;
        
        r = CTF[(scalar_value) * 3    ];
        g = CTF[(scalar_value) * 3 + 1];
        b = CTF[(scalar_value) * 3 + 2];
        
        // For this sample we have do not yet have any opacity or
        // shaded intensity yet
        red_shaded_value   = opacity * ( final_rd * r + final_rs );
        green_shaded_value = opacity * ( final_gd * g + final_gs );
        blue_shaded_value  = opacity * ( final_bd * b + final_bs );
        
        // Accumulate intensity and opacity for this sample location
        ////////////////////////////////////////////////////////////////////////////////////////////
//         accum_red_intensity   += red_shaded_value   * remaining_opacity;
//         accum_green_intensity += green_shaded_value * remaining_opacity;
//         accum_blue_intensity  += blue_shaded_value  * remaining_opacity;
        accum_red_intensity   += red_shaded_value   * remaining_opacity * fx;
        accum_green_intensity += green_shaded_value * remaining_opacity * fx;
        accum_blue_intensity  += blue_shaded_value  * remaining_opacity * fx;
        ////////////////////////////////////////////////////////////////////////////////////////////
        remaining_opacity *= (1.0 - opacity);
        }

      // Increment our position and compute our voxel location
      ray_position[0] += ray_increment[0];
      ray_position[1] += ray_increment[1];
      ray_position[2] += ray_increment[2];      
      voxel[0] = vtkFloorFuncMacro( ray_position[0] );
      voxel[1] = vtkFloorFuncMacro( ray_position[1] );
      voxel[2] = vtkFloorFuncMacro( ray_position[2] );
      }
    }

  // Cap the accumulated intensity at 1.0
  if ( accum_red_intensity > 1.0 )
    {
    accum_red_intensity = 1.0;
    }
  if ( accum_green_intensity > 1.0 )
    {
    accum_green_intensity = 1.0;
    }
  if ( accum_blue_intensity > 1.0 )
    {
    accum_blue_intensity = 1.0;
    }
  
  if( remaining_opacity < VTK_REMAINING_OPACITY )
    {
    remaining_opacity = 0.0;
    }

  // Set the return pixel value.  The depth value is the distance to the
  // center of the volume.
  dynamicInfo->Color[0] = accum_red_intensity;
  dynamicInfo->Color[1] = accum_green_intensity;
  dynamicInfo->Color[2] = accum_blue_intensity;
  dynamicInfo->Color[3] = 1.0 - remaining_opacity;
  dynamicInfo->NumberOfStepsTaken = steps_this_ray;

 
}


// This is the templated function that actually casts a ray and computes
// the composite value.  This version uses trilinear interpolation, and
// does perform shading.
template <class T>
void vtkCastRay_TrilinSample_Shaded( T *data_ptr, vtkVolumeRayCastDynamicInfo *dynamicInfo,
                                     vtkVolumeRayCastStaticInfo *staticInfo, udg::Vector3 * aColorBleeding, double aObscuranceFactor )
{
  unsigned char   *grad_mag_ptr = NULL;
  unsigned char   *gmptr;
  float           accum_red_intensity;
  float           accum_green_intensity;
  float           accum_blue_intensity;
  float           remaining_opacity;
  float           opacity;
  int             loop;
  int             xinc, yinc, zinc;
  int             voxel[3];
  float           ray_position[3];
  float           A, B, C, D, E, F, G, H;
  int             A_n, B_n, C_n, D_n, E_n, F_n, G_n, H_n;
  float           final_rd, final_gd, final_bd;
  float           final_rs, final_gs, final_bs;
  int             Binc, Cinc, Dinc, Einc, Finc, Ginc, Hinc;
  T               *dptr;
  float           *SOTF;
  float           *CTF;
  float           *GTF;
  float           *GOTF;
  float           x, y, z, t1, t2, t3;
  float           tA, tB, tC, tD, tE, tF, tG, tH;
  float           *red_d_shade, *green_d_shade, *blue_d_shade;
  float           *red_s_shade, *green_s_shade, *blue_s_shade;
  unsigned short  *encoded_normals, *nptr;
  float           red_shaded_value, green_shaded_value, blue_shaded_value;
  int             offset;
  int             steps_this_ray = 0;
  int             gradient_value;
  int             scalar_value;
  float           r, g, b;
  int             grad_op_is_constant;
  float           gradient_opacity_constant;
  int             num_steps;
  float           *ray_start, *ray_increment;


  num_steps = dynamicInfo->NumberOfStepsToTake;
  ray_start = dynamicInfo->TransformedStart;
  ray_increment = dynamicInfo->TransformedIncrement;

  // Get diffuse shading table pointers
  red_d_shade = staticInfo->RedDiffuseShadingTable;
  green_d_shade = staticInfo->GreenDiffuseShadingTable;
  blue_d_shade = staticInfo->BlueDiffuseShadingTable;


  // Get diffuse shading table pointers
  red_s_shade = staticInfo->RedSpecularShadingTable;
  green_s_shade = staticInfo->GreenSpecularShadingTable;
  blue_s_shade = staticInfo->BlueSpecularShadingTable;

  // Get a pointer to the encoded normals for this volume
  encoded_normals = staticInfo->EncodedNormals;

  // Get the scalar opacity transfer function which maps scalar input values
  // to opacities
  SOTF =  staticInfo->Volume->GetCorrectedScalarOpacityArray();

  // Get the color transfer function which maps scalar input values
  // to RGB values
  CTF =  staticInfo->Volume->GetRGBArray();
  GTF =  staticInfo->Volume->GetGrayArray();

  // Get the gradient opacity transfer function for this volume (which maps
  // gradient magnitudes to opacities)
  GOTF =  staticInfo->Volume->GetGradientOpacityArray();

  // Get the gradient opacity constant. If this number is greater than
  // or equal to 0.0, then the gradient opacity transfer function is
  // a constant at that value, otherwise it is not a constant function
  gradient_opacity_constant = staticInfo->Volume->GetGradientOpacityConstant();
  grad_op_is_constant = ( gradient_opacity_constant >= 0.0 );

  // Get a pointer to the gradient magnitudes for this volume
  if ( !grad_op_is_constant )
    {
    grad_mag_ptr = staticInfo->GradientMagnitudes;
    }

  // Move the increments into local variables
  xinc = staticInfo->DataIncrement[0];
  yinc = staticInfo->DataIncrement[1];
  zinc = staticInfo->DataIncrement[2];

  // Initialize the ray position and voxel location
  ray_position[0] = ray_start[0];
  ray_position[1] = ray_start[1];
  ray_position[2] = ray_start[2];
  voxel[0] = vtkFloorFuncMacro( ray_position[0] );
  voxel[1] = vtkFloorFuncMacro( ray_position[1] );
  voxel[2] = vtkFloorFuncMacro( ray_position[2] );

  // So far we haven't accumulated anything
  accum_red_intensity   = 0.0;
  accum_green_intensity = 0.0;
  accum_blue_intensity  = 0.0;
  remaining_opacity     = 1.0;

  // Compute the increments to get to the other 7 voxel vertices from A
  Binc = xinc;
  Cinc = yinc;
  Dinc = xinc + yinc;
  Einc = zinc;
  Finc = zinc + xinc;
  Ginc = zinc + yinc;
  Hinc = zinc + xinc + yinc;
  
  // Two cases - we are working with a gray or RGB transfer
  // function - break them up to make it more efficient
  if ( staticInfo->ColorChannels == 1 ) 
    {
    // For each step along the ray
    for ( loop = 0; 
          loop < num_steps && remaining_opacity > VTK_REMAINING_OPACITY; 
          loop++ )
      {     
      // We've taken another step
      steps_this_ray++;
      
      offset = voxel[2] * zinc + voxel[1] * yinc + voxel[0];
      dptr = data_ptr + offset;
      nptr = encoded_normals + offset;
    
      A = *(dptr);
      B = *(dptr + Binc);
      C = *(dptr + Cinc);
      D = *(dptr + Dinc);
      E = *(dptr + Einc);
      F = *(dptr + Finc);
      G = *(dptr + Ginc);
      H = *(dptr + Hinc);
      
      // Compute our offset in the voxel, and use that to trilinearly
      // interpolate a value
      x = ray_position[0] - (float) voxel[0];
      y = ray_position[1] - (float) voxel[1];
      z = ray_position[2] - (float) voxel[2];
      
      t1 = 1.0 - x;
      t2 = 1.0 - y;
      t3 = 1.0 - z;
      
      tA = t1 * t2 * t3;
      tB =  x * t2 * t3;
      tC = t1 *  y * t3;
      tD =  x *  y * t3;
      tE = t1 * t2 *  z;
      tF =  x * t2 *  z;
      tG = t1 *  y *  z;
      tH =  x *  y *  z;
      
      scalar_value = (int) (
                            A * tA + B * tB + C * tC + D * tD + 
                            E * tE + F * tF + G * tG + H * tH );

      //////////////////////////////////////////////////////////////////////////////////////////////
      udg::Vector3 vColor = aColorBleeding[offset       ] * tA
                          + aColorBleeding[offset + Binc] * tB
                          + aColorBleeding[offset + Cinc] * tC
                          + aColorBleeding[offset + Dinc] * tD
                          + aColorBleeding[offset + Einc] * tE
                          + aColorBleeding[offset + Finc] * tF
                          + aColorBleeding[offset + Ginc] * tG
                          + aColorBleeding[offset + Hinc] * tH;
      //////////////////////////////////////////////////////////////////////////////////////////////
      
      if ( scalar_value < 0 ) 
        {
        scalar_value = 0;
        }
      else if ( scalar_value > staticInfo->Volume->GetArraySize() - 1 )
        {
        scalar_value = (int)(staticInfo->Volume->GetArraySize() - 1);
        }
      
      opacity = SOTF[scalar_value];

      // If we have some opacity based on the scalar value transfer function,
      // then multiply by the opacity from the gradient magnitude transfer
      // function
      if ( opacity )
        {
        if ( !grad_op_is_constant )
          {
          gmptr = grad_mag_ptr + offset;
      
          A = *(gmptr);
          B = *(gmptr + Binc);
          C = *(gmptr + Cinc);
          D = *(gmptr + Dinc);
          E = *(gmptr + Einc);
          F = *(gmptr + Finc);
          G = *(gmptr + Ginc);
          H = *(gmptr + Hinc);
          
          gradient_value = (int) (
                                  A * tA + B * tB + C * tC + D * tD + 
                                  E * tE + F * tF + G * tG + H * tH );
          if ( gradient_value < 0 )
            {
            gradient_value = 0;
            }
          else if ( gradient_value > 255 )
            {
            gradient_value = 255;
            }
          
          opacity *= GOTF[gradient_value];
          }
        else
          {
          opacity *= gradient_opacity_constant;
          }
        }

      // If we have a combined opacity value, then compute the shading
      if ( opacity )
        {
        A_n = *(nptr);
        B_n = *(nptr + Binc);
        C_n = *(nptr + Cinc);
        D_n = *(nptr + Dinc);
        E_n = *(nptr + Einc);
        F_n = *(nptr + Finc);
        G_n = *(nptr + Ginc);
        H_n = *(nptr + Hinc);

        final_rd = 
          red_d_shade[ A_n ] * tA + red_d_shade[ B_n ] * tB +   
          red_d_shade[ C_n ] * tC + red_d_shade[ D_n ] * tD + 
          red_d_shade[ E_n ] * tE + red_d_shade[ F_n ] * tF +   
          red_d_shade[ G_n ] * tG + red_d_shade[ H_n ] * tH;
        
        final_rs = 
          red_s_shade[ A_n ] * tA + red_s_shade[ B_n ] * tB +   
          red_s_shade[ C_n ] * tC + red_s_shade[ D_n ] * tD + 
          red_s_shade[ E_n ] * tE + red_s_shade[ F_n ] * tF +   
          red_s_shade[ G_n ] * tG + red_s_shade[ H_n ] * tH;
        
        r = GTF[(scalar_value)];

        // For this sample we have do not yet have any opacity or
        // shaded intensity yet
        red_shaded_value   = opacity * ( final_rd * r + final_rs );
        
        // Accumulate intensity and opacity for this sample location
        ////////////////////////////////////////////////////////////////////////////////////////////
//         accum_red_intensity   += red_shaded_value * remaining_opacity;
        accum_red_intensity   += red_shaded_value * remaining_opacity * aObscuranceFactor * ( vColor.x + vColor.y + vColor.z ) / 3.0;
        ////////////////////////////////////////////////////////////////////////////////////////////
        remaining_opacity *= (1.0 - opacity);
        }

      // Increment our position and compute our voxel location
      ray_position[0] += ray_increment[0];
      ray_position[1] += ray_increment[1];
      ray_position[2] += ray_increment[2];      
      voxel[0] = vtkFloorFuncMacro( ray_position[0] );
      voxel[1] = vtkFloorFuncMacro( ray_position[1] );
      voxel[2] = vtkFloorFuncMacro( ray_position[2] );
      }
    accum_green_intensity = accum_red_intensity;
    accum_blue_intensity = accum_red_intensity;
    }
  else if ( staticInfo->ColorChannels == 3 )
    {
    // For each step along the ray
    for ( loop = 0; 
          loop < num_steps && remaining_opacity > VTK_REMAINING_OPACITY; 
          loop++ )
      {     
      // We've taken another step
      steps_this_ray++;
      
      offset = voxel[2] * zinc + voxel[1] * yinc + voxel[0];
      dptr = data_ptr + offset;
      nptr = encoded_normals + offset;
    
      A = *(dptr);
      B = *(dptr + Binc);
      C = *(dptr + Cinc);
      D = *(dptr + Dinc);
      E = *(dptr + Einc);
      F = *(dptr + Finc);
      G = *(dptr + Ginc);
      H = *(dptr + Hinc);
      
      // Compute our offset in the voxel, and use that to trilinearly
      // interpolate a value
      x = ray_position[0] - (float) voxel[0];
      y = ray_position[1] - (float) voxel[1];
      z = ray_position[2] - (float) voxel[2];
      
      t1 = 1.0 - x;
      t2 = 1.0 - y;
      t3 = 1.0 - z;
      
      tA = t1 * t2 * t3;
      tB =  x * t2 * t3;
      tC = t1 *  y * t3;
      tD =  x *  y * t3;
      tE = t1 * t2 *  z;
      tF =  x * t2 *  z;
      tG = t1 *  y *  z;
      tH =  x *  y *  z;
      
      scalar_value = (int) (
                            A * tA + B * tB + C * tC + D * tD + 
                            E * tE + F * tF + G * tG + H * tH );

      //////////////////////////////////////////////////////////////////////////////////////////////
      udg::Vector3 vColor = aColorBleeding[offset       ] * tA
                          + aColorBleeding[offset + Binc] * tB
                          + aColorBleeding[offset + Cinc] * tC
                          + aColorBleeding[offset + Dinc] * tD
                          + aColorBleeding[offset + Einc] * tE
                          + aColorBleeding[offset + Finc] * tF
                          + aColorBleeding[offset + Ginc] * tG
                          + aColorBleeding[offset + Hinc] * tH;
      //////////////////////////////////////////////////////////////////////////////////////////////
      
      if ( scalar_value < 0 ) 
        {
        scalar_value = 0;
        }
      else if ( scalar_value > staticInfo->Volume->GetArraySize() - 1 )
        {
        scalar_value = (int)(staticInfo->Volume->GetArraySize() - 1);
        }
      
      opacity = SOTF[scalar_value];
      
      if ( opacity )
        {
          if ( !grad_op_is_constant )
            {
            gmptr = grad_mag_ptr + offset;
      
            A = *(gmptr);
            B = *(gmptr + Binc);
            C = *(gmptr + Cinc);
            D = *(gmptr + Dinc);
            E = *(gmptr + Einc);
            F = *(gmptr + Finc);
            G = *(gmptr + Ginc);
            H = *(gmptr + Hinc);
            
            gradient_value = (int) (
                                    A * tA + B * tB + C * tC + D * tD + 
                                    E * tE + F * tF + G * tG + H * tH );
            if ( gradient_value < 0 )
              {
              gradient_value = 0;
              }
            else if ( gradient_value > 255 )
              {
              gradient_value = 255;
              }

            opacity *= GOTF[gradient_value];
            }
          else
            {
            opacity *= gradient_opacity_constant;
            }
        }

      // If we have a combined opacity value, then compute the shading
      if ( opacity )
        {
        A_n = *(nptr);
        B_n = *(nptr + Binc);
        C_n = *(nptr + Cinc);
        D_n = *(nptr + Dinc);
        E_n = *(nptr + Einc);
        F_n = *(nptr + Finc);
        G_n = *(nptr + Ginc);
        H_n = *(nptr + Hinc);
        
        final_rd = 
          red_d_shade[ A_n ] * tA + red_d_shade[ B_n ] * tB +   
          red_d_shade[ C_n ] * tC + red_d_shade[ D_n ] * tD + 
          red_d_shade[ E_n ] * tE + red_d_shade[ F_n ] * tF +   
          red_d_shade[ G_n ] * tG + red_d_shade[ H_n ] * tH;
        
        final_gd = 
          green_d_shade[ A_n ] * tA + green_d_shade[ B_n ] * tB +       
          green_d_shade[ C_n ] * tC + green_d_shade[ D_n ] * tD + 
          green_d_shade[ E_n ] * tE + green_d_shade[ F_n ] * tF +       
          green_d_shade[ G_n ] * tG + green_d_shade[ H_n ] * tH;
        
        final_bd = 
          blue_d_shade[ A_n ] * tA + blue_d_shade[ B_n ] * tB +         
          blue_d_shade[ C_n ] * tC + blue_d_shade[ D_n ] * tD + 
          blue_d_shade[ E_n ] * tE + blue_d_shade[ F_n ] * tF + 
          blue_d_shade[ G_n ] * tG + blue_d_shade[ H_n ] * tH;
        
        final_rs = 
          red_s_shade[ A_n ] * tA + red_s_shade[ B_n ] * tB +   
          red_s_shade[ C_n ] * tC + red_s_shade[ D_n ] * tD + 
          red_s_shade[ E_n ] * tE + red_s_shade[ F_n ] * tF +   
          red_s_shade[ G_n ] * tG + red_s_shade[ H_n ] * tH;
        
        final_gs = 
          green_s_shade[ A_n ] * tA + green_s_shade[ B_n ] * tB +       
          green_s_shade[ C_n ] * tC + green_s_shade[ D_n ] * tD + 
          green_s_shade[ E_n ] * tE + green_s_shade[ F_n ] * tF +       
          green_s_shade[ G_n ] * tG + green_s_shade[ H_n ] * tH;
        
        final_bs = 
          blue_s_shade[ A_n ] * tA + blue_s_shade[ B_n ] * tB +         
          blue_s_shade[ C_n ] * tC + blue_s_shade[ D_n ] * tD + 
          blue_s_shade[ E_n ] * tE + blue_s_shade[ F_n ] * tF + 
          blue_s_shade[ G_n ] * tG + blue_s_shade[ H_n ] * tH;
        
        r = CTF[(scalar_value) * 3    ];
        g = CTF[(scalar_value) * 3 + 1];
        b = CTF[(scalar_value) * 3 + 2];
        
        // For this sample we have do not yet have any opacity or
        // shaded intensity yet
        red_shaded_value   = opacity * ( final_rd * r + final_rs );
        green_shaded_value = opacity * ( final_gd * g + final_gs );
        blue_shaded_value  = opacity * ( final_bd * b + final_bs );
        
        // Accumulate intensity and opacity for this sample location
        ////////////////////////////////////////////////////////////////////////////////////////////
//         accum_red_intensity   += red_shaded_value   * remaining_opacity;
//         accum_green_intensity += green_shaded_value * remaining_opacity;
//         accum_blue_intensity  += blue_shaded_value  * remaining_opacity;
        accum_red_intensity   += red_shaded_value   * remaining_opacity * aObscuranceFactor * vColor.x;
        accum_green_intensity += green_shaded_value * remaining_opacity * aObscuranceFactor * vColor.y;
        accum_blue_intensity  += blue_shaded_value  * remaining_opacity * aObscuranceFactor * vColor.z;
        ////////////////////////////////////////////////////////////////////////////////////////////
        remaining_opacity *= (1.0 - opacity);
        }

      // Increment our position and compute our voxel location
      ray_position[0] += ray_increment[0];
      ray_position[1] += ray_increment[1];
      ray_position[2] += ray_increment[2];      
      voxel[0] = vtkFloorFuncMacro( ray_position[0] );
      voxel[1] = vtkFloorFuncMacro( ray_position[1] );
      voxel[2] = vtkFloorFuncMacro( ray_position[2] );
      }
    }

  // Cap the accumulated intensity at 1.0
  if ( accum_red_intensity > 1.0 )
    {
    accum_red_intensity = 1.0;
    }
  if ( accum_green_intensity > 1.0 )
    {
    accum_green_intensity = 1.0;
    }
  if ( accum_blue_intensity > 1.0 )
    {
    accum_blue_intensity = 1.0;
    }
  
  if( remaining_opacity < VTK_REMAINING_OPACITY )
    {
    remaining_opacity = 0.0;
    }

  // Set the return pixel value.  The depth value is the distance to the
  // center of the volume.
  dynamicInfo->Color[0] = accum_red_intensity;
  dynamicInfo->Color[1] = accum_green_intensity;
  dynamicInfo->Color[2] = accum_blue_intensity;
  dynamicInfo->Color[3] = 1.0 - remaining_opacity;
  dynamicInfo->NumberOfStepsTaken = steps_this_ray;

 
}


// Description:
// This is the templated function that actually casts a ray and computes
// the composite value.  This version uses trilinear interpolation and
// does not compute shading
template <class T>
void vtkCastRay_TrilinVertices_Unshaded( T *data_ptr, vtkVolumeRayCastDynamicInfo *dynamicInfo,
                                         vtkVolumeRayCastStaticInfo *staticInfo,
                                         double * aObscurance, double aObscuranceFactor, double aObscuranceFilterLow, double aObscuranceFilterHigh,
                                         bool aFxObscurance, double aFxContour )
{
  unsigned char   *grad_mag_ptr = NULL;
  unsigned char   *goptr;
  float           accum_red_intensity;
  float           accum_green_intensity;
  float           accum_blue_intensity;
  float           remaining_opacity;
  float           red_value, green_value, blue_value;
  float           opacity;
  int             loop;
  int             xinc, yinc, zinc;
  int             voxel[3];
  float           ray_position[3];
  int             prev_voxel[3];
  float           A, B, C, D, E, F, G, H;
  float           Ago, Bgo, Cgo, Dgo, Ego, Fgo, Ggo, Hgo;
  int             Binc, Cinc, Dinc, Einc, Finc, Ginc, Hinc;
  T               *dptr;
  float           *SOTF;
  float           *CTF;
  float           *GTF;
  float           *GOTF;
  float           x, y, z, t1, t2, t3;
  float           weight;
  int             offset;
  int             steps_this_ray = 0;
  int             num_steps;
  float           *ray_start, *ray_increment;
  int             grad_op_is_constant;
  float           gradient_opacity_constant;

  num_steps = dynamicInfo->NumberOfStepsToTake;
  ray_start = dynamicInfo->TransformedStart;
  ray_increment = dynamicInfo->TransformedIncrement;
 
  // Get the scalar opacity transfer function which maps scalar input values
  // to opacities
  SOTF =  staticInfo->Volume->GetCorrectedScalarOpacityArray();

  // Get the color transfer function which maps scalar input values
  // to RGB colors
  CTF =  staticInfo->Volume->GetRGBArray();
  GTF =  staticInfo->Volume->GetGrayArray();

  // Get the gradient opacity transfer function for this volume (which maps
  // gradient magnitudes to opacities)
  GOTF =  staticInfo->Volume->GetGradientOpacityArray();

  // Get the gradient opacity constant. If this number is greater than
  // or equal to 0.0, then the gradient opacity transfer function is
  // a constant at that value, otherwise it is not a constant function
  gradient_opacity_constant = staticInfo->Volume->GetGradientOpacityConstant();
  grad_op_is_constant = ( gradient_opacity_constant >= 0.0 );

  // Get a pointer to the gradient magnitudes for this volume
  if ( !grad_op_is_constant )
    {
    grad_mag_ptr = staticInfo->GradientMagnitudes;
    }

  // Move the increments into local variables
  xinc = staticInfo->DataIncrement[0];
  yinc = staticInfo->DataIncrement[1];
  zinc = staticInfo->DataIncrement[2];

  // Initialize the ray position and voxel location
  ray_position[0] = ray_start[0];
  ray_position[1] = ray_start[1];
  ray_position[2] = ray_start[2];
  voxel[0] = vtkFloorFuncMacro( ray_position[0] );
  voxel[1] = vtkFloorFuncMacro( ray_position[1] );
  voxel[2] = vtkFloorFuncMacro( ray_position[2] );

  // So far we have not accumulated anything
  accum_red_intensity     = 0.0;
  accum_green_intensity   = 0.0;
  accum_blue_intensity    = 0.0;
  remaining_opacity       = 1.0;

  // Compute the increments to get to the other 7 voxel vertices from A
  Binc = xinc;
  Cinc = yinc;
  Dinc = xinc + yinc;
  Einc = zinc;
  Finc = zinc + xinc;
  Ginc = zinc + yinc;
  Hinc = zinc + xinc + yinc;
  
  // Compute the values for the first pass through the loop
  offset = voxel[2] * zinc + voxel[1] * yinc + voxel[0];
  dptr   = data_ptr + offset;

  A = SOTF[(*(dptr))];
  B = SOTF[(*(dptr + Binc))];
  C = SOTF[(*(dptr + Cinc))];
  D = SOTF[(*(dptr + Dinc))];
  E = SOTF[(*(dptr + Einc))];
  F = SOTF[(*(dptr + Finc))];
  G = SOTF[(*(dptr + Ginc))];
  H = SOTF[(*(dptr + Hinc))];  

  if ( !grad_op_is_constant )
    {
    goptr  = grad_mag_ptr + offset;
    Ago = GOTF[(*(goptr))];
    Bgo = GOTF[(*(goptr + Binc))];
    Cgo = GOTF[(*(goptr + Cinc))];
    Dgo = GOTF[(*(goptr + Dinc))];
    Ego = GOTF[(*(goptr + Einc))];
    Fgo = GOTF[(*(goptr + Finc))];
    Ggo = GOTF[(*(goptr + Ginc))];
    Hgo = GOTF[(*(goptr + Hinc))];  
    }
  else 
    {
    Ago = Bgo = Cgo = Dgo = Ego = Fgo = Ggo = Hgo = 1.0;
    }

  // Keep track of previous voxel to know when we step into a new one  
  prev_voxel[0] = voxel[0];
  prev_voxel[1] = voxel[1];
  prev_voxel[2] = voxel[2];

  //////////////////////////////////////////////////////////////////////////////////////////////////
  udg::Vector3 fxDirection( dynamicInfo->TransformedDirection[0], dynamicInfo->TransformedDirection[1], dynamicInfo->TransformedDirection[2] );
  fxDirection.normalize();
  vtkVolumeRayCastMapper * fxMapper = vtkVolumeRayCastMapper::SafeDownCast( staticInfo->Volume->GetMapper() );
  vtkEncodedGradientEstimator * fxGradientEstimator = fxMapper->GetGradientEstimator();
  unsigned short * fxEncodedNormals = fxGradientEstimator->GetEncodedNormals();
  vtkDirectionEncoder * fxDirectionEncoder = fxGradientEstimator->GetDirectionEncoder();
  //////////////////////////////////////////////////////////////////////////////////////////////////

  // Two cases - we are working with a gray or RGB transfer
  // function - break them up to make it more efficient
  if ( staticInfo->ColorChannels == 1 ) 
    {
    // For each step along the ray
    for ( loop = 0; 
          loop < num_steps && remaining_opacity > VTK_REMAINING_OPACITY; 
          loop++ )
      {     
      // We've taken another step
      steps_this_ray++;
      
      // Have we moved into a new voxel? If so we need to recompute A-H
      if ( prev_voxel[0] != voxel[0] ||
           prev_voxel[1] != voxel[1] ||
           prev_voxel[2] != voxel[2] )
        {
        offset = voxel[2] * zinc + voxel[1] * yinc + voxel[0];
        dptr   = data_ptr + offset;
        
        A = SOTF[(*(dptr))];
        B = SOTF[(*(dptr + Binc))];
        C = SOTF[(*(dptr + Cinc))];
        D = SOTF[(*(dptr + Dinc))];
        E = SOTF[(*(dptr + Einc))];
        F = SOTF[(*(dptr + Finc))];
        G = SOTF[(*(dptr + Ginc))];
        H = SOTF[(*(dptr + Hinc))];

        if ( !grad_op_is_constant )
          {
          goptr  = grad_mag_ptr + offset;
          Ago = GOTF[(*(goptr))];
          Bgo = GOTF[(*(goptr + Binc))];
          Cgo = GOTF[(*(goptr + Cinc))];
          Dgo = GOTF[(*(goptr + Dinc))];
          Ego = GOTF[(*(goptr + Einc))];
          Fgo = GOTF[(*(goptr + Finc))];
          Ggo = GOTF[(*(goptr + Ginc))];
          Hgo = GOTF[(*(goptr + Hinc))];  
          }
        else 
          {
            Ago = Bgo = Cgo = Dgo = Ego = Fgo = Ggo = Hgo = 1.0;
          }

        prev_voxel[0] = voxel[0];
        prev_voxel[1] = voxel[1];
        prev_voxel[2] = voxel[2];
        }
      
      // Compute our offset in the voxel, and use that to trilinearly
      // interpolate the value
      x = ray_position[0] - (float) voxel[0];
      y = ray_position[1] - (float) voxel[1];
      z = ray_position[2] - (float) voxel[2];
      
      t1 = 1.0 - x;
      t2 = 1.0 - y;
      t3 = 1.0 - z;
      
      // For this sample we have do not yet have any opacity
      opacity          = 0.0;
      red_value        = 0.0;
      
      // Now add the opacity in vertex by vertex.  If any of the A-H
      // have a non-transparent opacity value, then add its contribution
      // to the opacity
      if ( A && Ago )
        {
        weight     = t1*t2*t3 * A * Ago;
        opacity   += weight;
        ////////////////////////////////////////////////////////////////////////////////////////////
//         red_value += weight * GTF[((*dptr))];

        double fx = 1.0;

        // contour
        if ( aFxContour > 0.0 )
        {
          float * fxGradient = fxDirectionEncoder->GetDecodedGradient( fxEncodedNormals[offset] );
          udg::Vector3 fxNormal( fxGradient[0], fxGradient[1], fxGradient[2] );
          double fxDotProduct = fxDirection * fxNormal;
          if ( fxDotProduct < 0.0 ) fxDotProduct = -fxDotProduct;
          fx *= fxDotProduct < aFxContour ? 0.0 : 1.0;
        }

        // obscurance
        if ( aFxObscurance && fx > 0.0 )
        {
          double fxObscurance = aObscurance[offset];
          if ( fxObscurance < aObscuranceFilterLow ) fxObscurance = 0.0;
          else if ( fxObscurance > aObscuranceFilterHigh ) fxObscurance = 1.0;
          fx *= aObscuranceFactor * fxObscurance;
        }

        red_value += weight * GTF[((*dptr))] * fx;
        ////////////////////////////////////////////////////////////////////////////////////////////
        }
      
      if ( B && Bgo )
        {
        weight     = x*t2*t3 * B * Bgo;
        opacity   += weight;
        ////////////////////////////////////////////////////////////////////////////////////////////
//         red_value += weight * GTF[((*(dptr + Binc)))];

        double fx = 1.0;

        // contour
        if ( aFxContour > 0.0 )
        {
          float * fxGradient = fxDirectionEncoder->GetDecodedGradient( fxEncodedNormals[offset + Binc] );
          udg::Vector3 fxNormal( fxGradient[0], fxGradient[1], fxGradient[2] );
          double fxDotProduct = fxDirection * fxNormal;
          if ( fxDotProduct < 0.0 ) fxDotProduct = -fxDotProduct;
          fx *= fxDotProduct < aFxContour ? 0.0 : 1.0;
        }

        // obscurance
        if ( aFxObscurance && fx > 0.0 )
        {
          double fxObscurance = aObscurance[offset + Binc];
          if ( fxObscurance < aObscuranceFilterLow ) fxObscurance = 0.0;
          else if ( fxObscurance > aObscuranceFilterHigh ) fxObscurance = 1.0;
          fx *= aObscuranceFactor * fxObscurance;
        }

        red_value += weight * GTF[((*(dptr + Binc)))] * fx;
        ////////////////////////////////////////////////////////////////////////////////////////////
        }
      
      if ( C && Cgo )
        {
        weight     = t1*y*t3 * C * Cgo;
        opacity   += weight;
        ////////////////////////////////////////////////////////////////////////////////////////////
//         red_value += weight * GTF[((*(dptr + Cinc)))];

        double fx = 1.0;

        // contour
        if ( aFxContour > 0.0 )
        {
          float * fxGradient = fxDirectionEncoder->GetDecodedGradient( fxEncodedNormals[offset + Cinc] );
          udg::Vector3 fxNormal( fxGradient[0], fxGradient[1], fxGradient[2] );
          double fxDotProduct = fxDirection * fxNormal;
          if ( fxDotProduct < 0.0 ) fxDotProduct = -fxDotProduct;
          fx *= fxDotProduct < aFxContour ? 0.0 : 1.0;
        }

        // obscurance
        if ( aFxObscurance && fx > 0.0 )
        {
          double fxObscurance = aObscurance[offset + Cinc];
          if ( fxObscurance < aObscuranceFilterLow ) fxObscurance = 0.0;
          else if ( fxObscurance > aObscuranceFilterHigh ) fxObscurance = 1.0;
          fx *= aObscuranceFactor * fxObscurance;
        }

        red_value += weight * GTF[((*(dptr + Cinc)))] * fx;
        ////////////////////////////////////////////////////////////////////////////////////////////
        }
      
      if ( D && Dgo )
        {
        weight     = x*y*t3 * D * Dgo;
        opacity   += weight;
        ////////////////////////////////////////////////////////////////////////////////////////////
//         red_value += weight * GTF[((*(dptr + Dinc)))];

        double fx = 1.0;

        // contour
        if ( aFxContour > 0.0 )
        {
          float * fxGradient = fxDirectionEncoder->GetDecodedGradient( fxEncodedNormals[offset + Dinc] );
          udg::Vector3 fxNormal( fxGradient[0], fxGradient[1], fxGradient[2] );
          double fxDotProduct = fxDirection * fxNormal;
          if ( fxDotProduct < 0.0 ) fxDotProduct = -fxDotProduct;
          fx *= fxDotProduct < aFxContour ? 0.0 : 1.0;
        }

        // obscurance
        if ( aFxObscurance && fx > 0.0 )
        {
          double fxObscurance = aObscurance[offset + Dinc];
          if ( fxObscurance < aObscuranceFilterLow ) fxObscurance = 0.0;
          else if ( fxObscurance > aObscuranceFilterHigh ) fxObscurance = 1.0;
          fx *= aObscuranceFactor * fxObscurance;
        }

        red_value += weight * GTF[((*(dptr + Dinc)))] * fx;
        ////////////////////////////////////////////////////////////////////////////////////////////
        }
      
      if ( E && Ego )
        {
        weight     = t1*t2*z * E * Ego; 
        opacity   += weight;
        ////////////////////////////////////////////////////////////////////////////////////////////
//         red_value += weight * GTF[((*(dptr + Einc)))];

        double fx = 1.0;

        // contour
        if ( aFxContour > 0.0 )
        {
          float * fxGradient = fxDirectionEncoder->GetDecodedGradient( fxEncodedNormals[offset + Einc] );
          udg::Vector3 fxNormal( fxGradient[0], fxGradient[1], fxGradient[2] );
          double fxDotProduct = fxDirection * fxNormal;
          if ( fxDotProduct < 0.0 ) fxDotProduct = -fxDotProduct;
          fx *= fxDotProduct < aFxContour ? 0.0 : 1.0;
        }

        // obscurance
        if ( aFxObscurance && fx > 0.0 )
        {
          double fxObscurance = aObscurance[offset + Einc];
          if ( fxObscurance < aObscuranceFilterLow ) fxObscurance = 0.0;
          else if ( fxObscurance > aObscuranceFilterHigh ) fxObscurance = 1.0;
          fx *= aObscuranceFactor * fxObscurance;
        }

        red_value += weight * GTF[((*(dptr + Einc)))] * fx;
        ////////////////////////////////////////////////////////////////////////////////////////////
        }
      
      if ( F && Fgo )
        {
        weight     = x*t2*z * F * Fgo;
        opacity   += weight;
        ////////////////////////////////////////////////////////////////////////////////////////////
//         red_value += weight * GTF[((*(dptr + Finc)))];

        double fx = 1.0;

        // contour
        if ( aFxContour > 0.0 )
        {
          float * fxGradient = fxDirectionEncoder->GetDecodedGradient( fxEncodedNormals[offset + Finc] );
          udg::Vector3 fxNormal( fxGradient[0], fxGradient[1], fxGradient[2] );
          double fxDotProduct = fxDirection * fxNormal;
          if ( fxDotProduct < 0.0 ) fxDotProduct = -fxDotProduct;
          fx *= fxDotProduct < aFxContour ? 0.0 : 1.0;
        }

        // obscurance
        if ( aFxObscurance && fx > 0.0 )
        {
          double fxObscurance = aObscurance[offset + Finc];
          if ( fxObscurance < aObscuranceFilterLow ) fxObscurance = 0.0;
          else if ( fxObscurance > aObscuranceFilterHigh ) fxObscurance = 1.0;
          fx *= aObscuranceFactor * fxObscurance;
        }

        red_value += weight * GTF[((*(dptr + Finc)))] * fx;
        ////////////////////////////////////////////////////////////////////////////////////////////
        }
      
      if ( G && Ggo )
        {
        weight     = t1*y*z * G * Ggo;
        opacity   += weight;
        ////////////////////////////////////////////////////////////////////////////////////////////
//         red_value += weight * GTF[((*(dptr + Ginc)))];

        double fx = 1.0;

        // contour
        if ( aFxContour > 0.0 )
        {
          float * fxGradient = fxDirectionEncoder->GetDecodedGradient( fxEncodedNormals[offset + Ginc] );
          udg::Vector3 fxNormal( fxGradient[0], fxGradient[1], fxGradient[2] );
          double fxDotProduct = fxDirection * fxNormal;
          if ( fxDotProduct < 0.0 ) fxDotProduct = -fxDotProduct;
          fx *= fxDotProduct < aFxContour ? 0.0 : 1.0;
        }

        // obscurance
        if ( aFxObscurance && fx > 0.0 )
        {
          double fxObscurance = aObscurance[offset + Ginc];
          if ( fxObscurance < aObscuranceFilterLow ) fxObscurance = 0.0;
          else if ( fxObscurance > aObscuranceFilterHigh ) fxObscurance = 1.0;
          fx *= aObscuranceFactor * fxObscurance;
        }

        red_value += weight * GTF[((*(dptr + Ginc)))] * fx;
        ////////////////////////////////////////////////////////////////////////////////////////////
        } 
      
      if ( H && Hgo )
        {
        weight     = x*z*y * H * Hgo;
        opacity   += weight;
        ////////////////////////////////////////////////////////////////////////////////////////////
//         red_value += weight * GTF[((*(dptr + Hinc)))];

        double fx = 1.0;

        // contour
        if ( aFxContour > 0.0 )
        {
          float * fxGradient = fxDirectionEncoder->GetDecodedGradient( fxEncodedNormals[offset + Hinc] );
          udg::Vector3 fxNormal( fxGradient[0], fxGradient[1], fxGradient[2] );
          double fxDotProduct = fxDirection * fxNormal;
          if ( fxDotProduct < 0.0 ) fxDotProduct = -fxDotProduct;
          fx *= fxDotProduct < aFxContour ? 0.0 : 1.0;
        }

        // obscurance
        if ( aFxObscurance && fx > 0.0 )
        {
          double fxObscurance = aObscurance[offset + Hinc];
          if ( fxObscurance < aObscuranceFilterLow ) fxObscurance = 0.0;
          else if ( fxObscurance > aObscuranceFilterHigh ) fxObscurance = 1.0;
          fx *= aObscuranceFactor * fxObscurance;
        }

        red_value += weight * GTF[((*(dptr + Hinc)))] * fx;
        ////////////////////////////////////////////////////////////////////////////////////////////
        }
      
      // Accumulate intensity and opacity for this sample location
      accum_red_intensity   += remaining_opacity * red_value;
      remaining_opacity *= (1.0 - opacity);
      
      // Increment our position and compute our voxel location
      ray_position[0] += ray_increment[0];
      ray_position[1] += ray_increment[1];
      ray_position[2] += ray_increment[2];      
      voxel[0] = vtkFloorFuncMacro( ray_position[0] );
      voxel[1] = vtkFloorFuncMacro( ray_position[1] );
      voxel[2] = vtkFloorFuncMacro( ray_position[2] );
      }
    accum_green_intensity = accum_red_intensity;
    accum_blue_intensity = accum_red_intensity;
    }
  else if ( staticInfo->ColorChannels == 3 )
    {
    // For each step along the ray
    for ( loop = 0; 
          loop < num_steps && remaining_opacity > VTK_REMAINING_OPACITY; 
          loop++ )
      {     
      // We've taken another step
      steps_this_ray++;
      
      // Have we moved into a new voxel? If so we need to recompute A-H
      if ( prev_voxel[0] != voxel[0] ||
           prev_voxel[1] != voxel[1] ||
           prev_voxel[2] != voxel[2] )
        {
        offset = voxel[2] * zinc + voxel[1] * yinc + voxel[0];
        dptr = data_ptr + offset;
        
        A = SOTF[(*(dptr))];
        B = SOTF[(*(dptr + Binc))];
        C = SOTF[(*(dptr + Cinc))];
        D = SOTF[(*(dptr + Dinc))];
        E = SOTF[(*(dptr + Einc))];
        F = SOTF[(*(dptr + Finc))];
        G = SOTF[(*(dptr + Ginc))];
        H = SOTF[(*(dptr + Hinc))];

        if ( !grad_op_is_constant ) 
          {
          goptr  = grad_mag_ptr + offset;
          Ago = GOTF[(*(goptr))];
          Bgo = GOTF[(*(goptr + Binc))];
          Cgo = GOTF[(*(goptr + Cinc))];
          Dgo = GOTF[(*(goptr + Dinc))];
          Ego = GOTF[(*(goptr + Einc))];
          Fgo = GOTF[(*(goptr + Finc))];
          Ggo = GOTF[(*(goptr + Ginc))];
          Hgo = GOTF[(*(goptr + Hinc))];  
          }
        else 
          {
            Ago = Bgo = Cgo = Dgo = Ego = Fgo = Ggo = Hgo = 1.0;
          }

        prev_voxel[0] = voxel[0];
        prev_voxel[1] = voxel[1];
        prev_voxel[2] = voxel[2];
        }
      
      // Compute our offset in the voxel, and use that to trilinearly
      // interpolate the value
      x = ray_position[0] - (float) voxel[0];
      y = ray_position[1] - (float) voxel[1];
      z = ray_position[2] - (float) voxel[2];
      
      t1 = 1.0 - x;
      t2 = 1.0 - y;
      t3 = 1.0 - z;
      
      // For this sample we have do not yet have any opacity
      opacity           = 0.0;
      red_value         = 0.0;
      green_value       = 0.0;
      blue_value        = 0.0;
      
      // Now add the opacity in vertex by vertex.  If any of the A-H
      // have a non-transparent opacity value, then add its contribution
      // to the opacity
      if ( A && Ago )
        {
        weight         = t1*t2*t3 * A * Ago;
        opacity       += weight;
        ////////////////////////////////////////////////////////////////////////////////////////////
//         red_value     += weight * CTF[((*dptr)) * 3    ];
//         green_value   += weight * CTF[((*dptr)) * 3 + 1];
//         blue_value    += weight * CTF[((*dptr)) * 3 + 2];

        double fx = 1.0;

        // contour
        if ( aFxContour > 0.0 )
        {
          float * fxGradient = fxDirectionEncoder->GetDecodedGradient( fxEncodedNormals[offset] );
          udg::Vector3 fxNormal( fxGradient[0], fxGradient[1], fxGradient[2] );
          double fxDotProduct = fxDirection * fxNormal;
          if ( fxDotProduct < 0.0 ) fxDotProduct = -fxDotProduct;
          fx *= fxDotProduct < aFxContour ? 0.0 : 1.0;
        }

        // obscurance
        if ( aFxObscurance && fx > 0.0 )
        {
          double fxObscurance = aObscurance[offset];
          if ( fxObscurance < aObscuranceFilterLow ) fxObscurance = 0.0;
          else if ( fxObscurance > aObscuranceFilterHigh ) fxObscurance = 1.0;
          fx *= aObscuranceFactor * fxObscurance;
        }

        red_value     += weight * CTF[((*dptr)) * 3    ] * fx;
        green_value   += weight * CTF[((*dptr)) * 3 + 1] * fx;
        blue_value    += weight * CTF[((*dptr)) * 3 + 2] * fx;
        ////////////////////////////////////////////////////////////////////////////////////////////
        }
      
      if ( B && Bgo )
        {
        weight         = x*t2*t3 * B * Bgo;
        opacity       += weight;
        ////////////////////////////////////////////////////////////////////////////////////////////
//         red_value     += weight * CTF[((*(dptr + Binc))) * 3    ];
//         green_value   += weight * CTF[((*(dptr + Binc))) * 3 + 1];
//         blue_value    += weight * CTF[((*(dptr + Binc))) * 3 + 2];

        double fx = 1.0;

        // contour
        if ( aFxContour > 0.0 )
        {
          float * fxGradient = fxDirectionEncoder->GetDecodedGradient( fxEncodedNormals[offset + Binc] );
          udg::Vector3 fxNormal( fxGradient[0], fxGradient[1], fxGradient[2] );
          double fxDotProduct = fxDirection * fxNormal;
          if ( fxDotProduct < 0.0 ) fxDotProduct = -fxDotProduct;
          fx *= fxDotProduct < aFxContour ? 0.0 : 1.0;
        }

        // obscurance
        if ( aFxObscurance && fx > 0.0 )
        {
          double fxObscurance = aObscurance[offset + Binc];
          if ( fxObscurance < aObscuranceFilterLow ) fxObscurance = 0.0;
          else if ( fxObscurance > aObscuranceFilterHigh ) fxObscurance = 1.0;
          fx *= aObscuranceFactor * fxObscurance;
        }

        red_value     += weight * CTF[((*(dptr + Binc))) * 3    ] * fx;
        green_value   += weight * CTF[((*(dptr + Binc))) * 3 + 1] * fx;
        blue_value    += weight * CTF[((*(dptr + Binc))) * 3 + 2] * fx;
        ////////////////////////////////////////////////////////////////////////////////////////////
        }
      
      if ( C && Cgo )
        {
        weight         = t1*y*t3 * C * Cgo;
        opacity       += weight;
        ////////////////////////////////////////////////////////////////////////////////////////////
//         red_value     += weight * CTF[((*(dptr + Cinc))) * 3    ];
//         green_value   += weight * CTF[((*(dptr + Cinc))) * 3 + 1];
//         blue_value    += weight * CTF[((*(dptr + Cinc))) * 3 + 2];

        double fx = 1.0;

        // contour
        if ( aFxContour > 0.0 )
        {
          float * fxGradient = fxDirectionEncoder->GetDecodedGradient( fxEncodedNormals[offset + Cinc] );
          udg::Vector3 fxNormal( fxGradient[0], fxGradient[1], fxGradient[2] );
          double fxDotProduct = fxDirection * fxNormal;
          if ( fxDotProduct < 0.0 ) fxDotProduct = -fxDotProduct;
          fx *= fxDotProduct < aFxContour ? 0.0 : 1.0;
        }

        // obscurance
        if ( aFxObscurance && fx > 0.0 )
        {
          double fxObscurance = aObscurance[offset + Cinc];
          if ( fxObscurance < aObscuranceFilterLow ) fxObscurance = 0.0;
          else if ( fxObscurance > aObscuranceFilterHigh ) fxObscurance = 1.0;
          fx *= aObscuranceFactor * fxObscurance;
        }

        red_value     += weight * CTF[((*(dptr + Cinc))) * 3    ] * fx;
        green_value   += weight * CTF[((*(dptr + Cinc))) * 3 + 1] * fx;
        blue_value    += weight * CTF[((*(dptr + Cinc))) * 3 + 2] * fx;
        ////////////////////////////////////////////////////////////////////////////////////////////
        }
      
      if ( D && Dgo )
        {
        weight         = x*y*t3 * D * Dgo;
        opacity       += weight;
        ////////////////////////////////////////////////////////////////////////////////////////////
//         red_value     += weight * CTF[((*(dptr + Dinc))) * 3    ];
//         green_value   += weight * CTF[((*(dptr + Dinc))) * 3 + 1];
//         blue_value    += weight * CTF[((*(dptr + Dinc))) * 3 + 2];

        double fx = 1.0;

        // contour
        if ( aFxContour > 0.0 )
        {
          float * fxGradient = fxDirectionEncoder->GetDecodedGradient( fxEncodedNormals[offset + Dinc] );
          udg::Vector3 fxNormal( fxGradient[0], fxGradient[1], fxGradient[2] );
          double fxDotProduct = fxDirection * fxNormal;
          if ( fxDotProduct < 0.0 ) fxDotProduct = -fxDotProduct;
          fx *= fxDotProduct < aFxContour ? 0.0 : 1.0;
        }

        // obscurance
        if ( aFxObscurance && fx > 0.0 )
        {
          double fxObscurance = aObscurance[offset + Dinc];
          if ( fxObscurance < aObscuranceFilterLow ) fxObscurance = 0.0;
          else if ( fxObscurance > aObscuranceFilterHigh ) fxObscurance = 1.0;
          fx *= aObscuranceFactor * fxObscurance;
        }

        red_value     += weight * CTF[((*(dptr + Dinc))) * 3    ] * fx;
        green_value   += weight * CTF[((*(dptr + Dinc))) * 3 + 1] * fx;
        blue_value    += weight * CTF[((*(dptr + Dinc))) * 3 + 2] * fx;
        ////////////////////////////////////////////////////////////////////////////////////////////
        }
      
      if ( E && Ego )
        {
        weight         = t1*t2*z * E * Ego;
        opacity       += weight;
        ////////////////////////////////////////////////////////////////////////////////////////////
//         red_value     += weight * CTF[((*(dptr + Einc))) * 3    ];
//         green_value   += weight * CTF[((*(dptr + Einc))) * 3 + 1];
//         blue_value    += weight * CTF[((*(dptr + Einc))) * 3 + 2];

        double fx = 1.0;

        // contour
        if ( aFxContour > 0.0 )
        {
          float * fxGradient = fxDirectionEncoder->GetDecodedGradient( fxEncodedNormals[offset + Einc] );
          udg::Vector3 fxNormal( fxGradient[0], fxGradient[1], fxGradient[2] );
          double fxDotProduct = fxDirection * fxNormal;
          if ( fxDotProduct < 0.0 ) fxDotProduct = -fxDotProduct;
          fx *= fxDotProduct < aFxContour ? 0.0 : 1.0;
        }

        // obscurance
        if ( aFxObscurance && fx > 0.0 )
        {
          double fxObscurance = aObscurance[offset + Einc];
          if ( fxObscurance < aObscuranceFilterLow ) fxObscurance = 0.0;
          else if ( fxObscurance > aObscuranceFilterHigh ) fxObscurance = 1.0;
          fx *= aObscuranceFactor * fxObscurance;
        }

        red_value     += weight * CTF[((*(dptr + Einc))) * 3    ] * fx;
        green_value   += weight * CTF[((*(dptr + Einc))) * 3 + 1] * fx;
        blue_value    += weight * CTF[((*(dptr + Einc))) * 3 + 2] * fx;
        ////////////////////////////////////////////////////////////////////////////////////////////
        }
      
      if ( F && Fgo )
        {
        weight         = x*t2*z * F * Fgo;
        opacity       += weight;
        ////////////////////////////////////////////////////////////////////////////////////////////
//         red_value     += weight * CTF[((*(dptr + Finc))) * 3    ];
//         green_value   += weight * CTF[((*(dptr + Finc))) * 3 + 1];
//         blue_value    += weight * CTF[((*(dptr + Finc))) * 3 + 2];

        double fx = 1.0;

        // contour
        if ( aFxContour > 0.0 )
        {
          float * fxGradient = fxDirectionEncoder->GetDecodedGradient( fxEncodedNormals[offset + Finc] );
          udg::Vector3 fxNormal( fxGradient[0], fxGradient[1], fxGradient[2] );
          double fxDotProduct = fxDirection * fxNormal;
          if ( fxDotProduct < 0.0 ) fxDotProduct = -fxDotProduct;
          fx *= fxDotProduct < aFxContour ? 0.0 : 1.0;
        }

        // obscurance
        if ( aFxObscurance && fx > 0.0 )
        {
          double fxObscurance = aObscurance[offset + Finc];
          if ( fxObscurance < aObscuranceFilterLow ) fxObscurance = 0.0;
          else if ( fxObscurance > aObscuranceFilterHigh ) fxObscurance = 1.0;
          fx *= aObscuranceFactor * fxObscurance;
        }

        red_value     += weight * CTF[((*(dptr + Finc))) * 3    ] * fx;
        green_value   += weight * CTF[((*(dptr + Finc))) * 3 + 1] * fx;
        blue_value    += weight * CTF[((*(dptr + Finc))) * 3 + 2] * fx;
        ////////////////////////////////////////////////////////////////////////////////////////////
        }
      
      if ( G && Ggo )
        {
        weight         = t1*y*z * G * Ggo;
        opacity       += weight;
        ////////////////////////////////////////////////////////////////////////////////////////////
//         red_value     +=  weight * CTF[((*(dptr + Ginc))) * 3    ];
//         green_value   +=  weight * CTF[((*(dptr + Ginc))) * 3 + 1];
//         blue_value    +=  weight * CTF[((*(dptr + Ginc))) * 3 + 2];

        double fx = 1.0;

        // contour
        if ( aFxContour > 0.0 )
        {
          float * fxGradient = fxDirectionEncoder->GetDecodedGradient( fxEncodedNormals[offset + Ginc] );
          udg::Vector3 fxNormal( fxGradient[0], fxGradient[1], fxGradient[2] );
          double fxDotProduct = fxDirection * fxNormal;
          if ( fxDotProduct < 0.0 ) fxDotProduct = -fxDotProduct;
          fx *= fxDotProduct < aFxContour ? 0.0 : 1.0;
        }

        // obscurance
        if ( aFxObscurance && fx > 0.0 )
        {
          double fxObscurance = aObscurance[offset + Ginc];
          if ( fxObscurance < aObscuranceFilterLow ) fxObscurance = 0.0;
          else if ( fxObscurance > aObscuranceFilterHigh ) fxObscurance = 1.0;
          fx *= aObscuranceFactor * fxObscurance;
        }

        red_value     +=  weight * CTF[((*(dptr + Ginc))) * 3    ] * fx;
        green_value   +=  weight * CTF[((*(dptr + Ginc))) * 3 + 1] * fx;
        blue_value    +=  weight * CTF[((*(dptr + Ginc))) * 3 + 2] * fx;
        ////////////////////////////////////////////////////////////////////////////////////////////
        } 
      
      if ( H && Hgo )
        {
        weight         = x*z*y * H * Hgo;
        opacity       += weight;
        ////////////////////////////////////////////////////////////////////////////////////////////
//         red_value     += weight * CTF[((*(dptr + Hinc))) * 3    ];
//         green_value   += weight * CTF[((*(dptr + Hinc))) * 3 + 1];
//         blue_value    += weight * CTF[((*(dptr + Hinc))) * 3 + 2];

        double fx = 1.0;

        // contour
        if ( aFxContour > 0.0 )
        {
          float * fxGradient = fxDirectionEncoder->GetDecodedGradient( fxEncodedNormals[offset + Hinc] );
          udg::Vector3 fxNormal( fxGradient[0], fxGradient[1], fxGradient[2] );
          double fxDotProduct = fxDirection * fxNormal;
          if ( fxDotProduct < 0.0 ) fxDotProduct = -fxDotProduct;
          fx *= fxDotProduct < aFxContour ? 0.0 : 1.0;
        }

        // obscurance
        if ( aFxObscurance && fx > 0.0 )
        {
          double fxObscurance = aObscurance[offset + Hinc];
          if ( fxObscurance < aObscuranceFilterLow ) fxObscurance = 0.0;
          else if ( fxObscurance > aObscuranceFilterHigh ) fxObscurance = 1.0;
          fx *= aObscuranceFactor * fxObscurance;
        }

        red_value     += weight * CTF[((*(dptr + Hinc))) * 3    ] * fx;
        green_value   += weight * CTF[((*(dptr + Hinc))) * 3 + 1] * fx;
        blue_value    += weight * CTF[((*(dptr + Hinc))) * 3 + 2] * fx;
        ////////////////////////////////////////////////////////////////////////////////////////////
        }
      
      // Accumulate intensity and opacity for this sample location
      accum_red_intensity   += remaining_opacity * red_value;
      accum_green_intensity += remaining_opacity * green_value;
      accum_blue_intensity  += remaining_opacity * blue_value;
      remaining_opacity *= (1.0 - opacity);
      
      // Increment our position and compute our voxel location
      ray_position[0] += ray_increment[0];
      ray_position[1] += ray_increment[1];
      ray_position[2] += ray_increment[2];      
      voxel[0] = vtkFloorFuncMacro( ray_position[0] );
      voxel[1] = vtkFloorFuncMacro( ray_position[1] );
      voxel[2] = vtkFloorFuncMacro( ray_position[2] );
      }
    }

  // Cap the accumulated intensity at 1.0
  if ( accum_red_intensity > 1.0 )
    {
    accum_red_intensity = 1.0;
    }
  if ( accum_green_intensity > 1.0 )
    {
    accum_green_intensity = 1.0;
    }
  if ( accum_blue_intensity > 1.0 )
    {
    accum_blue_intensity = 1.0;
    }
  
  if( remaining_opacity < VTK_REMAINING_OPACITY )
    {
    remaining_opacity = 0.0;
    }

  // Set the return pixel value.  The depth value is the distance to the
  // center of the volume.
  dynamicInfo->Color[0] = accum_red_intensity;
  dynamicInfo->Color[1] = accum_green_intensity;
  dynamicInfo->Color[2] = accum_blue_intensity;
  dynamicInfo->Color[3] = 1.0 - remaining_opacity;
  dynamicInfo->NumberOfStepsTaken = steps_this_ray;


}


// Description:
// This is the templated function that actually casts a ray and computes
// the composite value.  This version uses trilinear interpolation and
// does not compute shading
template <class T>
void vtkCastRay_TrilinVertices_Unshaded( T *data_ptr, vtkVolumeRayCastDynamicInfo *dynamicInfo,
                                         vtkVolumeRayCastStaticInfo *staticInfo, udg::Vector3 * aColorBleeding, double aObscuranceFactor )
{
  unsigned char   *grad_mag_ptr = NULL;
  unsigned char   *goptr;
  float           accum_red_intensity;
  float           accum_green_intensity;
  float           accum_blue_intensity;
  float           remaining_opacity;
  float           red_value, green_value, blue_value;
  float           opacity;
  int             loop;
  int             xinc, yinc, zinc;
  int             voxel[3];
  float           ray_position[3];
  int             prev_voxel[3];
  float           A, B, C, D, E, F, G, H;
  float           Ago, Bgo, Cgo, Dgo, Ego, Fgo, Ggo, Hgo;
  int             Binc, Cinc, Dinc, Einc, Finc, Ginc, Hinc;
  T               *dptr;
  float           *SOTF;
  float           *CTF;
  float           *GTF;
  float           *GOTF;
  float           x, y, z, t1, t2, t3;
  float           weight;
  int             offset;
  int             steps_this_ray = 0;
  int             num_steps;
  float           *ray_start, *ray_increment;
  int             grad_op_is_constant;
  float           gradient_opacity_constant;

  num_steps = dynamicInfo->NumberOfStepsToTake;
  ray_start = dynamicInfo->TransformedStart;
  ray_increment = dynamicInfo->TransformedIncrement;
 
  // Get the scalar opacity transfer function which maps scalar input values
  // to opacities
  SOTF =  staticInfo->Volume->GetCorrectedScalarOpacityArray();

  // Get the color transfer function which maps scalar input values
  // to RGB colors
  CTF =  staticInfo->Volume->GetRGBArray();
  GTF =  staticInfo->Volume->GetGrayArray();

  // Get the gradient opacity transfer function for this volume (which maps
  // gradient magnitudes to opacities)
  GOTF =  staticInfo->Volume->GetGradientOpacityArray();

  // Get the gradient opacity constant. If this number is greater than
  // or equal to 0.0, then the gradient opacity transfer function is
  // a constant at that value, otherwise it is not a constant function
  gradient_opacity_constant = staticInfo->Volume->GetGradientOpacityConstant();
  grad_op_is_constant = ( gradient_opacity_constant >= 0.0 );

  // Get a pointer to the gradient magnitudes for this volume
  if ( !grad_op_is_constant )
    {
    grad_mag_ptr = staticInfo->GradientMagnitudes;
    }

  // Move the increments into local variables
  xinc = staticInfo->DataIncrement[0];
  yinc = staticInfo->DataIncrement[1];
  zinc = staticInfo->DataIncrement[2];

  // Initialize the ray position and voxel location
  ray_position[0] = ray_start[0];
  ray_position[1] = ray_start[1];
  ray_position[2] = ray_start[2];
  voxel[0] = vtkFloorFuncMacro( ray_position[0] );
  voxel[1] = vtkFloorFuncMacro( ray_position[1] );
  voxel[2] = vtkFloorFuncMacro( ray_position[2] );

  // So far we have not accumulated anything
  accum_red_intensity     = 0.0;
  accum_green_intensity   = 0.0;
  accum_blue_intensity    = 0.0;
  remaining_opacity       = 1.0;

  // Compute the increments to get to the other 7 voxel vertices from A
  Binc = xinc;
  Cinc = yinc;
  Dinc = xinc + yinc;
  Einc = zinc;
  Finc = zinc + xinc;
  Ginc = zinc + yinc;
  Hinc = zinc + xinc + yinc;
  
  // Compute the values for the first pass through the loop
  offset = voxel[2] * zinc + voxel[1] * yinc + voxel[0];
  dptr   = data_ptr + offset;

  A = SOTF[(*(dptr))];
  B = SOTF[(*(dptr + Binc))];
  C = SOTF[(*(dptr + Cinc))];
  D = SOTF[(*(dptr + Dinc))];
  E = SOTF[(*(dptr + Einc))];
  F = SOTF[(*(dptr + Finc))];
  G = SOTF[(*(dptr + Ginc))];
  H = SOTF[(*(dptr + Hinc))];  

  if ( !grad_op_is_constant )
    {
    goptr  = grad_mag_ptr + offset;
    Ago = GOTF[(*(goptr))];
    Bgo = GOTF[(*(goptr + Binc))];
    Cgo = GOTF[(*(goptr + Cinc))];
    Dgo = GOTF[(*(goptr + Dinc))];
    Ego = GOTF[(*(goptr + Einc))];
    Fgo = GOTF[(*(goptr + Finc))];
    Ggo = GOTF[(*(goptr + Ginc))];
    Hgo = GOTF[(*(goptr + Hinc))];  
    }
  else 
    {
    Ago = Bgo = Cgo = Dgo = Ego = Fgo = Ggo = Hgo = 1.0;
    }

  // Keep track of previous voxel to know when we step into a new one  
  prev_voxel[0] = voxel[0];
  prev_voxel[1] = voxel[1];
  prev_voxel[2] = voxel[2];

  // Two cases - we are working with a gray or RGB transfer
  // function - break them up to make it more efficient
  if ( staticInfo->ColorChannels == 1 ) 
    {
    // For each step along the ray
    for ( loop = 0; 
          loop < num_steps && remaining_opacity > VTK_REMAINING_OPACITY; 
          loop++ )
      {     
      // We've taken another step
      steps_this_ray++;
      
      // Have we moved into a new voxel? If so we need to recompute A-H
      if ( prev_voxel[0] != voxel[0] ||
           prev_voxel[1] != voxel[1] ||
           prev_voxel[2] != voxel[2] )
        {
        offset = voxel[2] * zinc + voxel[1] * yinc + voxel[0];
        dptr   = data_ptr + offset;
        
        A = SOTF[(*(dptr))];
        B = SOTF[(*(dptr + Binc))];
        C = SOTF[(*(dptr + Cinc))];
        D = SOTF[(*(dptr + Dinc))];
        E = SOTF[(*(dptr + Einc))];
        F = SOTF[(*(dptr + Finc))];
        G = SOTF[(*(dptr + Ginc))];
        H = SOTF[(*(dptr + Hinc))];

        if ( !grad_op_is_constant )
          {
          goptr  = grad_mag_ptr + offset;
          Ago = GOTF[(*(goptr))];
          Bgo = GOTF[(*(goptr + Binc))];
          Cgo = GOTF[(*(goptr + Cinc))];
          Dgo = GOTF[(*(goptr + Dinc))];
          Ego = GOTF[(*(goptr + Einc))];
          Fgo = GOTF[(*(goptr + Finc))];
          Ggo = GOTF[(*(goptr + Ginc))];
          Hgo = GOTF[(*(goptr + Hinc))];  
          }
        else 
          {
            Ago = Bgo = Cgo = Dgo = Ego = Fgo = Ggo = Hgo = 1.0;
          }

        prev_voxel[0] = voxel[0];
        prev_voxel[1] = voxel[1];
        prev_voxel[2] = voxel[2];
        }
      
      // Compute our offset in the voxel, and use that to trilinearly
      // interpolate the value
      x = ray_position[0] - (float) voxel[0];
      y = ray_position[1] - (float) voxel[1];
      z = ray_position[2] - (float) voxel[2];
      
      t1 = 1.0 - x;
      t2 = 1.0 - y;
      t3 = 1.0 - z;
      
      // For this sample we have do not yet have any opacity
      opacity          = 0.0;
      red_value        = 0.0;
      
      // Now add the opacity in vertex by vertex.  If any of the A-H
      // have a non-transparent opacity value, then add its contribution
      // to the opacity
      if ( A && Ago )
        {
        weight     = t1*t2*t3 * A * Ago;
        opacity   += weight;
        ////////////////////////////////////////////////////////////////////////////////////////////
//         red_value += weight * GTF[((*dptr))];
        udg::Vector3 vColor = aColorBleeding[offset];
        red_value += weight * GTF[((*dptr))] * aObscuranceFactor * ( vColor.x + vColor.y + vColor.z ) / 3.0;
        ////////////////////////////////////////////////////////////////////////////////////////////
        }
      
      if ( B && Bgo )
        {
        weight     = x*t2*t3 * B * Bgo;
        opacity   += weight;
        ////////////////////////////////////////////////////////////////////////////////////////////
//         red_value += weight * GTF[((*(dptr + Binc)))];
        udg::Vector3 vColor = aColorBleeding[offset + Binc];
        red_value += weight * GTF[((*(dptr + Binc)))] * aObscuranceFactor * ( vColor.x + vColor.y + vColor.z ) / 3.0;
        ////////////////////////////////////////////////////////////////////////////////////////////
        }
      
      if ( C && Cgo )
        {
        weight     = t1*y*t3 * C * Cgo;
        opacity   += weight;
        ////////////////////////////////////////////////////////////////////////////////////////////
//         red_value += weight * GTF[((*(dptr + Cinc)))];
        udg::Vector3 vColor = aColorBleeding[offset + Cinc];
        red_value += weight * GTF[((*(dptr + Cinc)))] * aObscuranceFactor * ( vColor.x + vColor.y + vColor.z ) / 3.0;
        ////////////////////////////////////////////////////////////////////////////////////////////
        }
      
      if ( D && Dgo )
        {
        weight     = x*y*t3 * D * Dgo;
        opacity   += weight;
        ////////////////////////////////////////////////////////////////////////////////////////////
//         red_value += weight * GTF[((*(dptr + Dinc)))];
        udg::Vector3 vColor = aColorBleeding[offset + Dinc];
        red_value += weight * GTF[((*(dptr + Dinc)))] * aObscuranceFactor * ( vColor.x + vColor.y + vColor.z ) / 3.0;
        ////////////////////////////////////////////////////////////////////////////////////////////
        }
      
      if ( E && Ego )
        {
        weight     = t1*t2*z * E * Ego; 
        opacity   += weight;
        ////////////////////////////////////////////////////////////////////////////////////////////
//         red_value += weight * GTF[((*(dptr + Einc)))];
        udg::Vector3 vColor = aColorBleeding[offset + Einc];
        red_value += weight * GTF[((*(dptr + Einc)))] * aObscuranceFactor * ( vColor.x + vColor.y + vColor.z ) / 3.0;
        ////////////////////////////////////////////////////////////////////////////////////////////
        }
      
      if ( F && Fgo )
        {
        weight     = x*t2*z * F * Fgo;
        opacity   += weight;
        ////////////////////////////////////////////////////////////////////////////////////////////
//         red_value += weight * GTF[((*(dptr + Finc)))];
        udg::Vector3 vColor = aColorBleeding[offset + Finc];
        red_value += weight * GTF[((*(dptr + Finc)))] * aObscuranceFactor * ( vColor.x + vColor.y + vColor.z ) / 3.0;
        ////////////////////////////////////////////////////////////////////////////////////////////
        }
      
      if ( G && Ggo )
        {
        weight     = t1*y*z * G * Ggo;
        opacity   += weight;
        ////////////////////////////////////////////////////////////////////////////////////////////
//         red_value += weight * GTF[((*(dptr + Ginc)))];
        udg::Vector3 vColor = aColorBleeding[offset + Ginc];
        red_value += weight * GTF[((*(dptr + Ginc)))] * aObscuranceFactor * ( vColor.x + vColor.y + vColor.z ) / 3.0;
        ////////////////////////////////////////////////////////////////////////////////////////////
        } 
      
      if ( H && Hgo )
        {
        weight     = x*z*y * H * Hgo;
        opacity   += weight;
        ////////////////////////////////////////////////////////////////////////////////////////////
//         red_value += weight * GTF[((*(dptr + Hinc)))];
        udg::Vector3 vColor = aColorBleeding[offset + Hinc];
        red_value += weight * GTF[((*(dptr + Hinc)))] * aObscuranceFactor * ( vColor.x + vColor.y + vColor.z ) / 3.0;
        ////////////////////////////////////////////////////////////////////////////////////////////
        }
      
      // Accumulate intensity and opacity for this sample location
      accum_red_intensity   += remaining_opacity * red_value;
      remaining_opacity *= (1.0 - opacity);
      
      // Increment our position and compute our voxel location
      ray_position[0] += ray_increment[0];
      ray_position[1] += ray_increment[1];
      ray_position[2] += ray_increment[2];      
      voxel[0] = vtkFloorFuncMacro( ray_position[0] );
      voxel[1] = vtkFloorFuncMacro( ray_position[1] );
      voxel[2] = vtkFloorFuncMacro( ray_position[2] );
      }
    accum_green_intensity = accum_red_intensity;
    accum_blue_intensity = accum_red_intensity;
    }
  else if ( staticInfo->ColorChannels == 3 )
    {
    // For each step along the ray
    for ( loop = 0; 
          loop < num_steps && remaining_opacity > VTK_REMAINING_OPACITY; 
          loop++ )
      {     
      // We've taken another step
      steps_this_ray++;
      
      // Have we moved into a new voxel? If so we need to recompute A-H
      if ( prev_voxel[0] != voxel[0] ||
           prev_voxel[1] != voxel[1] ||
           prev_voxel[2] != voxel[2] )
        {
        offset = voxel[2] * zinc + voxel[1] * yinc + voxel[0];
        dptr = data_ptr + offset;
        
        A = SOTF[(*(dptr))];
        B = SOTF[(*(dptr + Binc))];
        C = SOTF[(*(dptr + Cinc))];
        D = SOTF[(*(dptr + Dinc))];
        E = SOTF[(*(dptr + Einc))];
        F = SOTF[(*(dptr + Finc))];
        G = SOTF[(*(dptr + Ginc))];
        H = SOTF[(*(dptr + Hinc))];

        if ( !grad_op_is_constant ) 
          {
          goptr  = grad_mag_ptr + offset;
          Ago = GOTF[(*(goptr))];
          Bgo = GOTF[(*(goptr + Binc))];
          Cgo = GOTF[(*(goptr + Cinc))];
          Dgo = GOTF[(*(goptr + Dinc))];
          Ego = GOTF[(*(goptr + Einc))];
          Fgo = GOTF[(*(goptr + Finc))];
          Ggo = GOTF[(*(goptr + Ginc))];
          Hgo = GOTF[(*(goptr + Hinc))];  
          }
        else 
          {
            Ago = Bgo = Cgo = Dgo = Ego = Fgo = Ggo = Hgo = 1.0;
          }

        prev_voxel[0] = voxel[0];
        prev_voxel[1] = voxel[1];
        prev_voxel[2] = voxel[2];
        }
      
      // Compute our offset in the voxel, and use that to trilinearly
      // interpolate the value
      x = ray_position[0] - (float) voxel[0];
      y = ray_position[1] - (float) voxel[1];
      z = ray_position[2] - (float) voxel[2];
      
      t1 = 1.0 - x;
      t2 = 1.0 - y;
      t3 = 1.0 - z;
      
      // For this sample we have do not yet have any opacity
      opacity           = 0.0;
      red_value         = 0.0;
      green_value       = 0.0;
      blue_value        = 0.0;
      
      // Now add the opacity in vertex by vertex.  If any of the A-H
      // have a non-transparent opacity value, then add its contribution
      // to the opacity
      if ( A && Ago )
        {
        weight         = t1*t2*t3 * A * Ago;
        opacity       += weight;
        ////////////////////////////////////////////////////////////////////////////////////////////
//         red_value     += weight * CTF[((*dptr)) * 3    ];
//         green_value   += weight * CTF[((*dptr)) * 3 + 1];
//         blue_value    += weight * CTF[((*dptr)) * 3 + 2];
        udg::Vector3 vColor = aColorBleeding[offset];
        red_value     += weight * CTF[((*dptr)) * 3    ] * aObscuranceFactor * vColor.x;
        green_value   += weight * CTF[((*dptr)) * 3 + 1] * aObscuranceFactor * vColor.y;
        blue_value    += weight * CTF[((*dptr)) * 3 + 2] * aObscuranceFactor * vColor.z;
        ////////////////////////////////////////////////////////////////////////////////////////////
        }
      
      if ( B && Bgo )
        {
        weight         = x*t2*t3 * B * Bgo;
        opacity       += weight;
        ////////////////////////////////////////////////////////////////////////////////////////////
//         red_value     += weight * CTF[((*(dptr + Binc))) * 3    ];
//         green_value   += weight * CTF[((*(dptr + Binc))) * 3 + 1];
//         blue_value    += weight * CTF[((*(dptr + Binc))) * 3 + 2];
        udg::Vector3 vColor = aColorBleeding[offset + Binc];
        red_value     += weight * CTF[((*(dptr + Binc))) * 3    ] * aObscuranceFactor * vColor.x;
        green_value   += weight * CTF[((*(dptr + Binc))) * 3 + 1] * aObscuranceFactor * vColor.y;
        blue_value    += weight * CTF[((*(dptr + Binc))) * 3 + 2] * aObscuranceFactor * vColor.z;
        ////////////////////////////////////////////////////////////////////////////////////////////
        }                                                                               
      
      if ( C && Cgo )
        {
        weight         = t1*y*t3 * C * Cgo;
        opacity       += weight;
        ////////////////////////////////////////////////////////////////////////////////////////////
//         red_value     += weight * CTF[((*(dptr + Cinc))) * 3    ];
//         green_value   += weight * CTF[((*(dptr + Cinc))) * 3 + 1];
//         blue_value    += weight * CTF[((*(dptr + Cinc))) * 3 + 2];
        udg::Vector3 vColor = aColorBleeding[offset + Cinc];
        red_value     += weight * CTF[((*(dptr + Cinc))) * 3    ] * aObscuranceFactor * vColor.x;
        green_value   += weight * CTF[((*(dptr + Cinc))) * 3 + 1] * aObscuranceFactor * vColor.y;
        blue_value    += weight * CTF[((*(dptr + Cinc))) * 3 + 2] * aObscuranceFactor * vColor.z;
        ////////////////////////////////////////////////////////////////////////////////////////////
        }                                                                               
      
      if ( D && Dgo )
        {
        weight         = x*y*t3 * D * Dgo;
        opacity       += weight;
        ////////////////////////////////////////////////////////////////////////////////////////////
//         red_value     += weight * CTF[((*(dptr + Dinc))) * 3    ];
//         green_value   += weight * CTF[((*(dptr + Dinc))) * 3 + 1];
//         blue_value    += weight * CTF[((*(dptr + Dinc))) * 3 + 2];
        udg::Vector3 vColor = aColorBleeding[offset + Dinc];
        red_value     += weight * CTF[((*(dptr + Dinc))) * 3    ] * aObscuranceFactor * vColor.x;
        green_value   += weight * CTF[((*(dptr + Dinc))) * 3 + 1] * aObscuranceFactor * vColor.y;
        blue_value    += weight * CTF[((*(dptr + Dinc))) * 3 + 2] * aObscuranceFactor * vColor.z;
        ////////////////////////////////////////////////////////////////////////////////////////////
        }
      
      if ( E && Ego )
        {
        weight         = t1*t2*z * E * Ego;
        opacity       += weight;
        ////////////////////////////////////////////////////////////////////////////////////////////
//         red_value     += weight * CTF[((*(dptr + Einc))) * 3    ];
//         green_value   += weight * CTF[((*(dptr + Einc))) * 3 + 1];
//         blue_value    += weight * CTF[((*(dptr + Einc))) * 3 + 2];
        udg::Vector3 vColor = aColorBleeding[offset + Einc];
        red_value     += weight * CTF[((*(dptr + Einc))) * 3    ] * aObscuranceFactor * vColor.x;
        green_value   += weight * CTF[((*(dptr + Einc))) * 3 + 1] * aObscuranceFactor * vColor.y;
        blue_value    += weight * CTF[((*(dptr + Einc))) * 3 + 2] * aObscuranceFactor * vColor.z;
        ////////////////////////////////////////////////////////////////////////////////////////////
        }
      
      if ( F && Fgo )
        {
        weight         = x*t2*z * F * Fgo;
        opacity       += weight;
        ////////////////////////////////////////////////////////////////////////////////////////////
//         red_value     += weight * CTF[((*(dptr + Finc))) * 3    ];
//         green_value   += weight * CTF[((*(dptr + Finc))) * 3 + 1];
//         blue_value    += weight * CTF[((*(dptr + Finc))) * 3 + 2];
        udg::Vector3 vColor = aColorBleeding[offset + Finc];
        red_value     += weight * CTF[((*(dptr + Finc))) * 3    ] * aObscuranceFactor * vColor.x;
        green_value   += weight * CTF[((*(dptr + Finc))) * 3 + 1] * aObscuranceFactor * vColor.y;
        blue_value    += weight * CTF[((*(dptr + Finc))) * 3 + 2] * aObscuranceFactor * vColor.z;
        ////////////////////////////////////////////////////////////////////////////////////////////
        }
      
      if ( G && Ggo )
        {
        weight         = t1*y*z * G * Ggo;
        opacity       += weight;
        ////////////////////////////////////////////////////////////////////////////////////////////
//         red_value     +=  weight * CTF[((*(dptr + Ginc))) * 3    ];
//         green_value   +=  weight * CTF[((*(dptr + Ginc))) * 3 + 1];
//         blue_value    +=  weight * CTF[((*(dptr + Ginc))) * 3 + 2];
        udg::Vector3 vColor = aColorBleeding[offset + Ginc];
        red_value     +=  weight * CTF[((*(dptr + Ginc))) * 3    ] * aObscuranceFactor * vColor.x;
        green_value   +=  weight * CTF[((*(dptr + Ginc))) * 3 + 1] * aObscuranceFactor * vColor.y;
        blue_value    +=  weight * CTF[((*(dptr + Ginc))) * 3 + 2] * aObscuranceFactor * vColor.z;
        ////////////////////////////////////////////////////////////////////////////////////////////
        } 
      
      if ( H && Hgo )
        {
        weight         = x*z*y * H * Hgo;
        opacity       += weight;
        ////////////////////////////////////////////////////////////////////////////////////////////
//         red_value     += weight * CTF[((*(dptr + Hinc))) * 3    ];
//         green_value   += weight * CTF[((*(dptr + Hinc))) * 3 + 1];
//         blue_value    += weight * CTF[((*(dptr + Hinc))) * 3 + 2];
        udg::Vector3 vColor = aColorBleeding[offset + Hinc];
        red_value     += weight * CTF[((*(dptr + Hinc))) * 3    ] * aObscuranceFactor * vColor.x;
        green_value   += weight * CTF[((*(dptr + Hinc))) * 3 + 1] * aObscuranceFactor * vColor.y;
        blue_value    += weight * CTF[((*(dptr + Hinc))) * 3 + 2] * aObscuranceFactor * vColor.z;
        ////////////////////////////////////////////////////////////////////////////////////////////
        }
      
      // Accumulate intensity and opacity for this sample location
      accum_red_intensity   += remaining_opacity * red_value;
      accum_green_intensity += remaining_opacity * green_value;
      accum_blue_intensity  += remaining_opacity * blue_value;
      remaining_opacity *= (1.0 - opacity);
      
      // Increment our position and compute our voxel location
      ray_position[0] += ray_increment[0];
      ray_position[1] += ray_increment[1];
      ray_position[2] += ray_increment[2];      
      voxel[0] = vtkFloorFuncMacro( ray_position[0] );
      voxel[1] = vtkFloorFuncMacro( ray_position[1] );
      voxel[2] = vtkFloorFuncMacro( ray_position[2] );
      }
    }

  // Cap the accumulated intensity at 1.0
  if ( accum_red_intensity > 1.0 )
    {
    accum_red_intensity = 1.0;
    }
  if ( accum_green_intensity > 1.0 )
    {
    accum_green_intensity = 1.0;
    }
  if ( accum_blue_intensity > 1.0 )
    {
    accum_blue_intensity = 1.0;
    }
  
  if( remaining_opacity < VTK_REMAINING_OPACITY )
    {
    remaining_opacity = 0.0;
    }

  // Set the return pixel value.  The depth value is the distance to the
  // center of the volume.
  dynamicInfo->Color[0] = accum_red_intensity;
  dynamicInfo->Color[1] = accum_green_intensity;
  dynamicInfo->Color[2] = accum_blue_intensity;
  dynamicInfo->Color[3] = 1.0 - remaining_opacity;
  dynamicInfo->NumberOfStepsTaken = steps_this_ray;


}


// Description:
// This is the templated function that actually casts a ray and computes
// the composite value.  This version uses trilinear interpolation, and
// does perform shading.
template <class T>
void vtkCastRay_TrilinVertices_Shaded( T *data_ptr, vtkVolumeRayCastDynamicInfo *dynamicInfo,
                                       vtkVolumeRayCastStaticInfo *staticInfo,
                                       double * aObscurance, double aObscuranceFactor, double aObscuranceFilterLow, double aObscuranceFilterHigh,
                                       bool aFxObscurance, double aFxContour )
{
  unsigned char   *grad_mag_ptr = NULL;
  unsigned char   *goptr;
  float           accum_red_intensity;
  float           accum_green_intensity;
  float           accum_blue_intensity;
  float           remaining_opacity;
  float           opacity;
  int             loop;
  int             xinc, yinc, zinc;
  int             voxel[3];
  float           ray_position[3];
  int             prev_voxel[3];
  float           A, B, C, D, E, F, G, H;
  float           Ago, Bgo, Cgo, Dgo, Ego, Fgo, Ggo, Hgo;
  int             Binc, Cinc, Dinc, Einc, Finc, Ginc, Hinc;
  T               *dptr;
  float           *SOTF;
  float           *CTF;
  float           *GTF;
  float           *GOTF;
  float           x, y, z, t1, t2, t3;
  float           weight;
  float           *red_d_shade, *green_d_shade, *blue_d_shade;
  float           *red_s_shade, *green_s_shade, *blue_s_shade;
  unsigned short  *encoded_normals, *nptr;
  float           red_shaded_value, green_shaded_value, blue_shaded_value;
  int             offset;
  int             steps_this_ray = 0;
  int             grad_op_is_constant;
  float           gradient_opacity_constant;
  int             num_steps;
  float           *ray_start, *ray_increment;


  num_steps = dynamicInfo->NumberOfStepsToTake;
  ray_start = dynamicInfo->TransformedStart;
  ray_increment = dynamicInfo->TransformedIncrement;

  // Get diffuse shading table pointers
  red_d_shade = staticInfo->RedDiffuseShadingTable;
  green_d_shade = staticInfo->GreenDiffuseShadingTable;
  blue_d_shade = staticInfo->BlueDiffuseShadingTable;


  // Get diffuse shading table pointers
  red_s_shade = staticInfo->RedSpecularShadingTable;
  green_s_shade = staticInfo->GreenSpecularShadingTable;
  blue_s_shade = staticInfo->BlueSpecularShadingTable;

  // Get a pointer to the encoded normals for this volume
  encoded_normals = staticInfo->EncodedNormals;

  // Get the scalar opacity transfer function which maps scalar input values
  // to opacities
  SOTF =  staticInfo->Volume->GetCorrectedScalarOpacityArray();

  // Get the color transfer function which maps scalar input values
  // to RGB values
  CTF =  staticInfo->Volume->GetRGBArray();
  GTF =  staticInfo->Volume->GetGrayArray();

  // Get the gradient opacity transfer function for this volume (which maps
  // gradient magnitudes to opacities)
  GOTF =  staticInfo->Volume->GetGradientOpacityArray();

  // Get the gradient opacity constant. If this number is greater than
  // or equal to 0.0, then the gradient opacity transfer function is
  // a constant at that value, otherwise it is not a constant function
  gradient_opacity_constant = staticInfo->Volume->GetGradientOpacityConstant();
  grad_op_is_constant = ( gradient_opacity_constant >= 0.0 );

  // Get a pointer to the gradient magnitudes for this volume
  if ( !grad_op_is_constant )
    {
    grad_mag_ptr = staticInfo->GradientMagnitudes;
    }

  // Move the increments into local variables
  xinc = staticInfo->DataIncrement[0];
  yinc = staticInfo->DataIncrement[1];
  zinc = staticInfo->DataIncrement[2];

  // Initialize the ray position and voxel location
  ray_position[0] = ray_start[0];
  ray_position[1] = ray_start[1];
  ray_position[2] = ray_start[2];
  voxel[0] = vtkFloorFuncMacro( ray_position[0] );
  voxel[1] = vtkFloorFuncMacro( ray_position[1] );
  voxel[2] = vtkFloorFuncMacro( ray_position[2] );

  // So far we haven't accumulated anything
  accum_red_intensity     = 0.0;
  accum_green_intensity   = 0.0;
  accum_blue_intensity    = 0.0;
  remaining_opacity       = 1.0;

  // Compute the increments to get to the other 7 voxel vertices from A
  Binc = xinc;
  Cinc = yinc;
  Dinc = xinc + yinc;
  Einc = zinc;
  Finc = zinc + xinc;
  Ginc = zinc + yinc;
  Hinc = zinc + xinc + yinc;
  
   // Compute the values for the first pass through the loop
  offset = voxel[2] * zinc + voxel[1] * yinc + voxel[0];
  dptr   = data_ptr + offset;
  nptr   = encoded_normals + offset;

  A = SOTF[(*(dptr))];
  B = SOTF[(*(dptr + Binc))];
  C = SOTF[(*(dptr + Cinc))];
  D = SOTF[(*(dptr + Dinc))];
  E = SOTF[(*(dptr + Einc))];
  F = SOTF[(*(dptr + Finc))];
  G = SOTF[(*(dptr + Ginc))];
  H = SOTF[(*(dptr + Hinc))];

  if ( !grad_op_is_constant )
    {
    goptr  = grad_mag_ptr + offset;
    Ago = GOTF[(*(goptr))];
    Bgo = GOTF[(*(goptr + Binc))];
    Cgo = GOTF[(*(goptr + Cinc))];
    Dgo = GOTF[(*(goptr + Dinc))];
    Ego = GOTF[(*(goptr + Einc))];
    Fgo = GOTF[(*(goptr + Finc))];
    Ggo = GOTF[(*(goptr + Ginc))];
    Hgo = GOTF[(*(goptr + Hinc))];  
    }
  else
    {
    Ago = Bgo = Cgo = Dgo = Ego = Fgo = Ggo = Hgo = 1.0;
    }


  // Keep track of previous voxel to know when we step into a new one   
  prev_voxel[0] = voxel[0];
  prev_voxel[1] = voxel[1];
  prev_voxel[2] = voxel[2];

  //////////////////////////////////////////////////////////////////////////////////////////////////
  udg::Vector3 fxDirection( dynamicInfo->TransformedDirection[0], dynamicInfo->TransformedDirection[1], dynamicInfo->TransformedDirection[2] );
  fxDirection.normalize();
  vtkVolumeRayCastMapper * fxMapper = vtkVolumeRayCastMapper::SafeDownCast( staticInfo->Volume->GetMapper() );
  vtkEncodedGradientEstimator * fxGradientEstimator = fxMapper->GetGradientEstimator();
  unsigned short * fxEncodedNormals = fxGradientEstimator->GetEncodedNormals();
  vtkDirectionEncoder * fxDirectionEncoder = fxGradientEstimator->GetDirectionEncoder();
  //////////////////////////////////////////////////////////////////////////////////////////////////

  // Two cases - we are working with a single color or a color transfer
  // function - break them up to make it more efficient
  if ( staticInfo->ColorChannels == 1 )
    {
    // For each step along the ray
    for ( loop = 0; 
          loop < num_steps && remaining_opacity > VTK_REMAINING_OPACITY; 
          loop++ )
      {     
      // We've taken another step
      steps_this_ray++;
      
      // Have we moved into a new voxel? If so we need to recompute A-H
      if ( prev_voxel[0] != voxel[0] ||
           prev_voxel[1] != voxel[1] ||
           prev_voxel[2] != voxel[2] )
        {
        offset = voxel[2] * zinc + voxel[1] * yinc + voxel[0];
        dptr   = data_ptr + offset;
        nptr   = encoded_normals + offset;
        //goptr  = grad_mag_ptr + offset;
        
        A = SOTF[(*(dptr))];
        B = SOTF[(*(dptr + Binc))];
        C = SOTF[(*(dptr + Cinc))];
        D = SOTF[(*(dptr + Dinc))];
        E = SOTF[(*(dptr + Einc))];
        F = SOTF[(*(dptr + Finc))];
        G = SOTF[(*(dptr + Ginc))];
        H = SOTF[(*(dptr + Hinc))];
        
        if ( !grad_op_is_constant )
          {
          goptr  = grad_mag_ptr + offset;
          Ago = GOTF[(*(goptr))];
          Bgo = GOTF[(*(goptr + Binc))];
          Cgo = GOTF[(*(goptr + Cinc))];
          Dgo = GOTF[(*(goptr + Dinc))];
          Ego = GOTF[(*(goptr + Einc))];
          Fgo = GOTF[(*(goptr + Finc))];
          Ggo = GOTF[(*(goptr + Ginc))];
          Hgo = GOTF[(*(goptr + Hinc))];  
          }
        else
          {
          Ago = Bgo = Cgo = Dgo = Ego = Fgo = Ggo = Hgo = 1.0;
          }

        prev_voxel[0] = voxel[0];
        prev_voxel[1] = voxel[1];
        prev_voxel[2] = voxel[2];
        }
      
      // Compute our offset in the voxel, and use that to trilinearly
      // interpolate a value
      x = ray_position[0] - (float) voxel[0];
      y = ray_position[1] - (float) voxel[1];
      z = ray_position[2] - (float) voxel[2];
      
      t1 = 1.0 - x;
      t2 = 1.0 - y;
      t3 = 1.0 - z;
      
      // For this sample we have do not yet have any opacity or
      // shaded intensity yet
      opacity = 0.0;
      red_shaded_value = 0.0;
      
      // Now add the opacity and shaded intensity value in vertex by 
      // vertex.  If any of the A-H have a non-transparent opacity value, 
      // then add its contribution to the opacity
      if ( A && Ago )
        {
        weight = t1*t2*t3 * A * Ago;
        opacity += weight;
        ////////////////////////////////////////////////////////////////////////////////////////////
//         red_shaded_value   += weight * ( red_d_shade[ *(nptr) ] * 
//                                      GTF[*(dptr)] + 
//                                      red_s_shade[ *(nptr) ] );

        double fx = 1.0;

        // contour
        if ( aFxContour > 0.0 )
        {
          float * fxGradient = fxDirectionEncoder->GetDecodedGradient( fxEncodedNormals[offset] );
          udg::Vector3 fxNormal( fxGradient[0], fxGradient[1], fxGradient[2] );
          double fxDotProduct = fxDirection * fxNormal;
          if ( fxDotProduct < 0.0 ) fxDotProduct = -fxDotProduct;
          fx *= fxDotProduct < aFxContour ? 0.0 : 1.0;
        }

        // obscurance
        if ( aFxObscurance && fx > 0.0 )
        {
          double fxObscurance = aObscurance[offset];
          if ( fxObscurance < aObscuranceFilterLow ) fxObscurance = 0.0;
          else if ( fxObscurance > aObscuranceFilterHigh ) fxObscurance = 1.0;
          fx *= aObscuranceFactor * fxObscurance;
        }

        red_shaded_value   += weight * ( red_d_shade[ *(nptr) ] * 
                                     GTF[*(dptr)] + 
                                     red_s_shade[ *(nptr) ] ) * fx;
        ////////////////////////////////////////////////////////////////////////////////////////////
        }
      
      if ( B && Bgo )
        {
        weight = x*t2*t3 * B * Bgo;
        opacity += weight;
        ////////////////////////////////////////////////////////////////////////////////////////////
//         red_shaded_value   += weight * ( red_d_shade[ *(nptr + Binc) ] * 
//                                      GTF[*(dptr+Binc)] + 
//                                      red_s_shade[ *(nptr + Binc) ] );

        double fx = 1.0;

        // contour
        if ( aFxContour > 0.0 )
        {
          float * fxGradient = fxDirectionEncoder->GetDecodedGradient( fxEncodedNormals[offset + Binc] );
          udg::Vector3 fxNormal( fxGradient[0], fxGradient[1], fxGradient[2] );
          double fxDotProduct = fxDirection * fxNormal;
          if ( fxDotProduct < 0.0 ) fxDotProduct = -fxDotProduct;
          fx *= fxDotProduct < aFxContour ? 0.0 : 1.0;
        }

        // obscurance
        if ( aFxObscurance && fx > 0.0 )
        {
          double fxObscurance = aObscurance[offset + Binc];
          if ( fxObscurance < aObscuranceFilterLow ) fxObscurance = 0.0;
          else if ( fxObscurance > aObscuranceFilterHigh ) fxObscurance = 1.0;
          fx *= aObscuranceFactor * fxObscurance;
        }

        red_shaded_value   += weight * ( red_d_shade[ *(nptr + Binc) ] * 
                                     GTF[*(dptr+Binc)] + 
                                     red_s_shade[ *(nptr + Binc) ] ) * fx;
        ////////////////////////////////////////////////////////////////////////////////////////////
        }
      
      if ( C && Cgo )
        {
        weight = t1*y*t3 * C * Cgo;
        opacity += weight;
        ////////////////////////////////////////////////////////////////////////////////////////////
//         red_shaded_value   += weight * ( red_d_shade[ *(nptr + Cinc) ] *
//                                      GTF[*(dptr+Cinc)] + 
//                                      red_s_shade[ *(nptr + Cinc) ] );

        double fx = 1.0;

        // contour
        if ( aFxContour > 0.0 )
        {
          float * fxGradient = fxDirectionEncoder->GetDecodedGradient( fxEncodedNormals[offset + Cinc] );
          udg::Vector3 fxNormal( fxGradient[0], fxGradient[1], fxGradient[2] );
          double fxDotProduct = fxDirection * fxNormal;
          if ( fxDotProduct < 0.0 ) fxDotProduct = -fxDotProduct;
          fx *= fxDotProduct < aFxContour ? 0.0 : 1.0;
        }

        // obscurance
        if ( aFxObscurance && fx > 0.0 )
        {
          double fxObscurance = aObscurance[offset + Cinc];
          if ( fxObscurance < aObscuranceFilterLow ) fxObscurance = 0.0;
          else if ( fxObscurance > aObscuranceFilterHigh ) fxObscurance = 1.0;
          fx *= aObscuranceFactor * fxObscurance;
        }

        red_shaded_value   += weight * ( red_d_shade[ *(nptr + Cinc) ] *
                                     GTF[*(dptr+Cinc)] + 
                                     red_s_shade[ *(nptr + Cinc) ] ) * fx;
        ////////////////////////////////////////////////////////////////////////////////////////////
        }

      if ( D && Dgo )
        {
        weight = x*y*t3 * D * Dgo;
        opacity += weight;
        ////////////////////////////////////////////////////////////////////////////////////////////
//         red_shaded_value   += weight * ( red_d_shade[ *(nptr + Dinc) ] *
//                                      GTF[*(dptr+Dinc)] + 
//                                      red_s_shade[ *(nptr + Dinc) ] );

        double fx = 1.0;

        // contour
        if ( aFxContour > 0.0 )
        {
          float * fxGradient = fxDirectionEncoder->GetDecodedGradient( fxEncodedNormals[offset + Dinc] );
          udg::Vector3 fxNormal( fxGradient[0], fxGradient[1], fxGradient[2] );
          double fxDotProduct = fxDirection * fxNormal;
          if ( fxDotProduct < 0.0 ) fxDotProduct = -fxDotProduct;
          fx *= fxDotProduct < aFxContour ? 0.0 : 1.0;
        }

        // obscurance
        if ( aFxObscurance && fx > 0.0 )
        {
          double fxObscurance = aObscurance[offset + Dinc];
          if ( fxObscurance < aObscuranceFilterLow ) fxObscurance = 0.0;
          else if ( fxObscurance > aObscuranceFilterHigh ) fxObscurance = 1.0;
          fx *= aObscuranceFactor * fxObscurance;
        }

        red_shaded_value   += weight * ( red_d_shade[ *(nptr + Dinc) ] *
                                     GTF[*(dptr+Dinc)] + 
                                     red_s_shade[ *(nptr + Dinc) ] ) * fx;
        ////////////////////////////////////////////////////////////////////////////////////////////
        }
      
      if ( E && Ego )
        {
        weight = t1*t2*z * E * Ego;
        opacity += weight;
        ////////////////////////////////////////////////////////////////////////////////////////////
//         red_shaded_value   += weight * ( red_d_shade[ *(nptr + Einc) ] *
//                                      GTF[*(dptr+Einc)] + 
//                                      red_s_shade[ *(nptr + Einc) ] );

        double fx = 1.0;

        // contour
        if ( aFxContour > 0.0 )
        {
          float * fxGradient = fxDirectionEncoder->GetDecodedGradient( fxEncodedNormals[offset + Einc] );
          udg::Vector3 fxNormal( fxGradient[0], fxGradient[1], fxGradient[2] );
          double fxDotProduct = fxDirection * fxNormal;
          if ( fxDotProduct < 0.0 ) fxDotProduct = -fxDotProduct;
          fx *= fxDotProduct < aFxContour ? 0.0 : 1.0;
        }

        // obscurance
        if ( aFxObscurance && fx > 0.0 )
        {
          double fxObscurance = aObscurance[offset + Einc];
          if ( fxObscurance < aObscuranceFilterLow ) fxObscurance = 0.0;
          else if ( fxObscurance > aObscuranceFilterHigh ) fxObscurance = 1.0;
          fx *= aObscuranceFactor * fxObscurance;
        }

        red_shaded_value   += weight * ( red_d_shade[ *(nptr + Einc) ] *
                                     GTF[*(dptr+Einc)] + 
                                     red_s_shade[ *(nptr + Einc) ] ) * fx;
        ////////////////////////////////////////////////////////////////////////////////////////////
        }
      
      if ( F && Fgo )
        {
        weight = x*z*t2 * F * Fgo;
        opacity += weight;
        ////////////////////////////////////////////////////////////////////////////////////////////
//         red_shaded_value   += weight * ( red_d_shade[ *(nptr + Finc) ] *
//                                      GTF[*(dptr+Finc)] + 
//                                      red_s_shade[ *(nptr + Finc) ] );

        double fx = 1.0;

        // contour
        if ( aFxContour > 0.0 )
        {
          float * fxGradient = fxDirectionEncoder->GetDecodedGradient( fxEncodedNormals[offset + Finc] );
          udg::Vector3 fxNormal( fxGradient[0], fxGradient[1], fxGradient[2] );
          double fxDotProduct = fxDirection * fxNormal;
          if ( fxDotProduct < 0.0 ) fxDotProduct = -fxDotProduct;
          fx *= fxDotProduct < aFxContour ? 0.0 : 1.0;
        }

        // obscurance
        if ( aFxObscurance && fx > 0.0 )
        {
          double fxObscurance = aObscurance[offset + Finc];
          if ( fxObscurance < aObscuranceFilterLow ) fxObscurance = 0.0;
          else if ( fxObscurance > aObscuranceFilterHigh ) fxObscurance = 1.0;
          fx *= aObscuranceFactor * fxObscurance;
        }

        red_shaded_value   += weight * ( red_d_shade[ *(nptr + Finc) ] *
                                     GTF[*(dptr+Finc)] + 
                                     red_s_shade[ *(nptr + Finc) ] ) * fx;
        ////////////////////////////////////////////////////////////////////////////////////////////
        }
      
      if ( G && Ggo )
        {
        weight = t1*y*z * G * Ggo;
        opacity += weight;
        ////////////////////////////////////////////////////////////////////////////////////////////
//         red_shaded_value   += weight * ( red_d_shade[ *(nptr + Ginc) ] *
//                                      GTF[*(dptr+Ginc)] + 
//                                      red_s_shade[ *(nptr + Ginc) ] );

        double fx = 1.0;

        // contour
        if ( aFxContour > 0.0 )
        {
          float * fxGradient = fxDirectionEncoder->GetDecodedGradient( fxEncodedNormals[offset + Ginc] );
          udg::Vector3 fxNormal( fxGradient[0], fxGradient[1], fxGradient[2] );
          double fxDotProduct = fxDirection * fxNormal;
          if ( fxDotProduct < 0.0 ) fxDotProduct = -fxDotProduct;
          fx *= fxDotProduct < aFxContour ? 0.0 : 1.0;
        }

        // obscurance
        if ( aFxObscurance && fx > 0.0 )
        {
          double fxObscurance = aObscurance[offset + Ginc];
          if ( fxObscurance < aObscuranceFilterLow ) fxObscurance = 0.0;
          else if ( fxObscurance > aObscuranceFilterHigh ) fxObscurance = 1.0;
          fx *= aObscuranceFactor * fxObscurance;
        }

        red_shaded_value   += weight * ( red_d_shade[ *(nptr + Ginc) ] *
                                     GTF[*(dptr+Ginc)] + 
                                     red_s_shade[ *(nptr + Ginc) ] ) * fx;
        ////////////////////////////////////////////////////////////////////////////////////////////
      } 
      
      if ( H && Hgo )
        {
        weight = x*z*y * H * Hgo;
        opacity += weight;
        ////////////////////////////////////////////////////////////////////////////////////////////
//         red_shaded_value   += weight * ( red_d_shade[ *(nptr + Hinc) ] *
//                                      GTF[*(dptr+Hinc)] + 
//                                      red_s_shade[ *(nptr + Hinc) ] );

        double fx = 1.0;

        // contour
        if ( aFxContour > 0.0 )
        {
          float * fxGradient = fxDirectionEncoder->GetDecodedGradient( fxEncodedNormals[offset + Hinc] );
          udg::Vector3 fxNormal( fxGradient[0], fxGradient[1], fxGradient[2] );
          double fxDotProduct = fxDirection * fxNormal;
          if ( fxDotProduct < 0.0 ) fxDotProduct = -fxDotProduct;
          fx *= fxDotProduct < aFxContour ? 0.0 : 1.0;
        }

        // obscurance
        if ( aFxObscurance && fx > 0.0 )
        {
          double fxObscurance = aObscurance[offset + Hinc];
          if ( fxObscurance < aObscuranceFilterLow ) fxObscurance = 0.0;
          else if ( fxObscurance > aObscuranceFilterHigh ) fxObscurance = 1.0;
          fx *= aObscuranceFactor * fxObscurance;
        }

        red_shaded_value   += weight * ( red_d_shade[ *(nptr + Hinc) ] *
                                     GTF[*(dptr+Hinc)] + 
                                     red_s_shade[ *(nptr + Hinc) ] ) * fx;
        ////////////////////////////////////////////////////////////////////////////////////////////
        }
      
      // Accumulate intensity and opacity for this sample location
      accum_red_intensity   += red_shaded_value   * remaining_opacity;
      remaining_opacity *= (1.0 - opacity);
      
      // Increment our position and compute our voxel location
      ray_position[0] += ray_increment[0];
      ray_position[1] += ray_increment[1];
      ray_position[2] += ray_increment[2];      
      voxel[0] = vtkFloorFuncMacro( ray_position[0] );
      voxel[1] = vtkFloorFuncMacro( ray_position[1] );
      voxel[2] = vtkFloorFuncMacro( ray_position[2] );
      }
    accum_green_intensity = accum_red_intensity;
    accum_blue_intensity = accum_red_intensity;
    }
  else if ( staticInfo->ColorChannels == 3 )
    {
    // For each step along the ray
    for ( loop = 0; 
          loop < num_steps && remaining_opacity > VTK_REMAINING_OPACITY; 
          loop++ )
      {     
      // We've taken another step
      steps_this_ray++;
      
      // Have we moved into a new voxel? If so we need to recompute A-H
      if ( prev_voxel[0] != voxel[0] ||
           prev_voxel[1] != voxel[1] ||
           prev_voxel[2] != voxel[2] )
        {
        offset = voxel[2] * zinc + voxel[1] * yinc + voxel[0];
        dptr   = data_ptr + offset;
        nptr   = encoded_normals + offset;
        
        A = SOTF[(*(dptr))];
        B = SOTF[(*(dptr + Binc))];
        C = SOTF[(*(dptr + Cinc))];
        D = SOTF[(*(dptr + Dinc))];
        E = SOTF[(*(dptr + Einc))];
        F = SOTF[(*(dptr + Finc))];
        G = SOTF[(*(dptr + Ginc))];
        H = SOTF[(*(dptr + Hinc))];
        
        if ( !grad_op_is_constant )
          {
          goptr  = grad_mag_ptr + offset;
          Ago = GOTF[(*(goptr))];
          Bgo = GOTF[(*(goptr + Binc))];
          Cgo = GOTF[(*(goptr + Cinc))];
          Dgo = GOTF[(*(goptr + Dinc))];
          Ego = GOTF[(*(goptr + Einc))];
          Fgo = GOTF[(*(goptr + Finc))];
          Ggo = GOTF[(*(goptr + Ginc))];
          Hgo = GOTF[(*(goptr + Hinc))];  
          }
        else
          {
          Ago = Bgo = Cgo = Dgo = Ego = Fgo = Ggo = Hgo = 1.0;
          }

        prev_voxel[0] = voxel[0];
        prev_voxel[1] = voxel[1];
        prev_voxel[2] = voxel[2];
        }
      
      // Compute our offset in the voxel, and use that to trilinearly
      // interpolate a value
      x = ray_position[0] - (float) voxel[0];
      y = ray_position[1] - (float) voxel[1];
      z = ray_position[2] - (float) voxel[2];
      
      t1 = 1.0 - x;
      t2 = 1.0 - y;
      t3 = 1.0 - z;
      
      // For this sample we have do not yet have any opacity or
      // shaded intensity yet
      opacity = 0.0;
      red_shaded_value = 0.0;
      green_shaded_value = 0.0;
      blue_shaded_value = 0.0;
      
      // Now add the opacity and shaded intensity value in vertex by 
      // vertex.  If any of the A-H have a non-transparent opacity value, 
      // then add its contribution to the opacity
      if ( A )
        {
        weight = t1*t2*t3 * A * Ago;
        opacity += weight;
        ////////////////////////////////////////////////////////////////////////////////////////////
//         red_shaded_value   += weight * ( red_d_shade[ *(nptr) ] * 
//                                      CTF[*(dptr) * 3] + 
//                                      red_s_shade[ *(nptr) ] );
//         green_shaded_value += weight * ( green_d_shade[ *(nptr) ] * 
//                                      CTF[*(dptr) * 3 + 1] + + 
//                                      green_s_shade[ *(nptr) ] );
//         blue_shaded_value  += weight * ( blue_d_shade[ *(nptr) ] * 
//                                      CTF[*(dptr) * 3 + 2] +
//                                      blue_s_shade[ *(nptr) ]  );

        double fx = 1.0;

        // contour
        if ( aFxContour > 0.0 )
        {
          float * fxGradient = fxDirectionEncoder->GetDecodedGradient( fxEncodedNormals[offset] );
          udg::Vector3 fxNormal( fxGradient[0], fxGradient[1], fxGradient[2] );
          double fxDotProduct = fxDirection * fxNormal;
          if ( fxDotProduct < 0.0 ) fxDotProduct = -fxDotProduct;
          fx *= fxDotProduct < aFxContour ? 0.0 : 1.0;
        }

        // obscurance
        if ( aFxObscurance && fx > 0.0 )
        {
          double fxObscurance = aObscurance[offset];
          if ( fxObscurance < aObscuranceFilterLow ) fxObscurance = 0.0;
          else if ( fxObscurance > aObscuranceFilterHigh ) fxObscurance = 1.0;
          fx *= aObscuranceFactor * fxObscurance;
        }

        red_shaded_value   += weight * ( red_d_shade[ *(nptr) ] * 
                                     CTF[*(dptr) * 3] + 
                                     red_s_shade[ *(nptr) ] ) * fx;
        green_shaded_value += weight * ( green_d_shade[ *(nptr) ] * 
                                     CTF[*(dptr) * 3 + 1] + + 
                                     green_s_shade[ *(nptr) ] ) * fx;
        blue_shaded_value  += weight * ( blue_d_shade[ *(nptr) ] * 
                                     CTF[*(dptr) * 3 + 2] +
                                     blue_s_shade[ *(nptr) ]  ) * fx;
        ////////////////////////////////////////////////////////////////////////////////////////////
        }
      
      if ( B )
        {
        weight = x*t2*t3 * B * Bgo;
        opacity += weight;
        ////////////////////////////////////////////////////////////////////////////////////////////
//         red_shaded_value   += weight * ( red_d_shade[ *(nptr + Binc) ] * 
//                                      CTF[*(dptr+Binc) * 3] + 
//                                      red_s_shade[ *(nptr + Binc) ] );
//         green_shaded_value += weight * ( green_d_shade[ *(nptr + Binc) ] *
//                                      CTF[*(dptr+Binc) * 3 + 1] + 
//                                      green_s_shade[ *(nptr + Binc) ] );
//         blue_shaded_value  += weight * ( blue_d_shade[ *(nptr + Binc) ] *
//                                      CTF[*(dptr+Binc) * 3 + 2] +
//                                      blue_s_shade[ *(nptr + Binc) ] );

        double fx = 1.0;

        // contour
        if ( aFxContour > 0.0 )
        {
          float * fxGradient = fxDirectionEncoder->GetDecodedGradient( fxEncodedNormals[offset + Binc] );
          udg::Vector3 fxNormal( fxGradient[0], fxGradient[1], fxGradient[2] );
          double fxDotProduct = fxDirection * fxNormal;
          if ( fxDotProduct < 0.0 ) fxDotProduct = -fxDotProduct;
          fx *= fxDotProduct < aFxContour ? 0.0 : 1.0;
        }

        // obscurance
        if ( aFxObscurance && fx > 0.0 )
        {
          double fxObscurance = aObscurance[offset + Binc];
          if ( fxObscurance < aObscuranceFilterLow ) fxObscurance = 0.0;
          else if ( fxObscurance > aObscuranceFilterHigh ) fxObscurance = 1.0;
          fx *= aObscuranceFactor * fxObscurance;
        }

        red_shaded_value   += weight * ( red_d_shade[ *(nptr + Binc) ] * 
                                     CTF[*(dptr+Binc) * 3] + 
                                     red_s_shade[ *(nptr + Binc) ] ) * fx;
        green_shaded_value += weight * ( green_d_shade[ *(nptr + Binc) ] *
                                     CTF[*(dptr+Binc) * 3 + 1] + 
                                     green_s_shade[ *(nptr + Binc) ] ) * fx;
        blue_shaded_value  += weight * ( blue_d_shade[ *(nptr + Binc) ] *
                                     CTF[*(dptr+Binc) * 3 + 2] +
                                     blue_s_shade[ *(nptr + Binc) ] ) * fx;
        ////////////////////////////////////////////////////////////////////////////////////////////
        }
      
      if ( C )
        {
        weight = t1*y*t3 * C * Cgo;
        opacity += weight;
        ////////////////////////////////////////////////////////////////////////////////////////////
//         red_shaded_value   += weight * ( red_d_shade[ *(nptr + Cinc) ] *
//                                      CTF[*(dptr+Cinc) * 3] + 
//                                      red_s_shade[ *(nptr + Cinc) ] );
//         green_shaded_value += weight * ( green_d_shade[ *(nptr + Cinc) ] *
//                                      CTF[*(dptr+Cinc) * 3 + 1] + 
//                                      green_s_shade[ *(nptr + Cinc) ] );
//         blue_shaded_value  += weight * ( blue_d_shade[ *(nptr + Cinc) ] *
//                                      CTF[*(dptr+Cinc) * 3 + 2] +
//                                      blue_s_shade[ *(nptr + Cinc) ] );

        double fx = 1.0;

        // contour
        if ( aFxContour > 0.0 )
        {
          float * fxGradient = fxDirectionEncoder->GetDecodedGradient( fxEncodedNormals[offset + Cinc] );
          udg::Vector3 fxNormal( fxGradient[0], fxGradient[1], fxGradient[2] );
          double fxDotProduct = fxDirection * fxNormal;
          if ( fxDotProduct < 0.0 ) fxDotProduct = -fxDotProduct;
          fx *= fxDotProduct < aFxContour ? 0.0 : 1.0;
        }

        // obscurance
        if ( aFxObscurance && fx > 0.0 )
        {
          double fxObscurance = aObscurance[offset + Cinc];
          if ( fxObscurance < aObscuranceFilterLow ) fxObscurance = 0.0;
          else if ( fxObscurance > aObscuranceFilterHigh ) fxObscurance = 1.0;
          fx *= aObscuranceFactor * fxObscurance;
        }

        red_shaded_value   += weight * ( red_d_shade[ *(nptr + Cinc) ] *
                                     CTF[*(dptr+Cinc) * 3] + 
                                     red_s_shade[ *(nptr + Cinc) ] ) * fx;
        green_shaded_value += weight * ( green_d_shade[ *(nptr + Cinc) ] *
                                     CTF[*(dptr+Cinc) * 3 + 1] + 
                                     green_s_shade[ *(nptr + Cinc) ] ) * fx;
        blue_shaded_value  += weight * ( blue_d_shade[ *(nptr + Cinc) ] *
                                     CTF[*(dptr+Cinc) * 3 + 2] +
                                     blue_s_shade[ *(nptr + Cinc) ] ) * fx;
        ////////////////////////////////////////////////////////////////////////////////////////////
        }

      if ( D )
        {
        weight = x*y*t3 * D * Dgo;
        opacity += weight;
        ////////////////////////////////////////////////////////////////////////////////////////////
//         red_shaded_value   += weight * ( red_d_shade[ *(nptr + Dinc) ] *
//                                      CTF[*(dptr+Dinc) * 3] + 
//                                      red_s_shade[ *(nptr + Dinc) ] );
//         green_shaded_value += weight * ( green_d_shade[ *(nptr + Dinc) ] *
//                                      CTF[*(dptr+Dinc) * 3 + 1] + 
//                                      green_s_shade[ *(nptr + Dinc) ] );
//         blue_shaded_value  += weight * ( blue_d_shade[ *(nptr + Dinc) ] *
//                                      CTF[*(dptr+Dinc) * 3 + 2] +
//                                      blue_s_shade[ *(nptr + Dinc) ] );

        double fx = 1.0;

        // contour
        if ( aFxContour > 0.0 )
        {
          float * fxGradient = fxDirectionEncoder->GetDecodedGradient( fxEncodedNormals[offset + Dinc] );
          udg::Vector3 fxNormal( fxGradient[0], fxGradient[1], fxGradient[2] );
          double fxDotProduct = fxDirection * fxNormal;
          if ( fxDotProduct < 0.0 ) fxDotProduct = -fxDotProduct;
          fx *= fxDotProduct < aFxContour ? 0.0 : 1.0;
        }

        // obscurance
        if ( aFxObscurance && fx > 0.0 )
        {
          double fxObscurance = aObscurance[offset + Dinc];
          if ( fxObscurance < aObscuranceFilterLow ) fxObscurance = 0.0;
          else if ( fxObscurance > aObscuranceFilterHigh ) fxObscurance = 1.0;
          fx *= aObscuranceFactor * fxObscurance;
        }

        red_shaded_value   += weight * ( red_d_shade[ *(nptr + Dinc) ] *
                                     CTF[*(dptr+Dinc) * 3] + 
                                     red_s_shade[ *(nptr + Dinc) ] ) * fx;
        green_shaded_value += weight * ( green_d_shade[ *(nptr + Dinc) ] *
                                     CTF[*(dptr+Dinc) * 3 + 1] + 
                                     green_s_shade[ *(nptr + Dinc) ] ) * fx;
        blue_shaded_value  += weight * ( blue_d_shade[ *(nptr + Dinc) ] *
                                     CTF[*(dptr+Dinc) * 3 + 2] +
                                     blue_s_shade[ *(nptr + Dinc) ] ) * fx;
        ////////////////////////////////////////////////////////////////////////////////////////////
        }
      
      if ( E )
        {
        weight = t1*t2*z * E * Ego;
        opacity += weight;
        ////////////////////////////////////////////////////////////////////////////////////////////
//         red_shaded_value   += weight * ( red_d_shade[ *(nptr + Einc) ] *
//                                      CTF[*(dptr+Einc) * 3] + 
//                                      red_s_shade[ *(nptr + Einc) ] );
//         green_shaded_value += weight * ( green_d_shade[ *(nptr + Einc) ] *
//                                      CTF[*(dptr+Einc) * 3 + 1] + 
//                                      green_s_shade[ *(nptr + Einc) ] );
//         blue_shaded_value  += weight * ( blue_d_shade[ *(nptr + Einc) ] *
//                                      CTF[*(dptr+Einc) * 3 + 2] +
//                                      blue_s_shade[ *(nptr + Einc) ] );

        double fx = 1.0;

        // contour
        if ( aFxContour > 0.0 )
        {
          float * fxGradient = fxDirectionEncoder->GetDecodedGradient( fxEncodedNormals[offset + Einc] );
          udg::Vector3 fxNormal( fxGradient[0], fxGradient[1], fxGradient[2] );
          double fxDotProduct = fxDirection * fxNormal;
          if ( fxDotProduct < 0.0 ) fxDotProduct = -fxDotProduct;
          fx *= fxDotProduct < aFxContour ? 0.0 : 1.0;
        }

        // obscurance
        if ( aFxObscurance && fx > 0.0 )
        {
          double fxObscurance = aObscurance[offset + Einc];
          if ( fxObscurance < aObscuranceFilterLow ) fxObscurance = 0.0;
          else if ( fxObscurance > aObscuranceFilterHigh ) fxObscurance = 1.0;
          fx *= aObscuranceFactor * fxObscurance;
        }

        red_shaded_value   += weight * ( red_d_shade[ *(nptr + Einc) ] *
                                     CTF[*(dptr+Einc) * 3] + 
                                     red_s_shade[ *(nptr + Einc) ] ) * fx;
        green_shaded_value += weight * ( green_d_shade[ *(nptr + Einc) ] *
                                     CTF[*(dptr+Einc) * 3 + 1] + 
                                     green_s_shade[ *(nptr + Einc) ] ) * fx;
        blue_shaded_value  += weight * ( blue_d_shade[ *(nptr + Einc) ] *
                                     CTF[*(dptr+Einc) * 3 + 2] +
                                     blue_s_shade[ *(nptr + Einc) ] ) * fx;
        ////////////////////////////////////////////////////////////////////////////////////////////
        }
      
      if ( F )
        {
        weight = x*z*t2 * F * Fgo;
        opacity += weight;
        ////////////////////////////////////////////////////////////////////////////////////////////
//         red_shaded_value   += weight * ( red_d_shade[ *(nptr + Finc) ] *
//                                      CTF[*(dptr+Finc) * 3] + 
//                                      red_s_shade[ *(nptr + Finc) ] );
//         green_shaded_value += weight * ( green_d_shade[ *(nptr + Finc) ] *
//                                      CTF[*(dptr+Finc) * 3 + 1] + 
//                                      green_s_shade[ *(nptr + Finc) ] );
//         blue_shaded_value  += weight * ( blue_d_shade[ *(nptr + Finc) ] *
//                                      CTF[*(dptr+Finc) * 3 + 2] +
//                                      blue_s_shade[ *(nptr + Finc) ] );

        double fx = 1.0;

        // contour
        if ( aFxContour > 0.0 )
        {
          float * fxGradient = fxDirectionEncoder->GetDecodedGradient( fxEncodedNormals[offset + Finc] );
          udg::Vector3 fxNormal( fxGradient[0], fxGradient[1], fxGradient[2] );
          double fxDotProduct = fxDirection * fxNormal;
          if ( fxDotProduct < 0.0 ) fxDotProduct = -fxDotProduct;
          fx *= fxDotProduct < aFxContour ? 0.0 : 1.0;
        }

        // obscurance
        if ( aFxObscurance && fx > 0.0 )
        {
          double fxObscurance = aObscurance[offset + Finc];
          if ( fxObscurance < aObscuranceFilterLow ) fxObscurance = 0.0;
          else if ( fxObscurance > aObscuranceFilterHigh ) fxObscurance = 1.0;
          fx *= aObscuranceFactor * fxObscurance;
        }

        red_shaded_value   += weight * ( red_d_shade[ *(nptr + Finc) ] *
                                     CTF[*(dptr+Finc) * 3] + 
                                     red_s_shade[ *(nptr + Finc) ] ) * fx;
        green_shaded_value += weight * ( green_d_shade[ *(nptr + Finc) ] *
                                     CTF[*(dptr+Finc) * 3 + 1] + 
                                     green_s_shade[ *(nptr + Finc) ] ) * fx;
        blue_shaded_value  += weight * ( blue_d_shade[ *(nptr + Finc) ] *
                                     CTF[*(dptr+Finc) * 3 + 2] +
                                     blue_s_shade[ *(nptr + Finc) ] ) * fx;
        ////////////////////////////////////////////////////////////////////////////////////////////
        }
      
      if ( G )
        {
        weight = t1*y*z * G * Ggo;
        opacity += weight;
        ////////////////////////////////////////////////////////////////////////////////////////////
//         red_shaded_value   += weight * ( red_d_shade[ *(nptr + Ginc) ] *
//                                      CTF[*(dptr+Ginc) * 3] + 
//                                      red_s_shade[ *(nptr + Ginc) ] );
//         green_shaded_value += weight * ( green_d_shade[ *(nptr + Ginc) ] *
//                                      CTF[*(dptr+Ginc) * 3 + 1] + 
//                                      green_s_shade[ *(nptr + Ginc) ] );
//         blue_shaded_value  += weight * ( blue_d_shade[ *(nptr + Ginc) ] *
//                                      CTF[*(dptr+Ginc) * 3 + 2] +
//                                      blue_s_shade[ *(nptr + Ginc) ] );

        double fx = 1.0;

        // contour
        if ( aFxContour > 0.0 )
        {
          float * fxGradient = fxDirectionEncoder->GetDecodedGradient( fxEncodedNormals[offset + Ginc] );
          udg::Vector3 fxNormal( fxGradient[0], fxGradient[1], fxGradient[2] );
          double fxDotProduct = fxDirection * fxNormal;
          if ( fxDotProduct < 0.0 ) fxDotProduct = -fxDotProduct;
          fx *= fxDotProduct < aFxContour ? 0.0 : 1.0;
        }

        // obscurance
        if ( aFxObscurance && fx > 0.0 )
        {
          double fxObscurance = aObscurance[offset + Ginc];
          if ( fxObscurance < aObscuranceFilterLow ) fxObscurance = 0.0;
          else if ( fxObscurance > aObscuranceFilterHigh ) fxObscurance = 1.0;
          fx *= aObscuranceFactor * fxObscurance;
        }

        red_shaded_value   += weight * ( red_d_shade[ *(nptr + Ginc) ] *
                                     CTF[*(dptr+Ginc) * 3] + 
                                     red_s_shade[ *(nptr + Ginc) ] ) * fx;
        green_shaded_value += weight * ( green_d_shade[ *(nptr + Ginc) ] *
                                     CTF[*(dptr+Ginc) * 3 + 1] + 
                                     green_s_shade[ *(nptr + Ginc) ] ) * fx;
        blue_shaded_value  += weight * ( blue_d_shade[ *(nptr + Ginc) ] *
                                     CTF[*(dptr+Ginc) * 3 + 2] +
                                     blue_s_shade[ *(nptr + Ginc) ] ) * fx;
        ////////////////////////////////////////////////////////////////////////////////////////////
      } 
      
      if ( H )
        {
        weight = x*z*y * H * Hgo;
        opacity += weight;
        ////////////////////////////////////////////////////////////////////////////////////////////
//         red_shaded_value   += weight * ( red_d_shade[ *(nptr + Hinc) ] *
//                                      CTF[*(dptr+Hinc) * 3] + 
//                                      red_s_shade[ *(nptr + Hinc) ] );
//         green_shaded_value += weight * ( green_d_shade[ *(nptr + Hinc) ] *
//                                      CTF[*(dptr+Hinc) * 3 + 1] + 
//                                      green_s_shade[ *(nptr + Hinc) ] );
//         blue_shaded_value  += weight * ( blue_d_shade[ *(nptr + Hinc) ] *
//                                      CTF[*(dptr+Hinc) * 3 + 2] +
//                                      blue_s_shade[ *(nptr + Hinc) ] );

        double fx = 1.0;

        // contour
        if ( aFxContour > 0.0 )
        {
          float * fxGradient = fxDirectionEncoder->GetDecodedGradient( fxEncodedNormals[offset + Hinc] );
          udg::Vector3 fxNormal( fxGradient[0], fxGradient[1], fxGradient[2] );
          double fxDotProduct = fxDirection * fxNormal;
          if ( fxDotProduct < 0.0 ) fxDotProduct = -fxDotProduct;
          fx *= fxDotProduct < aFxContour ? 0.0 : 1.0;
        }

        // obscurance
        if ( aFxObscurance && fx > 0.0 )
        {
          double fxObscurance = aObscurance[offset + Hinc];
          if ( fxObscurance < aObscuranceFilterLow ) fxObscurance = 0.0;
          else if ( fxObscurance > aObscuranceFilterHigh ) fxObscurance = 1.0;
          fx *= aObscuranceFactor * fxObscurance;
        }

        red_shaded_value   += weight * ( red_d_shade[ *(nptr + Hinc) ] *
                                     CTF[*(dptr+Hinc) * 3] + 
                                     red_s_shade[ *(nptr + Hinc) ] ) * fx;
        green_shaded_value += weight * ( green_d_shade[ *(nptr + Hinc) ] *
                                     CTF[*(dptr+Hinc) * 3 + 1] + 
                                     green_s_shade[ *(nptr + Hinc) ] ) * fx;
        blue_shaded_value  += weight * ( blue_d_shade[ *(nptr + Hinc) ] *
                                     CTF[*(dptr+Hinc) * 3 + 2] +
                                     blue_s_shade[ *(nptr + Hinc) ] ) * fx;
        ////////////////////////////////////////////////////////////////////////////////////////////
        }
      
      // Accumulate intensity and opacity for this sample location
      accum_red_intensity   += red_shaded_value   * remaining_opacity;
      accum_green_intensity += green_shaded_value * remaining_opacity;
      accum_blue_intensity  += blue_shaded_value  * remaining_opacity;
      remaining_opacity *= (1.0 - opacity);
      
      // Increment our position and compute our voxel location
      ray_position[0] += ray_increment[0];
      ray_position[1] += ray_increment[1];
      ray_position[2] += ray_increment[2];      
      voxel[0] = vtkFloorFuncMacro( ray_position[0] );
      voxel[1] = vtkFloorFuncMacro( ray_position[1] );
      voxel[2] = vtkFloorFuncMacro( ray_position[2] );
      }
    }

  // Cap the accumulated intensity at 1.0
  if ( accum_red_intensity > 1.0 )
    {
    accum_red_intensity = 1.0;
    }
  if ( accum_green_intensity > 1.0 )
    {
    accum_green_intensity = 1.0;
    }
  if ( accum_blue_intensity > 1.0 )
    {
    accum_blue_intensity = 1.0;
    }
  
  if( remaining_opacity < VTK_REMAINING_OPACITY )
    {
    remaining_opacity = 0.0;
    }

  // Set the return pixel value.  The depth value is the distance to the
  // center of the volume.
  dynamicInfo->Color[0] = accum_red_intensity;
  dynamicInfo->Color[1] = accum_green_intensity;
  dynamicInfo->Color[2] = accum_blue_intensity;
  dynamicInfo->Color[3] = 1.0 - remaining_opacity;
  dynamicInfo->NumberOfStepsTaken = steps_this_ray;


}


// Description:
// This is the templated function that actually casts a ray and computes
// the composite value.  This version uses trilinear interpolation, and
// does perform shading.
template <class T>
void vtkCastRay_TrilinVertices_Shaded( T *data_ptr, vtkVolumeRayCastDynamicInfo *dynamicInfo,
                                       vtkVolumeRayCastStaticInfo *staticInfo, udg::Vector3 * aColorBleeding, double aObscuranceFactor )
{
  unsigned char   *grad_mag_ptr = NULL;
  unsigned char   *goptr;
  float           accum_red_intensity;
  float           accum_green_intensity;
  float           accum_blue_intensity;
  float           remaining_opacity;
  float           opacity;
  int             loop;
  int             xinc, yinc, zinc;
  int             voxel[3];
  float           ray_position[3];
  int             prev_voxel[3];
  float           A, B, C, D, E, F, G, H;
  float           Ago, Bgo, Cgo, Dgo, Ego, Fgo, Ggo, Hgo;
  int             Binc, Cinc, Dinc, Einc, Finc, Ginc, Hinc;
  T               *dptr;
  float           *SOTF;
  float           *CTF;
  float           *GTF;
  float           *GOTF;
  float           x, y, z, t1, t2, t3;
  float           weight;
  float           *red_d_shade, *green_d_shade, *blue_d_shade;
  float           *red_s_shade, *green_s_shade, *blue_s_shade;
  unsigned short  *encoded_normals, *nptr;
  float           red_shaded_value, green_shaded_value, blue_shaded_value;
  int             offset;
  int             steps_this_ray = 0;
  int             grad_op_is_constant;
  float           gradient_opacity_constant;
  int             num_steps;
  float           *ray_start, *ray_increment;


  num_steps = dynamicInfo->NumberOfStepsToTake;
  ray_start = dynamicInfo->TransformedStart;
  ray_increment = dynamicInfo->TransformedIncrement;

  // Get diffuse shading table pointers
  red_d_shade = staticInfo->RedDiffuseShadingTable;
  green_d_shade = staticInfo->GreenDiffuseShadingTable;
  blue_d_shade = staticInfo->BlueDiffuseShadingTable;


  // Get diffuse shading table pointers
  red_s_shade = staticInfo->RedSpecularShadingTable;
  green_s_shade = staticInfo->GreenSpecularShadingTable;
  blue_s_shade = staticInfo->BlueSpecularShadingTable;

  // Get a pointer to the encoded normals for this volume
  encoded_normals = staticInfo->EncodedNormals;

  // Get the scalar opacity transfer function which maps scalar input values
  // to opacities
  SOTF =  staticInfo->Volume->GetCorrectedScalarOpacityArray();

  // Get the color transfer function which maps scalar input values
  // to RGB values
  CTF =  staticInfo->Volume->GetRGBArray();
  GTF =  staticInfo->Volume->GetGrayArray();

  // Get the gradient opacity transfer function for this volume (which maps
  // gradient magnitudes to opacities)
  GOTF =  staticInfo->Volume->GetGradientOpacityArray();

  // Get the gradient opacity constant. If this number is greater than
  // or equal to 0.0, then the gradient opacity transfer function is
  // a constant at that value, otherwise it is not a constant function
  gradient_opacity_constant = staticInfo->Volume->GetGradientOpacityConstant();
  grad_op_is_constant = ( gradient_opacity_constant >= 0.0 );

  // Get a pointer to the gradient magnitudes for this volume
  if ( !grad_op_is_constant )
    {
    grad_mag_ptr = staticInfo->GradientMagnitudes;
    }

  // Move the increments into local variables
  xinc = staticInfo->DataIncrement[0];
  yinc = staticInfo->DataIncrement[1];
  zinc = staticInfo->DataIncrement[2];

  // Initialize the ray position and voxel location
  ray_position[0] = ray_start[0];
  ray_position[1] = ray_start[1];
  ray_position[2] = ray_start[2];
  voxel[0] = vtkFloorFuncMacro( ray_position[0] );
  voxel[1] = vtkFloorFuncMacro( ray_position[1] );
  voxel[2] = vtkFloorFuncMacro( ray_position[2] );

  // So far we haven't accumulated anything
  accum_red_intensity     = 0.0;
  accum_green_intensity   = 0.0;
  accum_blue_intensity    = 0.0;
  remaining_opacity       = 1.0;

  // Compute the increments to get to the other 7 voxel vertices from A
  Binc = xinc;
  Cinc = yinc;
  Dinc = xinc + yinc;
  Einc = zinc;
  Finc = zinc + xinc;
  Ginc = zinc + yinc;
  Hinc = zinc + xinc + yinc;
  
   // Compute the values for the first pass through the loop
  offset = voxel[2] * zinc + voxel[1] * yinc + voxel[0];
  dptr   = data_ptr + offset;
  nptr   = encoded_normals + offset;

  A = SOTF[(*(dptr))];
  B = SOTF[(*(dptr + Binc))];
  C = SOTF[(*(dptr + Cinc))];
  D = SOTF[(*(dptr + Dinc))];
  E = SOTF[(*(dptr + Einc))];
  F = SOTF[(*(dptr + Finc))];
  G = SOTF[(*(dptr + Ginc))];
  H = SOTF[(*(dptr + Hinc))];

  if ( !grad_op_is_constant )
    {
    goptr  = grad_mag_ptr + offset;
    Ago = GOTF[(*(goptr))];
    Bgo = GOTF[(*(goptr + Binc))];
    Cgo = GOTF[(*(goptr + Cinc))];
    Dgo = GOTF[(*(goptr + Dinc))];
    Ego = GOTF[(*(goptr + Einc))];
    Fgo = GOTF[(*(goptr + Finc))];
    Ggo = GOTF[(*(goptr + Ginc))];
    Hgo = GOTF[(*(goptr + Hinc))];  
    }
  else
    {
    Ago = Bgo = Cgo = Dgo = Ego = Fgo = Ggo = Hgo = 1.0;
    }


  // Keep track of previous voxel to know when we step into a new one   
  prev_voxel[0] = voxel[0];
  prev_voxel[1] = voxel[1];
  prev_voxel[2] = voxel[2];

  // Two cases - we are working with a single color or a color transfer
  // function - break them up to make it more efficient
  if ( staticInfo->ColorChannels == 1 )
    {
    // For each step along the ray
    for ( loop = 0; 
          loop < num_steps && remaining_opacity > VTK_REMAINING_OPACITY; 
          loop++ )
      {     
      // We've taken another step
      steps_this_ray++;
      
      // Have we moved into a new voxel? If so we need to recompute A-H
      if ( prev_voxel[0] != voxel[0] ||
           prev_voxel[1] != voxel[1] ||
           prev_voxel[2] != voxel[2] )
        {
        offset = voxel[2] * zinc + voxel[1] * yinc + voxel[0];
        dptr   = data_ptr + offset;
        nptr   = encoded_normals + offset;
        //goptr  = grad_mag_ptr + offset;
        
        A = SOTF[(*(dptr))];
        B = SOTF[(*(dptr + Binc))];
        C = SOTF[(*(dptr + Cinc))];
        D = SOTF[(*(dptr + Dinc))];
        E = SOTF[(*(dptr + Einc))];
        F = SOTF[(*(dptr + Finc))];
        G = SOTF[(*(dptr + Ginc))];
        H = SOTF[(*(dptr + Hinc))];
        
        if ( !grad_op_is_constant )
          {
          goptr  = grad_mag_ptr + offset;
          Ago = GOTF[(*(goptr))];
          Bgo = GOTF[(*(goptr + Binc))];
          Cgo = GOTF[(*(goptr + Cinc))];
          Dgo = GOTF[(*(goptr + Dinc))];
          Ego = GOTF[(*(goptr + Einc))];
          Fgo = GOTF[(*(goptr + Finc))];
          Ggo = GOTF[(*(goptr + Ginc))];
          Hgo = GOTF[(*(goptr + Hinc))];  
          }
        else
          {
          Ago = Bgo = Cgo = Dgo = Ego = Fgo = Ggo = Hgo = 1.0;
          }

        prev_voxel[0] = voxel[0];
        prev_voxel[1] = voxel[1];
        prev_voxel[2] = voxel[2];
        }
      
      // Compute our offset in the voxel, and use that to trilinearly
      // interpolate a value
      x = ray_position[0] - (float) voxel[0];
      y = ray_position[1] - (float) voxel[1];
      z = ray_position[2] - (float) voxel[2];
      
      t1 = 1.0 - x;
      t2 = 1.0 - y;
      t3 = 1.0 - z;
      
      // For this sample we have do not yet have any opacity or
      // shaded intensity yet
      opacity = 0.0;
      red_shaded_value = 0.0;
      
      // Now add the opacity and shaded intensity value in vertex by 
      // vertex.  If any of the A-H have a non-transparent opacity value, 
      // then add its contribution to the opacity
      if ( A && Ago )
        {
        weight = t1*t2*t3 * A * Ago;
        opacity += weight;
        ////////////////////////////////////////////////////////////////////////////////////////////
//         red_shaded_value   += weight * ( red_d_shade[ *(nptr) ] * 
//                                      GTF[*(dptr)] + 
//                                      red_s_shade[ *(nptr) ] );
        udg::Vector3 vColor = aColorBleeding[offset];
        red_shaded_value   += weight * ( red_d_shade[ *(nptr) ] * 
                                     GTF[*(dptr)] + 
                                     red_s_shade[ *(nptr) ] ) * aObscuranceFactor * ( vColor.x + vColor.y + vColor.z ) / 3.0;
        ////////////////////////////////////////////////////////////////////////////////////////////
        }
      
      if ( B && Bgo )
        {
        weight = x*t2*t3 * B * Bgo;
        opacity += weight;
        ////////////////////////////////////////////////////////////////////////////////////////////
//         red_shaded_value   += weight * ( red_d_shade[ *(nptr + Binc) ] * 
//                                      GTF[*(dptr+Binc)] + 
//                                      red_s_shade[ *(nptr + Binc) ] );
        udg::Vector3 vColor = aColorBleeding[offset + Binc];
        red_shaded_value   += weight * ( red_d_shade[ *(nptr + Binc) ] * 
                                     GTF[*(dptr+Binc)] + 
                                     red_s_shade[ *(nptr + Binc) ] ) * aObscuranceFactor * ( vColor.x + vColor.y + vColor.z ) / 3.0;
        ////////////////////////////////////////////////////////////////////////////////////////////
        }
      
      if ( C && Cgo )
        {
        weight = t1*y*t3 * C * Cgo;
        opacity += weight;
        ////////////////////////////////////////////////////////////////////////////////////////////
//         red_shaded_value   += weight * ( red_d_shade[ *(nptr + Cinc) ] *
//                                      GTF[*(dptr+Cinc)] + 
//                                      red_s_shade[ *(nptr + Cinc) ] );
        udg::Vector3 vColor = aColorBleeding[offset + Cinc];
        red_shaded_value   += weight * ( red_d_shade[ *(nptr + Cinc) ] *
                                     GTF[*(dptr+Cinc)] + 
                                     red_s_shade[ *(nptr + Cinc) ] ) * aObscuranceFactor * ( vColor.x + vColor.y + vColor.z ) / 3.0;
        ////////////////////////////////////////////////////////////////////////////////////////////
        }

      if ( D && Dgo )
        {
        weight = x*y*t3 * D * Dgo;
        opacity += weight;
        ////////////////////////////////////////////////////////////////////////////////////////////
//         red_shaded_value   += weight * ( red_d_shade[ *(nptr + Dinc) ] *
//                                      GTF[*(dptr+Dinc)] + 
//                                      red_s_shade[ *(nptr + Dinc) ] );
        udg::Vector3 vColor = aColorBleeding[offset + Dinc];
        red_shaded_value   += weight * ( red_d_shade[ *(nptr + Dinc) ] *
                                     GTF[*(dptr+Dinc)] + 
                                     red_s_shade[ *(nptr + Dinc) ] ) * aObscuranceFactor * ( vColor.x + vColor.y + vColor.z ) / 3.0;
        ////////////////////////////////////////////////////////////////////////////////////////////
        }
      
      if ( E && Ego )
        {
        weight = t1*t2*z * E * Ego;
        opacity += weight;
        ////////////////////////////////////////////////////////////////////////////////////////////
//         red_shaded_value   += weight * ( red_d_shade[ *(nptr + Einc) ] *
//                                      GTF[*(dptr+Einc)] + 
//                                      red_s_shade[ *(nptr + Einc) ] );
        udg::Vector3 vColor = aColorBleeding[offset + Einc];
        red_shaded_value   += weight * ( red_d_shade[ *(nptr + Einc) ] *
                                     GTF[*(dptr+Einc)] + 
                                     red_s_shade[ *(nptr + Einc) ] ) * aObscuranceFactor * ( vColor.x + vColor.y + vColor.z ) / 3.0;
        ////////////////////////////////////////////////////////////////////////////////////////////
        }
      
      if ( F && Fgo )
        {
        weight = x*z*t2 * F * Fgo;
        opacity += weight;
        ////////////////////////////////////////////////////////////////////////////////////////////
//         red_shaded_value   += weight * ( red_d_shade[ *(nptr + Finc) ] *
//                                      GTF[*(dptr+Finc)] + 
//                                      red_s_shade[ *(nptr + Finc) ] );
        udg::Vector3 vColor = aColorBleeding[offset + Finc];
        red_shaded_value   += weight * ( red_d_shade[ *(nptr + Finc) ] *
                                     GTF[*(dptr+Finc)] + 
                                     red_s_shade[ *(nptr + Finc) ] ) * aObscuranceFactor * ( vColor.x + vColor.y + vColor.z ) / 3.0;
        ////////////////////////////////////////////////////////////////////////////////////////////
        }
      
      if ( G && Ggo )
        {
        weight = t1*y*z * G * Ggo;
        opacity += weight;
        ////////////////////////////////////////////////////////////////////////////////////////////
//         red_shaded_value   += weight * ( red_d_shade[ *(nptr + Ginc) ] *
//                                      GTF[*(dptr+Ginc)] + 
//                                      red_s_shade[ *(nptr + Ginc) ] );
        udg::Vector3 vColor = aColorBleeding[offset + Ginc];
        red_shaded_value   += weight * ( red_d_shade[ *(nptr + Ginc) ] *
                                     GTF[*(dptr+Ginc)] + 
                                     red_s_shade[ *(nptr + Ginc) ] ) * aObscuranceFactor * ( vColor.x + vColor.y + vColor.z ) / 3.0;
        ////////////////////////////////////////////////////////////////////////////////////////////
      } 
      
      if ( H && Hgo )
        {
        weight = x*z*y * H * Hgo;
        opacity += weight;
        ////////////////////////////////////////////////////////////////////////////////////////////
//         red_shaded_value   += weight * ( red_d_shade[ *(nptr + Hinc) ] *
//                                      GTF[*(dptr+Hinc)] + 
//                                      red_s_shade[ *(nptr + Hinc) ] );
        udg::Vector3 vColor = aColorBleeding[offset + Hinc];
        red_shaded_value   += weight * ( red_d_shade[ *(nptr + Hinc) ] *
                                     GTF[*(dptr+Hinc)] + 
                                     red_s_shade[ *(nptr + Hinc) ] ) * aObscuranceFactor * ( vColor.x + vColor.y + vColor.z ) / 3.0;
        ////////////////////////////////////////////////////////////////////////////////////////////
        }
      
      // Accumulate intensity and opacity for this sample location
      accum_red_intensity   += red_shaded_value   * remaining_opacity;
      remaining_opacity *= (1.0 - opacity);
      
      // Increment our position and compute our voxel location
      ray_position[0] += ray_increment[0];
      ray_position[1] += ray_increment[1];
      ray_position[2] += ray_increment[2];      
      voxel[0] = vtkFloorFuncMacro( ray_position[0] );
      voxel[1] = vtkFloorFuncMacro( ray_position[1] );
      voxel[2] = vtkFloorFuncMacro( ray_position[2] );
      }
    accum_green_intensity = accum_red_intensity;
    accum_blue_intensity = accum_red_intensity;
    }
  else if ( staticInfo->ColorChannels == 3 )
    {
    // For each step along the ray
    for ( loop = 0; 
          loop < num_steps && remaining_opacity > VTK_REMAINING_OPACITY; 
          loop++ )
      {     
      // We've taken another step
      steps_this_ray++;
      
      // Have we moved into a new voxel? If so we need to recompute A-H
      if ( prev_voxel[0] != voxel[0] ||
           prev_voxel[1] != voxel[1] ||
           prev_voxel[2] != voxel[2] )
        {
        offset = voxel[2] * zinc + voxel[1] * yinc + voxel[0];
        dptr   = data_ptr + offset;
        nptr   = encoded_normals + offset;
        
        A = SOTF[(*(dptr))];
        B = SOTF[(*(dptr + Binc))];
        C = SOTF[(*(dptr + Cinc))];
        D = SOTF[(*(dptr + Dinc))];
        E = SOTF[(*(dptr + Einc))];
        F = SOTF[(*(dptr + Finc))];
        G = SOTF[(*(dptr + Ginc))];
        H = SOTF[(*(dptr + Hinc))];
        
        if ( !grad_op_is_constant )
          {
          goptr  = grad_mag_ptr + offset;
          Ago = GOTF[(*(goptr))];
          Bgo = GOTF[(*(goptr + Binc))];
          Cgo = GOTF[(*(goptr + Cinc))];
          Dgo = GOTF[(*(goptr + Dinc))];
          Ego = GOTF[(*(goptr + Einc))];
          Fgo = GOTF[(*(goptr + Finc))];
          Ggo = GOTF[(*(goptr + Ginc))];
          Hgo = GOTF[(*(goptr + Hinc))];  
          }
        else
          {
          Ago = Bgo = Cgo = Dgo = Ego = Fgo = Ggo = Hgo = 1.0;
          }

        prev_voxel[0] = voxel[0];
        prev_voxel[1] = voxel[1];
        prev_voxel[2] = voxel[2];
        }
      
      // Compute our offset in the voxel, and use that to trilinearly
      // interpolate a value
      x = ray_position[0] - (float) voxel[0];
      y = ray_position[1] - (float) voxel[1];
      z = ray_position[2] - (float) voxel[2];
      
      t1 = 1.0 - x;
      t2 = 1.0 - y;
      t3 = 1.0 - z;
      
      // For this sample we have do not yet have any opacity or
      // shaded intensity yet
      opacity = 0.0;
      red_shaded_value = 0.0;
      green_shaded_value = 0.0;
      blue_shaded_value = 0.0;
      
      // Now add the opacity and shaded intensity value in vertex by 
      // vertex.  If any of the A-H have a non-transparent opacity value, 
      // then add its contribution to the opacity
      if ( A )
        {
        weight = t1*t2*t3 * A * Ago;
        opacity += weight;
        ////////////////////////////////////////////////////////////////////////////////////////////
//         red_shaded_value   += weight * ( red_d_shade[ *(nptr) ] * 
//                                      CTF[*(dptr) * 3] + 
//                                      red_s_shade[ *(nptr) ] );
//         green_shaded_value += weight * ( green_d_shade[ *(nptr) ] * 
//                                      CTF[*(dptr) * 3 + 1] + + 
//                                      green_s_shade[ *(nptr) ] );
//         blue_shaded_value  += weight * ( blue_d_shade[ *(nptr) ] * 
//                                      CTF[*(dptr) * 3 + 2] +
//                                      blue_s_shade[ *(nptr) ]  );
        udg::Vector3 vColor = aColorBleeding[offset];
        red_shaded_value   += weight * ( red_d_shade[ *(nptr) ] * 
                                     CTF[*(dptr) * 3] + 
                                     red_s_shade[ *(nptr) ] ) * aObscuranceFactor * vColor.x;
        green_shaded_value += weight * ( green_d_shade[ *(nptr) ] * 
                                     CTF[*(dptr) * 3 + 1] + + 
                                     green_s_shade[ *(nptr) ] ) * aObscuranceFactor * vColor.y;
        blue_shaded_value  += weight * ( blue_d_shade[ *(nptr) ] * 
                                     CTF[*(dptr) * 3 + 2] +
                                     blue_s_shade[ *(nptr) ]  ) * aObscuranceFactor * vColor.z;
        ////////////////////////////////////////////////////////////////////////////////////////////
        }
      
      if ( B )
        {
        weight = x*t2*t3 * B * Bgo;
        opacity += weight;
        ////////////////////////////////////////////////////////////////////////////////////////////
//         red_shaded_value   += weight * ( red_d_shade[ *(nptr + Binc) ] * 
//                                      CTF[*(dptr+Binc) * 3] + 
//                                      red_s_shade[ *(nptr + Binc) ] );
//         green_shaded_value += weight * ( green_d_shade[ *(nptr + Binc) ] *
//                                      CTF[*(dptr+Binc) * 3 + 1] + 
//                                      green_s_shade[ *(nptr + Binc) ] );
//         blue_shaded_value  += weight * ( blue_d_shade[ *(nptr + Binc) ] *
//                                      CTF[*(dptr+Binc) * 3 + 2] +
//                                      blue_s_shade[ *(nptr + Binc) ] );
        udg::Vector3 vColor = aColorBleeding[offset + Binc];
        red_shaded_value   += weight * ( red_d_shade[ *(nptr + Binc) ] * 
                                     CTF[*(dptr+Binc) * 3] + 
                                     red_s_shade[ *(nptr + Binc) ] ) * aObscuranceFactor * vColor.x;
        green_shaded_value += weight * ( green_d_shade[ *(nptr + Binc) ] *
                                     CTF[*(dptr+Binc) * 3 + 1] + 
                                     green_s_shade[ *(nptr + Binc) ] ) * aObscuranceFactor * vColor.y;
        blue_shaded_value  += weight * ( blue_d_shade[ *(nptr + Binc) ] *
                                     CTF[*(dptr+Binc) * 3 + 2] +
                                     blue_s_shade[ *(nptr + Binc) ] ) * aObscuranceFactor * vColor.z;
        ////////////////////////////////////////////////////////////////////////////////////////////
        }
      
      if ( C )
        {
        weight = t1*y*t3 * C * Cgo;
        opacity += weight;
        ////////////////////////////////////////////////////////////////////////////////////////////
//         red_shaded_value   += weight * ( red_d_shade[ *(nptr + Cinc) ] *
//                                      CTF[*(dptr+Cinc) * 3] + 
//                                      red_s_shade[ *(nptr + Cinc) ] );
//         green_shaded_value += weight * ( green_d_shade[ *(nptr + Cinc) ] *
//                                      CTF[*(dptr+Cinc) * 3 + 1] + 
//                                      green_s_shade[ *(nptr + Cinc) ] );
//         blue_shaded_value  += weight * ( blue_d_shade[ *(nptr + Cinc) ] *
//                                      CTF[*(dptr+Cinc) * 3 + 2] +
//                                      blue_s_shade[ *(nptr + Cinc) ] );
        udg::Vector3 vColor = aColorBleeding[offset + Cinc];
        red_shaded_value   += weight * ( red_d_shade[ *(nptr + Cinc) ] *
                                     CTF[*(dptr+Cinc) * 3] + 
                                     red_s_shade[ *(nptr + Cinc) ] ) * aObscuranceFactor * vColor.x;
        green_shaded_value += weight * ( green_d_shade[ *(nptr + Cinc) ] *
                                     CTF[*(dptr+Cinc) * 3 + 1] +
                                     green_s_shade[ *(nptr + Cinc) ] ) * aObscuranceFactor * vColor.y;
        blue_shaded_value  += weight * ( blue_d_shade[ *(nptr + Cinc) ] *
                                     CTF[*(dptr+Cinc) * 3 + 2] +
                                     blue_s_shade[ *(nptr + Cinc) ] ) * aObscuranceFactor * vColor.z;
        ////////////////////////////////////////////////////////////////////////////////////////////
        }

      if ( D )
        {
        weight = x*y*t3 * D * Dgo;
        opacity += weight;
        ////////////////////////////////////////////////////////////////////////////////////////////
//         red_shaded_value   += weight * ( red_d_shade[ *(nptr + Dinc) ] *
//                                      CTF[*(dptr+Dinc) * 3] + 
//                                      red_s_shade[ *(nptr + Dinc) ] );
//         green_shaded_value += weight * ( green_d_shade[ *(nptr + Dinc) ] *
//                                      CTF[*(dptr+Dinc) * 3 + 1] + 
//                                      green_s_shade[ *(nptr + Dinc) ] );
//         blue_shaded_value  += weight * ( blue_d_shade[ *(nptr + Dinc) ] *
//                                      CTF[*(dptr+Dinc) * 3 + 2] +
//                                      blue_s_shade[ *(nptr + Dinc) ] );
        udg::Vector3 vColor = aColorBleeding[offset + Dinc];
        red_shaded_value   += weight * ( red_d_shade[ *(nptr + Dinc) ] *
                                     CTF[*(dptr+Dinc) * 3] + 
                                     red_s_shade[ *(nptr + Dinc) ] ) * aObscuranceFactor * vColor.x;
        green_shaded_value += weight * ( green_d_shade[ *(nptr + Dinc) ] *
                                     CTF[*(dptr+Dinc) * 3 + 1] +
                                     green_s_shade[ *(nptr + Dinc) ] ) * aObscuranceFactor * vColor.y;
        blue_shaded_value  += weight * ( blue_d_shade[ *(nptr + Dinc) ] *
                                     CTF[*(dptr+Dinc) * 3 + 2] +
                                     blue_s_shade[ *(nptr + Dinc) ] ) * aObscuranceFactor * vColor.z;
        ////////////////////////////////////////////////////////////////////////////////////////////
        }
      
      if ( E )
        {
        weight = t1*t2*z * E * Ego;
        opacity += weight;
        ////////////////////////////////////////////////////////////////////////////////////////////
//         red_shaded_value   += weight * ( red_d_shade[ *(nptr + Einc) ] *
//                                      CTF[*(dptr+Einc) * 3] + 
//                                      red_s_shade[ *(nptr + Einc) ] );
//         green_shaded_value += weight * ( green_d_shade[ *(nptr + Einc) ] *
//                                      CTF[*(dptr+Einc) * 3 + 1] + 
//                                      green_s_shade[ *(nptr + Einc) ] );
//         blue_shaded_value  += weight * ( blue_d_shade[ *(nptr + Einc) ] *
//                                      CTF[*(dptr+Einc) * 3 + 2] +
//                                      blue_s_shade[ *(nptr + Einc) ] );
        udg::Vector3 vColor = aColorBleeding[offset + Einc];
        red_shaded_value   += weight * ( red_d_shade[ *(nptr + Einc) ] *
                                     CTF[*(dptr+Einc) * 3] + 
                                     red_s_shade[ *(nptr + Einc) ] ) * aObscuranceFactor * vColor.x;
        green_shaded_value += weight * ( green_d_shade[ *(nptr + Einc) ] *
                                     CTF[*(dptr+Einc) * 3 + 1] +
                                     green_s_shade[ *(nptr + Einc) ] ) * aObscuranceFactor * vColor.y;
        blue_shaded_value  += weight * ( blue_d_shade[ *(nptr + Einc) ] *
                                     CTF[*(dptr+Einc) * 3 + 2] +
                                     blue_s_shade[ *(nptr + Einc) ] ) * aObscuranceFactor * vColor.z;
        ////////////////////////////////////////////////////////////////////////////////////////////
        }
      
      if ( F )
        {
        weight = x*z*t2 * F * Fgo;
        opacity += weight;
        ////////////////////////////////////////////////////////////////////////////////////////////
//         red_shaded_value   += weight * ( red_d_shade[ *(nptr + Finc) ] *
//                                      CTF[*(dptr+Finc) * 3] + 
//                                      red_s_shade[ *(nptr + Finc) ] );
//         green_shaded_value += weight * ( green_d_shade[ *(nptr + Finc) ] *
//                                      CTF[*(dptr+Finc) * 3 + 1] + 
//                                      green_s_shade[ *(nptr + Finc) ] );
//         blue_shaded_value  += weight * ( blue_d_shade[ *(nptr + Finc) ] *
//                                      CTF[*(dptr+Finc) * 3 + 2] +
//                                      blue_s_shade[ *(nptr + Finc) ] );
        udg::Vector3 vColor = aColorBleeding[offset + Finc];
        red_shaded_value   += weight * ( red_d_shade[ *(nptr + Finc) ] *
                                     CTF[*(dptr+Finc) * 3] + 
                                     red_s_shade[ *(nptr + Finc) ] ) * aObscuranceFactor * vColor.x;
        green_shaded_value += weight * ( green_d_shade[ *(nptr + Finc) ] *
                                     CTF[*(dptr+Finc) * 3 + 1] +
                                     green_s_shade[ *(nptr + Finc) ] ) * aObscuranceFactor * vColor.y;
        blue_shaded_value  += weight * ( blue_d_shade[ *(nptr + Finc) ] *
                                     CTF[*(dptr+Finc) * 3 + 2] +
                                     blue_s_shade[ *(nptr + Finc) ] ) * aObscuranceFactor * vColor.z;
        ////////////////////////////////////////////////////////////////////////////////////////////
        }
      
      if ( G )
        {
        weight = t1*y*z * G * Ggo;
        opacity += weight;
        ////////////////////////////////////////////////////////////////////////////////////////////
//         red_shaded_value   += weight * ( red_d_shade[ *(nptr + Ginc) ] *
//                                      CTF[*(dptr+Ginc) * 3] + 
//                                      red_s_shade[ *(nptr + Ginc) ] );
//         green_shaded_value += weight * ( green_d_shade[ *(nptr + Ginc) ] *
//                                      CTF[*(dptr+Ginc) * 3 + 1] + 
//                                      green_s_shade[ *(nptr + Ginc) ] );
//         blue_shaded_value  += weight * ( blue_d_shade[ *(nptr + Ginc) ] *
//                                      CTF[*(dptr+Ginc) * 3 + 2] +
//                                      blue_s_shade[ *(nptr + Ginc) ] );
        udg::Vector3 vColor = aColorBleeding[offset + Ginc];
        red_shaded_value   += weight * ( red_d_shade[ *(nptr + Ginc) ] *
                                     CTF[*(dptr+Ginc) * 3] + 
                                     red_s_shade[ *(nptr + Ginc) ] ) * aObscuranceFactor * vColor.x;
        green_shaded_value += weight * ( green_d_shade[ *(nptr + Ginc) ] *
                                     CTF[*(dptr+Ginc) * 3 + 1] +
                                     green_s_shade[ *(nptr + Ginc) ] ) * aObscuranceFactor * vColor.y;
        blue_shaded_value  += weight * ( blue_d_shade[ *(nptr + Ginc) ] *
                                     CTF[*(dptr+Ginc) * 3 + 2] +
                                     blue_s_shade[ *(nptr + Ginc) ] ) * aObscuranceFactor * vColor.z;
        ////////////////////////////////////////////////////////////////////////////////////////////
      } 
      
      if ( H )
        {
        weight = x*z*y * H * Hgo;
        opacity += weight;
        ////////////////////////////////////////////////////////////////////////////////////////////
//         red_shaded_value   += weight * ( red_d_shade[ *(nptr + Hinc) ] *
//                                      CTF[*(dptr+Hinc) * 3] + 
//                                      red_s_shade[ *(nptr + Hinc) ] );
//         green_shaded_value += weight * ( green_d_shade[ *(nptr + Hinc) ] *
//                                      CTF[*(dptr+Hinc) * 3 + 1] + 
//                                      green_s_shade[ *(nptr + Hinc) ] );
//         blue_shaded_value  += weight * ( blue_d_shade[ *(nptr + Hinc) ] *
//                                      CTF[*(dptr+Hinc) * 3 + 2] +
//                                      blue_s_shade[ *(nptr + Hinc) ] );
        udg::Vector3 vColor = aColorBleeding[offset + Hinc];
        red_shaded_value   += weight * ( red_d_shade[ *(nptr + Hinc) ] *
                                     CTF[*(dptr+Hinc) * 3] + 
                                     red_s_shade[ *(nptr + Hinc) ] ) * aObscuranceFactor * vColor.x;
        green_shaded_value += weight * ( green_d_shade[ *(nptr + Hinc) ] *
                                     CTF[*(dptr+Hinc) * 3 + 1] +
                                     green_s_shade[ *(nptr + Hinc) ] ) * aObscuranceFactor * vColor.y;
        blue_shaded_value  += weight * ( blue_d_shade[ *(nptr + Hinc) ] *
                                     CTF[*(dptr+Hinc) * 3 + 2] +
                                     blue_s_shade[ *(nptr + Hinc) ] ) * aObscuranceFactor * vColor.z;
        ////////////////////////////////////////////////////////////////////////////////////////////
        }
      
      // Accumulate intensity and opacity for this sample location
      accum_red_intensity   += red_shaded_value   * remaining_opacity;
      accum_green_intensity += green_shaded_value * remaining_opacity;
      accum_blue_intensity  += blue_shaded_value  * remaining_opacity;
      remaining_opacity *= (1.0 - opacity);
      
      // Increment our position and compute our voxel location
      ray_position[0] += ray_increment[0];
      ray_position[1] += ray_increment[1];
      ray_position[2] += ray_increment[2];      
      voxel[0] = vtkFloorFuncMacro( ray_position[0] );
      voxel[1] = vtkFloorFuncMacro( ray_position[1] );
      voxel[2] = vtkFloorFuncMacro( ray_position[2] );
      }
    }

  // Cap the accumulated intensity at 1.0
  if ( accum_red_intensity > 1.0 )
    {
    accum_red_intensity = 1.0;
    }
  if ( accum_green_intensity > 1.0 )
    {
    accum_green_intensity = 1.0;
    }
  if ( accum_blue_intensity > 1.0 )
    {
    accum_blue_intensity = 1.0;
    }
  
  if( remaining_opacity < VTK_REMAINING_OPACITY )
    {
    remaining_opacity = 0.0;
    }

  // Set the return pixel value.  The depth value is the distance to the
  // center of the volume.
  dynamicInfo->Color[0] = accum_red_intensity;
  dynamicInfo->Color[1] = accum_green_intensity;
  dynamicInfo->Color[2] = accum_blue_intensity;
  dynamicInfo->Color[3] = 1.0 - remaining_opacity;
  dynamicInfo->NumberOfStepsTaken = steps_this_ray;


}


// Constructor for the vtkVolumeRayCastCompositeFunctionFx class
vtkVolumeRayCastCompositeFunctionFx::vtkVolumeRayCastCompositeFunctionFx()
{
  this->CompositeMethod = VTK_COMPOSITE_INTERPOLATE_FIRST;
  this->Obscurance = 0;
  this->Saliency = 0;
}

// Destruct the vtkVolumeRayCastCompositeFunctionFx
vtkVolumeRayCastCompositeFunctionFx::~vtkVolumeRayCastCompositeFunctionFx()
{
}

// This is called from RenderAnImage (in vtkDepthPARCMapper.cxx)
// It uses the integer data type flag that is passed in to
// determine what type of ray needs to be cast (which is handled
// by a templated function.  It also uses the shading and
// interpolation types to determine which templated function
// to call.
void vtkVolumeRayCastCompositeFunctionFx::CastRay( vtkVolumeRayCastDynamicInfo *dynamicInfo,
                                                 vtkVolumeRayCastStaticInfo *staticInfo )
{
  void *data_ptr;

  data_ptr = staticInfo->ScalarDataPointer;

  // Cast the ray for the data type and shading/interpolation type
  if ( staticInfo->InterpolationType == VTK_NEAREST_INTERPOLATION )
    {
    if ( staticInfo->Shading == 0 )
      {
      // Nearest neighbor and no shading
      switch ( staticInfo->ScalarDataType )
        {
        case VTK_UNSIGNED_CHAR:
          if ( Color )
            vtkCastRay_NN_Unshaded( (unsigned char *)data_ptr, dynamicInfo,
                                    staticInfo, ColorBleeding, ObscuranceFactor );
          else
            vtkCastRay_NN_Unshaded( (unsigned char *)data_ptr, dynamicInfo,
                                    staticInfo, Obscurance, ObscuranceFactor, ObscuranceFilterLow, ObscuranceFilterHigh,
                                    FxObscurance, FxContour, FxSaliency, Saliency );
          break;
        case VTK_UNSIGNED_SHORT:
          if ( Color )
            vtkCastRay_NN_Unshaded( (unsigned short *)data_ptr, dynamicInfo,
                                    staticInfo, ColorBleeding, ObscuranceFactor );
          else
            vtkCastRay_NN_Unshaded( (unsigned short *)data_ptr, dynamicInfo,
                                    staticInfo, Obscurance, ObscuranceFactor, ObscuranceFilterLow, ObscuranceFilterHigh,
                                    FxObscurance, FxContour, FxSaliency, Saliency );
          break;
        default:
          vtkWarningMacro ( << "Unsigned char and unsigned short are the only supported datatypes for rendering" );
          break;
        }
      }
    else
      {
      // Nearest neighbor and shading
      switch ( staticInfo->ScalarDataType )
        {
        case VTK_UNSIGNED_CHAR:
          if ( Color )
            vtkCastRay_NN_Shaded( (unsigned char *)data_ptr, dynamicInfo, staticInfo, ColorBleeding, ObscuranceFactor );
          else
            vtkCastRay_NN_Shaded( (unsigned char *)data_ptr, dynamicInfo, staticInfo, Obscurance, ObscuranceFactor, ObscuranceFilterLow, ObscuranceFilterHigh,
                                  FxObscurance, FxContour );
          break;
        case VTK_UNSIGNED_SHORT:
          if ( Color )
            vtkCastRay_NN_Shaded( (unsigned short *)data_ptr, dynamicInfo, staticInfo, ColorBleeding, ObscuranceFactor );
          else
            vtkCastRay_NN_Shaded( (unsigned short *)data_ptr, dynamicInfo, staticInfo, Obscurance, ObscuranceFactor, ObscuranceFilterLow, ObscuranceFilterHigh,
                                  FxObscurance, FxContour );
          break;
        default:
          vtkWarningMacro ( << "Unsigned char and unsigned short are the only supported datatypes for rendering" );
          break;
        }
      }
    }
  else 
    {
    // Trilinear interpolation and no shading
    if ( staticInfo->Shading == 0 )
      {
      if ( this->CompositeMethod == VTK_COMPOSITE_INTERPOLATE_FIRST )
        {
        switch ( staticInfo->ScalarDataType )
          {
          case VTK_UNSIGNED_CHAR:
            if ( Color )
              vtkCastRay_TrilinSample_Unshaded( (unsigned char *)data_ptr,
                                                dynamicInfo, staticInfo, ColorBleeding, ObscuranceFactor );
            else
              vtkCastRay_TrilinSample_Unshaded( (unsigned char *)data_ptr,
                                                dynamicInfo, staticInfo, Obscurance, ObscuranceFactor, ObscuranceFilterLow, ObscuranceFilterHigh,
                                                FxObscurance, FxContour );
            break;
          case VTK_UNSIGNED_SHORT:
            if ( Color )
              vtkCastRay_TrilinSample_Unshaded( (unsigned short *)data_ptr,
                                                dynamicInfo, staticInfo, ColorBleeding, ObscuranceFactor );
            else
              vtkCastRay_TrilinSample_Unshaded( (unsigned short *)data_ptr,
                                                dynamicInfo, staticInfo, Obscurance, ObscuranceFactor, ObscuranceFilterLow, ObscuranceFilterHigh,
                                                FxObscurance, FxContour );
            break;
          default:
            vtkWarningMacro ( << "Unsigned char and unsigned short are the only supported datatypes for rendering" );
            break;
          }
        }
      else
        {
        switch ( staticInfo->ScalarDataType )
          {
          case VTK_UNSIGNED_CHAR:
            if ( Color )
              vtkCastRay_TrilinVertices_Unshaded( (unsigned char *)data_ptr,
                                                  dynamicInfo, staticInfo, ColorBleeding, ObscuranceFactor );
            else
              vtkCastRay_TrilinVertices_Unshaded( (unsigned char *)data_ptr,
                                                  dynamicInfo, staticInfo, Obscurance, ObscuranceFactor, ObscuranceFilterLow, ObscuranceFilterHigh,
                                                  FxObscurance, FxContour );
            break;
          case VTK_UNSIGNED_SHORT:
            if ( Color )
              vtkCastRay_TrilinVertices_Unshaded( (unsigned short *)data_ptr,
                                                  dynamicInfo, staticInfo, ColorBleeding, ObscuranceFactor );
            else
              vtkCastRay_TrilinVertices_Unshaded( (unsigned short *)data_ptr,
                                                  dynamicInfo, staticInfo, Obscurance, ObscuranceFactor, ObscuranceFilterLow, ObscuranceFilterHigh,
                                                  FxObscurance, FxContour );
            break;
          default:
            vtkWarningMacro ( << "Unsigned char and unsigned short are the only supported datatypes for rendering" );
            break;
          }
        }
      } 
    else
      {
      // Trilinear interpolation and shading
      if ( this->CompositeMethod == VTK_COMPOSITE_INTERPOLATE_FIRST )
        {
        switch ( staticInfo->ScalarDataType )
          {
          case VTK_UNSIGNED_CHAR:
            if ( Color )
              vtkCastRay_TrilinSample_Shaded( (unsigned char *)data_ptr, 
                                              dynamicInfo, staticInfo, ColorBleeding, ObscuranceFactor );
            else
              vtkCastRay_TrilinSample_Shaded( (unsigned char *)data_ptr, 
                                              dynamicInfo, staticInfo, Obscurance, ObscuranceFactor, ObscuranceFilterLow, ObscuranceFilterHigh,
                                              FxObscurance, FxContour );
            break;
          case VTK_UNSIGNED_SHORT:
            if ( Color )
              vtkCastRay_TrilinSample_Shaded( (unsigned short *)data_ptr, 
                                              dynamicInfo, staticInfo, ColorBleeding, ObscuranceFactor );
            else
              vtkCastRay_TrilinSample_Shaded( (unsigned short *)data_ptr, 
                                              dynamicInfo, staticInfo, Obscurance, ObscuranceFactor, ObscuranceFilterLow, ObscuranceFilterHigh,
                                              FxObscurance, FxContour );
            break;
          default:
            vtkWarningMacro ( << "Unsigned char and unsigned short are the only supported datatypes for rendering" );
            break;
          }
        }       
      else
        {
        switch ( staticInfo->ScalarDataType )
          {
          case VTK_UNSIGNED_CHAR:
            if ( Color )
              vtkCastRay_TrilinVertices_Shaded( (unsigned char *)data_ptr, 
                                                dynamicInfo, staticInfo, ColorBleeding, ObscuranceFactor );
            else
              vtkCastRay_TrilinVertices_Shaded( (unsigned char *)data_ptr, 
                                                dynamicInfo, staticInfo, Obscurance, ObscuranceFactor, ObscuranceFilterLow, ObscuranceFilterHigh,
                                                FxObscurance, FxContour );
            break;
          case VTK_UNSIGNED_SHORT:
            if ( Color )
              vtkCastRay_TrilinVertices_Shaded( (unsigned short *)data_ptr, 
                                                dynamicInfo, staticInfo, ColorBleeding, ObscuranceFactor );
            else
              vtkCastRay_TrilinVertices_Shaded( (unsigned short *)data_ptr, 
                                                dynamicInfo, staticInfo, Obscurance, ObscuranceFactor, ObscuranceFilterLow, ObscuranceFilterHigh,
                                                FxObscurance, FxContour );
            break;
          default:
            vtkWarningMacro ( << "Unsigned char and unsigned short are the only supported datatypes for rendering" );
            break;
          }
        }
      }
    }
}

float vtkVolumeRayCastCompositeFunctionFx::GetZeroOpacityThreshold( vtkVolume
                                                                  *vol )
{
  return vol->GetProperty()->GetScalarOpacity()->GetFirstNonZeroValue();
}

// We don't need to do any specific initialization here...
void vtkVolumeRayCastCompositeFunctionFx::SpecificFunctionInitialize( 
                                vtkRenderer *vtkNotUsed(ren), 
                                vtkVolume *vtkNotUsed(vol),
                                vtkVolumeRayCastStaticInfo *vtkNotUsed(staticInfo),
                                vtkVolumeRayCastMapper *vtkNotUsed(mapper) )
{
}

// Description:
// Return the composite method as a descriptive character string.
const char *vtkVolumeRayCastCompositeFunctionFx::GetCompositeMethodAsString(void)
{
  if( this->CompositeMethod == VTK_COMPOSITE_INTERPOLATE_FIRST )
    {
    return "Interpolate First";
    }
  if( this->CompositeMethod == VTK_COMPOSITE_CLASSIFY_FIRST )
    {
    return "Classify First";
    }
  else
    {
    return "Unknown";
    }
}

// Print method for vtkVolumeRayCastCompositeFunctionFx
// Since there is nothing local to print, just print the object stuff.
void vtkVolumeRayCastCompositeFunctionFx::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "Composite Method: " << this->GetCompositeMethodAsString()
     << "\n";

}





