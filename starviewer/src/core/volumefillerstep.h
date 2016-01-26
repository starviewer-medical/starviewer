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

#ifndef UDG_VOLUMEFILLERSTEP_H
#define UDG_VOLUMEFILLERSTEP_H

#include "patientfillerstep.h"

namespace udg {

class DICOMTagReader;

/**
 * @brief The VolumeFillerStep class has the responsibility of assigning images to volumes.
 *
 * Additionally, it creates the volume thumbnails.
 */
class VolumeFillerStep : public PatientFillerStep
{
public:
    VolumeFillerStep();

    virtual bool fillIndividually() override;

    virtual QString name() override;

private:
    void processDICOMFile(const DICOMTagReader *dicomReader);
    void processEnhancedDICOMFile(const DICOMTagReader *dicomReader);

    /// Donat un dicomReader guardem a la cache el corresponent thumbnail.
    /// La intenció d'aquest mètode és estalviar temps en la càrrega de thumbnails per arxius
    /// multiframe i enhanced ja que actualment és molt costós perquè hem de carregar tot el volum
    /// a memòria i aquí podem aprofitar que el dataset està a memòria evitant la càrrega posterior
    /// Tot i així es pot fer servir en altres casos que es cregui necessari avançar la creació del thumbnail
    void saveThumbnail(const DICOMTagReader *dicomReader);

};

} // namespace udg

#endif // UDG_VOLUMEFILLERSTEP_H
