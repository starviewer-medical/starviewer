/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGREFERENCEDLINESFILLERSTEP_H
#define UDGREFERENCEDLINESFILLERSTEP_H

#include "patientfillerstep.h"
#include <QCache>

namespace udg {

    class Patient;
    class Series;
    class Study;
    class Image;

/**
    Mòdul que s'encarrega d'omplir la informació general d'objectes DICOM que són imatges. Un dels seus requisits és que es tingui l'etiqueta de DICOMClassified i que la Series a tractar sigui d'imatges

    @author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
 */
class ReferenceLinesFillerStep : public PatientFillerStep
{
    public:
        ReferenceLinesFillerStep();

        ~ReferenceLinesFillerStep();

        bool fill();

        QString name() {  return "ReferenceLinesFillerStep";  }

    private:
        /// Mètodes per processar la informació específica de pacient,series i imatge
        void processSeries( Series *series );
        void processImage( Image *image );
        /// Metode per buscar una imatge relacionada dins del study en qüestió.
        /// Retorna NUL si la imatge no s'ha trobat
        Image *findImageIn( Study *study , QString SOPInstanceUID );

        /// Caché d'imatges mapejades pel SOPInstanceUID. Es neteja a cada estudi.
        QCache<QString, Image> m_cacheImageSet;
};

}

#endif
