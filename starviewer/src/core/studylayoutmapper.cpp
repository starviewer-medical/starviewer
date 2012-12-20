#include "studylayoutmapper.h"

#include "optimalviewersgridestimator.h"
#include "viewerslayout.h"
#include "patient.h"
#include "volume.h"
#include "changesliceqviewercommand.h"
#include "logging.h"

namespace udg {

StudyLayoutMapper::StudyLayoutMapper()
{
}

StudyLayoutMapper::~StudyLayoutMapper()
{
}

void StudyLayoutMapper::applyConfig(const StudyLayoutConfig &config, ViewersLayout *layout, Patient *patient)
{
    if (!layout || !patient)
    {
        return;
    }

    // Primer trobem els estudis que compleixin amb els requisits
    // Requisit 1: Que contingui la modalitat de la configuració
    QList<Study*> matchingStudies = getMatchingStudies(config, patient);
    
    if (matchingStudies.isEmpty())
    {
        return;
    }

    // Procedim a aplicar el layout sobre els estudis que han coincidit
    layout->cleanUp();
    
    // Un cop tenim els estudis, ara necessitem filtrar a nivell dels volums/imatges que necessitem
    QList<QPair<Volume*, int> > candidateImages = getImagesToPlace(config, matchingStudies);
    int numberOfMinimumViewersRequired = candidateImages.count();

    // Si el nombre màxim de visors per configuració és menor al nombre mínim de visors requerits, haurem d'actualitzar aquest paràmetre abans de crear el grid
    int maxNumbersOfViewersByConfig = config.getMaximumNumberOfViewers();
    if (maxNumbersOfViewersByConfig > 0)
    {
        if (maxNumbersOfViewersByConfig < numberOfMinimumViewersRequired)
        {
            numberOfMinimumViewersRequired = maxNumbersOfViewersByConfig;
        }
    }
    // Ara ja sabem el nombre mínim de visors requerits, ara cal calcular quina és la distribució idònia en graella
    OptimalViewersGridEstimator gridEstimator;
    QPair<int, int> grid = gridEstimator.getOptimalGrid(numberOfMinimumViewersRequired);
    // Assignem el grid al layout
    int rows = grid.first;
    int columns = grid.second;
    layout->setGrid(rows, columns);
    // Col·loquem les imatges en el layout donat
    placeImagesInCurrentLayout(candidateImages, config.getUnfoldDirection(), layout);
}

QList<Study*> StudyLayoutMapper::getMatchingStudies(const StudyLayoutConfig &config, Patient *patient)
{
    QList<Study*> matchingStudies;
    QStringList modalities = config.getModalities();

    foreach (const QString configModality, modalities)
    {
        matchingStudies << patient->getStudiesByModality(configModality);
    }

    return matchingStudies;
}

QList<QPair<Volume*, int> > StudyLayoutMapper::getImagesToPlace(const StudyLayoutConfig &config, const QList<Study*> &matchingStudies)
{
    QList<StudyLayoutConfig::ExclusionCriteriaType> exclusionCriteria = config.getExclusionCriteria();
    QList<QPair<Volume*, int> > candidateImages;
    // Primer calculem el nombre total de sèries o imatges
    foreach (Study *study, matchingStudies)
    {
        foreach (Series *series, study->getViewableSeries())
        {
            bool matchesExclusionCriteria = false;
            if (!exclusionCriteria.isEmpty())
            {
                foreach (StudyLayoutConfig::ExclusionCriteriaType criteria, exclusionCriteria)
                {
                    switch (criteria)
                    {
                        case StudyLayoutConfig::Localizer:
                            if (series->isCTLocalizer())
                            {
                                matchesExclusionCriteria = true;
                            }
                            break;

                        case StudyLayoutConfig::Survey:
                            if (series->isMRSurvey())
                            {
                                matchesExclusionCriteria = true;
                            }
                            break;
                    }
                }
            }
                        
            if (!matchesExclusionCriteria)
            {
                switch (config.getUnfoldType())
                {
                    case StudyLayoutConfig::UnfoldImages:
                        // TODO Cal pensar què fer en el cas que tinguem fases. Ara mateix no les estem contemplant.
                        for (int i = 0; i < series->getNumberOfVolumes(); ++i)
                        {
                            Volume *currentVolume = series->getVolumesList().at(i);
                            for (int slice = 0; slice < currentVolume->getNumberOfSlicesPerPhase(); ++slice)
                            {
                                candidateImages << QPair<Volume*, int>(currentVolume, slice);
                            }
                        }
                        break;

                    case StudyLayoutConfig::UnfoldSeries:
                        for (int i = 0; i < series->getNumberOfVolumes(); ++i)
                        {
                            Volume *currentVolume = series->getVolumesList().at(i);
                            candidateImages << QPair<Volume*, int>(currentVolume, 0);
                        }
                        break;
                }
                
            }
        }
    }

    return candidateImages;
}

void StudyLayoutMapper::placeImagesInCurrentLayout(const QList<QPair<Volume*, int> > &volumesToPlace, StudyLayoutConfig::UnfoldDirectionType unfoldDirection, ViewersLayout *layout)
{
    int rows = layout->getVisibleRows();
    int columns = layout->getVisibleColumns();
    int numberOfVolumesToPlace = volumesToPlace.count();

    // Comprovem que tinguem el nombre suficient de files i columnes pel nombre de volums
    if (numberOfVolumesToPlace > rows * columns)
    {
        DEBUG_LOG(QString("No hi ha suficients visors pel nombre passat de volums/imatges. #Volums/imatges: %1. Files: %2, Columnes: %3. "
            "Limitem el nombre de volums a col·locar a files * columnes = %4").arg(numberOfVolumesToPlace).arg(rows).arg(columns).arg(rows * columns));
        // Limitem el nombre de volums al nombre total de visors disponibles
        numberOfVolumesToPlace = rows * columns;
    }
    // Ara toca assignar els inputs
    int numberOfPlacedVolumes = 0;
    
    if (unfoldDirection == StudyLayoutConfig::LeftToRightFirst)
    {
        for (int i = 0; i < rows; ++i)
        {
            for (int j = 0; j < columns; ++j)
            {
                if (numberOfPlacedVolumes < numberOfVolumesToPlace)
                {
                    Q2DViewer *viewer = layout->getViewerWidget(i * columns + j)->getViewer();
                    Volume * volume = volumesToPlace.at(numberOfPlacedVolumes).first;
                    ChangeSliceQViewerCommand *command = new ChangeSliceQViewerCommand(viewer, volumesToPlace.at(numberOfPlacedVolumes).second);
                    viewer->setInputAsynchronously(volume, command);
                    ++numberOfPlacedVolumes;
                }
            }
        }
    }
    else if (unfoldDirection == StudyLayoutConfig::TopToBottomFirst)
    {
        for (int i = 0; i < columns; ++i)
        {
            for (int j = 0; j < rows; ++j)
            {
                if (numberOfPlacedVolumes < numberOfVolumesToPlace)
                {
                    Q2DViewer *viewer = layout->getViewerWidget(j * columns + i)->getViewer();
                    Volume * volume = volumesToPlace.at(numberOfPlacedVolumes).first;
                    ChangeSliceQViewerCommand *command = new ChangeSliceQViewerCommand(viewer, volumesToPlace.at(numberOfPlacedVolumes).second);
                    viewer->setInputAsynchronously(volume, command);
                    ++numberOfPlacedVolumes;
                }
            }
        }
    }
}

} // End namespace udg
