/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkVolumeRayCastCompositeFunctionObscurances.h,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkVolumeRayCastCompositeFunctionObscurances - a ray function for compositing

// .SECTION Description
// vtkVolumeRayCastCompositeFunctionObscurances is a ray function that can be used
// within a vtkVolumeRayCastMapper. This function performs compositing along
// the ray according to the properties stored in the vtkVolumeProperty for
// the volume. 

// .SECTION See Also
// vtkVolumeRayCastMapper vtkVolumeProperty vtkVolume

#ifndef __vtkVolumeRayCastCompositeFunctionObscurances_h
#define __vtkVolumeRayCastCompositeFunctionObscurances_h

#include "vtkVolumeRayCastFunction.h"

#define VTK_COMPOSITE_CLASSIFY_FIRST 0
#define VTK_COMPOSITE_INTERPOLATE_FIRST 1

class VTK_VOLUMERENDERING_EXPORT vtkVolumeRayCastCompositeFunctionObscurances : public vtkVolumeRayCastFunction
{
public:
  static vtkVolumeRayCastCompositeFunctionObscurances *New();
  vtkTypeRevisionMacro(vtkVolumeRayCastCompositeFunctionObscurances,vtkVolumeRayCastFunction);
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

  vtkSetMacro( Obscurance, double * );
  vtkGetMacro( Obscurance, double * );

protected:
  vtkVolumeRayCastCompositeFunctionObscurances();
  ~vtkVolumeRayCastCompositeFunctionObscurances();

//BTX
  void SpecificFunctionInitialize( vtkRenderer *ren,
                                   vtkVolume   *vol,
                                   vtkVolumeRayCastStaticInfo *staticInfo,
                                   vtkVolumeRayCastMapper *mapper );
//ETX
  
  int           CompositeMethod;
private:
  vtkVolumeRayCastCompositeFunctionObscurances(const vtkVolumeRayCastCompositeFunctionObscurances&);  // Not implemented.
  void operator=(const vtkVolumeRayCastCompositeFunctionObscurances&);  // Not implemented.

  double * Obscurance;
};


#endif
