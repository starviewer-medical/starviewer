/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkVolumeRayCastCompositeFunctionOptimalViewpoint.h,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkVolumeRayCastCompositeFunctionOptimalViewpoint - a ray function for compositing

// .SECTION Description
// vtkVolumeRayCastCompositeFunctionOptimalViewpoint is a ray function that can be used
// within a vtkVolumeRayCastMapper. This function performs compositing along
// the ray according to the properties stored in the vtkVolumeProperty for
// the volume. 

// .SECTION See Also
// vtkVolumeRayCastMapper vtkVolumeProperty vtkVolume

#ifndef __vtkVolumeRayCastCompositeFunctionOptimalViewpoint_h
#define __vtkVolumeRayCastCompositeFunctionOptimalViewpoint_h

#include "vtkVolumeRayCastFunction.h"

#define VTK_COMPOSITE_CLASSIFY_FIRST 0
#define VTK_COMPOSITE_INTERPOLATE_FIRST 1

namespace udg {
    class OptimalViewpointVolume;
};

class vtkVolumeRayCastCompositeFunctionOptimalViewpoint : public vtkVolumeRayCastFunction
{
public:
  static vtkVolumeRayCastCompositeFunctionOptimalViewpoint *New();
  vtkTypeRevisionMacro(vtkVolumeRayCastCompositeFunctionOptimalViewpoint,vtkVolumeRayCastFunction);
  void PrintSelf( ostream& os, vtkIndent indent );

  // Description:
  // Set the CompositeMethod to either Classify First or Interpolate First
  vtkSetClampMacro( CompositeMethod, int,
        VTK_COMPOSITE_CLASSIFY_FIRST, VTK_COMPOSITE_INTERPOLATE_FIRST );
  vtkGetMacro(CompositeMethod,int);
  void SetCompositeMethodToInterpolateFirst()
    {this->SetCompositeMethod(VTK_COMPOSITE_INTERPOLATE_FIRST);}
  void SetCompositeMethodToClassifyFirst() 
    {this->SetCompositeMethod(VTK_COMPOSITE_CLASSIFY_FIRST);}
  const char *GetCompositeMethodAsString(void);

//BTX
  void CastRay( vtkVolumeRayCastDynamicInfo *dynamicInfo,
                vtkVolumeRayCastStaticInfo *staticInfo);

  float GetZeroOpacityThreshold( vtkVolume *vol );
//ETX

    void setOptimalViewpointVolume( udg::OptimalViewpointVolume * optimalViewpointVolume );
    void setOpacityOn( bool opacityOn );
    void setComputing( bool on );

protected:
  vtkVolumeRayCastCompositeFunctionOptimalViewpoint();
  ~vtkVolumeRayCastCompositeFunctionOptimalViewpoint();

//BTX
  void SpecificFunctionInitialize( vtkRenderer *ren,
                                   vtkVolume   *vol,
                                   vtkVolumeRayCastStaticInfo *staticInfo,
                                   vtkVolumeRayCastMapper *mapper );
//ETX
  
  int           CompositeMethod;

    udg::OptimalViewpointVolume * m_optimalViewpointVolume;
    bool m_opacityOn;
    bool m_computing;

private:
  vtkVolumeRayCastCompositeFunctionOptimalViewpoint(const vtkVolumeRayCastCompositeFunctionOptimalViewpoint&);  // Not implemented.
  void operator=(const vtkVolumeRayCastCompositeFunctionOptimalViewpoint&);  // Not implemented.

  template <class T> void vtkCastRay_NN_Unshaded( T *data_ptr, vtkVolumeRayCastDynamicInfo *dynamicInfo, vtkVolumeRayCastStaticInfo *staticInfo );
  template <class T> void vtkCastRay_NN_Shaded( T *data_ptr, vtkVolumeRayCastDynamicInfo *dynamicInfo, vtkVolumeRayCastStaticInfo *staticInfo );
  template <class T> void vtkCastRay_TrilinSample_Unshaded( T *data_ptr, vtkVolumeRayCastDynamicInfo *dynamicInfo, vtkVolumeRayCastStaticInfo *staticInfo );
  template <class T> void vtkCastRay_TrilinSample_Shaded( T *data_ptr, vtkVolumeRayCastDynamicInfo *dynamicInfo, vtkVolumeRayCastStaticInfo *staticInfo );
  template <class T> void vtkCastRay_TrilinVertices_Unshaded( T *data_ptr, vtkVolumeRayCastDynamicInfo *dynamicInfo, vtkVolumeRayCastStaticInfo *staticInfo );
  template <class T> void vtkCastRay_TrilinVertices_Shaded( T *data_ptr, vtkVolumeRayCastDynamicInfo *dynamicInfo, vtkVolumeRayCastStaticInfo *staticInfo );

};


#endif
