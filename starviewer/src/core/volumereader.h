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
    /// Definim els tipus de lectors que podem fer servir segons les dades del volum
    enum PixelDataReaderType { ITKGDCMPixelDataReader, VTKGDCMPixelDataReader };

    /// Ens retorna els arxius que hem de llegir, sense retornar-ne cap de repetit
    QStringList getFilesToRead(Volume *volume) const;

    /// Retorna el tipus de lector mes adequat per llegir el volume que se li passa.
    PixelDataReaderType getSuitableReader(Volume *volume) const;

    /// Crea i posa a punt el tipus de reader que se li passa per poder llegir a continuacio
    void setUpReader(PixelDataReaderType readerType);

private:
    /// Classe amb la qual llegirem les dades d'imatge del volum
    VolumePixelDataReader *m_volumePixelDataReader;
};

} // End namespace udg

#endif
