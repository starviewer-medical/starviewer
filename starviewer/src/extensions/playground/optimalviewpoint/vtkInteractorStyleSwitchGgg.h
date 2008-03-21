/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkInteractorStyleSwitch.h,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkInteractorStyleSwitch - class to swap between interactory styles
// .SECTION Description
// The class vtkInteractorStyleSwitch allows handles interactively switching
// between four interactor styles -- joystick actor, joystick camera,
// trackball actor, and trackball camera.  Type 'j' or 't' to select
// joystick or trackball, and type 'c' or 'a' to select camera or actor.
// The default interactor style is joystick camera.
// .SECTION See Also
// vtkInteractorStyleJoystickActor vtkInteractorStyleJoystickCamera
// vtkInteractorStyleTrackballActor vtkInteractorStyleTrackballCamera



/***************************************************************************
 *   Copyright (C) 2006-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.edu/GGG/index.html                                    *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
/**
 * \class vtkInteractorStyleSwitchGgg
 *
 * És una versió modificada de vtkInteractorStyleSwitch que fa servir
 * vtkInteractorStyleJoystickActorGgg en lloc de vtkInteractorStyleJoystickActor
 * i vtkInteractorStyleTrackballActorGgg en lloc de
 * vtkInteractorStyleTrackballActor. Aquestes classes permeten una interacció
 * més ràpida amb el volum.
 */



#ifndef __vtkInteractorStyleSwitchGgg_h
#define __vtkInteractorStyleSwitchGgg_h

#include "vtkInteractorStyle.h"

#define VTKIS_JOYSTICK  0
#define VTKIS_TRACKBALL 1

#define VTKIS_CAMERA    0
#define VTKIS_ACTOR     1

class vtkInteractorStyleJoystickActorGgg;
class vtkInteractorStyleJoystickCamera;
class vtkInteractorStyleTrackballActorGgg;
class vtkInteractorStyleTrackballCamera;

class vtkInteractorStyleSwitchGgg : public vtkInteractorStyle
{
public:
  static vtkInteractorStyleSwitchGgg *New();
  vtkTypeRevisionMacro(vtkInteractorStyleSwitchGgg, vtkInteractorStyle);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // The sub styles need the interactor too.
  void SetInteractor(vtkRenderWindowInteractor *iren);
  
  // Description:
  // We must override this method in order to pass the setting down to
  // the underlying styles
  void SetAutoAdjustCameraClippingRange( int value );
  
  // Description:
  // Set/Get current style
  vtkGetObjectMacro(CurrentStyle, vtkInteractorStyle);
  void SetCurrentStyleToJoystickActor();
  void SetCurrentStyleToJoystickCamera();
  void SetCurrentStyleToTrackballActor();
  void SetCurrentStyleToTrackballCamera();

  // Description:
  // Only care about the char event, which is used to switch between
  // different styles.
  virtual void OnChar();

  // Description:
  // Overridden from vtkInteractorObserver because the interactor styles
  // used by this class must also be updated.
  virtual void SetDefaultRenderer(vtkRenderer*);
  virtual void SetCurrentRenderer(vtkRenderer*);
  
protected:
  vtkInteractorStyleSwitchGgg();
  ~vtkInteractorStyleSwitchGgg();
  
  void SetCurrentStyle();
  
  vtkInteractorStyleJoystickActorGgg *JoystickActor;
  vtkInteractorStyleJoystickCamera *JoystickCamera;
  vtkInteractorStyleTrackballActorGgg *TrackballActor;
  vtkInteractorStyleTrackballCamera *TrackballCamera;
  vtkInteractorStyle* CurrentStyle;

  int JoystickOrTrackball;
  int CameraOrActor;

private:
  vtkInteractorStyleSwitchGgg(const vtkInteractorStyleSwitchGgg&);  // Not implemented.
  void operator=(const vtkInteractorStyleSwitchGgg&);  // Not implemented.
};

#endif
