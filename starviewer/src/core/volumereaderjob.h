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

#ifndef UDGVOLUMEREADERJOB_H
#define UDGVOLUMEREADERJOB_H

#include <ThreadWeaver/Job>

#include "identifier.h"

#include <QPointer>
#include <QMutex>

namespace udg {

class Volume;
class VolumeReader;

/**
    Classe que s'encarrega de llegir el pixel data d'un Volume en forma de job de ThreadWeaver, és a dir
    asíncronament.
  */
class VolumeReaderJob : public QObject, public ThreadWeaver::Job {
Q_OBJECT
public:
    /// Constructor, cal passar-li el volume del que es vol llegir el pixel data.
    VolumeReaderJob(Volume *volume, QObject *parent = 0);
    virtual ~VolumeReaderJob();

    /// Ens permet demanar, de manera asíncrona, que es cancel·li el job.
    /// El mètode retornarà inmediatament però el job no es cancel·larà fins al cap d'una estona.
    virtual void requestAbort();

    /// Ens indica si el volume s'ha llegit correctament. Si es fa un request abort, es retornarà que no s'ha llegit correctament.
    bool success() const;

    /// Ens retorna l'error en format visible per l'usuari de la última execució del job.
    /// Si no hi ha error, retorna cadena buida.
    /// TODO: Ens veiem obligats a fer-ho així de moment, per com està a VolumeReader. Idealment aquí només es retornaria
    /// el codi d'error i és des de la interfície que es converteix en missatge a l'usuari.
    QString getLastErrorMessageToUser() const;

    /// Retorna el volume
    Volume* getVolume() const;
    /// Returns the identifier of the volume, even if the volume is destructed.
    const Identifier& getVolumeIdentifier() const;

signals:
    /// Signal que s'emet amb el progrés de lectura
    void progress(VolumeReaderJob*, int progress);
    void done(ThreadWeaver::JobPointer);

protected:
    /// Mètode on realment es fa la càrrega. S'executa en un thread de threadweaver.
    virtual void run(ThreadWeaver::JobPointer self, ThreadWeaver::Thread *thread);
    virtual void defaultEnd(const ThreadWeaver::JobPointer &job, ThreadWeaver::Thread *thread);

private slots:
    /// Slot to emit the current progress
    void updateProgress(int value);
private:
    Volume *m_volumeToRead;
    /// Keeps the identifier of the volume to have access to it even if the volume is deleted.
    Identifier m_volumeIdentifier;
    bool m_volumeReadSuccessfully;
    QString m_lastErrorMessageToUser;

    /// Ens indica si s'ha fet o no un requestAbort
    bool m_abortRequested;

    /// Referència al volume reader per poder fer un requestAbort. Només serà vàlid mentre s'estigui executant "run()", a fora d'aquest no ho serà.
    /// Nota: no es pot fer el volumeReader membre de la classe ja que aquest crea objectes de Qt fills de "this" i this apuntaria a threads diferents
    /// (un a apuntaria al de gui, per ser crear al constructor, i els altres al del thread de threadweaver, per ser creats al run()).
    QPointer<VolumeReader> m_volumeReaderToAbort;

    /// Mutex per protegir els canvis de referència a m_volumeReaderToAbort en escenaris de multithreading.
    QMutex m_volumeReaderToAbortMutex;
};

} // End namespace udg

#endif // VOLUMEREADERJOB_H
