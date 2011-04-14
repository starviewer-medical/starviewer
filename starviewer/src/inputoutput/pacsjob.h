#ifndef UDGPACSJOB_H
#define UDGPACSJOB_H

#include <QObject>
#include <ThreadWeaver/Job>
#include <ThreadWeaver/Thread>

#include "pacsdevice.h"

namespace udg {

using namespace ThreadWeaver;

/**
    Classe base de la qual herederan totes les operacions que es facin amb el PACS. Aquesta classe conté els mètodes basics que s'han d'heredar.

    Aquesta classe hereda de ThreadWeaver::Job per així tenir automàticament la gestió de les cues que implementa, i permetre que les operacions
    amb el PACS s'executin en un thread independent.
*/
class PACSJob: public Job
{
Q_OBJECT
public:

    enum PACSJobType {SendDICOMFilesToPACSJobType, RetrieveDICOMFilesFromPACSJobType, QueryPACS};

    ///Constructor de la classe
    PACSJob(PacsDevice pacsDevice);

    ///Retorna l'identificador del PACSJob aquest identificador és únic per tots els PACSJob
    int getPACSJobID();

    ///Retorna el PacsDevice amb el qual s'ha construït el PACSJob
    PacsDevice getPacsDevice();
    
    ///Indica quin tipus de PACSJob és l'objecte
    virtual PACSJob::PACSJobType getPACSJobType() = 0;

    ///Mètode heredad de Job que serveix per cancel·lar l'execució del job actual. Si el job no s'està executant i està encara encuant pendent d'executar-se
    ///aquest mètode no farà res per això s'aconsella no utilitzar aquest mètode, en lloc seu utilitzar requestCancelPACSJob de PACSManager que en el cas
    ///que el job s'estigui executa sol·licita que es pari l'execució i si està encuat el desencua perquè no s'arribi a executar.
    void requestAbort();

    ///Retorna si s'ha sol·licitat abortar el job
    bool isAbortRequested();

    ///Mètode heredat de Job, s'executa just abans de desencuar el job, si ens densencuen vol dir que el job no s'executarà per tant
    ///des d'aquest mètode emetem el signal PACSJobCancelled
    void aboutToBeDequeued(WeaverInterface *weaver);

signals:
    ///Signal que s'emet quan un PACSJob ha començat a executar-se
    void PACSJobStarted(PACSJob *);

    ///Signal que s'emet quan un PACSJob ha acabat d'executar-se
    void PACSJobFinished(PACSJob *);

    ///Signal que s'emet quan un PACSJob s'ha cancel·lat
    void PACSJobCancelled(PACSJob *);

private slots:

    ///Slot que s'activa quan el job actual de ThreadWeaver comença a executar-se
    void threadWeaverJobStarted();

    ///Slot que s'activa quan el job actual de ThreadWeaver ha finalitzat
    void threadWeaverJobDone();

private:

    ///Mètode que han de reimplementar les classes filles per cancel·lar l'execució del job actual
    virtual void requestCancelJob() = 0;

private :

    static int m_jobIDCounter;
    int m_jobID;
    PacsDevice m_pacsDevice;
    bool m_abortIsRequested;
};

};

#endif
