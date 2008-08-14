/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGVOLUMEGENERATORSTEP_H
#define UDGVOLUMEGENERATORSTEP_H

#include "patientfillerstep.h"

namespace udg {

class Patient;
class Series;

/**
Step que s'encarrega de generar els volums de cada Serie d'imatges i registrar-los al repositori

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class VolumeGeneratorStep : public PatientFillerStep
{
public:
    VolumeGeneratorStep();

    ~VolumeGeneratorStep();

    bool fill();

    bool fillIndividually(){return false;};

    void postProcessing(){};

    QString name() {  return "VolumeGeneratorStep";  }

private:
    /// Mètode per processar la informació específica de la serie
    void processSeries( Series *series );

};

}

#endif
