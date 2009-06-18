#include "inputoutputsettings.h"
#include "settingsregistry.h"

#include "starviewerapplication.h" // pel UserDataRootPath
#include <QHostInfo> //pel "localhostName"

namespace udg {

// Definició de les claus
const QString cacheBaseKey("PACS/cache/");
const QString InputOutputSettings::databaseAbsoluteFilePathKey( cacheBaseKey + "sdatabasePath");
const QString InputOutputSettings::cachePathKey( cacheBaseKey + "PACS/cache/imagePath");
const QString InputOutputSettings::deleteLeastRecentlyUsedStudiesInDaysCriteriaKey( cacheBaseKey + "deleteOldStudiesHasNotViewedInDays");
const QString InputOutputSettings::deleteLeastRecentlyUsedStudiesNoFreeSpaceCriteriaKey( cacheBaseKey + "deleteOldStudiesIfNotEnoughSpaceAvailable");
const QString InputOutputSettings::minimumDaysUnusedToDeleteStudyKey( cacheBaseKey + "MaximumDaysNotViewedStudy");
const QString InputOutputSettings::minimumFreeGigaBytesForCacheKey( cacheBaseKey + "minimumSpaceRequiredToRetrieveInGbytes");
const QString InputOutputSettings::minimumGigaBytesToFreeIfCacheIsFullKey( cacheBaseKey + "GbytesOfOldStudiesToDeleteIfNotEnoughSapaceAvailable");

const QString risBaseKey("PACS/risRequests/");
const QString InputOutputSettings::listenToRISRequestsKey( risBaseKey + "listen");
const QString InputOutputSettings::risRequestsPortKey( risBaseKey + "listenPort");
const QString InputOutputSettings::risRequestViewOnceRetrievedKey( risBaseKey + "viewAutomaticallyAStudyRetrievedFromRisRequest");

const QString interfaceBaseKey("PACS/interface/");
const QString InputOutputSettings::lastOpenedDICOMDIRPathKey( interfaceBaseKey + "lastOpenedDICOMDIRPath");  
const QString InputOutputSettings::dicomdirStudyListColumnsWidthKey( interfaceBaseKey + "studyDicomdirList/");
const QString InputOutputSettings::createDicomdirStudyListColumnsWidthKey( interfaceBaseKey + "qCreateDicomdir/");
const QString InputOutputSettings::operationStateColumnsWidthKey( interfaceBaseKey + "qOperationState/");
const QString InputOutputSettings::pacsStudyListColumnsWidthKey( interfaceBaseKey + "studyPacsList/");

const QString pacsParamBaseKey("PACS/pacsparam/");
const QString InputOutputSettings::queryRetrieveLocalPortKey( pacsParamBaseKey + "localPort");
const QString InputOutputSettings::localAETitleKey( pacsParamBaseKey + "AETitle");
const QString InputOutputSettings::pacsConnectionTimeoutKey( pacsParamBaseKey + "timeout");
const QString InputOutputSettings::maximumPACSConnectionsKey( pacsParamBaseKey + "MaxConnects");

InputOutputSettings::InputOutputSettings()
{
}

InputOutputSettings::~InputOutputSettings()
{
}

void InputOutputSettings::init()
{
    SettingsRegistry *settingsRegistry = SettingsRegistry::instance();
    
    settingsRegistry->addSetting( databaseAbsoluteFilePathKey, UserDataRootPath + "pacs/database/dicom.sdb" );
    settingsRegistry->addSetting( cachePathKey, UserDataRootPath + "pacs/dicom/" );

    settingsRegistry->addSetting( deleteLeastRecentlyUsedStudiesInDaysCriteriaKey, true );
    settingsRegistry->addSetting( deleteLeastRecentlyUsedStudiesNoFreeSpaceCriteriaKey, true );
    settingsRegistry->addSetting( minimumDaysUnusedToDeleteStudyKey, 7 );
    settingsRegistry->addSetting( minimumFreeGigaBytesForCacheKey, 5 );
    settingsRegistry->addSetting( minimumGigaBytesToFreeIfCacheIsFullKey, 2 );

    settingsRegistry->addSetting( listenToRISRequestsKey, true );
    settingsRegistry->addSetting( risRequestsPortKey, 11110 );
    settingsRegistry->addSetting( risRequestViewOnceRetrievedKey, true );

    settingsRegistry->addSetting( lastOpenedDICOMDIRPathKey, QDir::homePath() );

    settingsRegistry->addSetting( queryRetrieveLocalPortKey, 4006 );
    settingsRegistry->addSetting( localAETitleKey, QHostInfo::localHostName() );
    settingsRegistry->addSetting( pacsConnectionTimeoutKey, 20 );
    settingsRegistry->addSetting( maximumPACSConnectionsKey, 3 );
}

} // end namespace udg 

