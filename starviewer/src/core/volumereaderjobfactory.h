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

#ifndef UDGASYNCHRONOUSVOLUMEREADER_H
#define UDGASYNCHRONOUSVOLUMEREADER_H

#include <QObject>
#include "singleton.h"

#include <QHash>

#include <ThreadWeaver/ResourceRestrictionPolicy>

namespace ThreadWeaver {
class Job;
class Queue;
}

namespace udg {

class Identifier;
class VolumeReaderJob;
class Volume;

/**
    Classe que permet llegir el pixel data d'un volume asíncronament.
    Tal i com està ara, necessita que aquesta es mantingui "viva" fins que el job de càrrega retorni
    com a finalitzat. Si es destrueix l'objecte abans, el comportament pot ser indefinit.
    Aquesta classe no es pot cridar de forma concurrent des de diferents threads.
  */

class VolumeReaderJobFactory : public QObject, public SingletonPointer<VolumeReaderJobFactory> {
Q_OBJECT
public:
    /// Starts reading the given volume asynchronously. Returns the job that performs the reading.
    QSharedPointer<VolumeReaderJob> read(Volume *volume);

    /// Cancel·la la càrrega de volume i, un cop cancel·lada, esborra volume.
    /// Si volume no s'està carregant, l'esborrarà directament.
    void cancelLoadingAndDeleteVolume(Volume *volume);

protected:
    friend class SingletonPointer<VolumeReaderJobFactory>;
    explicit VolumeReaderJobFactory(QObject *parent = 0);
    ~VolumeReaderJobFactory();

private slots:
    /// Marca el volume del job que se li passa conforme ja està carregat
    void unmarkVolumeFromJobAsLoading(ThreadWeaver::JobPointer job);

private:
    /// Ens indica si el volume que se li passa s'està carregant
    bool isVolumeLoading(Volume *volume) const;

    /// Marca el volume que se li passa conforme s'està carregant amb el job volumeReaderJob
    void markVolumeAsLoadingByJob(Volume *volume, QSharedPointer<VolumeReaderJob> volumeReaderJob);

    /// Desmarca el volume que se li passa conforme ja no s'està carregant.
    void unmarkVolumeAsLoading(const Identifier &volumeIdentifier);

    /// Ens retorna la instància de Weaver que hem de fer servir per treballar amb els jobs
    ThreadWeaver::Queue* getWeaverInstance() const;

    /// Ens retorna el VolumeReaderJob del Volume que se li passi, si aquest té un job assignat que l'està llegint. Si no, retornarà null.
    QSharedPointer<VolumeReaderJob> getVolumeReaderJob(Volume *volume) const;

    /// Assigna una política restrictiva si tenim el setting MaximumNumberOfVolumesLoadingConcurrently definit o si
    /// estem a windows 32 bits i hi ha possibilitat d'obrir volums que requereixin molta memòria.
    void assignResourceRestrictionPolicy(VolumeReaderJob *volumeReaderJob);

    /// Comprova si els volums del repositori compleixen alguna de les restriccions
    /// Si checkMultiframeImages és true, es comprova si existeixen imatges multiframe com a restricció
    /// Si modalitiesWithoutRestriction no és buit, qualsevol modalitat trobada que no es trobi a la llista es considerarà restricció
    /// Si retorna cert vol dir que s'han trobar les restriccions, fals altrament
    bool checkForResourceRestrictions(bool checkMultiframeImages, const QStringList &modalitiesWithoutRestriction);

private:
    /// Llista dels volums que s'estan carregant
    QHash<int, QSharedPointer<VolumeReaderJob> > m_volumesLoading;
    ThreadWeaver::ResourceRestrictionPolicy m_resourceRestrictionPolicy;
};

} // End namespace udg

#endif // UDGASYNCHRONOUSVOLUMEREADER_H
