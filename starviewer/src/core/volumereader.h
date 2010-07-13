#ifndef UDGVOLUMEREADER_H
#define UDGVOLUMEREADER_H

#include <QObject>

namespace udg {

class Volume;
class VolumePixelDataReader;

/**
    Classe encarregada de llegir les dades que han de definir un volum.
    Li passem un punter a Volume i aquesta s'encarrega de llegir i assignar-li la informació adequada.
 */
class VolumeReader : public QObject {
Q_OBJECT
public:   
    VolumeReader(QObject *parent = 0);
    ~VolumeReader();

    /// Donat un punter vàlid a Volume, llegeix les dades
    /// corresponents i li assigna.
    void read(Volume *volume);

signals:
    /// Ens indica el progrés del procés de lectura
    void progress(int progress);

private:
    /// Ens retorna els arxius que hem de llegir i escull quin hauria de ser el lector adequat
    const QStringList chooseFilesAndSuitableReader(Volume *volume);

    /// Posa a punt el reader adequat
    void setUpReader();

private:
    /// Classe amb la qual llegirem les dades d'imatge del volum
    VolumePixelDataReader *m_volumePixelDataReader;

    /// Definim els tipus de lectors que podem fer servir segons les dades del volum
    enum SuitablePixelDataReader { ITKGDCMPixelDataReader, VTKGDCMPixelDataReader };
    
    /// Amb aquesta variable sabrem quin és el lector que hem de fer servir
    SuitablePixelDataReader m_suitablePixelDataReader;
};

} // End namespace udg

#endif
