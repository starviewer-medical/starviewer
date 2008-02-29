/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGPHILIPSTAGSFILLERSTEP_H
#define UDGPHILIPSTAGSFILLERSTEP_H

#include "patientfillerstep.h"

namespace udg {

class Patient;
class Series;
class Image;

/**
Mòdul que s'encarrega d'emplanar els camps d'imatge que pertànyen a philips 

@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class PhilipsTagsFillerStep : public PatientFillerStep
{
public:
    PhilipsTagsFillerStep();

    ~PhilipsTagsFillerStep();

    bool fill();

    QString name() {  return "PhilipsTagsFillerStep";  }

private:
    /// Mètodes per processar la informació específica de series
    void processSeries( Series *series );

    ///Métode per extreure els tags philips de les imatges dicom
    void processImage( Image *image );
};

}

#endif
