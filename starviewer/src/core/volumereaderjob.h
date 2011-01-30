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

    /// Ens retorna l'error en format visible per l'usuari de la última execució del job.
    /// Si no hi ha error, retorna cadena buida.
    /// TODO: Ens veiem obligats a fer-ho així de moment, per com està a VolumeReader. Idealment aquí només es retornaria
    /// el codi d'error i és des de la interfície que es converteix en missatge a l'usuari.
    QString getLastErrorMessageToUser() const;

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
    QString m_lastErrorMessageToUser;
};

} // End namespace udg

#endif // VOLUMEREADERJOB_H
