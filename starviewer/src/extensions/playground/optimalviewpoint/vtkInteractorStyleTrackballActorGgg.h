/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkInteractorStyleTrackballActor.h,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkInteractorStyleTrackballActor - manipulate objects in the scene independent of each other
// .SECTION Description
// vtkInteractorStyleTrackballActor allows the user to interact with (rotate,
// pan, etc.) objects in the scene indendent of each other.  In trackball
// interaction, the magnitude of the mouse motion is proportional to the
// actor motion associated with a particular mouse binding. For example,
// small left-button motions cause small changes in the rotation of the
// actor around its center point.
//
// The mouse bindings are as follows. For a 3-button mouse, the left button
// is for rotation, the right button for zooming, the middle button for
// panning, and ctrl + left button for spinning.  (With fewer mouse buttons,
// ctrl + shift + left button is for zooming, and shift + left button is for
// panning.)

// .SECTION See Also
// vtkInteractorStyleTrackballCamera vtkInteractorStyleJoystickActor
// vtkInteractorStyleJoystickCamera

/***************************************************************************
 *   Copyright (C) 2006 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
// Versió modificada per fer servir un vtkPropPicker en lloc d'un vtkCellPicker.

#ifndef __vtkInteractorStyleTrackballActorGgg_h
#define __vtkInteractorStyleTrackballActorGgg_h

#include "vtkInteractorStyle.h"

class vtkPropPicker;

class VTK_RENDERING_EXPORT vtkInteractorStyleTrackballActorGgg : public vtkInteractorStyle
{
public:
  static vtkInteractorStyleTrackballActorGgg *New();
  vtkTypeRevisionMacro(vtkInteractorStyleTrackballActorGgg,vtkInteractorStyle);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Event bindings controlling the effects of pressing mouse buttons
  // or moving the mouse.
  virtual void OnMouseMove();
  virtual void OnLeftButtonDown();
  virtual void OnLeftButtonUp();
  virtual void OnMiddleButtonDown();
  virtual void OnMiddleButtonUp();
  virtual void OnRightButtonDown();
  virtual void OnRightButtonUp();

  // These methods for the different interactions in different modes
  // are overridden in subclasses to perform the correct motion. Since
  // they might be called from OnTimer, they do not have mouse coord parameters
  // (use interactor's GetEventPosition and GetLastEventPosition)
  virtual void Rotate();
  virtual void Spin();
  virtual void Pan();
  virtual void Dolly();
  virtual void UniformScale();

protected:
  vtkInteractorStyleTrackballActorGgg();
  ~vtkInteractorStyleTrackballActorGgg();

  void FindPickedActor(int x, int y);

  void Prop3DTransform(vtkProp3D *prop3D,
                       double *boxCenter,
                       int NumRotation,
                       double **rotate,
                       double *scale);
  
  double MotionFactor;

  vtkProp3D *InteractionProp;
  vtkPropPicker *InteractionPicker;

private:
  vtkInteractorStyleTrackballActorGgg(const vtkInteractorStyleTrackballActorGgg&);  // Not implemented.
  void operator=(const vtkInteractorStyleTrackballActorGgg&);  // Not implemented.
};

#endif
