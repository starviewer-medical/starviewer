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

#include <QObject>
#include <QStringList>

#include "patientorientation.h"
#include "dicomvaluerepresentationconverter.h"

namespace udg {

const QString AnatomicalPlane::getLabel(AnatomicalPlaneType orientation)
{
    QString label;

    switch (orientation)
    {
        case Axial:
            label = QObject::tr("AXIAL");
            break;

        case Sagittal:
            label = QObject::tr("SAGITTAL");
            break;

        case Coronal:
            label = QObject::tr("CORONAL");
            break;

        case Oblique:
            label = QObject::tr("OBLIQUE");
            break;

        case NotAvailable:
            label = QObject::tr("N/A");
            break;
    }

    return label;
}

const QString AnatomicalPlane::getLabelFromPatientOrientation(const PatientOrientation &orientation)
{
    return AnatomicalPlane::getLabel(getPlaneTypeFromPatientOrientation(orientation));
}

const AnatomicalPlane::AnatomicalPlaneType AnatomicalPlane::getPlaneTypeFromPatientOrientation(const PatientOrientation &orientation)
{
    AnatomicalPlaneType planeType;
    QString rowAxis = orientation.getRowDirectionLabel();
    QString columnAxis = orientation.getColumnDirectionLabel();

    if (!rowAxis.isEmpty() && !columnAxis.isEmpty())
    {
        if ((rowAxis.startsWith(PatientOrientation::RightLabel) || rowAxis.startsWith(PatientOrientation::LeftLabel)) &&
            (columnAxis.startsWith(PatientOrientation::AnteriorLabel) || columnAxis.startsWith(PatientOrientation::PosteriorLabel)))
        {
            planeType = AnatomicalPlane::Axial;
        }
        else if ((columnAxis.startsWith(PatientOrientation::RightLabel) || columnAxis.startsWith(PatientOrientation::LeftLabel)) &&
            (rowAxis.startsWith(PatientOrientation::AnteriorLabel) || rowAxis.startsWith(PatientOrientation::PosteriorLabel)))
        {
            planeType = AnatomicalPlane::Axial;
        }
        else if ((rowAxis.startsWith(PatientOrientation::RightLabel) || rowAxis.startsWith(PatientOrientation::LeftLabel)) &&
            (columnAxis.startsWith(PatientOrientation::HeadLabel) || columnAxis.startsWith(PatientOrientation::FeetLabel)))
        {
            planeType = AnatomicalPlane::Coronal;
        }
        else if ((columnAxis.startsWith(PatientOrientation::RightLabel) || columnAxis.startsWith(PatientOrientation::LeftLabel)) &&
            (rowAxis.startsWith(PatientOrientation::HeadLabel) || rowAxis.startsWith(PatientOrientation::FeetLabel)))
        {
            planeType = AnatomicalPlane::Coronal;
        }
        else if ((rowAxis.startsWith(PatientOrientation::AnteriorLabel) || rowAxis.startsWith(PatientOrientation::PosteriorLabel)) &&
            (columnAxis.startsWith(PatientOrientation::HeadLabel) || columnAxis.startsWith(PatientOrientation::FeetLabel)))
        {
            planeType = AnatomicalPlane::Sagittal;
        }
        else if ((columnAxis.startsWith(PatientOrientation::AnteriorLabel) || columnAxis.startsWith(PatientOrientation::PosteriorLabel)) &&
            (rowAxis.startsWith(PatientOrientation::HeadLabel) || rowAxis.startsWith(PatientOrientation::FeetLabel)))
        {
            planeType = AnatomicalPlane::Sagittal;
        }
        else
        {
            planeType = AnatomicalPlane::Oblique;
        }
    }
    else
    {
        planeType = AnatomicalPlane::NotAvailable;
    }

    return planeType;
}

const PatientOrientation AnatomicalPlane::getDefaultRadiologicalOrienation(AnatomicalPlaneType plane)
{
    PatientOrientation orientation;

    switch (plane)
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

} // End namespace udg
