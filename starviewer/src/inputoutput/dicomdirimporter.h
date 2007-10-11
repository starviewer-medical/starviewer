/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGDICOMDIRIMPORTER_H
#define UDGDICOMDIRIMPORTER_H

#include "dicomdirreader.h"

namespace udg {

class Status;
class DICOMImage;

/** Aquesta classe permet importar un dicomdir a la nostra base de ades
    @author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class DICOMDIRImporter{

public:
    Status import( QString dicomdirPath , QString studyUID , QString seriesUID , QString imageUID );

private :
    DICOMDIRReader m_readDicomdir;

    Status importStudy( QString studyUID , QString seriesUID , QString sopInstanceUID );

    Status importSeries( QString studyUID , QString seriesUID , QString sopInstanceUID );

    Status importImage(DICOMImage image);
};

}
#endif
