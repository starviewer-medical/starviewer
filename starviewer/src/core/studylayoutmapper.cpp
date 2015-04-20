/*************************************************************************************
  Copyright (C) 2014 Laboratori de Gràfics i Imatge, Universitat de Girona &
  Institut de Diagnòstic per la Imatge.
  Girona 2014. All rights reserved.
  http://starviewer.udg.edu

  This file is part of the Starviewer (Medical Imaging Software) open source project.
  It is subject to the license terms in the LICENSE file found in the top-level
  directory of this distribution and at http://starviewer.udg.edu/license. No part of
  the Starviewer (Medical Imaging Software) open source project, including this file,
  may be copied, modified, propagated, or distributed except according to the
  terms contained in the LICENSE file.
 *************************************************************************************/

#include "studylayoutmapper.h"

#include "optimalviewersgridestimator.h"
#include "viewerslayout.h"
#include "patient.h"
#include "volume.h"
#include "changesliceqviewercommand.h"
#include "logging.h"
#include "q2dviewerwidget.h"

namespace udg {

StudyLayoutMapper::StudyLayoutMapper()
{
}

StudyLayoutMapper::~StudyLayoutMapper()
{
}

void StudyLayoutMapper::applyConfig(const StudyLayoutConfig &config, ViewersLayout *layout, Study *study, const QRectF &geometry)
{
    applyConfig(config, layout, study, geometry, -1, -1);
}

QPair<int, int> StudyLayoutMapper::getOptimalViewersGrid(const StudyLayoutConfig &config, const QList<QPair<Volume *, int> > &candidateImages)
{
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

    return grid;
}

void StudyLayoutMapper::applyConfig(const StudyLayoutConfig &config, ViewersLayout *layout, Study *study, const QRectF &geometry, int rows, int columns)
{
    if (!layout || !study)
    {
        return;
    }

    // Primer trobem els estudis que compleixin amb els requisits
    QString configModality = config.getModality();
    QList<Study*> matchingStudies;
    if (configModality.isEmpty() || study->getModalities().contains(configModality))
    {
        // If modality is empty it means "any modality", so we'll apply the layout to all the studies
        matchingStudies << study;
    }
    
    if (matchingStudies.isEmpty())
    {
        return;
    }

    // Procedim a aplicar el layout sobre els estudis que han coincidit
    layout->cleanUp(geometry);
    
    // Un cop tenim els estudis, ara necessitem filtrar a nivell dels volums/imatges que necessitem
    QList<QPair<Volume*, int> > candidateImages = getImagesToPlace(config, matchingStudies);

    if (rows < 1 || columns < 1)
    {
        QPair<int, int> grid = getOptimalViewersGrid(config, candidateImages);
        // Assignem el grid al layout
        rows = grid.first;
        columns = grid.second;
    }
    layout->setGridInArea(rows, columns, geometry);
    // Col·loquem les imatges en el layout donat
    placeImagesInCurrentLayout(candidateImages, config.getUnfoldDirection(), layout, rows, columns, geometry);
    // Make the first viewer selected
    layout->setSelectedViewer(layout->getViewersInsideGeometry(geometry).at(0));
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
                for (int i = 0; i < series->getNumberOfVolumes(); ++i)
                {
                    Volume *currentVolume = series->getVolumesList().at(i);
                    if (config.getUnfoldType() == StudyLayoutConfig::UnfoldImages)
                    {
                        // TODO Cal pensar què fer en el cas que tinguem fases. Ara mateix no les estem contemplant.
                        for (int slice = 0; slice < currentVolume->getNumberOfSlicesPerPhase(); ++slice)
                        {
                            candidateImages << QPair<Volume*, int>(currentVolume, slice);
                        }
                    }
                    else
                    {
                        // StudyLayoutConfig::UnfoldSeries, col·loquem la primera imatge de cada sèrie
                        candidateImages << QPair<Volume*, int>(currentVolume, 0);
                    }
                }
            }
        }
    }

    return candidateImages;
}

void StudyLayoutMapper::placeImagesInCurrentLayout(const QList<QPair<Volume*, int> > &volumesToPlace, StudyLayoutConfig::UnfoldDirectionType unfoldDirection,
                                                   ViewersLayout *layout, int rows, int columns, const QRectF &geometry)
{
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

    QList<Q2DViewerWidget*> viewerWidgetList = layout->getViewersInsideGeometry(geometry);
    if (unfoldDirection == StudyLayoutConfig::LeftToRightFirst)
    {
        for (int i = 0; i < rows; ++i)
        {
            for (int j = 0; j < columns; ++j)
            {
                if (numberOfPlacedVolumes < numberOfVolumesToPlace)
                {
                    Q2DViewer *viewer = viewerWidgetList[i * columns + j]->getViewer();
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
                    Q2DViewer *viewer = viewerWidgetList[j * columns + i]->getViewer();
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
