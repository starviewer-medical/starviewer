#include "layoutmanager.h"

#include "patient.h"
#include "study.h"
#include "hangingprotocolmanager.h"
#include "relatedstudiesmanager.h"
#include "viewerslayout.h"
#include "coresettings.h"
#include "studylayoutconfigsloader.h"
#include "studylayoutmapper.h"
#include "studylayoutconfig.h"
#include "studylayoutconfigsettingsmanager.h"
#include "hangingprotocolmanager.h"

namespace udg {

LayoutManager::LayoutManager(Patient *patient, ViewersLayout *layout, QObject *parent)
 : QObject(parent)
{
    m_hangingProtocolManager = 0;
    m_relatedStudiesManager = 0;

    m_patient = patient;
    m_layout = layout;
}

LayoutManager::~LayoutManager()
{
}

void LayoutManager::initialize()
{
    setupHangingProtocols();
    applyProperLayoutChoice();
    m_layout->setSelectedViewer(m_layout->getViewerWidget(0));
}

void LayoutManager::setupHangingProtocols()
{
    if (m_hangingProtocolManager != 0)
    {
        m_hangingProtocolManager->cancelHangingProtocolDownloading();
        delete m_hangingProtocolManager;
    }
    m_hangingProtocolManager = new HangingProtocolManager();

    connect(m_patient, SIGNAL(patientFused()), SLOT(searchHangingProtocols()));
    searchHangingProtocols();
}

void LayoutManager::applyProperLayoutChoice()
{
    // We first look which modalities the pacient has in order to decide if look first for hanging protocols or automatic layouts
    Settings settings;
    QStringList modalitiesWithHPPriority = settings.getValue(CoreSettings::ModalitiesToApplyHangingProtocolsAsFirstOption)
        .toString().split(";", QString::SkipEmptyParts);
    QStringList patientModalities;
    foreach (Study *study, m_patient->getStudies())
    {
        patientModalities << study->getModalities();
    }

    bool hasToApplyHangingProtocol = false;
    QSet<QString> matchingModalities = modalitiesWithHPPriority.toSet().intersect(patientModalities.toSet());
    if (matchingModalities.isEmpty())
    {
        // First we find out which candidate configurations we have according to the patient's modalities
        applyLayoutCandidates(getLayoutCandidates(m_patient), m_patient);
    }
    else
    {
        hasToApplyHangingProtocol = true;
    }

    if (hasToApplyHangingProtocol)
    {
        searchAndApplyBestHangingProtocol();
    }
    else
    {
        // Updating only hanging protocols
        searchHangingProtocols();
    }
}

void LayoutManager::searchHangingProtocols()
{
    QList<HangingProtocol*> hangingCandidates = m_hangingProtocolManager->searchHangingProtocols(m_patient);
    emit hangingProtocolCandidatesFound(hangingCandidates);
}

void LayoutManager::searchAndApplyBestHangingProtocol()
{
    QList<HangingProtocol*> hangingCandidates = m_hangingProtocolManager->searchHangingProtocols(m_patient);
    emit hangingProtocolCandidatesFound(hangingCandidates);
    if (hangingCandidates.size() == 0)
    {
        // There are no hanging protocols available, applying automatic layouts
        applyLayoutCandidates(getLayoutCandidates(m_patient), m_patient);
    }
    else
    {
        m_hangingProtocolManager->setBestHangingProtocol(m_patient, hangingCandidates, m_layout);
    }
}

void LayoutManager::cancelOngoingOperations()
{
    m_hangingProtocolManager->cancelHangingProtocolDownloading();
}

QList<StudyLayoutConfig> LayoutManager::getLayoutCandidates(Patient *patient)
{
    QList<StudyLayoutConfig> configurationCandidates;
    
    if (!patient)
    {
        return configurationCandidates;
    }
    
    StudyLayoutConfigSettingsManager settingsManager;
    foreach (const StudyLayoutConfig &currentConfig, settingsManager.getConfigList())
    {
        if (!patient->getStudiesByModality(currentConfig.getModality()).isEmpty())
        {
            configurationCandidates << currentConfig;
        }
    }

    return configurationCandidates;
}

void LayoutManager::applyLayoutCandidates(const QList<StudyLayoutConfig> &candidates, Patient *patient)
{
    QStringList patientModalities;
    foreach (Study *study, patient->getStudies())
    {
        patientModalities << study->getModalities();
    }
    
    StudyLayoutConfig layoutToApply;
    if (!candidates.isEmpty())
    {
        // TODO We only take into account the first candidate, the others, if any, are discarded. We should take them into account too.
        layoutToApply = candidates.first();
    }
    else
    {
        // If no candidate found, we choose a default configuration.
        // This default configuration is not yet configurable through settings, could be done in a future enhancement.
        layoutToApply = StudyLayoutConfigsLoader::getDefaultConfigForModality(patientModalities.first());
    }

    StudyLayoutMapper mapper;
    mapper.applyConfig(layoutToApply, m_layout, patient);
}

void LayoutManager::setHangingProtocol(int hangingProtocolNumber)
{
    m_hangingProtocolManager->applyHangingProtocol(hangingProtocolNumber, m_layout, m_patient);
}

void LayoutManager::addHangingProtocolsWithPrevious(QList<Study*> studies)
{
    QList<HangingProtocol*> hangingCandidates = m_hangingProtocolManager->searchHangingProtocols(m_patient, studies);
    emit hangingProtocolCandidatesFound(hangingCandidates);
    // HACK To notify we ended searching related studies and thus we have all the hanging protocols available
    emit previousStudiesSearchEnded();
}

} // end namespace udg
