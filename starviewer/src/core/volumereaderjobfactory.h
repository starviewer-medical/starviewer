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
    /// Starts reading the given volume asynchronously. The factory will use the given requester in the emitted signals.
    /// It is recommended to pass the this pointer as requester.
    void read(void *requester, Volume *volume);

    /// Removes the requester from the list of requesters of the given volume.
    void cancelRead(void *requester, Volume *volume);
    // If the volume does not have any other requester, it should request to abort reading but we can't do it (see comments in implementation).

    /// Cancel·la la càrrega de volume i, un cop cancel·lada, esborra volume.
    /// Si volume no s'està carregant, l'esborrarà directament.
    void cancelLoadingAndDeleteVolume(Volume *volume);

signals:
    /// Emitted to update progress on a requested volume. The requester is the one given in read.
    void volumeReadingProgress(void *requester, Volume *volume, int progress);
    /// Emitted to notify the a volume has finished reading. The requester is the one given in read.
    /// \todo Could avoid exposing the job to the outside.
    void volumeReadingFinished(void *requester, VolumeReaderJob *job);

protected:
    friend class SingletonPointer<VolumeReaderJobFactory>;
    explicit VolumeReaderJobFactory(QObject *parent = 0);
    ~VolumeReaderJobFactory();

private slots:
    /// Emits volumeReadingProgress for each requester of the volume of this job.
    void onJobProgress(ThreadWeaver::JobPointer job, int progress);
    /// Emits volumeReadingFinished for each requester of the volume of this job and then removes the corresponding values from the hashes.
    void onJobDone(ThreadWeaver::JobPointer job);

private:
    /// Ens indica si el volume que se li passa s'està carregant
    bool isVolumeLoading(Volume *volume) const;

    /// Ens retorna la instància de Weaver que hem de fer servir per treballar amb els jobs
    ThreadWeaver::Queue* getWeaverInstance() const;

    /// Ens retorna el VolumeReaderJob del Volume que se li passi, si aquest té un job assignat que l'està llegint. Si no, retornarà null.
    QSharedPointer<VolumeReaderJob> getVolumeReaderJob(Volume *volume) const;

    /// Assigna una política restrictiva si tenim el setting MaximumNumberOfVolumesLoadingConcurrently definit o si
    /// estem a windows 32 bits i hi ha possibilitat d'obrir volums que requereixin molta memòria.
    void assignResourceRestrictionPolicy(VolumeReaderJob *volumeReaderJob);

private:
    /// Llista dels volums que s'estan carregant
    QHash<int, QSharedPointer<VolumeReaderJob> > m_volumesLoading;
    /// Maps a volume id to its requesters.
    QMultiHash<int, void*> m_volumeRequesters;
    ThreadWeaver::ResourceRestrictionPolicy m_resourceRestrictionPolicy;
};

} // End namespace udg

#endif // UDGASYNCHRONOUSVOLUMEREADER_H
