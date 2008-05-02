/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkVolumeRayCastCompositeFunctionFx.h,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkVolumeRayCastCompositeFunctionFx - a ray function for compositing

// .SECTION Description
// vtkVolumeRayCastCompositeFunctionFx is a ray function that can be used
// within a vtkVolumeRayCastMapper. This function performs compositing along
// the ray according to the properties stored in the vtkVolumeProperty for
// the volume. 

// .SECTION See Also
// vtkVolumeRayCastMapper vtkVolumeProperty vtkVolume

#ifndef __vtkVolumeRayCastCompositeFunctionFx_h
#define __vtkVolumeRayCastCompositeFunctionFx_h

#include "vtkVolumeRayCastFunction.h"

#define VTK_COMPOSITE_CLASSIFY_FIRST 0
#define VTK_COMPOSITE_INTERPOLATE_FIRST 1

namespace udg {
class Vector3;
}

class vtkVolumeRayCastCompositeFunctionFx : public vtkVolumeRayCastFunction
{
public:
  static vtkVolumeRayCastCompositeFunctionFx *New();
  vtkTypeRevisionMacro(vtkVolumeRayCastCompositeFunctionFx,vtkVolumeRayCastFunction);
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

  vtkSetMacro( ColorBleeding, udg::Vector3 * );
  vtkGetMacro( ColorBleeding, udg::Vector3 * );

  vtkSetMacro( Color, bool );
  vtkGetMacro( Color, bool );

  vtkSetMacro( ObscuranceFactor, double );
  vtkGetMacro( ObscuranceFactor, double );

  vtkSetMacro( ObscuranceFilterLow, double );
  vtkGetMacro( ObscuranceFilterLow, double );

  vtkSetMacro( ObscuranceFilterHigh, double );
  vtkGetMacro( ObscuranceFilterHigh, double );

  vtkSetMacro( FxObscurance, bool );
  vtkGetMacro( FxObscurance, bool );

  vtkSetMacro( FxContour, double );
  vtkGetMacro( FxContour, double );

  vtkSetMacro( Saliency, double * );
  vtkGetMacro( Saliency, double * );

  vtkSetMacro( FxSaliency, bool );
  vtkGetMacro( FxSaliency, bool );

protected:
  vtkVolumeRayCastCompositeFunctionFx();
  ~vtkVolumeRayCastCompositeFunctionFx();

//BTX
  void SpecificFunctionInitialize( vtkRenderer *ren,
                                   vtkVolume   *vol,
                                   vtkVolumeRayCastStaticInfo *staticInfo,
                                   vtkVolumeRayCastMapper *mapper );
//ETX
  
  int           CompositeMethod;
private:
  vtkVolumeRayCastCompositeFunctionFx(const vtkVolumeRayCastCompositeFunctionFx&);  // Not implemented.
  void operator=(const vtkVolumeRayCastCompositeFunctionFx&);  // Not implemented.

  double * Obscurance;
  udg::Vector3 * ColorBleeding;
  bool Color;
  double ObscuranceFactor;
  double ObscuranceFilterLow;
  double ObscuranceFilterHigh;
  bool FxObscurance;
  double FxContour;
  double * Saliency;
  bool FxSaliency;
};


#endif
