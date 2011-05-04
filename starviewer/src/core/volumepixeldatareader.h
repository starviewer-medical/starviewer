#ifndef UDGVOLUMEPIXELDATAREADER_H
#define UDGVOLUMEPIXELDATAREADER_H

#include <QObject>

namespace udg {

class VolumePixelData;

/**
    Classe encarregada de llegir únicament les dades de píxel d'arxius d'imatge mèdica.
    Aquesta classe únicament actuarà d'interfície pels diferents possibles motors de lectura
    que podem tenir (itk, gdcm, dcmtk, etc).
    Les dades es retornaran en format vtkImageData.
 */
class VolumePixelDataReader : public QObject {
Q_OBJECT
public:
    /// Tipus d'error que podem tenir al llegir
    enum PixelDataReadError { NoError = 1, SizeMismatch, InvalidFileName, MissingFile, OutOfMemory, ZeroSpacingNotAllowed, CannotReadFile, ReadAborted, UnknownError };

    VolumePixelDataReader(QObject *parent = 0);
    ~VolumePixelDataReader();

    /// Donada una llista de noms de fitxer, la llegeix i omple
    /// l'estructura d'imatge que fem servir internament.
    /// Ens retorna un enter que ens indicarà si hi ha hagut alguna mena d'error en el
    /// procés de lectura, seguint els valors enumerats definits a aquesta mateixa classe
    virtual int read(const QStringList &filenames) = 0;

    /// Mètode que cal reimplementar per tal de permètre cancel·lar una petició de lectura iniciada amb read().
    /// En el cas de que no estigui realitzant un read, no farà res.
    /// En el cas que estigui en mig d'una operació de lectura, caldrà retornar com a resultat un error ReadAborted.
    virtual void requestAbort() = 0;

    /// Ens retorna les dades llegides
    VolumePixelData* getVolumePixelData();

signals:
    /// Ens indica el progrés del procés de lectura
    void progress(int progress);

protected:
    /// Les dades d'imatge en format vtk
    VolumePixelData *m_volumePixelData;

};

} // End namespace udg

#endif
