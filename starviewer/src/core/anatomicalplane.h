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

#ifndef UDGANATOMICALPLANE_H
#define UDGANATOMICALPLANE_H

class QString;

namespace udg {

class PatientOrientation;

/**
 * @brief The AnatomicalPlane class defines the anatomical orientations of a patient.
 */
class AnatomicalPlane {
public:
    /// Anatomical orientation types.
    enum Plane { Axial, Sagittal, Coronal, Oblique, NotAvailable };

    AnatomicalPlane(Plane plane = Axial);

    /// Returns the label corresponding to this anatomical plane to display it in the UI.
    QString getLabel() const;

    /// Returns the label corresponding to the anatomical plane corresponding to the given patient orientation.
    static QString getLabelFromPatientOrientation(const PatientOrientation &orientation);

    /// Returns the anatomical plane corresponding to the given patient orientation.
    static AnatomicalPlane getPlaneFromPatientOrientation(const PatientOrientation &orientation);

    /// Returns the default patient orientation for this anatomical plane.
    PatientOrientation getDefaultRadiologicalOrientation() const;

    /// Conversion operator: allows to use an AnatomicalPlane in a switch.
    operator Plane() const;

private:
    Plane m_plane;

};

} // End namespace udg

#endif
