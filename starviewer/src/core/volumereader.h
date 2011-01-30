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

    /// Llegeix un Volume sense mostrar cap diàleg d'error
    /// TODO: S'hauria d'eliminar i fusionar amb read, ja que no s'hauria de mostrar cap error de tan avall
    bool readWithoutShowingError(Volume *volume);

    /// Mostra per pantalla un message box informant de l'error que s'ha produit al llegir el volum.
    /// Si no s'ha produit cap error, no fa res.
    void showMessageBoxWithLastError() const;

    /// Retorna un missatge que es pot mostrar a un usuari amb l'explicació de l'últim error.
    /// Si no hi ha cap "últim error" es retorna un QString buit.
    QString getLastErrorMessageToUser() const;

signals:
    /// Ens indica el progrés del procés de lectura
    void progress(int progress);

private:
    /// Executa el pixel reader i llegeix el volume
    void executePixelDataReader(Volume *volume);

    /// Grava un missatge de warning amb l'ultim error que s'ha produit al llegir el volum.
    /// Si no s'ha produit cap error, no fa res.
    void logWarningLastError(const QStringList &fileList) const;

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

    /// Guardem l'ultim error que s'ha produit al llegir
    int m_lastError;
};

} // End namespace udg

#endif
