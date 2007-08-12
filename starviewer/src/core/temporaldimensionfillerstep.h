/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGTEMPORALDIMENSIONFILLERSTEP_H
#define UDGTEMPORALDIMENSIONFILLERSTEP_H

#include "patientfillerstep.h"

namespace udg {

class Patient;
class Series;
class Image;

/**
Mòdul que s'encarrega d'identificar les sèries dinàmiques. Un dels seus requisits és que es tingui l'etiqueta de DICOMClassified i la ImageFillerStep

@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class TemporalDimensionFillerStep : public PatientFillerStep
{
public:
    TemporalDimensionFillerStep();

    ~TemporalDimensionFillerStep();

    bool fill();

    QString name() {  return "TemporalDimensionFillerStep";  }

private:
/// Mètodes per processar la informació específica de pacient i series
    void processPatient( Patient *patient );
    void processSeries( Series *series );
};

}

#endif
