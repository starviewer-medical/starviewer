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

#include "drawerbitmap.h"

#include "logging.h"
#include "mathtools.h"
#include "volumepixeldata.h"
#include "vtkimagedatacreator.h"

#include <QColor>

#include <vtkImageActor.h>
#include <vtkLookupTable.h>
#include <vtkImageMapToColors.h>

namespace udg {

DrawerBitmap::DrawerBitmap(QObject *parent)
 : DrawerPrimitive(parent)
{
    m_origin[0] = m_origin[1] = m_origin[2] = 0.0;
    m_spacing[0] = m_spacing[1] = m_spacing[2] = 1.0;
    m_width = m_height = 0;
    m_data = 0;
    m_backgroundOpacity = 0.0;
    m_backgroundColor = Qt::black;
    m_foregroundColor = Qt::white;
    m_imageActor = 0;
}

DrawerBitmap::~DrawerBitmap()
{
    emit dying(this);
}

void DrawerBitmap::setOrigin(double origin[3])
{
    memcpy(m_origin, origin, 3 * sizeof(double));
}

void DrawerBitmap::setSpacing(double spacing[3])
{
    memcpy(m_spacing, spacing, 3 * sizeof(double));
}

void DrawerBitmap::setData(unsigned int width, unsigned int height, unsigned char *data)
{
    m_width = width;
    m_height = height;    
    m_data = data;
}

void DrawerBitmap::setBackgroundOpacity(double opacity)
{
    m_backgroundOpacity = opacity;
}

void DrawerBitmap::setBackgroundColor(const QColor &color)
{
    m_backgroundColor = color;
}

void DrawerBitmap::setForegroundColor(const QColor &color)
{
    m_foregroundColor = color;
}

vtkProp* DrawerBitmap::getAsVtkProp()
{
    if (!m_imageActor)
    {
        if (m_data)
        {
            VtkImageDataCreator imageDataCreator;
            imageDataCreator.setOrigin(m_origin);
            imageDataCreator.setSpacing(m_spacing);
            vtkSmartPointer<vtkImageData> imageData = imageDataCreator.createVtkImageData(m_width, m_height, 1, m_data);

            // Construim LUT per aplicar transparències: fet a partir del codi de http://www.vtk.org/Wiki/VTK/Examples/Cxx/Images/Transparency
            vtkSmartPointer<vtkLookupTable> lookupTable = vtkSmartPointer<vtkLookupTable>::New();
            lookupTable->SetNumberOfTableValues(2);
            lookupTable->SetRange(0.0, 1.0);
            // Valor 0 té m_backgroundOpacity i color  m_backgroundColor
            lookupTable->SetTableValue(0, m_backgroundColor.redF(), m_backgroundColor.greenF(), m_backgroundColor.blueF(), m_backgroundOpacity); 
            // Valor 1 és opac amb color m_foregroundColor
            lookupTable->SetTableValue(1, m_foregroundColor.redF(), m_foregroundColor.greenF(), m_foregroundColor.blueF(), 1.0);
            lookupTable->Build();
 
            vtkSmartPointer<vtkImageMapToColors> mapTransparency = vtkSmartPointer<vtkImageMapToColors>::New();
            mapTransparency->SetLookupTable(lookupTable);
            mapTransparency->SetInputData(imageData);
            mapTransparency->PassAlphaToOutputOn();
 
            // Creem l'actor
            m_imageActor = vtkSmartPointer<vtkImageActor>::New();
            mapTransparency->Update();
            m_imageActor->SetInputData(mapTransparency->GetOutput());
            m_imageActor->SetDisplayExtent(0, m_width - 1, 0, m_height - 1, 0, 0);
            m_imageActor->SetVisibility(this->isVisible());
        }
        else
        {
            DEBUG_LOG("Error al passar les dades del bitmap a format vtkImageActor o bé no hi ha dades");
        }
    }

    return m_imageActor;
}

double DrawerBitmap::getDistanceToPointInDisplay(const Vector3 &displayPoint, Vector3 &closestDisplayPoint,
                                                 std::function<Vector3(const Vector3&)> worldToDisplay)
{
    // Everything in this method is in display coordinates
    auto bounds = getDisplayBounds(worldToDisplay);

    if (displayPoint.x >= bounds[0] && displayPoint.x <= bounds[1] && displayPoint.y >= bounds[2] && displayPoint.y <= bounds[3])
    {
        closestDisplayPoint = displayPoint;
        return 0.0;
    }

    Vector3 topLeftCorner{bounds[0], bounds[2], 0};
    Vector3 topRightCorner{bounds[1], bounds[2], 0};
    Vector3 bottomRightCorner{bounds[1], bounds[3], 0};
    Vector3 bottomLeftCorner{bounds[0], bounds[3], 0};
    
    double minimumDistanceFound = MathTools::DoubleMaximumValue;
    Vector3 localClosestPoint;

    double distance = MathTools::getPointToFiniteLineDistance(displayPoint, topLeftCorner, topRightCorner, localClosestPoint);
    if (distance < minimumDistanceFound)
    {
        minimumDistanceFound = distance;
        closestDisplayPoint = localClosestPoint;
    }

    distance = MathTools::getPointToFiniteLineDistance(displayPoint, topRightCorner, bottomRightCorner, localClosestPoint);
    if (distance < minimumDistanceFound)
    {
        minimumDistanceFound = distance;
        closestDisplayPoint = localClosestPoint;
    }
    
    distance = MathTools::getPointToFiniteLineDistance(displayPoint, bottomRightCorner, bottomLeftCorner, localClosestPoint);
    if (distance < minimumDistanceFound)
    {
        minimumDistanceFound = distance;
        closestDisplayPoint = localClosestPoint;
    }
    
    distance = MathTools::getPointToFiniteLineDistance(displayPoint, bottomLeftCorner, topLeftCorner, localClosestPoint);
    if (distance < minimumDistanceFound)
    {
        minimumDistanceFound = distance;
        closestDisplayPoint = localClosestPoint;
    }

    return minimumDistanceFound;
}

std::array<double, 4> DrawerBitmap::getDisplayBounds(std::function<Vector3(const Vector3&)> worldToDisplay)
{
    double minX = m_origin[0], maxX = m_origin[0] + m_spacing[0] * m_width,
           minY = m_origin[1], maxY = m_origin[1] + m_spacing[1] * m_height,
           minZ = m_origin[2], maxZ = m_origin[2] + m_spacing[2];
    Vector3 corners[] = { Vector3(minX, minY, minZ), Vector3(minX, minY, maxZ), Vector3(minX, maxY, minZ), Vector3(minX, maxY, maxZ),
                          Vector3(maxX, minY, minZ), Vector3(maxX, minY, maxZ), Vector3(maxX, maxY, minZ), Vector3(maxX, maxY, maxZ) };

    minX = minY = std::numeric_limits<double>::infinity();
    maxX = maxY = -std::numeric_limits<double>::infinity();

    for (auto corner : corners)
    {
        auto displayPoint = worldToDisplay(corner);
        minX = std::min(displayPoint.x, minX);
        maxX = std::max(displayPoint.x, maxX);
        minY = std::min(displayPoint.y, minY);
        maxY = std::max(displayPoint.y, maxY);
    }

    return std::array<double, 4>{{minX, maxX, minY, maxY}};
}

void DrawerBitmap::update()
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

void DrawerBitmap::updateVtkProp()
{
    if (m_imageActor)
    {
        // TODO De moment únicament contemplem que un cop creat el bitmap, l'únic que podrem modificar serà la seva visibilitat
        m_imageActor->SetVisibility(this->isVisible());
        this->setModified(false);
    }
    else
    {
        DEBUG_LOG("No es pot actualitzar el bitmap, ja que l'actor encara no s'ha creat");
    }
}

} // end namespace udg
