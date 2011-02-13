#ifndef UDGVOLUMEREADERJOB_H
#define UDGVOLUMEREADERJOB_H

#include <ThreadWeaver/Job>

namespace udg {

class Volume;

/**
    Classe que s'encarrega de llegir el pixel data d'un Volume en forma de job de ThreadWeaver, és a dir
    asíncronament.
    El comportament per defecte és que un cop executat el job i s'hagi retornat el signal jobDone aquest s'auto-esborrarà.
    Si es vol es pot canviar el comportament amb setAutoDelete(false) però caldrà tenir en compte que serà responsable
    de qui rebi el resultat del job encarregar-se de que aquest s'esborri correctament.
  */
class VolumeReaderJob : public ThreadWeaver::Job {
Q_OBJECT
public:
    /// Constructor, cal passar-li el volume del que es vol llegir el pixel data.
    VolumeReaderJob(Volume *volume, QObject *parent = 0);
    virtual ~VolumeReaderJob();

    /// Ens indica si el volume s'ha llegit correctament
    bool success() const;

    /// Indica si un cop executat el job, després del run, el job s'ha d'autoesborrar o és responsabilitat de qui crea el job.
    /// Per defecte està a true. Es fa amb deleteLater de manera que es pugui accedir al job just quan ha retornat del jobDone
    /// esborrant-se inmediatament després.
    void setAutoDelete(bool autoDelete);
    bool getAutoDelete() const;

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

private slots:
    /// Slot que s'encarrega d'esborrar el job si autoDelete és true
    void autoDelete();

private:
    Volume *m_volumeToRead;
    bool m_volumeReadSuccessfully;
    QString m_lastErrorMessageToUser;

    /// Ens indica si hem de fer un autoDelete al acabar el job
    bool m_autoDelete;
};

} // End namespace udg

#endif // VOLUMEREADERJOB_H
