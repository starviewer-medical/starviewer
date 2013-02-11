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
