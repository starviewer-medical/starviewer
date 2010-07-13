#ifndef UDGVOLUMEPIXELDATAREADER_H
#define UDGVOLUMEPIXELDATAREADER_H

#include <QObject>

class vtkImageData;

namespace udg {

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
    enum PixelDataReadError { NoError = 1, SizeMismatch, InvalidFileName, MissingFile, OutOfMemory, ZeroSpacingNotAllowed, UnknownError };
    
    VolumePixelDataReader(QObject *parent = 0);
    ~VolumePixelDataReader();

    /// Donada una llista de noms de fitxer, la llegeix i omple
    /// l'estructura d'imatge que fem servir internament.
    /// Ens retorna un enter que ens indicarà si hi ha hagut alguna mena d'error en el 
    /// procés de lectura, seguint els valors enumerats definits a aquesta mateixa classe
    virtual int read(const QStringList &filenames) = 0;
    
    /// Ens retorna les dades en format vtkImageData
    vtkImageData* getVTKData();

signals:
    /// Ens indica el progrés del procés de lectura
    void progress(int progress);

protected:
    /// Les dades d'imatge en format vtk
    vtkImageData *m_vtkImageData;

};

} // End namespace udg

#endif
