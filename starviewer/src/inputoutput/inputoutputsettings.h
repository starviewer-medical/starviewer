#ifndef UDGINPUTOUTPUTSETTINGS_H
#define UDGINPUTOUTPUTSETTINGS_H

#include "defaultsettings.h"

namespace udg {

class InputOutputSettings : public DefaultSettings
{
public:
    InputOutputSettings();
    ~InputOutputSettings();

    void init();

    /// Declaració de claus
    /// Path absolut de l'arxiu de base dades
    static const QString DatabaseAbsoluteFilePath;
    /// Path del directori de la cache
    static const QString CachePath;
    /// Polítiques d'autogestió de cache
    static const QString DeleteLeastRecentlyUsedStudiesInDaysCriteria;
    static const QString DeleteLeastRecentlyUsedStudiesNoFreeSpaceCriteria;
    static const QString MinimumGigaBytesToFreeIfCacheIsFull;
    static const QString MinimumFreeGigaBytesForCache;
    static const QString MinimumDaysUnusedToDeleteStudy;
    /// Controlar quin estudi està baixant-se
    static const QString RetrievingStudy;

    /// Conjunt de PACS que tenim seleccionats per defecte per consultar. 
    /// La llista consistirà en un string on cada PACS s'identificarà pel seu camp clau, els quals estaran delimitats per "//"
    static const QString DefaultPACSListToQuery;

    /// Paràmetres del RIS
    static const QString RisRequestsPort;
    static const QString ListenToRISRequests;
    static const QString RisRequestViewOnceRetrieved;

    /// Últim path des del que hem obert un dicomdir
    static const QString LastOpenedDICOMDIRPath;
    /// Geometria de les columnes del llistat d'estudis d'un DICOMDIR/llista d'operacions/PACS
    static const QString DicomdirStudyListColumnsWidth;
    static const QString CreateDicomdirStudyListColumnsWidth;
    static const QString OperationStateColumnsWidth;
    static const QString PacsStudyListColumnsWidth;
    static const QString ConfigurationScreenPACSList;
    static const QString LocalDatabaseStudyList;
    static const QString LocalDatabaseSplitterState;
    static const QString QueryScreenGeometry;

    /// Paràmetres de connexió amb un PACS
    static const QString LocalAETitle;
    static const QString QueryRetrieveLocalPort;
    static const QString PacsConnectionTimeout;
    static const QString MaximumPACSConnections;

    /// Llista de PACS
    static const QString PacsListConfigurationSectionName;

    /// Paràmetres de la configuració DICOMDIR
    /// Path del programa de gravació
    static const QString DICOMDIRBurningApplicationPathKey;
    /// Parameters del programa de gravació
    static const QString DICOMDIRBurningApplicationParametersKey;
    /// Paràmetres de gravació d'un CD quan són diferents amb els de DVD
    static const QString DICOMDIRBurningApplicationCDParametersKey;
    /// Paràmetres de gravació d'un DVD quan són diferents amb els de CD
    static const QString DICOMDIRBurningApplicationDVDParametersKey;
    /// CheckBox per saber si cal tenir o no en compte els paràmetres de gravació d'un CD o DVD
    static const QString DICOMDIRBurningApplicationHasDifferentCDDVDParametersKey;
    /// CheckBox per saber si cal convertir les imatges del DICOMDIR a little endian
    static const QString ConvertDICOMDIRImagesToLittleEndianKey;

};

} // end namespace udg 

#endif
