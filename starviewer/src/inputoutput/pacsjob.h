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

#ifndef UDGPACSJOB_H
#define UDGPACSJOB_H

#include <QObject>
#include <ThreadWeaver/Job>
#include <ThreadWeaver/Thread>

#include "pacsdevice.h"

namespace ThreadWeaver {

class WeaverInterface;

}

namespace udg {

class PACSJob;
typedef QSharedPointer<PACSJob> PACSJobPointer;

/**
    Classe base de la qual herederan totes les operacions que es facin amb el PACS. Aquesta classe conté els mètodes basics que s'han d'heredar.

    Aquesta classe hereda de ThreadWeaver::Job per així tenir automàticament la gestió de les cues que implementa, i permetre que les operacions
    amb el PACS s'executin en un thread independent.
  */
class PACSJob : public QObject, public ThreadWeaver::Job {
Q_OBJECT
public:
    enum PACSJobType { SendDICOMFilesToPACSJobType, RetrieveDICOMFilesFromPACSJobType, QueryPACS };

    /// Constructor de la classe
    PACSJob(PacsDevice pacsDevice);

    /// Retorna l'identificador del PACSJob aquest identificador és únic per tots els PACSJob
    int getPACSJobID();

    /// Retorna el PacsDevice amb el qual s'ha construït el PACSJob
    PacsDevice getPacsDevice();

    /// Indica quin tipus de PACSJob és l'objecte
    virtual PACSJob::PACSJobType getPACSJobType() = 0;

    /// Mètode heredad de Job que serveix per cancel·lar l'execució del job actual. Si el job no s'està executant i està encara encuant pendent d'executar-se
    /// aquest mètode no farà res per això s'aconsella no utilitzar aquest mètode, en lloc seu utilitzar requestCancelPACSJob de PACSManager que en el cas
    /// que el job s'estigui executa sol·licita que es pari l'execució i si està encuat el desencua perquè no s'arribi a executar.
    void requestAbort();

    /// Retorna si s'ha sol·licitat abortar el job
    bool isAbortRequested();

    /// Mètode heredat de Job, s'executa just abans de desencuar el job, si ens densencuen vol dir que el job no s'executarà per tant
    /// des d'aquest mètode emetem el signal PACSJobCancelled
    void aboutToBeDequeued(ThreadWeaver::QueueAPI *weaver);

    /// Sets the self pointer reference of this job.
    void setSelfPointer(const PACSJobPointer &self);

signals:
    /// Signal que s'emet quan un PACSJob ha començat a executar-se
    void PACSJobStarted(PACSJobPointer);

    /// Signal que s'emet quan un PACSJob ha acabat d'executar-se
    void PACSJobFinished(PACSJobPointer);

    /// Signal que s'emet quan un PACSJob s'ha cancel·lat
    void PACSJobCancelled(PACSJobPointer);

protected:
    virtual void defaultBegin(const ThreadWeaver::JobPointer &job, ThreadWeaver::Thread *thread);
    virtual void defaultEnd(const ThreadWeaver::JobPointer &job, ThreadWeaver::Thread *thread);

protected:
    /// Weak reference to a shared pointer of the job itself. It is needed to emit the PACSJobCancelled() signal with a shared pointer from aboutToBeDequeued().
    /// Since it's a weak pointer it won't keep the job alive.
    /// TODO This should be removed by redesigning the PACS jobs architecture.
    QWeakPointer<PACSJob> m_selfPointer;

private:
    /// Mètode que han de reimplementar les classes filles per cancel·lar l'execució del job actual
    virtual void requestCancelJob() = 0;

private:
    static int m_jobIDCounter;
    int m_jobID;
    PacsDevice m_pacsDevice;
    bool m_abortIsRequested;
};

};

Q_DECLARE_METATYPE(QSharedPointer<udg::PACSJob>)

#endif
