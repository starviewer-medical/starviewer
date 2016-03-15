/*************************************************************************************
  Copyright (C) 2014 Laboratori de Gràfics i Imatge, Universitat de Girona &
  Institut de Diagnòstic per la Imatge.
  Girona 2014. All rights reserved.
  http://starviewer.udg.edu

  This file is part of the Starviewer (Medical Imaging Software) open source project.
  It is subject to the license terms in the LICENSE file found in the top-level
  directory of this distribution and at http://starviewer.udg.edu/license. No part of
  the Starviewer (Medical Imaging Software) open source project, including this file,
  may be copied, modified, propagated, or distributed except according to the
  terms contained in the LICENSE file.
 *************************************************************************************/

#ifndef UDG_VTKTEXTACTORWITHBACKGROUND_H
#define UDG_VTKTEXTACTORWITHBACKGROUND_H

#include <vtkTextActor.h>

namespace udg {

class VtkCorrectImageBlend;

/**
 * @brief The VtkTextActorWithBackground class is a subclass of vtkTextActor that adds a background to the regular text actor.
 */
class VtkTextActorWithBackground : public vtkTextActor {

public:
    vtkTypeMacro(VtkTextActorWithBackground, vtkTextActor)

    /// Creates a new text actor with black transparent background.
    static VtkTextActorWithBackground* New();

    virtual void ShallowCopy(vtkProp *prop);

    virtual void PrintSelf(ostream &os, vtkIndent indent);

    /// Returns the bounding box coordinates of the text in viewport coordinates.
    /// The bbox array is populated with [ xmin, xmax, ymin, ymax ] values in that order.
    virtual void GetBoundingBox(vtkViewport *vport, double bbox[4]);

    /// Returns the background color of the text.
    vtkGetVector3Macro(BackgroundColor, double)
    /// Sets the background color of the text.
    vtkSetVector3Macro(BackgroundColor, double)

    /// Returns the background opacity of the text.
    vtkGetMacro(BackgroundOpacity, double)
    /// Sets the background opacity of the text.
    vtkSetMacro(BackgroundOpacity, double)

    /// Returns the margin around the text.
    vtkGetMacro(Margin, int)
    /// Sets the margin around the text.
    vtkSetMacro(Margin, int)

    /// Returns the bounds of the actor in world coordinates as (Xmin, Xmax, Ymin, Ymax, Zmin, Zmax).
    vtkGetVector6Macro(WorldBounds, double)

protected:
    VtkTextActorWithBackground();
    virtual ~VtkTextActorWithBackground();

    virtual bool RenderImage(vtkTextProperty *tprop, vtkViewport *viewport);

    virtual void ComputeRectangle(vtkViewport *viewport);
    virtual int UpdateRectangle(vtkViewport *viewport);

protected:
    /// Background color of the text actor.
    double BackgroundColor[3];
    /// Background opacity of the text actor.
    double BackgroundOpacity;

    /// Margin around the text.
    int Margin;

    /// Bounds of the actor in world coordinates.
    double WorldBounds[6];

private:
    // Not implemented.
    VtkTextActorWithBackground(const VtkTextActorWithBackground&);
    // Not implemented.
    void operator=(const VtkTextActorWithBackground&);

    /// Updates the bounds of the actor in world coordinates.
    void updateWorldBounds(vtkViewport *viewport);

private:
    /// Image data to represent the background.
    vtkImageData *m_backgroundImage;
    /// Used to blend the text image with the background image.
    VtkCorrectImageBlend *m_imageBlend;

};

} // namespace udg

#endif // UDG_VTKTEXTACTORWITHBACKGROUND_H
