/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkRunThroughFilter.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkRunThroughFilter - It is a copy of vtkPassThroughFilter!!! Filter which shallow copies it's input to it's output
// .SECTION Description
// This filter shallow copies it's input to it's output. It is normally
// used by PVSources with multiple outputs as the VTK filter in the
// dummy connection objects at each output.

#ifndef __vtkRunThroughFilter_h
#define __vtkRunThroughFilter_h

#include "vtkDataSetAlgorithm.h"

class vtkFieldData;

class  vtkRunThroughFilter : public vtkDataSetAlgorithm
{
public:
  vtkTypeMacro(vtkRunThroughFilter,vtkDataSetAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Create a new vtkRunThroughFilter.
  static vtkRunThroughFilter *New();


protected:

  vtkRunThroughFilter() {};
  virtual ~vtkRunThroughFilter() {};

  int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

private:
  vtkRunThroughFilter(const vtkRunThroughFilter&);  // Not implemented.
  void operator=(const vtkRunThroughFilter&);  // Not implemented.
};

#endif


