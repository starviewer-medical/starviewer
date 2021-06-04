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

#include "drawertext.h"
#include "logging.h"
#include "mathtools.h"
#include "applicationstylehelper.h"

#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkViewport.h>

namespace udg {

DrawerText::DrawerText(QObject *parent)
    : DrawerPrimitive(parent), m_vtkActor(nullptr)
{
    m_horizontalJustification = "Centered";
    m_verticalJustification = "Centered";
    m_fontFamily = "Arial";
    m_fontSize = ApplicationStyleHelper(true).getToolsFontSize();
    m_shadow = false;
    m_italic = false;
    m_bold = true;
    m_height = 0.05;
    m_width = 0.09;
    m_scaled = false;
    m_backgroundColor = QColor(0, 0, 0);
    m_backgroundOpacity = 0.85;
}

DrawerText::~DrawerText()
{
    emit dying(this);

    if (m_vtkActor)
    {
        m_vtkActor->Delete();
    }
}

void DrawerText::setAttachmentPoint(double point[3])
{
    for (int i = 0; i < 3; i++)
    {
        m_attachPoint[i] = point[i];
    }

    emit changed();
}

vtkProp* DrawerText::getAsVtkProp()
{
    if (!m_vtkActor)
    {
        // Creem el pipeline de l'm_vtkActor
        m_vtkActor = vtkTextActor::New();

        // Assignem el text
        if (!m_text.isEmpty())
        {
            m_vtkActor->SetInput(m_text.toUtf8().constData());
            if (m_isVisible)
            {
                m_vtkActor->VisibilityOn();
            }
        }
        else
        {
            m_vtkActor->VisibilityOff();
        }

        // Assignem la posició en pantalla
        m_vtkActor->GetPositionCoordinate()->SetCoordinateSystemToWorld();
        m_vtkActor->GetPositionCoordinate()->SetValue(m_attachPoint);

        // Li donem els atributs
        updateVtkActorProperties();
    }
    return m_vtkActor;
}

void DrawerText::update()
{
    switch (m_internalRepresentation)
    {
        case VTKRepresentation:
            updateVtkProp();
            break;

        case OpenGLRepresentation:
            break;
    }
}

void DrawerText::updateVtkProp()
{
    if (m_vtkActor)
    {
        // Assignem el text
        if (!m_text.isEmpty())
        {
            m_vtkActor->SetInput(m_text.toUtf8().constData());
            if (m_isVisible)
            {
                m_vtkActor->VisibilityOn();
            }
        }
        else
        {
            m_vtkActor->VisibilityOff();
        }
        // Assignem la posició en pantalla
        m_vtkActor->GetPositionCoordinate()->SetCoordinateSystemToWorld();
        m_vtkActor->GetPositionCoordinate()->SetValue(m_attachPoint);
        updateVtkActorProperties();
        this->setModified(false);
    }
    else
    {
        DEBUG_LOG("No es pot actualitzar la línia, ja que encara no s'ha creat!");
    }
}

void DrawerText::updateVtkActorProperties()
{
    vtkTextProperty *properties = m_vtkActor->GetTextProperty();

    // Sistema de coordenades
    m_vtkActor->GetPositionCoordinate()->SetReferenceCoordinate(this->getVtkCoordinateObject());

    // Mirem si s'ha d'escalar el text
    if (m_scaled)
    {
        m_vtkActor->SetTextScaleModeToViewport();
    }
    else
    {
        m_vtkActor->SetTextScaleModeToNone();
    }

    // Mirem l'opacitat
    properties->SetOpacity(m_opacity);

    // Assignem color
    properties->SetColor(m_color.redF(), m_color.greenF(), m_color.blueF());

    // Mirem l'opacitat
    properties->SetBackgroundOpacity(m_backgroundOpacity);

    // Assignem color
    properties->SetBackgroundColor(m_backgroundColor.redF(), m_backgroundColor.greenF(), m_backgroundColor.blueF());

    m_vtkActor->SetHeight(m_height);
    m_vtkActor->SetWidth(m_width);

    if (m_bold)
    {
        properties->BoldOn();
    }
    else
    {
        properties->BoldOff();
    }

    if (m_shadow)
    {
        properties->ShadowOn();
    }
    else
    {
        properties->ShadowOff();
    }

    if (m_italic)
    {
        properties->ItalicOn();
    }
    else
    {
        properties->ItalicOff();
    }

    // Assignem el tipus de font al text
    if (m_fontFamily == "Arial")
    {
        properties->SetFontFamilyToArial();
    }
    else if (m_fontFamily == "Courier")
    {
        properties->SetFontFamilyToCourier();
    }
    else if (m_fontFamily == "Times")
    {
        properties->SetFontFamilyToTimes();
    }
    else
    {
        DEBUG_LOG("Tipus de font no reconegut a l'intentar crear text!!");
    }

    // Assignem el tamany de la font
    properties->SetFontSize(m_fontSize);

    // Assignem el tipus de justificació horitzontal
    if (m_horizontalJustification == "Left")
    {
        properties->SetJustificationToLeft();
    }
    else if (m_horizontalJustification == "Centered")
    {
        properties->SetJustificationToCentered();
    }
    else if (m_horizontalJustification == "Right")
    {
        properties->SetJustificationToRight();
    }
    else
    {
        DEBUG_LOG("Tipus de justificació horitzontal no reconegut a l'intentar crear text!!");
    }

    // Assignem el tipus de justificació vertical
    if (m_verticalJustification == "Top")
    {
        properties->SetVerticalJustificationToTop();
    }
    else if (m_verticalJustification == "Centered")
    {
        properties->SetVerticalJustificationToCentered();
    }
    else if (m_verticalJustification == "Bottom")
    {
        properties->SetVerticalJustificationToBottom();
    }
    else
    {
        DEBUG_LOG("Tipus de justificació vertical no reconegut a l'intentar crear text!!");
    }

    // Mirem la visibilitat de l'actor
    m_vtkActor->SetVisibility(this->isVisible());
}

void DrawerText::setText(const QString &text)
{
    m_text = text;
    emit changed();
}

QString DrawerText::getText()
{
    return m_text;
}

double* DrawerText::getAttachmentPoint()
{
    return m_attachPoint;
}

QString DrawerText::getFontFamily()
{
    return m_fontFamily;
}

void DrawerText::setFontFamily(const QString &family)
{
    m_fontFamily = family;
    emit changed();
}

void DrawerText::setFontSize(int size)
{
    m_fontSize = size;
    emit changed();
}

int DrawerText::getFontSize()
{
    return m_fontSize;
}

void DrawerText::boldOn()
{
    bold(true);
}

void DrawerText::boldOff()
{
    bold(false);
}

void DrawerText::bold(bool bold)
{
    m_bold = bold;
    emit changed();
}

bool DrawerText::isBold()
{
    return m_bold;
}

void DrawerText::italicOn()
{
    italic(true);
}

void DrawerText::italicOff()
{
    italic(false);
}

void DrawerText::italic(bool italic)
{
    m_italic = italic;
    emit changed();
}

bool DrawerText::isItalic()
{
    return m_italic;
}

void DrawerText::shadowOn()
{
    setShadow(true);
}

void DrawerText::shadowOff()
{
    setShadow(false);
}

void DrawerText::setShadow(bool shadow)
{
    m_shadow = shadow;
    emit changed();
}

bool DrawerText::hasShadow()
{
    return m_shadow;
}

void DrawerText::setHorizontalJustification(const QString &horizontalJustification)
{
    m_horizontalJustification = horizontalJustification;
    emit changed();
}

QString DrawerText::getHorizontalJustification()
{
    return m_horizontalJustification;
}

void DrawerText::setVerticalJustification(const QString &verticalJustification)
{
    m_verticalJustification = verticalJustification;
    emit changed();
}

QString DrawerText::getVerticalJustification()
{
    return m_verticalJustification;
}

void DrawerText::scaledTextOn()
{
    scaledText(true);
}

void DrawerText::scaledTextOff()
{
    scaledText(false);
}

void DrawerText::scaledText(bool scaled)
{
    m_scaled = scaled;
    emit changed();
}

bool DrawerText::isTextScaled()
{
    return m_scaled;
}

double DrawerText::getDistanceToPoint(double *point3D, double closestPoint[3])
{
    if (isInside(point3D))
    {
        closestPoint[0] = point3D[0];
        closestPoint[1] = point3D[1];
        closestPoint[2] = point3D[2];

        return 0.0;
    }
    else
    {
        double bounds[6];
        getBounds(bounds);

        for (int i = 0; i < 3; i++)
        {
            if (point3D[i] < bounds[2*i])
            {
                closestPoint[i] = bounds[2*i];
            }
            else if (point3D[i] > bounds[2*i+1])
            {
                closestPoint[i] = bounds[2*i+1];
            }
            else
            {
                closestPoint[i] = point3D[i];
            }
        }

        return MathTools::getDistance3D(point3D, closestPoint);
    }
}

bool DrawerText::isInside(const double *point3D)
{
    double bounds[6];
    this->getBounds(bounds);

    if (((point3D[0] >= bounds[0] && point3D[0] <= bounds[1]) || qAbs(point3D[0] - bounds[0]) < 0.0001) &&
        ((point3D[1] >= bounds[2] && point3D[1] <= bounds[3]) || qAbs(point3D[1] - bounds[2]) < 0.0001) &&
        ((point3D[2] >= bounds[4] && point3D[2] <= bounds[5]) || qAbs(point3D[2] - bounds[4]) < 0.0001))
    {
        return true;
    }
    else
    {
        return false;
    }
}

void DrawerText::getBounds(double bounds[6])
{
    if (m_vtkActor && m_vtkActor->GetNumberOfConsumers() > 0)
    {
        vtkViewport *viewport = vtkViewport::SafeDownCast(m_vtkActor->GetConsumer(0));

        if (viewport)
        {
            double attachPointDisplay[3];
            viewport->SetWorldPoint(m_attachPoint[0], m_attachPoint[1], m_attachPoint[2], 1);
            viewport->WorldToDisplay();
            viewport->GetDisplayPoint(attachPointDisplay);

            double size[2];
            m_vtkActor->GetSize(viewport, size);

            auto displayToWorld = [=](double x, double y, double z) -> Vector3 {
                double homogeneousWorldPoint[4];
                viewport->SetDisplayPoint(x, y, z);
                viewport->DisplayToWorld();
                viewport->GetWorldPoint(homogeneousWorldPoint);
                double divisor = 1.0;
                if (homogeneousWorldPoint[3] != 0.0)
                {
                    divisor = homogeneousWorldPoint[3];
                }
                Vector3 worldPoint(homogeneousWorldPoint[0] / divisor,
                                   homogeneousWorldPoint[1] / divisor,
                                   homogeneousWorldPoint[2] / divisor);
                return worldPoint;
            };

            double left = 0;
            switch (m_vtkActor->GetTextProperty()->GetJustification())
            {
                case VTK_TEXT_LEFT: left = attachPointDisplay[0]; break;
                case VTK_TEXT_CENTERED: left = attachPointDisplay[0] - size[0] / 2; break;
                case VTK_TEXT_RIGHT: left = attachPointDisplay[0] - size[0]; break;
            }
            double right = left + size[0];
            double top = 0;
            switch (m_vtkActor->GetTextProperty()->GetVerticalJustification())
            {
                case VTK_TEXT_BOTTOM: top = attachPointDisplay[1] + size[1]; break;
                case VTK_TEXT_CENTERED: top = attachPointDisplay[1] + size[1] / 2; break;
                case VTK_TEXT_TOP: top = attachPointDisplay[1]; break;
            }
            double bottom = top - size[1];

            Vector3 corners[4] = { displayToWorld(left,  top,    attachPointDisplay[2]),
                                   displayToWorld(right, top,    attachPointDisplay[2]),
                                   displayToWorld(right, bottom, attachPointDisplay[2]),
                                   displayToWorld(left,  bottom, attachPointDisplay[2]) };

            bounds[0] = bounds[1] = corners[0].x;
            bounds[2] = bounds[3] = corners[0].y;
            bounds[4] = bounds[5] = corners[0].z;

            for (int i = 1; i < 4; i++)
            {
                bounds[0] = std::min(bounds[0], corners[i].x);
                bounds[1] = std::max(bounds[1], corners[i].x);
                bounds[2] = std::min(bounds[2], corners[i].y);
                bounds[3] = std::max(bounds[3], corners[i].y);
                bounds[4] = std::min(bounds[4], corners[i].z);
                bounds[5] = std::max(bounds[5], corners[i].z);
            }
        }
    }
}

void DrawerText::setBackgroundColor(QColor color)
{
    m_backgroundColor = color;
    emit changed();
}

QColor DrawerText::getBackgroundColor() const
{
    return m_backgroundColor;
}

void DrawerText::setBackgroundOpacity(double opacity)
{
    m_backgroundOpacity = opacity;
    emit changed();
}

double DrawerText::getBackgroundOpacity() const
{
    return m_backgroundOpacity;
}

}
