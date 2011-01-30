#ifndef UDGVOLUMEREADERJOB_H
#define UDGVOLUMEREADERJOB_H

#include <ThreadWeaver/Job>

namespace udg {

class Volume;

/**
    Classe que s'encarrega de llegir el pixel data d'un Volume en forma de job de ThreadWeaver, és a dir
    asíncronament.
  */
class VolumeReaderJob : public ThreadWeaver::Job {
Q_OBJECT
public:
    /// Constructor, cal passar-li el volume del que es vol llegir el pixel data.
    VolumeReaderJob(Volume *volume, QObject *parent = 0);
    virtual ~VolumeReaderJob();

    /// Ens indica si el volume s'ha llegit correctament
    bool success() const;

    /// Retorna el volume
    Volume* getVolume() const;

signals:
    /// Signal que s'emet amb el progrés de lectura
    void progress(int progress);

protected:
    /// Mètode on realment es fa la càrrega. S'executa en un thread de threadweaver.
    void run();

private:
    Volume *m_volumeToRead;
    bool m_volumeReadSuccessfully;
};

} // End namespace udg

#endif // VOLUMEREADERJOB_H
