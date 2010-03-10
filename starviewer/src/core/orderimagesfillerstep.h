/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
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
Mòdul que s'encarrega d'ordenar correctament les imatges de les sèries. Un dels seus requisits és que es tingui l'etiqueta de DICOMClassified, la ImageFillerStep i el TemporalDimensionFillerStep.

@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class OrderImagesFillerStep : public PatientFillerStep
{
public:
    OrderImagesFillerStep();

    ~OrderImagesFillerStep();

    bool fill();

    bool fillIndividually();

    void postProcessing();

    QString name() {  return "OrderImagesFillerStep";  }

private:
    /// Mètodes per processar la informació específica de series
    void processSeries( Series *series );
    void processImage( Image *image );

    /// Mètode per calcular la distància a on es troba la llesca
    double distance( Image *image );

    /// Mètode que transforma l'estructura d'imatges ordenades a una llista i l'insereix a la sèrie.
    void setOrderedImagesIntoSeries( Series *series );

    /// Estructura per guardar les imatges ordenades
    //  NormalVector    Distance    InstanceNumber0FrameNumber
    QMap< QString, QMap< double , QMap< unsigned long  , Image* >* >* > *m_orderedImageSet;

    //     Series        Volume     NormalVector    Distance  InstanceNumber0FrameNumber
    QHash< Series *, QMap< int, QMap< QString, QMap< double , QMap< unsigned long , Image* >* >* >* >* > OrderImagesInternalInfo;
};

}

#endif
