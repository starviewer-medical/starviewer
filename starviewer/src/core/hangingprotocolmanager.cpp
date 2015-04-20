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

#include "hangingprotocolmanager.h"

#include "viewerslayout.h"
#include "patient.h"
#include "series.h"
#include "image.h"
#include "volume.h"
#include "q2dviewerwidget.h"
#include "hangingprotocolsrepository.h"
#include "hangingprotocol.h"
#include "hangingprotocollayout.h"
#include "hangingprotocolmask.h"
#include "hangingprotocolimageset.h"
#include "hangingprotocoldisplayset.h"
#include "identifier.h"
#include "logging.h"
#include "volumerepository.h"
#include "applyhangingprotocolqviewercommand.h"
#include "hangingprotocolfiller.h"
// Necessari per poder anar a buscar prèvies
#include "../inputoutput/relatedstudiesmanager.h"

namespace udg {

HangingProtocolManager::HangingProtocolManager(QObject *parent)
 : QObject(parent)
{
    m_hangingProtocolsDownloading = new QHash<HangingProtocol*, QMultiHash<QString, StructPreviousStudyDownloading*>*>();
    m_relatedStudiesManager = new RelatedStudiesManager();

    copyHangingProtocolRepository();

    connect(m_relatedStudiesManager, SIGNAL(errorDownloadingStudy(QString)), SLOT(errorDownloadingPreviousStudies(QString)));
}

HangingProtocolManager::~HangingProtocolManager()
{
    cancelAllHangingProtocolsDownloading();
    delete m_hangingProtocolsDownloading;
    delete m_relatedStudiesManager;

    foreach (HangingProtocol *hangingProtocol, m_availableHangingProtocols)
    {
        delete hangingProtocol;
    }
    m_availableHangingProtocols.clear();
}

void HangingProtocolManager::copyHangingProtocolRepository()
{
    foreach (HangingProtocol *hangingProtocol, HangingProtocolsRepository::getRepository()->getItems())
    {
        m_availableHangingProtocols << new HangingProtocol(*hangingProtocol);
    }
}

QList<HangingProtocol*> HangingProtocolManager::searchHangingProtocols(Study *study)
{
    QList<Study*> previousStudies;

    return searchHangingProtocols(study, previousStudies);
}

QList<HangingProtocol*> HangingProtocolManager::searchHangingProtocols(Study *study, const QList<Study*> &previousStudies)
{
    QList<HangingProtocol*> outputHangingProtocolList;

    // Buscar el hangingProtocol que s'ajusta millor a l'estudi del pacient
    // Aprofitem per assignar ja les series, per millorar el rendiment
    foreach (HangingProtocol *hangingProtocolBase, m_availableHangingProtocols)
    {
        HangingProtocol *hangingProtocol = new HangingProtocol(*hangingProtocolBase);
        if (isModalityCompatible(hangingProtocol, study) && isInstitutionCompatible(hangingProtocol, study) && hangingProtocol->getNumberOfPriors() <= previousStudies.size())
        {
            HangingProtocolFiller hangingProtocolFiller;
            hangingProtocolFiller.fill(hangingProtocol, study, previousStudies);

            int numberOfFilledImageSets = hangingProtocol->countFilledImageSets();

            bool isValidHangingProtocol = false;

            if (hangingProtocol->isStrict())
            {
                if (numberOfFilledImageSets == hangingProtocol->getNumberOfImageSets())
                {
                    isValidHangingProtocol = true;
                }
            }
            else
            {
                if (numberOfFilledImageSets > 0)
                {
                    if (hangingProtocol->getNumberOfPriors() > 0)
                    {
                        int filledImageSetsWithPriors = hangingProtocol->countFilledImageSetsWithPriors();
                        if (filledImageSetsWithPriors == 0 || numberOfFilledImageSets == filledImageSetsWithPriors)
                        {
                            isValidHangingProtocol = false;
                        }
                        else
                        {
                            isValidHangingProtocol = true;
                        }
                    }
                    else
                    {
                        isValidHangingProtocol = true;
                    }
                }
            }

            if (isValidHangingProtocol)
            {
                outputHangingProtocolList << hangingProtocol;
            }
        }
    }

    if (outputHangingProtocolList.size() > 0)
    {
        // Noms per mostrar al log
        QString infoLog;
        foreach (HangingProtocol *hangingProtocol, outputHangingProtocolList)
        {
            infoLog += QString("%1, ").arg(hangingProtocol->getName());
        }
        INFO_LOG(QString("Hanging protocols carregats: %1").arg(infoLog));
    }
    else
    {
        INFO_LOG(QString("No s'ha trobat cap hanging protocol"));
    }

    return outputHangingProtocolList;
}

HangingProtocol* HangingProtocolManager::setBestHangingProtocol(Patient *patient, const QList<HangingProtocol*> &hangingProtocolList, ViewersLayout *layout, const QRectF &geometry)
{
    HangingProtocol *bestHangingProtocol = NULL;
    foreach (HangingProtocol *hangingProtocol, hangingProtocolList)
    {
        if (hangingProtocol->isBetterThan(bestHangingProtocol))
        {
            bestHangingProtocol = hangingProtocol;
        }
    }

    if (bestHangingProtocol)
    {
        DEBUG_LOG(QString("Hanging protocol que s'aplica: %1").arg(bestHangingProtocol->getName()));
        applyHangingProtocol(bestHangingProtocol, layout, patient, geometry);
    }

    return bestHangingProtocol;
}

void HangingProtocolManager::applyHangingProtocol(HangingProtocol *hangingProtocol, ViewersLayout *layout, Patient *patient)
{
    applyHangingProtocol(hangingProtocol, layout, patient, QRectF(0.0, 0.0, 1.0, 1.0));
}

void HangingProtocolManager::applyHangingProtocol(HangingProtocol *hangingProtocol, ViewersLayout *layout, Patient *patient, const QRectF &geometry)
{
    // Si hi havia algun estudi descarregant, es treu de la llista d'espera
    cancelAllHangingProtocolsDownloading();

    // Clean up viewer of the working area
    layout->cleanUp(geometry);

    QMultiHash<QString, StructPreviousStudyDownloading*> *studiesDownloading;

    if (m_hangingProtocolsDownloading->contains(hangingProtocol))
    {
        studiesDownloading = m_hangingProtocolsDownloading->value(hangingProtocol);
    }
    else
    {
        studiesDownloading = new QMultiHash<QString, StructPreviousStudyDownloading*>();
        m_hangingProtocolsDownloading->insert(hangingProtocol, studiesDownloading);
    }

    QList<HangingProtocolDisplaySet*> displaySets = hangingProtocol->getDisplaySets();
    for(int i = 0; i < displaySets.size(); ++i)
    {
        HangingProtocolDisplaySet *displaySet = displaySets[i];
        HangingProtocolImageSet *hangingProtocolImageSet = displaySet->getImageSet();
        Q2DViewerWidget *viewerWidget = layout->addViewer(layout->convertGeometry(displaySet->getGeometry(), geometry));

        if (i == 0)
        {
            layout->setSelectedViewer(viewerWidget);
        }
        
        if (hangingProtocolImageSet->isDownloaded() == false)
        {
            viewerWidget->getViewer()->setViewerStatus(QViewer::DownloadingVolume);

            StructPreviousStudyDownloading *structPreviousStudyDownloading = new StructPreviousStudyDownloading;
            structPreviousStudyDownloading->widgetToDisplay = viewerWidget;
            structPreviousStudyDownloading->displaySet = displaySet;

            bool isDownloading = studiesDownloading->contains(hangingProtocolImageSet->getPreviousStudyToDisplay()->getInstanceUID());

            studiesDownloading->insert(hangingProtocolImageSet->getPreviousStudyToDisplay()->getInstanceUID(), structPreviousStudyDownloading);

            if (!isDownloading && hangingProtocolImageSet->getPreviousStudyToDisplay()->getDICOMSource().getRetrievePACS().count() > 0)
            {
                //En principi sempre hauríem de tenir algun PACS al DICOMSource
                connect(patient, SIGNAL(studyAdded(Study*)), SLOT(previousStudyDownloaded(Study*)));
                m_relatedStudiesManager->retrieveAndLoad(hangingProtocolImageSet->getPreviousStudyToDisplay(),
                    hangingProtocolImageSet->getPreviousStudyToDisplay()->getDICOMSource().getRetrievePACS().at(0));
            }
        }
        else
        {
            setInputToViewer(viewerWidget, displaySet);
        }
    }

    INFO_LOG(QString("Hanging protocol aplicat: %1").arg(hangingProtocol->getName()));
}

bool HangingProtocolManager::isModalityCompatible(HangingProtocol *protocol, Study *study)
{
    foreach (const QString &modality, study->getModalities())
    {
        if (isModalityCompatible(protocol, modality))
        {
            return true;
        }
    }

    return false;
}

bool HangingProtocolManager::isModalityCompatible(HangingProtocol *protocol, const QString &modality)
{
    return protocol->getHangingProtocolMask()->getProtocolList().contains(modality);
}

bool HangingProtocolManager::isInstitutionCompatible(HangingProtocol *protocol, Study *study)
{
    foreach(Series *series, study->getSeries())
    {
        if (isValidInstitution(protocol, series->getInstitutionName()))
        {
            return true;
        }
    }

    return false;
}

bool HangingProtocolManager::isValidInstitution(HangingProtocol *protocol, const QString &institutionName)
{
    if (protocol->getInstitutionsRegularExpression().isEmpty())
    {
        return true;
    }

    return institutionName.contains(protocol->getInstitutionsRegularExpression());
}

void HangingProtocolManager::previousStudyDownloaded(Study *study)
{
    foreach (HangingProtocol *hangingProtocol, m_hangingProtocolsDownloading->keys())
    {
        QMultiHash<QString, StructPreviousStudyDownloading*> *studiesDownloading = m_hangingProtocolsDownloading->value(hangingProtocol);

        // Es busca quins estudis nous hi ha
        // List is sorted in reverse insertion order
        QList<StructPreviousStudyDownloading*> previousDownloadingList = studiesDownloading->values(study->getInstanceUID());
        studiesDownloading->remove(study->getInstanceUID());

        for (int i = previousDownloadingList.size() - 1; i >= 0; --i)
        {
            // Per cada estudi que esperàvem que es descarregués
            // Agafem l'estructura amb les dades que s'havien guardat per poder aplicar-ho
            StructPreviousStudyDownloading *structPreviousStudyDownloading = previousDownloadingList[i];

            // Busquem la millor serie de l'estudi que ho satisfa
            HangingProtocolFiller hangingProtocolFiller;
            hangingProtocolFiller.fillImageSetWithStudy(structPreviousStudyDownloading->displaySet->getImageSet(), study);

            Q2DViewerWidget *viewerWidget = structPreviousStudyDownloading->widgetToDisplay;
            structPreviousStudyDownloading->displaySet->getImageSet()->setDownloaded(true);

            viewerWidget->getViewer()->setViewerStatus(QViewer::NoVolumeInput);

            setInputToViewer(viewerWidget, structPreviousStudyDownloading->displaySet);

            delete structPreviousStudyDownloading;
        }

        if (studiesDownloading->isEmpty())
        {
            m_hangingProtocolsDownloading->remove(hangingProtocol);
            delete studiesDownloading;
        }
    }
}

void HangingProtocolManager::errorDownloadingPreviousStudies(const QString &studyUID)
{
    foreach (HangingProtocol *hangingProtocol, m_hangingProtocolsDownloading->keys())
    {
        QMultiHash<QString, StructPreviousStudyDownloading*> *studiesDownloading = m_hangingProtocolsDownloading->value(hangingProtocol);
        if (studiesDownloading->contains(studyUID))
        {
            // Si és un element que estavem esperant
            int count = studiesDownloading->count(studyUID);
            for (int i = 0; i < count; ++i)
            {
                // S'agafa i es treu de la llista
                StructPreviousStudyDownloading *element = studiesDownloading->take(studyUID);
                element->widgetToDisplay->getViewer()->setViewerStatus(QViewer::DownloadingError);
                delete element;
            }

            if (studiesDownloading->isEmpty())
            {
                m_hangingProtocolsDownloading->remove(hangingProtocol);
                delete studiesDownloading;
            }
        }
    }
}

void HangingProtocolManager::cancelAllHangingProtocolsDownloading()
{
    foreach (HangingProtocol *hangingProtocol, m_hangingProtocolsDownloading->keys())
    {
        cancelHangingProtocolDownloading(hangingProtocol);
    }
}

void HangingProtocolManager::cancelHangingProtocolDownloading(HangingProtocol *hangingProtocol)
{
    QMultiHash<QString, StructPreviousStudyDownloading*> *studiesDownloading = m_hangingProtocolsDownloading->take(hangingProtocol);
    foreach (const QString &key, studiesDownloading->keys())
    {
        // S'agafa i es treu de la llista l'element que s'està esperant
        // i es treu el label de downloading
        StructPreviousStudyDownloading *element = studiesDownloading->take(key);
        // The widget may have been destroyed before calling this method, so we must check that it's still valid
        if (element->widgetToDisplay)
        {
            element->widgetToDisplay->getViewer()->setViewerStatus(QViewer::NoVolumeInput);
        }
        delete element;
    }
    delete studiesDownloading;
}

void HangingProtocolManager::setInputToViewer(Q2DViewerWidget *viewerWidget, HangingProtocolDisplaySet *displaySet)
{
    Series *series = displaySet->getImageSet()->getSeriesToDisplay();

    if (series)
    {
        if (series->isViewable() && series->getFirstVolume())
        {
            Volume *inputVolume = NULL;
            if ((displaySet->getSlice() > -1 && series->getVolumesList().size() > 1) || displaySet->getImageSet()->getTypeOfItem() == "image")
            {
                Image *image;
                // TODO En el cas de fases no funcionaria, perquè l'índex no és correcte
                if (displaySet->getSlice() > -1)
                {
                    image = series->getImageByIndex(displaySet->getSlice());
                }
                else if (displaySet->getImageSet()->getTypeOfItem() == "image")
                {
                    image = series->getImageByIndex(displaySet->getImageSet()->getImageToDisplay());
                }

                Volume *volumeContainsImage = series->getVolumeOfImage(image);

                if (!volumeContainsImage)
                {
                    // No existeix cap imatge al tall corresponent, agafem el volum per defecte
                    inputVolume = series->getFirstVolume();
                }
                else
                {
                    // Tenim nou volum, i per tant, cal calcular el nou número de llesca
                    int slice = volumeContainsImage->getImages().indexOf(image);
                    displaySet->setSliceModifiedForVolumes(slice);

                    inputVolume = volumeContainsImage;
                }
            }
            else
            {
                inputVolume = series->getFirstVolume();
            }

            ApplyHangingProtocolQViewerCommand *command = new ApplyHangingProtocolQViewerCommand(viewerWidget, displaySet);
            viewerWidget->setInputAsynchronously(inputVolume, command);
        }
    }
}

}
