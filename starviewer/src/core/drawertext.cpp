#include "drawertext.h"
#include "logging.h"
#include "mathtools.h"
#include "q2dviewer.h"
#include "applicationstylehelper.h"
// Vtk
#include <vtkTextProperty.h>
#include <vtkTextActor.h>
#include <vtkCaptionActor2D.h>

namespace udg {

DrawerText::DrawerText(QObject *parent)
    : DrawerPrimitive(parent)
{
    m_horizontalJustification = "Centered";
    m_verticalJustification = "Centered";
    m_border = false;
    m_fontFamily = "Arial";
    m_fontSize = ApplicationStyleHelper().getToolsFontSize();
    m_shadow = false;
    m_italic = false;
    m_bold = false;
    m_height = 0.05;
    m_width = 0.09;
    m_scaled = false;
    m_padding = 0;
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
        m_vtkActor = vtkCaptionActor2D::New();

        // Assignem el text
        if (!m_text.isEmpty())
        {
            m_vtkActor->SetCaption(qPrintable(m_text));
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
        m_vtkActor->SetAttachmentPoint(m_attachPoint);

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
            m_vtkActor->SetCaption(qPrintable(m_text));
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
        m_vtkActor->SetAttachmentPoint(m_attachPoint);
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
    vtkTextProperty *properties = m_vtkActor->GetCaptionTextProperty();

    // Sistema de coordenades
    m_vtkActor->GetAttachmentPointCoordinate()->SetReferenceCoordinate(this->getVtkCoordinateObject());

    // Mirem si s'ha d'escalar el text
    if (m_scaled)
    {
        m_vtkActor->GetTextActor()->SetTextScaleModeToViewport();
    }
    else
    {
        m_vtkActor->GetTextActor()->SetTextScaleModeToNone();
    }

    // Mirem l'opacitat
    properties->SetOpacity(m_opacity);

    // Assignem color
    properties->SetColor(m_color.redF(), m_color.greenF(), m_color.blueF());

    // Mirem l'opacitat
    properties->SetBackgroundOpacity(m_backgroundOpacity);

    // Assignem color
    properties->SetBackgroundColor(m_backgroundColor.redF(), m_backgroundColor.greenF(), m_backgroundColor.blueF());

    m_vtkActor->SetPadding(m_padding);
    m_vtkActor->SetPosition(-1.0, -1.0);
    m_vtkActor->SetHeight(m_height);
    m_vtkActor->SetWidth(m_width);

    // Deshabilitem la línia que va des del punt de situació al text
    m_vtkActor->LeaderOff();
    m_vtkActor->ThreeDimensionalLeaderOff();

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

    // Mirem si el text té fons o no
    if (m_border)
    {
         m_vtkActor->BorderOn();
    }
    else
    {
        m_vtkActor->BorderOff();
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

void DrawerText::borderOn()
{
    borderEnabled(true);
}

void DrawerText::borderOff()
{
    borderEnabled(false);
}

void DrawerText::borderEnabled(bool enabled)
{
    m_border = enabled;
    emit changed();
}

bool DrawerText::isBorderEnabled()
{
    return m_border;
}

void DrawerText::setPadding(int padding)
{
    m_padding = padding;
    emit changed();
}

int DrawerText::getPadding()
{
    return m_padding;
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
        //TODO: S'hauria de fer que quan està fora calculés la distància a l'aresta més propera.
        return MathTools::DoubleMaximumValue;
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
    if (m_vtkActor)
    {
        m_vtkActor->GetTextActor()->GetLastBounds(bounds);
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
