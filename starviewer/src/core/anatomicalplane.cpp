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

#include "anatomicalplane.h"

#include "patientorientation.h"

#include <QObject>

namespace udg {

AnatomicalPlane::AnatomicalPlane(Plane plane)
    : m_plane(plane)
{
}

QString AnatomicalPlane::getLabel() const
{
    switch (m_plane)
    {
        case Axial:
            return QObject::tr("AXIAL");

        case Sagittal:
            return QObject::tr("SAGITTAL");

        case Coronal:
            return QObject::tr("CORONAL");

        case Oblique:
            return QObject::tr("OBLIQUE");

        case NotAvailable:
            return QObject::tr("N/A");

        default:
            return QString();
    }
}

QString AnatomicalPlane::getLabelFromPatientOrientation(const PatientOrientation &orientation)
{
    return getPlaneFromPatientOrientation(orientation).getLabel();
}

AnatomicalPlane AnatomicalPlane::getPlaneFromPatientOrientation(const PatientOrientation &orientation)
{
    QString rowAxis = orientation.getRowDirectionLabel();
    QString columnAxis = orientation.getColumnDirectionLabel();

    if (!rowAxis.isEmpty() && !columnAxis.isEmpty())
    {
        if ((rowAxis.startsWith(PatientOrientation::RightLabel) || rowAxis.startsWith(PatientOrientation::LeftLabel)) &&
            (columnAxis.startsWith(PatientOrientation::AnteriorLabel) || columnAxis.startsWith(PatientOrientation::PosteriorLabel)))
        {
            return Axial;
        }
        else if ((columnAxis.startsWith(PatientOrientation::RightLabel) || columnAxis.startsWith(PatientOrientation::LeftLabel)) &&
            (rowAxis.startsWith(PatientOrientation::AnteriorLabel) || rowAxis.startsWith(PatientOrientation::PosteriorLabel)))
        {
            return Axial;
        }
        else if ((rowAxis.startsWith(PatientOrientation::RightLabel) || rowAxis.startsWith(PatientOrientation::LeftLabel)) &&
            (columnAxis.startsWith(PatientOrientation::HeadLabel) || columnAxis.startsWith(PatientOrientation::FeetLabel)))
        {
            return Coronal;
        }
        else if ((columnAxis.startsWith(PatientOrientation::RightLabel) || columnAxis.startsWith(PatientOrientation::LeftLabel)) &&
            (rowAxis.startsWith(PatientOrientation::HeadLabel) || rowAxis.startsWith(PatientOrientation::FeetLabel)))
        {
            return Coronal;
        }
        else if ((rowAxis.startsWith(PatientOrientation::AnteriorLabel) || rowAxis.startsWith(PatientOrientation::PosteriorLabel)) &&
            (columnAxis.startsWith(PatientOrientation::HeadLabel) || columnAxis.startsWith(PatientOrientation::FeetLabel)))
        {
            return Sagittal;
        }
        else if ((columnAxis.startsWith(PatientOrientation::AnteriorLabel) || columnAxis.startsWith(PatientOrientation::PosteriorLabel)) &&
            (rowAxis.startsWith(PatientOrientation::HeadLabel) || rowAxis.startsWith(PatientOrientation::FeetLabel)))
        {
            return Sagittal;
        }
        else
        {
            return Oblique;
        }
    }
    else
    {
        return NotAvailable;
    }
}

PatientOrientation AnatomicalPlane::getDefaultRadiologicalOrientation() const
{
    PatientOrientation orientation;

    // We have to specify the labels for the right and down directions
    switch (m_plane)
    {
        case Axial:
            orientation.setLabels(PatientOrientation::LeftLabel, PatientOrientation::PosteriorLabel);
            break;

        case Sagittal:
            orientation.setLabels(PatientOrientation::PosteriorLabel, PatientOrientation::FeetLabel);
            break;

        case Coronal:
            orientation.setLabels(PatientOrientation::LeftLabel, PatientOrientation::FeetLabel);
            break;

        case Oblique:
        case NotAvailable:
        default:
            // We don't have enough information to provide such information
            break;
    }

    return orientation;
}

AnatomicalPlane::operator Plane() const
{
    return m_plane;
}

} // End namespace udg
