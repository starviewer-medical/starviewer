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

#include "vector3.h"

#include <QMap>
#include <QHash>

namespace udg {

class Patient;
class Series;
class Image;

/**
    Mòdul que s'encarrega d'ordenar correctament les imatges de les sèries. Un dels seus requisits és que es tingui l'etiqueta de DICOMClassified,
    la ImageFillerStep i el TemporalDimensionFillerStep.
  */
class OrderImagesFillerStep : public PatientFillerStep {
public:
    OrderImagesFillerStep();

    ~OrderImagesFillerStep();

    bool fillIndividually();

    void postProcessing();

private:
    /// Mètodes per processar la informació específica de series
    void processImage(Image *image);

    /// Mètode per calcular quantes fases per posició té realment cada imatge dins de cada sèrie i subvolum.
    void processPhasesPerPositionEvaluation(Image *image);
    
    /// Mètode que transforma l'estructura d'imatges ordenades a una llista i l'insereix a la sèrie.
    void setOrderedImagesIntoSeries(Series *series);

    //  Angle       NormalVector    Distance    InstanceNumber0FrameNumber
    QMap<double, QMap<QString, QMap<double, QMap<unsigned long, Image*>*>*>*> *m_orderedNormalsSet;

    //    Series        Volume     Angle     NormalVector    Distance  InstanceNumber0FrameNumber
    QHash<Series*, QMap<int, QMap<double, QMap<QString, QMap<double, QMap<unsigned long, Image*>*>*>*>*>*> m_orderImagesInternalInfo;
   
    //    Series       Volume     AcqNumber MultipleAcqNumbers?
    QHash<Series*, QHash<int, QPair<QString, bool>*> > m_acquisitionNumberEvaluation;

    Vector3 m_firstPlaneVector3D;
    Vector3 m_direction;

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
