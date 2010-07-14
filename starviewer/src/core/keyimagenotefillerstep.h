/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGKEYIMAGENOTEFILLERSTEP_H
#define UDGKEYIMAGENOTEFILLERSTEP_H

#include "patientfillerstep.h"
#include "keyimagenote.h"

namespace udg {

class Patient;
class Series;
class DICOMTagReader;
class DICOMReferencedImage;

/**
Mòdul que s'encarrega d'omplir la informació d'objectes KIN. Un dels seus prerequisits serà que s'hagi superat el mòdul DICOMFileClassifierFillerStep. Les Series a processar han de de ser de modalitat KO.

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class KeyImageNoteFillerStep : public PatientFillerStep
{
public:
    KeyImageNoteFillerStep();

    ~KeyImageNoteFillerStep();

    bool fillIndividually();

    void postProcessing(){};

    QString name() {  return "KeyImageNoteFillerStep";  }

private:
    /// Mètodes per processar la informació específica d'un Key Image Note
    bool processKeyImageNote(KeyImageNote *keyImageNote);

    /// Mètodes per a completar la informació necessaria per a crear un Key Image Note seguint l'estructura de Structured Report

    /// Lectura del template TID 2010 Key Object Selection ( PS 3.16 )
    void readSRTemplateKIN(KeyImageNote *keyImageNote, DICOMTagReader *reader);
    /// Lectura del DCID 7010 Key Object Selection document title
    KeyImageNote::DocumentTitle readKeyObjectSelectionDocumentTitle(DICOMTagReader *reader);
    /// Lectura del DCID 7011 Rejected for quality reasons
    KeyImageNote::RejectedForQualityReasons readRejectedForQualityReasons(DICOMTagReader *reader);
    /// Lectura del valor del concept name Key Object Description
    QString readKeyObjectDescription(DICOMTagReader *reader);
    /// Lectura del Template TID 1002 Observer Context
    KeyImageNote::ObserverType readObserverContextType(DICOMTagReader *reader);
    // Lectura del Template TID 1003 o TID 1004 dependent de si es tipus persona o dispositiu
    QString readObserverContextName(DICOMTagReader *reader, KeyImageNote::ObserverType type); 
    /// Llegeix les imatges referenciades al KIN
    QList <DICOMReferencedImage*> readReferencedImagesInKIN(DICOMTagReader *reader); 
};

}

#endif
