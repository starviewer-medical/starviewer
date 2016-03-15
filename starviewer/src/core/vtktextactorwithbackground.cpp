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

  This file incorporates work covered by the following copyright and
  permission notice:

    Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
    All rights reserved.
    See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

       This software is distributed WITHOUT ANY WARRANTY; without even
       the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
       PURPOSE.  See the above copyright notice for more information.
 *************************************************************************************/

#include "vtktextactorwithbackground.h"

#include "mathtools.h"
#include "vtkcorrectimageblend.h"

#include <vtkFloatArray.h>
#include <vtkImageClip.h>
#include <vtkImageData.h>
#include <vtkImageTranslateExtent.h>
#include <vtkMath.h>
#include <vtkObjectFactory.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkTextProperty.h>
#include <vtkTexture.h>

#include <algorithm>

namespace udg {

vtkStandardNewMacro(VtkTextActorWithBackground)

void VtkTextActorWithBackground::ShallowCopy(vtkProp *prop)
{
    VtkTextActorWithBackground *actor = VtkTextActorWithBackground::SafeDownCast(prop);

    if (actor)
    {
        this->SetBackgroundColor(actor->GetBackgroundColor());
        this->SetBackgroundOpacity(actor->GetBackgroundOpacity());
        this->SetMargin(actor->GetMargin());
    }

    this->Superclass::ShallowCopy(prop);
}

void VtkTextActorWithBackground::PrintSelf(ostream &os, vtkIndent indent)
{
    vtkTextActor::PrintSelf(os, indent);

    os << indent << "BackgroundColor: (" << this->BackgroundColor[0] << ", " << this->BackgroundColor[1] << ", " << this->BackgroundColor[2] << ")\n";
    os << indent << "BackgroundOpacity: " << this->BackgroundOpacity << "\n";
    os << indent << "Margin: " << this->Margin << "\n";
    os << indent << "WorldBounds: (" << this->WorldBounds[0] << ", " << this->WorldBounds[1] << ", " << this->WorldBounds[2] << ", "
                                     << this->WorldBounds[3] << ", " << this->WorldBounds[4] << ", " << this->WorldBounds[5] << ")\n";
}

void VtkTextActorWithBackground::GetBoundingBox(vtkViewport *vport, double bbox[4])
{
    this->Superclass::GetBoundingBox(vport, bbox);

    bbox[0] -= this->Margin;
    bbox[1] += this->Margin;
    bbox[2] -= this->Margin;
    bbox[3] += this->Margin;
}

VtkTextActorWithBackground::VtkTextActorWithBackground()
{
    for (int i = 0; i < 3; i++)
    {
        this->BackgroundColor[i] = 0.0;
    }

    this->BackgroundOpacity = 0.0;

    this->Margin = 0;

    for (int i = 0; i < 6; i++)
    {
        this->WorldBounds[i] = 0.0;
    }

    m_backgroundImage = vtkImageData::New();

    m_imageBlend = VtkCorrectImageBlend::New();
    m_imageBlend->AddInputData(m_backgroundImage);
    m_imageBlend->AddInputData(this->ImageData);
}

VtkTextActorWithBackground::~VtkTextActorWithBackground()
{
    m_backgroundImage->Delete();
    m_imageBlend->Delete();
}

bool VtkTextActorWithBackground::RenderImage(vtkTextProperty *tprop, vtkViewport *viewport)
{
    bool returnValue = this->Superclass::RenderImage(tprop, viewport);

    if (returnValue)
    {
        // 1. Clip and translate the image data to just have the minimum size to fit the text

        int textBoundingBox[4];

        if (!this->GetImageBoundingBox(this->ScaledTextProperty, viewport, textBoundingBox))
        {
            vtkErrorMacro("Cannot compute bounding box.")
            return false;
        }

        int extent[6];
        this->ImageData->GetExtent(extent);
        extent[0] = textBoundingBox[0];
        extent[1] = textBoundingBox[1];
        extent[2] = textBoundingBox[2];
        extent[3] = textBoundingBox[3];

        vtkImageClip *imageClip = vtkImageClip::New();
        imageClip->SetInputData(this->ImageData);
        imageClip->SetOutputWholeExtent(extent);
        imageClip->ClipDataOn();

        vtkImageTranslateExtent *imageTranslateExtent = vtkImageTranslateExtent::New();
        imageTranslateExtent->SetInputConnection(imageClip->GetOutputPort());
        imageTranslateExtent->SetTranslation(this->Margin, this->Margin, 0);
        imageTranslateExtent->Update();

        this->ImageData->ShallowCopy(imageTranslateExtent->GetOutput());

        imageClip->Delete();
        imageTranslateExtent->Delete();

        // 2. Create a background image with the necessary texture size: big enough for the text and the margins and with dimensions that are powers of 2

        int textDimensions[3];
        int textDimensionsWithMargin[3];
        textDimensions[0] = (textBoundingBox[1] - textBoundingBox[0] + 1);
        textDimensions[1] = (textBoundingBox[3] - textBoundingBox[2] + 1);
        textDimensionsWithMargin[0] = textDimensions[0] + 2 * this->Margin;
        textDimensionsWithMargin[1] = textDimensions[1] + 2 * this->Margin;

        extent[1] = extent[0] + MathTools::roundUpToPowerOf2(textDimensionsWithMargin[0]) - 1;
        extent[3] = extent[2] + MathTools::roundUpToPowerOf2(textDimensionsWithMargin[1]) - 1;

        m_backgroundImage->SetSpacing(this->ImageData->GetSpacing());
        m_backgroundImage->SetExtent(extent);
        m_backgroundImage->AllocateScalars(VTK_UNSIGNED_CHAR, 4);

        unsigned char *backgroundPointer = static_cast<unsigned char*>(m_backgroundImage->GetScalarPointer());

        for (int i = 0; i < m_backgroundImage->GetNumberOfPoints(); i++)
        {
            backgroundPointer[4 * i + 0] = static_cast<unsigned char>(this->BackgroundColor[0] * 255.0);
            backgroundPointer[4 * i + 1] = static_cast<unsigned char>(this->BackgroundColor[1] * 255.0);
            backgroundPointer[4 * i + 2] = static_cast<unsigned char>(this->BackgroundColor[2] * 255.0);
            backgroundPointer[4 * i + 3] = static_cast<unsigned char>(this->BackgroundOpacity * 255.0);
        }

        m_backgroundImage->Modified();
    }

    return returnValue;
}

void VtkTextActorWithBackground::ComputeRectangle(vtkViewport *viewport)
{
    this->RectanglePoints->Reset();

    int textDimensions[2];
    int textDimensionsWithMargin[2];

    if (this->ImageData)
    {
        int textBoundingBox[4];

        if (!this->GetImageBoundingBox(this->ScaledTextProperty, viewport, textBoundingBox))
        {
            vtkErrorMacro("Cannot compute bounding box.")
            return;
        }

        textDimensions[0] = (textBoundingBox[1] - textBoundingBox[0] + 1);
        textDimensions[1] = (textBoundingBox[3] - textBoundingBox[2] + 1);
        textDimensionsWithMargin[0] = textDimensions[0] + 2 * this->Margin;
        textDimensionsWithMargin[1] = textDimensions[1] + 2 * this->Margin;

        int textureDimensions[3];
        m_backgroundImage->GetDimensions(textureDimensions);

        // compute TCoords.
        vtkFloatArray *textureCoordinates = vtkFloatArray::SafeDownCast(this->Rectangle->GetPointData()->GetTCoords());
        // Add a fudge factor to the texture coordinates to prevent the top
        // row of pixels from being truncated on some systems.
        // If there is a margin the fudge factor is not needed.
        float fudgeFactor = this->Margin == 0 ? 0.001f : 0;
        float tcXMax = std::min(1.0f, (textDimensionsWithMargin[0] + fudgeFactor) / textureDimensions[0]);
        float tcYMax = std::min(1.0f, (textDimensionsWithMargin[1] + fudgeFactor) / textureDimensions[1]);

        textureCoordinates->InsertComponent(0, 0, 0.0);
        textureCoordinates->InsertComponent(0, 1, 0.0);

        textureCoordinates->InsertComponent(1, 0, 0.0);
        textureCoordinates->InsertComponent(1, 1, tcYMax);

        textureCoordinates->InsertComponent(2, 0, tcXMax);
        textureCoordinates->InsertComponent(2, 1, tcYMax);

        textureCoordinates->InsertComponent(3, 0, tcXMax);
        textureCoordinates->InsertComponent(3, 1, 0.0);
    }
    else
    {
        textDimensions[0] = textDimensions[1] = 0;
        textDimensionsWithMargin[0] = textDimensionsWithMargin[1] = 0;
    }

    double xo = 0.0, yo = 0.0;

    // When TextScaleMode is PROP, we justify text based on the rectangle
    // formed by Position & Position2 coordinates
    if ((this->TextScaleMode == TEXT_SCALE_MODE_PROP) || this->UseBorderAlign)
    {
        double position1[3], position2[3];
        this->PositionCoordinate->GetValue(position1);
        this->Position2Coordinate->GetValue(position2);
        this->SpecifiedToDisplay(position1, viewport, this->PositionCoordinate->GetCoordinateSystem());
        this->SpecifiedToDisplay(position2, viewport, this->Position2Coordinate->GetCoordinateSystem());
        double maxWidth = position2[0] - position1[0];
        double maxHeight = position2[1] - position1[1];

        // I could get rid of "GetAlignmentPoint" and use justification directly.
        switch (this->GetAlignmentPoint())
        {
            case 0:
                break;
            case 1:
                xo = (maxWidth - textDimensionsWithMargin[0]) * 0.5;
                break;
            case 2:
                xo = (maxWidth - textDimensionsWithMargin[0]);
                break;
            case 3:
                yo = (maxHeight - textDimensionsWithMargin[1]) * 0.5;
                break;
            case 4:
                xo = (maxWidth - textDimensionsWithMargin[0]) * 0.5;
                yo = (maxHeight - textDimensionsWithMargin[1]) * 0.5;
            break;
            case 5:
                xo = (maxWidth - textDimensionsWithMargin[0]);
                yo = (maxHeight - textDimensionsWithMargin[1]) * 0.5;
                break;
            case 6:
                yo = (maxHeight - textDimensionsWithMargin[1]);
                break;
            case 7:
                xo = (maxWidth - textDimensionsWithMargin[0]) * 0.5;
                yo = (maxHeight - textDimensionsWithMargin[1]);
                break;
            case 8:
                xo = (maxWidth - textDimensionsWithMargin[0]);
                yo = (maxHeight - textDimensionsWithMargin[1]);
                break;
            default:
                vtkErrorMacro(<< "Bad alignment point value.");
        }

        //handle line offset.  make sure we stay within the bounds defined by
        //position1 & position2
        double offset = this->TextProperty->GetLineOffset();

        if ((yo + offset + textDimensionsWithMargin[1]) > maxHeight)
        {
            yo = maxHeight - textDimensionsWithMargin[1];
        }
        else if ((yo + offset) < 0)
        {
            yo = 0;
        }
        else
        {
            yo += offset;
        }
    }
    else
    {
        // I could get rid of "GetAlignmentPoint" and use justification directly.
        switch (this->GetAlignmentPoint())
        {
            case 0:
                break;
            case 1:
                xo = -textDimensionsWithMargin[0] * 0.5;
                break;
            case 2:
                xo = -textDimensionsWithMargin[0];
                break;
            case 3:
                yo = -textDimensionsWithMargin[1] * 0.5;
                break;
            case 4:
                yo = -textDimensionsWithMargin[1] * 0.5;
                xo = -textDimensionsWithMargin[0] * 0.5;
                break;
            case 5:
                yo = -textDimensionsWithMargin[1] * 0.5;
                xo = -textDimensionsWithMargin[0];
                break;
            case 6:
                yo = -textDimensionsWithMargin[1];
                break;
            case 7:
                yo = -textDimensionsWithMargin[1];
                xo = -textDimensionsWithMargin[0] * 0.5;
                break;
            case 8:
                yo = -textDimensionsWithMargin[1];
                xo = -textDimensionsWithMargin[0];
                break;
            default:
                vtkErrorMacro(<< "Bad alignment point value.");
        }

        // handle line offset
        yo += this->TextProperty->GetLineOffset();
    } //end unscaled text case

    // I could do this with a transform, but it is simple enough
    // to rotate the four corners in 2D ...
    double radians = vtkMath::RadiansFromDegrees(this->Orientation);
    double cosine = cos(radians);
    double sine = sin(radians);

    double x = xo;
    double y = yo;
    this->RectanglePoints->InsertNextPoint(cosine * x - sine * y, sine * x + cosine * y, 0.0);
    x = xo;
    y = yo + textDimensionsWithMargin[1];
    this->RectanglePoints->InsertNextPoint(cosine * x - sine * y, sine * x + cosine * y, 0.0);
    x = xo + textDimensionsWithMargin[0];
    y = yo + textDimensionsWithMargin[1];
    this->RectanglePoints->InsertNextPoint(cosine * x - sine * y, sine * x + cosine * y, 0.0);
    x = xo + textDimensionsWithMargin[0];
    y = yo;
    this->RectanglePoints->InsertNextPoint(cosine * x - sine * y, sine * x + cosine * y, 0.0);
}

int VtkTextActorWithBackground::UpdateRectangle(vtkViewport *viewport)
{
    int returnValue = this->Superclass::UpdateRectangle(viewport);

    if (returnValue)
    {
        m_imageBlend->Update();
        this->Texture->SetInputData(m_imageBlend->GetOutput());
        this->Texture->Modified();
        this->BuildTime.Modified();
    }

    updateWorldBounds(viewport);

    return returnValue;
}

void VtkTextActorWithBackground::updateWorldBounds(vtkViewport *viewport)
{
    double position[3];
    this->PositionCoordinate->GetValue(position);
    this->SpecifiedToDisplay(position, viewport, this->PositionCoordinate->GetCoordinateSystem());

    vtkPoints *boundingPoints = vtkPoints::New();
    boundingPoints->SetNumberOfPoints(4);

    for (int i = 0; i < 4; i++)
    {
        double point[3];
        this->RectanglePoints->GetPoint(i, point);
        point[0] = position[0] + point[0];
        point[1] = position[1] + point[1];
        point[2] = position[2];
        this->DisplayToSpecified(point, viewport, this->PositionCoordinate->GetCoordinateSystem());
        boundingPoints->SetPoint(i, point);
    }

    boundingPoints->GetBounds(this->WorldBounds);
    boundingPoints->Delete();
}

} // namespace udg
