/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGDICOMANONYMIZER_H
#define UDGDICOMANONYMIZER_H

/**
  * Aquesta classe permet anonimitzar fitxers DICOM seguint les normes descrites pel Basic Application Level Confidentiality Profile de DICOM
  * que podem trobar al annex E del PS 3.15, mantenint la consistència en tags com Frame Of Reference o Image Of Reference, Study Instance UID, 
  * Series Instance UID, ... després de ser anonimitzats. Per defecte també treu els tags privats de les imatges ja que aquests poden contenir
  * informació sensible del pacient, ens aconsellen que els treiem a http://groups.google.com/group/comp.protocols.dicom/browse_thread/thread/fb89f7f5d120db44
  *
  * Ens permet anonimitzar fitxers sols o tots els fitxers dins i subdirectoris del directori especificat.
  */

#include <QHash>

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
    ///Atenció!!! si utilitzem aquesta opció per anonimitzar diversos fitxers d'un mateix estudi, aquests fitxers s'han d'anonimitzar utilitzant la mateixa
    ///instància del DICOMAnonymizer per mantenir la consitència de Tags com Study Instance UID, Series Instance UID, Frame Of Reference, Image Reference ...
    ///Si no es respecta aquest requisit passarà que imatges d'un mateix estudi després de ser anonimitzades tindran Study Instance UID diferents.
    bool anonymizeDICOMFile(QString inputPathFile, QString outputPathFile);

    ///En comptes d'eliminar el valor del StudyID tal com indica el Basic Profile el substitueix per un valor arbitrari
    ///Aquesta opció està pensada pel DICOMDIR en que és obligatori que els estudis tinguin PatientID, tots els estudis a anonimitzar que tinguin en comú 
    ///mateix el Patient ID abans de ser anonimitzats, després de ser-ho tindran un nou Patient ID en comú, d'aquesta manera es podrà veure que aquells estudis
    ///són del mateix pacient.
    void setReplacePatientIDInsteadOfRemove(bool replace);
    bool getReplacePatientIDInsteadOfRemove();

    ///En comptes d'eliminar el valor del StudyID tal com indica el Basic Profile el substitueix per un valor arbitrari
    ///Aquesta opció està pensada pel DICOMDIR en que és obligatori que els estudis tinguin StudyID
    void setReplaceStudyIDInsteadOfRemove(bool replace);
    bool getReplaceStudyIDInsteadOfRemove();
    
    ///Indica si s'han de treure els tags privats de les imatges. Per defecte es treuen si no s'indica el contrari, ja que els tags privats poden
    ///contenir informació sensible del pacient que segons el Basic Application Level Confidentiality Profile de DICOM s'ha d'anonimitzar, com podria
    ///ser el nom del pacient, edat, ....
    void setRemovePrivateTags(bool removePritaveTags);
    bool getRemovePrivateTags();

private:

    ///Inicialitza les variables de gdcm necessàries per anonimitzar
    void initializeGDCM();

    ///Retorna el valor de PatientID anonimitzat a partir del PatientID original del fitxer. Aquest mètode és consistent de manera que si li passem 
    ///una o més vegades el mateix PatientID sempre retornarà el mateix valor com a PatientID anonimitzat.
    QString getAnonimyzedPatientID(QString originalPatientID);

    ///Retorna el valor de StudyID anonimitzat a partir del Study Instance UID original del fitxer. Aquest mètode és consistent de manera que si li passem 
    ///una o més vegades el mateix study Instance UID sempre retornarà el mateix valor com de Study ID anonimitzat.
    QString getAnonymizedStudyID(QString originalStudyInstanceUID);

    ///Retorna el valor d'un Tag en un string, si no troba el tag retorna un string buit
    QString readTagValue(gdcm::File *gdcmFile, gdcm::Tag);

private:

    bool m_replacePatientIDInsteadOfRemove;
    bool m_replaceStudyIDInsteadOfRemove;
    bool m_removePritaveTags;

    QHash<QString, QString> hashOriginalPatientIDToAnonimyzedPatientID;
    QHash<QString, QString> hashOriginalStudyInstanceUIDToAnonimyzedStudyID;

    gdcm::gdcmAnonymizerStarviewer *gdcmAnonymizer;
};

};

#endif
