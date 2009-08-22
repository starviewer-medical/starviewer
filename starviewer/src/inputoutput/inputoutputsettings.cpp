#include "inputoutputsettings.h"
#include "settingsregistry.h"

#include "starviewerapplication.h" // pel UserDataRootPath
#include <QHostInfo> //pel "localhostName"
#include <QDesktopServices>

namespace udg {

// Definició de les claus
const QString CacheBase("PACS/cache/");
const QString InputOutputSettings::DatabaseAbsoluteFilePath( CacheBase + "sdatabasePath");
const QString InputOutputSettings::CachePath( CacheBase + "imagePath");
const QString InputOutputSettings::DeleteLeastRecentlyUsedStudiesInDaysCriteria( CacheBase + "deleteOldStudiesHasNotViewedInDays");
const QString InputOutputSettings::DeleteLeastRecentlyUsedStudiesNoFreeSpaceCriteria( CacheBase + "deleteOldStudiesIfNotEnoughSpaceAvailable");
const QString InputOutputSettings::MinimumDaysUnusedToDeleteStudy( CacheBase + "MaximumDaysNotViewedStudy");
const QString InputOutputSettings::MinimumFreeGigaBytesForCache( CacheBase + "minimumSpaceRequiredToRetrieveInGbytes");
const QString InputOutputSettings::MinimumGigaBytesToFreeIfCacheIsFull( CacheBase + "GbytesOfOldStudiesToDeleteIfNotEnoughSapaceAvailable");

const QString InputOutputSettings::RetrievingStudy("/PACS/RetrievingStudy");

const QString RisBase("PACS/risRequests/");
const QString InputOutputSettings::ListenToRISRequests( RisBase + "listen");
const QString InputOutputSettings::RisRequestsPort( RisBase + "listenPort");
const QString InputOutputSettings::RisRequestViewOnceRetrieved( RisBase + "viewAutomaticallyAStudyRetrievedFromRisRequest");

const QString InterfaceBase("PACS/interface/");
const QString InputOutputSettings::LastOpenedDICOMDIRPath( InterfaceBase + "lastOpenedDICOMDIRPath");  
const QString InputOutputSettings::DicomdirStudyListColumnsWidth( InterfaceBase + "studyDicomdirList/");
const QString InputOutputSettings::CreateDicomdirStudyListColumnsWidth( InterfaceBase + "qCreateDicomdir/");
const QString InputOutputSettings::OperationStateColumnsWidth( InterfaceBase + "qOperationState/");
const QString InputOutputSettings::PacsStudyListColumnsWidth( InterfaceBase + "studyPacsList/");
const QString InputOutputSettings::ConfigurationScreenPACSList( InterfaceBase + "qConfigurationPacsDevice/");
const QString InputOutputSettings::LocalDatabaseStudyList( InterfaceBase + "studyCacheList/");
const QString InputOutputSettings::LocalDatabaseSplitterState( InterfaceBase + "queryscreen/StudyTreeSeriesListQSplitterState");
const QString InputOutputSettings::QueryScreenGeometry( InterfaceBase + "queryscreen/geometry");

const QString PacsParamBase("PACS/pacsparam/");
const QString InputOutputSettings::QueryRetrieveLocalPort( PacsParamBase + "localPort");
const QString InputOutputSettings::LocalAETitle( PacsParamBase + "AETitle");
const QString InputOutputSettings::PacsConnectionTimeout( PacsParamBase + "timeout");
const QString InputOutputSettings::MaximumPACSConnections( PacsParamBase + "MaxConnects");

const QString DICOMDIRBaseKey("DICOMDIR/");
const QString InputOutputSettings::DICOMDIRBurningApplicationPathKey( DICOMDIRBaseKey + "DICOMDIRBurningApplication");
const QString InputOutputSettings::DICOMDIRBurningApplicationParametersKey( DICOMDIRBaseKey + "DICOMDIRBurningApplicationParameters");
const QString InputOutputSettings::DICOMDIRBurningApplicationCDParametersKey( DICOMDIRBaseKey + "DICOMDIRBurningApplicationCDParameters" );
const QString InputOutputSettings::DICOMDIRBurningApplicationDVDParametersKey( DICOMDIRBaseKey + "DICOMDIRBurningApplicationDVDParameters" );
const QString InputOutputSettings::DICOMDIRBurningApplicationHasDifferentCDDVDParametersKey( DICOMDIRBaseKey + "DICOMDIRBurningApplicationHasDifferentCDDVDParameters" );
const QString InputOutputSettings::ConvertDICOMDIRImagesToLittleEndianKey( DICOMDIRBaseKey + "ConvertDICOMDIRImagesToLittleEndian" );

InputOutputSettings::InputOutputSettings()
{
}

InputOutputSettings::~InputOutputSettings()
{
}

void InputOutputSettings::init()
{
    SettingsRegistry *settingsRegistry = SettingsRegistry::instance();
    
    settingsRegistry->addSetting( DatabaseAbsoluteFilePath, UserDataRootPath + "pacs/database/dicom.sdb", Settings::Parseable );
    settingsRegistry->addSetting( CachePath, UserDataRootPath + "pacs/dicom/", Settings::Parseable );

    settingsRegistry->addSetting( DeleteLeastRecentlyUsedStudiesInDaysCriteria, true );
    settingsRegistry->addSetting( DeleteLeastRecentlyUsedStudiesNoFreeSpaceCriteria, true );
    settingsRegistry->addSetting( MinimumDaysUnusedToDeleteStudy, 7 );
    settingsRegistry->addSetting( MinimumFreeGigaBytesForCache, 5 );
    settingsRegistry->addSetting( MinimumGigaBytesToFreeIfCacheIsFull, 2 );

    settingsRegistry->addSetting( ListenToRISRequests, true );
    settingsRegistry->addSetting( RisRequestsPort, 11110 );
    settingsRegistry->addSetting( RisRequestViewOnceRetrieved, true );

    settingsRegistry->addSetting( LastOpenedDICOMDIRPath, QDir::homePath() );

    settingsRegistry->addSetting( QueryRetrieveLocalPort, 4006 );
    // TODO podríem definir-lo directament amb %HOSTNAME%
    settingsRegistry->addSetting( LocalAETitle, QHostInfo::localHostName(), Settings::Parseable );
    settingsRegistry->addSetting( PacsConnectionTimeout, 20 );
    settingsRegistry->addSetting( MaximumPACSConnections, 3 );

    settingsRegistry->addSetting( ConvertDICOMDIRImagesToLittleEndianKey, false );
#if defined(Q_OS_WIN) //WINDOWS
    settingsRegistry->addSetting( DICOMDIRBurningApplicationPathKey, QString::fromLocal8Bit(qgetenv("ProgramFiles")) + "\\ImgBurn\\ImgBurn.exe" );
    settingsRegistry->addSetting( DICOMDIRBurningApplicationParametersKey, "/MODE write /SRC %1 /EJECT YES /VERIFY NO /CLOSESUCCESS /START" );
    settingsRegistry->addSetting( DICOMDIRBurningApplicationCDParametersKey, "" );
    settingsRegistry->addSetting( DICOMDIRBurningApplicationDVDParametersKey, "" );
    settingsRegistry->addSetting( DICOMDIRBurningApplicationHasDifferentCDDVDParametersKey, false );
#elif defined(Q_OS_MAC) //MAC
    settingsRegistry->addSetting( DICOMDIRBurningApplicationPathKey, QDesktopServices::storageLocation(QDesktopServices::ApplicationsLocation) + "/Burn.app/Contents/MacOS/Burn" );
    settingsRegistry->addSetting( DICOMDIRBurningApplicationParametersKey, "%1" );
    settingsRegistry->addSetting( DICOMDIRBurningApplicationCDParametersKey, "" );
    settingsRegistry->addSetting( DICOMDIRBurningApplicationDVDParametersKey, "" );
    settingsRegistry->addSetting( DICOMDIRBurningApplicationHasDifferentCDDVDParametersKey, false );
#else // UNIX
    settingsRegistry->addSetting( DICOMDIRBurningApplicationPathKey, "/usr/bin/k3b" );
    settingsRegistry->addSetting( DICOMDIRBurningApplicationParametersKey, "--nosplash %1" );
    settingsRegistry->addSetting( DICOMDIRBurningApplicationCDParametersKey, "--nosplash --cdimage %1" );
    settingsRegistry->addSetting( DICOMDIRBurningApplicationDVDParametersKey, "--nosplash --dvdimage %1" );
    settingsRegistry->addSetting( DICOMDIRBurningApplicationHasDifferentCDDVDParametersKey, true );
#endif
}

} // end namespace udg 

