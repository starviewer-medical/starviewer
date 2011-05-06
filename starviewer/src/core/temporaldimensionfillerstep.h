#ifndef UDGTEMPORALDIMENSIONFILLERSTEP_H
#define UDGTEMPORALDIMENSIONFILLERSTEP_H

#include "patientfillerstep.h"
#include <QHash>
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

    QString name() 
    {
        return "TemporalDimensionFillerStep";
    }

private:

    struct VolumeInfo
    {
        int numberOfPhases;
        int numberOfImages;
        bool isCTLocalizer;
        QString firstImagePosition;
        QString firstAcquisitionNumber;
        bool multipleAcquisitionNumber;
    };

    /// Estructura que s'utiliza en cas que es processi l'step individiualment per fitxers.
    QHash<Series *, QHash<int, VolumeInfo*>*> TemporalDimensionInternalInfo;
};

}

#endif
