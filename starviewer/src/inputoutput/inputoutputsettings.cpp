#include "inputoutputsettings.h"
#include "settingsregistry.h"

#include <QHostInfo> // Pel "localhostName"
#include <QDesktopServices>

#include "starviewerapplication.h" // Pel UserDataRootPath
#include "qstudytreewidget.h"
#include "qoperationstatescreen.h"

namespace udg {

// Definició de les claus
const QString CacheBase("PACS/cache/");
const QString InputOutputSettings::DatabaseAbsoluteFilePath(CacheBase + "sdatabasePath");
const QString InputOutputSettings::CachePath(CacheBase + "imagePath");
const QString InputOutputSettings::DeleteLeastRecentlyUsedStudiesInDaysCriteria(CacheBase + "deleteOldStudiesHasNotViewedInDays");
const QString InputOutputSettings::DeleteLeastRecentlyUsedStudiesNoFreeSpaceCriteria(CacheBase + "deleteOldStudiesIfNotEnoughSpaceAvailable");
const QString InputOutputSettings::MinimumDaysUnusedToDeleteStudy(CacheBase + "MaximumDaysNotViewedStudy");
const QString InputOutputSettings::MinimumFreeGigaBytesForCache(CacheBase + "minimumSpaceRequiredToRetrieveInGbytes");
const QString InputOutputSettings::MinimumGigaBytesToFreeIfCacheIsFull(CacheBase + "GbytesOfOldStudiesToDeleteIfNotEnoughSapaceAvailable");

const QString InputOutputSettings::RetrievingStudy("/PACS/RetrievingStudy");
const QString InputOutputSettings::DefaultPACSListToQuery("PACS/defaultPACSListToQuery");

const QString RISBase("PACS/risRequests/");
const QString InputOutputSettings::ListenToRISRequests(RISBase + "listen");
const QString InputOutputSettings::RISRequestsPort(RISBase + "listenPort");
const QString InputOutputSettings::RISRequestViewOnceRetrieved(RISBase + "viewAutomaticallyAStudyRetrievedFromRisRequest");

const QString InterfaceBase("PACS/interface/");
const QString InputOutputSettings::LastOpenedDICOMDIRPath(InterfaceBase + "lastOpenedDICOMDIRPath");  
const QString InputOutputSettings::DICOMDIRStudyListColumnsWidth(InterfaceBase + "studyDicomdirList/");
const QString InputOutputSettings::CreateDICOMDIRStudyListColumnsWidth(InterfaceBase + "qCreateDicomdir/");
const QString InputOutputSettings::OperationStateColumnsWidth(InterfaceBase + "qOperationState/");
const QString InputOutputSettings::PACSStudyListColumnsWidth(InterfaceBase + "studyPacsList/");
const QString InputOutputSettings::ConfigurationScreenPACSList(InterfaceBase + "qConfigurationPacsDevice/");
const QString InputOutputSettings::LocalDatabaseStudyList(InterfaceBase + "studyCacheList/");
const QString InputOutputSettings::LocalDatabaseSplitterState(InterfaceBase + "queryscreen/StudyTreeSeriesListQSplitterState");
const QString InputOutputSettings::QueryScreenGeometry(InterfaceBase + "queryscreen/geometry");
const QString InputOutputSettings::PACSStudyListSortByColumn(InterfaceBase + "studyPacsList/sortByColumn");
const QString InputOutputSettings::PACSStudyListSortOrder(InterfaceBase + "studyPacsList/sortOrder");
const QString InputOutputSettings::LocalDatabaseStudyListSortByColumn(InterfaceBase + "studyCacheList/sortByColumn");
const QString InputOutputSettings::LocalDatabaseStudyListSortOrder(InterfaceBase + "studyCacheList/sortOrder");
const QString InputOutputSettings::DICOMDIRStudyListSortByColumn(InterfaceBase + "studyDicomdirList/sortByColumn");
const QString InputOutputSettings::DICOMDIRStudyListSortOrder(InterfaceBase + "studyDicomdirList/sortOrder");
const QString InputOutputSettings::OperationStateListSortByColumn(InterfaceBase + "qOperationState/sortByColumn");
const QString InputOutputSettings::OperationStateListSortOrder(InterfaceBase + "qOperationState/sortOrder");

const QString PACSParametersBase("PACS/pacsparam/");
const QString InputOutputSettings::QueryRetrieveLocalPort(PACSParametersBase + "localPort");
const QString InputOutputSettings::LocalAETitle(PACSParametersBase + "AETitle");
const QString InputOutputSettings::PACSConnectionTimeout(PACSParametersBase + "timeout");
const QString InputOutputSettings::MaximumPACSConnections(PACSParametersBase + "MaxConnects");

const QString InputOutputSettings::PacsListConfigurationSectionName = "PacsList";

const QString DICOMDIRBaseKey("DICOMDIR/");
const QString InputOutputSettings::DICOMDIRBurningApplicationPathKey(DICOMDIRBaseKey + "DICOMDIRBurningApplication");
const QString InputOutputSettings::DICOMDIRBurningApplicationParametersKey(DICOMDIRBaseKey + "DICOMDIRBurningApplicationParameters");
const QString InputOutputSettings::DICOMDIRBurningApplicationCDParametersKey(DICOMDIRBaseKey + "DICOMDIRBurningApplicationCDParameters");
const QString InputOutputSettings::DICOMDIRBurningApplicationDVDParametersKey(DICOMDIRBaseKey + "DICOMDIRBurningApplicationDVDParameters");
const QString InputOutputSettings::DICOMDIRBurningApplicationHasDifferentCDDVDParametersKey(DICOMDIRBaseKey + "DICOMDIRBurningApplicationHasDifferentCDDVDParameters");
const QString InputOutputSettings::ConvertDICOMDIRImagesToLittleEndianKey(DICOMDIRBaseKey + "ConvertDICOMDIRImagesToLittleEndian");
const QString InputOutputSettings::DICOMDIRFolderPathToCopy(DICOMDIRBaseKey + "DICOMDIRFolderPathToCopy");
const QString InputOutputSettings::CopyFolderContentToDICOMDIROnCDOrDVD(DICOMDIRBaseKey + "CopyFolderContentToDICOMDIRCdDvd");
const QString InputOutputSettings::CopyFolderContentToDICOMDIROnUSBOrHardDisk(DICOMDIRBaseKey + "CopyFolderContentToDICOMDIRUsbHardDisk");

const QString InstitutionInformationBase("InstitutionInformation/");
const QString InputOutputSettings::InstitutionName(InstitutionInformationBase + "InstitutionName");
const QString InputOutputSettings::InstitutionTown(InstitutionInformationBase + "InstitutionTown");
const QString InputOutputSettings::InstitutionAddress(InstitutionInformationBase + "InstitutionAddress");
const QString InputOutputSettings::InstitutionZipCode(InstitutionInformationBase + "InstitutionZipCode");
const QString InputOutputSettings::InstitutionCountry(InstitutionInformationBase + "InstitutionCountry");
const QString InputOutputSettings::InstitutionPhoneNumber(InstitutionInformationBase + "InstitutionPhoneNumber");
const QString InputOutputSettings::InstitutionEmail(InstitutionInformationBase + "InstitutionEmail");

InputOutputSettings::InputOutputSettings()
{
}

InputOutputSettings::~InputOutputSettings()
{
}

void InputOutputSettings::init()
{
    SettingsRegistry *settingsRegistry = SettingsRegistry::instance();
    
    settingsRegistry->addSetting(DatabaseAbsoluteFilePath, UserDataRootPath + "pacs/database/dicom.sdb", Settings::Parseable);
    settingsRegistry->addSetting(CachePath, UserDataRootPath + "pacs/dicom/", Settings::Parseable);

    settingsRegistry->addSetting(DeleteLeastRecentlyUsedStudiesInDaysCriteria, true);
    settingsRegistry->addSetting(DeleteLeastRecentlyUsedStudiesNoFreeSpaceCriteria, true);
    settingsRegistry->addSetting(MinimumDaysUnusedToDeleteStudy, 7);
    settingsRegistry->addSetting(MinimumFreeGigaBytesForCache, 5);
    settingsRegistry->addSetting(MinimumGigaBytesToFreeIfCacheIsFull, 2);

    settingsRegistry->addSetting(ListenToRISRequests, true);
    settingsRegistry->addSetting(RISRequestsPort, 11110);
    settingsRegistry->addSetting(RISRequestViewOnceRetrieved, true);

    settingsRegistry->addSetting(LastOpenedDICOMDIRPath, QDir::homePath());

    settingsRegistry->addSetting(QueryRetrieveLocalPort, 4006);
    // TODO podríem definir-lo directament amb %HOSTNAME%
    settingsRegistry->addSetting(LocalAETitle, QHostInfo::localHostName(), Settings::Parseable);
    settingsRegistry->addSetting(PACSConnectionTimeout, 20);
    settingsRegistry->addSetting(MaximumPACSConnections, 3);

    settingsRegistry->addSetting(ConvertDICOMDIRImagesToLittleEndianKey, false);
#if defined(Q_OS_WIN) //WINDOWS
    settingsRegistry->addSetting(DICOMDIRBurningApplicationPathKey, QString::fromLocal8Bit(qgetenv("ProgramFiles")) + "\\ImgBurn\\ImgBurn.exe");
    settingsRegistry->addSetting(DICOMDIRBurningApplicationParametersKey, "/MODE write /SRC %1 /EJECT YES /VERIFY NO /CLOSESUCCESS /START");
    settingsRegistry->addSetting(DICOMDIRBurningApplicationCDParametersKey, "");
    settingsRegistry->addSetting(DICOMDIRBurningApplicationDVDParametersKey, "");
    settingsRegistry->addSetting(DICOMDIRBurningApplicationHasDifferentCDDVDParametersKey, false);
#elif defined(Q_OS_MAC) //MAC
    settingsRegistry->addSetting(DICOMDIRBurningApplicationPathKey, QDesktopServices::storageLocation(QDesktopServices::ApplicationsLocation) + "/Burn.app/Contents/MacOS/Burn");
    settingsRegistry->addSetting(DICOMDIRBurningApplicationParametersKey, "%1");
    settingsRegistry->addSetting(DICOMDIRBurningApplicationCDParametersKey, "");
    settingsRegistry->addSetting(DICOMDIRBurningApplicationDVDParametersKey, "");
    settingsRegistry->addSetting(DICOMDIRBurningApplicationHasDifferentCDDVDParametersKey, false);
#else // UNIX
    settingsRegistry->addSetting(DICOMDIRBurningApplicationPathKey, "/usr/bin/k3b");
    settingsRegistry->addSetting(DICOMDIRBurningApplicationParametersKey, "--nosplash %1");
    settingsRegistry->addSetting(DICOMDIRBurningApplicationCDParametersKey, "--nosplash --cdimage %1");
    settingsRegistry->addSetting(DICOMDIRBurningApplicationDVDParametersKey, "--nosplash --dvdimage %1");
    settingsRegistry->addSetting(DICOMDIRBurningApplicationHasDifferentCDDVDParametersKey, true);
#endif

    settingsRegistry->addSetting(DICOMDIRFolderPathToCopy, "");
    settingsRegistry->addSetting(CopyFolderContentToDICOMDIROnCDOrDVD, false);
    settingsRegistry->addSetting(CopyFolderContentToDICOMDIROnUSBOrHardDisk, false);

    settingsRegistry->addSetting(PACSStudyListSortByColumn, QStudyTreeWidget::ObjectName);
    settingsRegistry->addSetting(PACSStudyListSortOrder, (int) Qt::AscendingOrder);
    settingsRegistry->addSetting(LocalDatabaseStudyListSortByColumn, QStudyTreeWidget::ObjectName);
    settingsRegistry->addSetting(LocalDatabaseStudyListSortOrder, Qt::AscendingOrder);
    settingsRegistry->addSetting(DICOMDIRStudyListSortByColumn, QStudyTreeWidget::ObjectName);
    settingsRegistry->addSetting(DICOMDIRStudyListSortOrder, Qt::AscendingOrder);
    settingsRegistry->addSetting(OperationStateListSortByColumn, QOperationStateScreen::Date);
    settingsRegistry->addSetting(OperationStateListSortOrder, Qt::AscendingOrder);}

} // end namespace udg 

