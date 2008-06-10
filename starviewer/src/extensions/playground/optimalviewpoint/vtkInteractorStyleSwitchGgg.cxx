/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkInteractorStyleSwitch.cxx,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/



/***************************************************************************
 *   Copyright (C) 2006-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.edu/GGG/index.html                                    *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
// Versió modificada per fer servir les classes
// vtkInteractorStyleJoystickActorGgg i vtkInteractorStyleTrackballActorGgg
// en lloc de vtkInteractorStyleJoystickActor i
// vtkInteractorStyleTrackballActor.



#include "vtkInteractorStyleSwitchGgg.h"

#include "vtkCallbackCommand.h"
#include "vtkCommand.h"
#include "vtkInteractorStyleJoystickActorGgg.h"
#include "vtkInteractorStyleJoystickCamera.h"
#include "vtkInteractorStyleTrackballActorGgg.h"
#include "vtkInteractorStyleTrackballCamera.h"
#include "vtkObjectFactory.h"
#include "vtkRenderWindowInteractor.h"

vtkCxxRevisionMacro(vtkInteractorStyleSwitchGgg, "$Revision: 1.24 $");
vtkStandardNewMacro(vtkInteractorStyleSwitchGgg);

//----------------------------------------------------------------------------
vtkInteractorStyleSwitchGgg::vtkInteractorStyleSwitchGgg() 
{
  this->JoystickActor = vtkInteractorStyleJoystickActorGgg::New();
  this->JoystickCamera = vtkInteractorStyleJoystickCamera::New();
  this->TrackballActor = vtkInteractorStyleTrackballActorGgg::New();
  this->TrackballCamera = vtkInteractorStyleTrackballCamera::New();
  this->JoystickOrTrackball = VTKIS_JOYSTICK;
  this->CameraOrActor = VTKIS_CAMERA;
  this->CurrentStyle = 0;
}

//----------------------------------------------------------------------------
vtkInteractorStyleSwitchGgg::~vtkInteractorStyleSwitchGgg() 
{
  this->JoystickActor->Delete();
  this->JoystickActor = NULL;
  
  this->JoystickCamera->Delete();
  this->JoystickCamera = NULL;
  
  this->TrackballActor->Delete();
  this->TrackballActor = NULL;
  
  this->TrackballCamera->Delete();
  this->TrackballCamera = NULL;
}

//----------------------------------------------------------------------------
void vtkInteractorStyleSwitchGgg::SetAutoAdjustCameraClippingRange( int value )
{
  if ( value == this->AutoAdjustCameraClippingRange )
    {
    return;
    }
  
  if ( value < 0 || value > 1 )
    {
    vtkErrorMacro("Value must be between 0 and 1 for" <<
                  " SetAutoAdjustCameraClippingRange");
    return;
    }
  
  this->AutoAdjustCameraClippingRange = value;
  this->JoystickActor->SetAutoAdjustCameraClippingRange( value );
  this->JoystickCamera->SetAutoAdjustCameraClippingRange( value );
  this->TrackballActor->SetAutoAdjustCameraClippingRange( value );
  this->TrackballCamera->SetAutoAdjustCameraClippingRange( value );
  
  this->Modified();
}

//----------------------------------------------------------------------------
void vtkInteractorStyleSwitchGgg::SetCurrentStyleToJoystickActor()
{
  this->JoystickOrTrackball = VTKIS_JOYSTICK;
  this->CameraOrActor = VTKIS_ACTOR;
  this->SetCurrentStyle();
}

//----------------------------------------------------------------------------
void vtkInteractorStyleSwitchGgg::SetCurrentStyleToJoystickCamera()
{
  this->JoystickOrTrackball = VTKIS_JOYSTICK;
  this->CameraOrActor = VTKIS_CAMERA;
  this->SetCurrentStyle();
}

//----------------------------------------------------------------------------
void vtkInteractorStyleSwitchGgg::SetCurrentStyleToTrackballActor()
{
  this->JoystickOrTrackball = VTKIS_TRACKBALL;
  this->CameraOrActor = VTKIS_ACTOR;
  this->SetCurrentStyle();
}

//----------------------------------------------------------------------------
void vtkInteractorStyleSwitchGgg::SetCurrentStyleToTrackballCamera()
{
  this->JoystickOrTrackball = VTKIS_TRACKBALL;
  this->CameraOrActor = VTKIS_CAMERA;
  this->SetCurrentStyle();
}

//----------------------------------------------------------------------------
void vtkInteractorStyleSwitchGgg::OnChar() 
{
  switch (this->Interactor->GetKeyCode())
    {
    case 'j':
    case 'J':
      this->JoystickOrTrackball = VTKIS_JOYSTICK;
      this->EventCallbackCommand->SetAbortFlag(1);
      break;
    case 't':
    case 'T':
      this->JoystickOrTrackball = VTKIS_TRACKBALL;
      this->EventCallbackCommand->SetAbortFlag(1);
      break;
    case 'c':
    case 'C':  
      this->CameraOrActor = VTKIS_CAMERA;
      this->EventCallbackCommand->SetAbortFlag(1);
      break;
    case 'a':
    case 'A':
      this->CameraOrActor = VTKIS_ACTOR;
      this->EventCallbackCommand->SetAbortFlag(1);
      break;
    }
  // Set the CurrentStyle pointer to the picked style
  this->SetCurrentStyle();
}

//----------------------------------------------------------------------------
// this will do nothing if the CurrentStyle matchs
// JoystickOrTrackball and CameraOrActor
// It should! If the this->Interactor was changed (using SetInteractor()),
// and the currentstyle should not change.
void vtkInteractorStyleSwitchGgg::SetCurrentStyle()
{
  // if the currentstyle does not match JoystickOrTrackball 
  // and CameraOrActor ivars, then call SetInteractor(0)
  // on the Currentstyle to remove all of the observers.
  // Then set the Currentstyle and call SetInteractor with 
  // this->Interactor so the callbacks are set for the 
  // currentstyle.
  if (this->JoystickOrTrackball == VTKIS_JOYSTICK &&
      this->CameraOrActor == VTKIS_CAMERA)
    {
    if(this->CurrentStyle != this->JoystickCamera)
      {
      if(this->CurrentStyle)
        {
        this->CurrentStyle->SetInteractor(0);
        }
      this->CurrentStyle = this->JoystickCamera;
      }
    }
  else if (this->JoystickOrTrackball == VTKIS_JOYSTICK &&
           this->CameraOrActor == VTKIS_ACTOR)
    { 
    if(this->CurrentStyle != this->JoystickActor)
      {
      if(this->CurrentStyle)
        {
        this->CurrentStyle->SetInteractor(0);
        }
      this->CurrentStyle = this->JoystickActor;
      }
    }
  else if (this->JoystickOrTrackball == VTKIS_TRACKBALL &&
           this->CameraOrActor == VTKIS_CAMERA)
    {
    if(this->CurrentStyle != this->TrackballCamera)
      {
      if(this->CurrentStyle)
        {
        this->CurrentStyle->SetInteractor(0);
        }
      this->CurrentStyle = this->TrackballCamera;
      }
    }
  else if (this->JoystickOrTrackball == VTKIS_TRACKBALL &&
           this->CameraOrActor == VTKIS_ACTOR)
    { 
      if(this->CurrentStyle != this->TrackballActor)
        {
        if(this->CurrentStyle)
          {
          this->CurrentStyle->SetInteractor(0);
          }
        this->CurrentStyle = this->TrackballActor;
        }
    }
  if (this->CurrentStyle)
    {
    this->CurrentStyle->SetInteractor(this->Interactor);
    }
}

//----------------------------------------------------------------------------
void vtkInteractorStyleSwitchGgg::SetInteractor(vtkRenderWindowInteractor *iren)
{
  if(iren == this->Interactor)
    {
    return;
    }
  // if we already have an Interactor then stop observing it
  if(this->Interactor)
    {
    this->Interactor->RemoveObserver(this->EventCallbackCommand);
    }
  this->Interactor = iren;
  // add observers for each of the events handled in ProcessEvents
  if(iren)
    {
    iren->AddObserver(vtkCommand::CharEvent, 
                      this->EventCallbackCommand,
                      this->Priority);

    iren->AddObserver(vtkCommand::DeleteEvent, 
                      this->EventCallbackCommand,
                      this->Priority);
    }
  this->SetCurrentStyle();
}

//----------------------------------------------------------------------------
void vtkInteractorStyleSwitchGgg::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  os << indent << "CurrentStyle " << this->CurrentStyle << "\n";
  if (this->CurrentStyle)
    {
    vtkIndent next_indent = indent.GetNextIndent();
    os << next_indent << this->CurrentStyle->GetClassName() << "\n";
    this->CurrentStyle->PrintSelf(os, indent.GetNextIndent());
    }
}

//----------------------------------------------------------------------------
void vtkInteractorStyleSwitchGgg::SetDefaultRenderer(vtkRenderer* renderer)
{
  this->vtkInteractorStyle::SetDefaultRenderer(renderer);
  this->JoystickActor->SetDefaultRenderer(renderer);
  this->JoystickCamera->SetDefaultRenderer(renderer);
  this->TrackballActor->SetDefaultRenderer(renderer);
  this->TrackballCamera->SetDefaultRenderer(renderer);
}

//----------------------------------------------------------------------------
void vtkInteractorStyleSwitchGgg::SetCurrentRenderer(vtkRenderer* renderer)
{
  this->vtkInteractorStyle::SetCurrentRenderer(renderer);
  this->JoystickActor->SetCurrentRenderer(renderer);
  this->JoystickCamera->SetCurrentRenderer(renderer);
  this->TrackballActor->SetCurrentRenderer(renderer);
  this->TrackballCamera->SetCurrentRenderer(renderer);
}
