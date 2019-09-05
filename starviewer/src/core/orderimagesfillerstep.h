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

#ifndef UDGORDERIMAGESFILLERSTEP_H
#define UDGORDERIMAGESFILLERSTEP_H

#include "patientfillerstep.h"

#include <QHash>
#include <QMap>
#include <QVector3D>

namespace udg {

class Image;

/**
 * @brief The OrderImagesFillerStep class orders images inside each volume.
 *
 * To achieve this it decides the order and then calls setOrderNumberInVolume() for each image and setImages() for each series (so that the series has the list
 * of images in the correct order). For more information read qms://doc/065875b1.
 */
class OrderImagesFillerStep : public PatientFillerStep
{
public:
    OrderImagesFillerStep();
    ~OrderImagesFillerStep() override;

    bool fillIndividually() override;
    void postProcessing() override;

    /// Returns true if the given volume of the given series can be spatially sorted. It's public to allow testing.
    bool canBeSpatiallySorted(Series *series, int volume) const;

private:
    /// Mètodes per processar la informació específica de series
    void processImage(Image *image);

    /// Mètode per calcular quantes fases per posició té realment cada imatge dins de cada sèrie i subvolum.
    void processPhasesPerPositionEvaluation(Image *image);
    
    /// Mètode que transforma l'estructura d'imatges ordenades a una llista i l'insereix a la sèrie.
    void setOrderedImagesIntoSeries(Series *series);

private:
    /// Auxiliary struct that holds a sample image and the number of images with a specific position and orientation.
    struct ImageAndCounter
    {
        /// One of the images in the represented position and orientation.
        Image *image;
        /// Number of images in the represented position and orientation.
        int count;
    };

    /**
     * @brief A hash that stores an ImageAndCounter for each encountered position and orientation.
     *
     * The key is a string constructed from the position and the orientation.
     */
    struct ImageAndCounterPerPosition : public QHash<QString, ImageAndCounter>
    {
        /// Returns the key to be used to insert the given image into the hash, created from its position and orientation.
        static QString hashKey(const Image *image);
        /// Updates this hash with the given image.
        void add(Image *image);
        /// Returns true if there are the same number of images in each position and orientation, and false otherwise.
        bool hasRegularPhases() const;
    };

    /// Stores an instance of ImageAndCounterPerPosition for each series and volume.
    QHash<Series*, QHash<int, ImageAndCounterPerPosition>> m_imageAndCounterPerPositionPerVolume;

    //  Angle       NormalVector    Distance    InstanceNumber0FrameNumber
    QMap<double, QMap<QString, QMap<double, QMap<unsigned long, Image*>*>*>*> *m_orderedNormalsSet;

    //    Series        Volume     Angle     NormalVector    Distance  InstanceNumber0FrameNumber
    QHash<Series*, QMap<int, QMap<double, QMap<QString, QMap<double, QMap<unsigned long, Image*>*>*>*>*>*> m_orderImagesInternalInfo;
   
    //    Series       Volume     AcqNumber MultipleAcqNumbers?
    QHash<Series*, QHash<int, QPair<QString, bool>*> > m_acquisitionNumberEvaluation;

    QVector3D m_firstPlaneVector3D;
    QVector3D m_direction;

    /// Tipus per definir un hash per comptar les fases corresponents a cada posició
    /// La clau del hash és un string amb la posició de la imatge (ImagePositionPatient) i el valor associat compta les ocurrències (fases) d'aquesta posició.
    /// Si tenim igual nombre de fases a totes les posicions, podem dir que és un volum amb fases
    typedef QHash<QString, int> PhasesPerPositionHashType;

    /// Hash en el que per cada sèrie, mapejem un hash que indica per cada volume number, quantes fases per posició té
    /// Aquest ens servirà en el post processat per decidir quins subvolums s'han d'ordenar per instance number 
    /// en cas que no totes les imatges d'un mateix subvolum no tinguin el mateix nombre de fases
    /// <Sèrie, <VolumeNumber, <PhasesPerPositionHash> > >
    QHash<Series*, QHash<int, PhasesPerPositionHashType*>*> m_phasesPerPositionEvaluation;

    /// Hash amb que per cada sèrie mapejem un hash on s'indica per cada número de subvolum, si totes les posicions tenen el mateix nombre de fases.
    /// En cas que no, cal ordenar el corresponent subvolum per instance number com en el cas dels m_acquisitionNumberEvaluation
    /// TODO Potser amb aquesta avaluació seria suficient i la que es fa per Acquisition Number es podria eliminar
    /// <Sèrie, <VolumeNumber, SameNumberOfPhasesPerPosition?> >
    QHash<Series*, QHash<int, bool>*> m_sameNumberOfPhasesPerPositionPerVolumeInSeriesHash;
};

}

#endif
