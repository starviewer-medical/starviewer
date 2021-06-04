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

#ifndef UDG_DRAWERARROW_H
#define UDG_DRAWERARROW_H

#include "drawerprimitive.h"

#include "vector3.h"

#include <vtkNew.h>

class vtkActor2D;
class vtkLineSource;
class vtkPolyDataMapper2D;
class vtkPropAssembly;

namespace udg {

/**
 * @brief The DrawerArrow class draws a line with an arrow tip at the end.
 */
class DrawerArrow : public DrawerPrimitive
{
    Q_OBJECT

public:
    explicit DrawerArrow(QObject *parent = nullptr);
    ~DrawerArrow() override;

    /// Returns the start point of the arrow.
    const Vector3& getStartPoint() const;
    /// Sets the start point of the arrow.
    void setStartPoint(const Vector3 &point);

    /// Returns the end point of the arrow.
    const Vector3& getEndPoint() const;
    /// Sets the end point of the arrow.
    void setEndPoint(const Vector3 &point);

    /// Sets a scale value for the tip of the arrow. To achieve good results this should be a measure of the size of the image.
    void setScale(double scale);
    /// Sets the view plane normal needed to draw the tip lines with the correct orientation.
    void setViewPlaneNormal(const Vector3 &normal);

    vtkProp* getAsVtkProp() override;
    double getDistanceToPoint(double *point3D, double closestPoint[3]) override;
    void getBounds(double bounds[6]) override;

public slots:
    void update() override;

protected slots:
    void updateVtkProp() override;

private:
    void updateVtkActorProperties();

private:
    /// First point of the arrow.
    Vector3 m_startPoint;
    /// Final point of the arrow, with the arrow tip.
    Vector3 m_endPoint;

    /// This is used to limit the size of the tip of the arrow.
    double m_scale;
    /// This is used to draw the lines of the tip of the arrow on the view plane.
    Vector3 m_viewPlaneNormal;

    vtkNew<vtkLineSource> m_lineSource;
    vtkNew<vtkLineSource> m_tipTopSource;
    vtkNew<vtkLineSource> m_tipBottomSource;

    vtkNew<vtkPolyDataMapper2D> m_lineMapper;
    vtkNew<vtkPolyDataMapper2D> m_tipTopMapper;
    vtkNew<vtkPolyDataMapper2D> m_tipBottomMapper;

    vtkNew<vtkActor2D> m_lineActor;
    vtkNew<vtkActor2D> m_tipTopActor;
    vtkNew<vtkActor2D> m_tipBottomActor;
    vtkNew<vtkActor2D> m_backgroundLineActor;
    vtkNew<vtkActor2D> m_backgroundTipTopActor;
    vtkNew<vtkActor2D> m_backgroundTipBottomActor;

    vtkNew<vtkPropAssembly> m_propAssembly;
};

} // namespace udg

#endif // UDG_DRAWERARROW_H
