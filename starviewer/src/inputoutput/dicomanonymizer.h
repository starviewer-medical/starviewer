/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGDICOMANONYMIZER_H
#define UDGDICOMANONYMIZER_H

//TODO: Explicar funcionament classe

//TODO:Intentar treure
#include <gdcmGlobal.h>

#include "gdcmanonymizerstarviewer.h"

class QString;

namespace udg {

class DICOMAnonymizer
{

public:

    DICOMAnonymizer();
    ~DICOMAnonymizer();

    ///Ens anonimitza els fitxers d'un Directori
    bool anonymyzeDICOMFilesDirectory(QString directoryPath);

    ///Ens anonimitza un fitxer DICOM
    bool anonymizeDICOMFile(QString inputPathFile, QString outputPathFile);

    ///En comptes d'eliminar el valor del StudyID tal com indica el Basic Profile el substitueix per un valor arbitrari
    ///Aquesta opció està pensada pel DICOMDIR en que és obligatori que els estudis tinguin PatientID
    void setReplacePatientIDInsteadOfRemove(bool replace);
    bool getReplacePatientIDInsteadOfRemove();

    ///En comptes d'eliminar el valor del StudyID tal com indica el Basic Profile el substitueix per un valor arbitrari
    ///Aquesta opció està pensada pel DICOMDIR en que és obligatori que els estudis tinguin StudyID
    void setReplaceStudyIDInsteadOfRemove(bool replace);
    bool getReplaceStudyIDInsteadOfRemove();
    
    ///Indica si s'han de treure els tags privats de les imatges, per defecte no els treiem
    void setRemovePrivateTags(bool removePritaveTags);
    bool getRemovePrivateTags();


private:

    ///Inicialitza les variables de gdcm necessàries per anonimitzar
    bool initializeGDCM();

private:

    bool m_replacePatientIDInsteadOfRemove;
    bool m_replaceStudyIDInsteadOfRemove;
    bool m_removePritaveTags;

    gdcm::Global *gdcmGlobalInstance;
    gdcm::gdcmAnonymizerStarviewer *gdcmAnonymizer;
};

};

#endif
