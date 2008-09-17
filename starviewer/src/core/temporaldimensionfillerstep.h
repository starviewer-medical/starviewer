/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
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

@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class TemporalDimensionFillerStep : public PatientFillerStep
{
public:
    TemporalDimensionFillerStep();

    ~TemporalDimensionFillerStep();

    bool fill();

    bool fillIndividually();

    void postProcessing();

    void abort();

    QString name() {  return "TemporalDimensionFillerStep";  }

private:

    struct SeriesInfo
    {
        int numberOfPhases;
        int numberOfImages;
        bool isCTLocalizer;
        QString firstImagePosition;
    };

    /// Mètodes per processar la informació específica de series
    void processSeries( Series *series );

    /// Estructura que s'utiliza en cas que es processi l'step individiualment per fitxers.
    QHash< Series *, SeriesInfo *> TemporalDimensionInternalInfo;
};

}

#endif
