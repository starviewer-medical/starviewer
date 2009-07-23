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

    /// Paràmetres del RIS
    static const QString RisRequestsPort;
    static const QString ListenToRISRequests;
    static const QString RisRequestViewOnceRetrieved;

    /// Últim path des del que hem obert un dicomdir
    static const QString LastOpenedDICOMDIRPath;
    /// Geometria de les columnes del llistat d'estudis d'un DICOMDIR/llista d'operacions
    static const QString DicomdirStudyListColumnsWidth;
    static const QString CreateDicomdirStudyListColumnsWidth;
    static const QString OperationStateColumnsWidth;
    static const QString PacsStudyListColumnsWidth;

    /// Paràmetres de connexió amb un PACS
    static const QString LocalAETitle;
    static const QString QueryRetrieveLocalPort;
    static const QString PacsConnectionTimeout;
    static const QString MaximumPACSConnections;
};

} // end namespace udg 

#endif
