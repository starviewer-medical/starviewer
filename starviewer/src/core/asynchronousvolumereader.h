#ifndef UDGASYNCHRONOUSVOLUMEREADER_H
#define UDGASYNCHRONOUSVOLUMEREADER_H

#include <QObject>
#include <QHash>

#include <threadweaver/ResourceRestrictionPolicy.h>

namespace ThreadWeaver {
class Job;
class Weaver;
}

namespace udg {

class VolumeReaderJob;
class Volume;

/**
    Classe que permet llegir el pixel data d'un volume asíncronament.
    Tal i com està ara, necessita que aquesta es mantingui "viva" fins que el job de càrrega retorni
    com a finalitzat. Si es destrueix l'objecte abans, el comportament pot ser indefinit.
    Aquesta classe no es pot cridar de forma concurrent des de diferents threads.
  */

class AsynchronousVolumeReader : public QObject {
Q_OBJECT
public:
    explicit AsynchronousVolumeReader(QObject *parent = 0);

    /// Llegeix un Volume de manera asíncrona.
    /// Retorna un VolumeReaderJob per tal de poder saber quan és que aquest estarà carregat.
    VolumeReaderJob* read(Volume *volume);

    /// Cancel·la la càrrega de volume i, un cop cancel·lada, esborra volume.
    /// Si volume no s'està carregant, l'esborrarà directament.
    void cancelLoadingAndDeleteVolume(Volume *volume);

private slots:
    /// Marca el volume del job que se li passa conforme ja està carregat
    void unmarkVolumeFromJobAsLoading(ThreadWeaver::Job *job);

private:
    /// Ens indica si el volume que se li passa s'està carregant
    bool isVolumeLoading(Volume *volume) const;

    /// Marca el volume que se li passa conforme s'està carregant amb el job volumeReaderJob
    void markVolumeAsLoadingByJob(Volume *volume, VolumeReaderJob *volumeReaderJob);

    /// Desmarca el volume que se li passa conforme ja no s'està carregant.
    void unmarkVolumeAsLoading(Volume *volume);

    /// Ens retorna la instància de Weaver que hem de fer servir per treballar amb els jobs
    ThreadWeaver::Weaver* getWeaverInstance() const;

    /// Ens retorna el VolumeReaderJob del Volume que se li passi, si aquest té un job assignat que l'està llegint. Si no, retornarà null.
    VolumeReaderJob* getVolumeReaderJob(Volume *volume) const;

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
    static QHash<int, VolumeReaderJob*> m_volumesLoading;
    static ThreadWeaver::ResourceRestrictionPolicy m_resourceRestrictionPolicy;
};

} // End namespace udg

#endif // UDGASYNCHRONOUSVOLUMEREADER_H
