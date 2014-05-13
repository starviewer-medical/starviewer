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
    Classe que encapsularà les orientacions anatòmiques del pacient
  */
class AnatomicalPlane {
public:
    /// Tipus d'orientacions anatòmiques
    enum AnatomicalPlaneType { Axial, Sagittal, Coronal, Oblique, NotAvailable };

    /// Donada una orientació anatòmica, ens retorna la corresponent etiqueta per mostrar a l'interfície
    static const QString getLabel(AnatomicalPlaneType orientation);

    /// Ens retorna l'etiqueta/tipus del pla anatòmic que es correspon amb el PatientOrientation donat
    static const QString getLabelFromPatientOrientation(const PatientOrientation &orientation);
    static const AnatomicalPlaneType getPlaneTypeFromPatientOrientation(const PatientOrientation &orientation);

    /// Returns the default patient orientation for an specific anatomical plane
    static const PatientOrientation getDefaultRadiologicalOrienation(AnatomicalPlaneType plane);
};

} // End namespace udg

#endif
