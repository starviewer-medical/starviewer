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
#include "vtktextactorwithbackground.h"
// Vtk
#include <vtkTextProperty.h>

namespace udg {

DrawerText::DrawerText(QObject *parent)
 : DrawerPrimitive(parent)
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
    m_vtkActor = 0;
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

void DrawerText::setAttachmentPoint(Vector3 point)
{
    m_attachPoint = std::move(point);
    emit changed();
}

vtkProp* DrawerText::getAsVtkProp()
{
    if (!m_vtkActor)
    {
        // Creem el pipeline de l'm_vtkActor
        m_vtkActor = VtkTextActorWithBackground::New();
        m_vtkActor->SetMargin(2);

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
        m_vtkActor->GetPositionCoordinate()->SetValue(m_attachPoint.data());

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
        m_vtkActor->GetPositionCoordinate()->SetValue(m_attachPoint.data());
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
    m_vtkActor->SetBackgroundOpacity(m_backgroundOpacity);

    // Assignem color
    m_vtkActor->SetBackgroundColor(m_backgroundColor.redF(), m_backgroundColor.greenF(), m_backgroundColor.blueF());

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

const Vector3& DrawerText::getAttachmentPoint() const
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

double DrawerText::getDistanceToPoint(const Vector3 &point3D, Vector3 &closestPoint)
{
    if (isInside(point3D))
    {
        closestPoint = point3D;
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

bool DrawerText::isInside(const Vector3 &point3D)
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
    if (m_vtkActor)
    {
        m_vtkActor->GetWorldBounds(bounds);
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
