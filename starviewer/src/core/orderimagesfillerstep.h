#ifndef UDGORDERIMAGESFILLERSTEP_H
#define UDGORDERIMAGESFILLERSTEP_H

#include "patientfillerstep.h"
#include <QMap>
#include <QHash>
#include <QString>

namespace udg {

class Patient;
class Series;
class Image;

/**
    Mòdul que s'encarrega d'ordenar correctament les imatges de les sèries. Un dels seus requisits és que es tingui l'etiqueta de DICOMClassified,
    la ImageFillerStep i el TemporalDimensionFillerStep.
  */
class OrderImagesFillerStep : public PatientFillerStep {
public:
    OrderImagesFillerStep();

    ~OrderImagesFillerStep();

    bool fillIndividually();

    void postProcessing();

    QString name()
    {
        return "OrderImagesFillerStep";
    }

private:
    /// Mètodes per processar la informació específica de series
    void processImage(Image *image);

    /// Mètode que transforma l'estructura d'imatges ordenades a una llista i l'insereix a la sèrie.
    void setOrderedImagesIntoSeries(Series *series);

    /// Estructura per guardar les imatges ordenades
    //  NormalVector    Distance    InstanceNumberOFrameNumber
    QMap<QString, QMap<double, QMap<unsigned long, Image*>*>*> *m_orderedImageSet;

    //    Series        Volume     NormalVector    Distance  InstanceNumberOFrameNumber
    QHash<Series*, QMap<int, QMap<QString, QMap<double, QMap<unsigned long, Image*>*>*>*>*> m_orderImagesInternalInfo;

    //    Series       Volume     AcqNumber MultipleAcqNumbers?
    QHash<Series*, QHash<int, QPair<QString, bool>*> > m_acquisitionNumberEvaluation;
};

}

#endif
