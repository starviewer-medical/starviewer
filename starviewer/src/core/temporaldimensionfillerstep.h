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

#ifndef UDGTEMPORALDIMENSIONFILLERSTEP_H
#define UDGTEMPORALDIMENSIONFILLERSTEP_H

#include "patientfillerstep.h"
#include <QHash>
#include <QString>

namespace udg {

class Patient;
class Series;
class Image;

/**
    Mòdul que s'encarrega d'identificar les sèries dinàmiques. Un dels seus requisits és que es tingui l'etiqueta de DICOMClassified i la ImageFillerStep
  */
class TemporalDimensionFillerStep : public PatientFillerStep {
public:
    TemporalDimensionFillerStep();

    ~TemporalDimensionFillerStep();

    bool fillIndividually();

    void postProcessing();

protected:
    struct VolumeInfo
    {
        int numberOfPhases;
        int numberOfImages;
        bool isCTLocalizer;
        QString firstImagePosition;
        QString firstAcquisitionNumber;
        bool multipleAcquisitionNumber;
        /// Hash per comptar les fases corresponents a cada posició
        /// La clau del hash és un string amb la posició de la imatge (ImagePositionPatient) i el valor associat compta les ocurrències (fases) d'aquesta posició.
        /// Si tenim igual nombre de fases a totes les posicions, podem dir que és un volum amb fases.
        QHash<QString, int> phasesPerPositionHash;
    };

    /// Estructura que s'utiliza en cas que es processi l'step individiualment per fitxers.
    ///  <Sèrie,             <VolumeNumber, VolumeInfo>
    QHash<Series*, QHash<int, VolumeInfo*>*> TemporalDimensionInternalInfo;
};

}

#endif
