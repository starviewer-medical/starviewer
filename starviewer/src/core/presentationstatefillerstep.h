#ifndef UDGPRESENTATIONSTATEFILLERSTEP_H
#define UDGPRESENTATIONSTATEFILLERSTEP_H

#include "patientfillerstep.h"

namespace udg {

class Patient;
class Series;

/**
Mòdul que s'encarrega d'omplir la informació d'objectes PresentationState. Un dels seus prerequisits serà que s'hagi superat el mòdul DICOMFileClassifierFillerStep. Les Series a processar han de de ser de modalitat PR.
*/
class PresentationStateFillerStep : public PatientFillerStep
{
public:
    PresentationStateFillerStep();

    ~PresentationStateFillerStep();

    bool fillIndividually(){return false;};

    void postProcessing(){};

    QString name() { return "PresentationStateFillerStep"; }

private:
    // TODO mètode per implementar
    void processPresentationState();

};

}

#endif
