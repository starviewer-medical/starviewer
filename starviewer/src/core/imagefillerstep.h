/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGIMAGEFILLERSTEP_H
#define UDGIMAGEFILLERSTEP_H

#include "patientfillerstep.h"

namespace udg {

class Patient;
class Series;
class Image;
class DICOMTagReader;

/**
Mòdul que s'encarrega d'omplir la informació general d'objectes DICOM que són imatges. Un dels seus requisits és que es tingui l'etiqueta de DICOMClassified i que la Series a tractar sigui d'imatges

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class ImageFillerStep : public PatientFillerStep
{
public:
    ImageFillerStep();

    ~ImageFillerStep();

    bool fill();

    bool fillIndividually();

    void postProcessing() {}

    QString name() {  return "ImageFillerStep";  }

private:
    /// Mètodes per processar la informació específica de pacient,series i imatge
    void processPatient( Patient *patient );
    void processSeries( Series *series );
    bool processImage( Image *image );
    bool processImage( Image *image , DICOMTagReader *dicomReader );

    /// Helper method per obtenir l'string corresponent a un direction cosines. Donat un vector de direcció determina la seva etiqueta d'orientació R,L,A,P,S,I
    QString mapDirectionCosinesToOrientationString( double vector[3] );
};

}

#endif
