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
    /// TODO: Fer que es retornin tipus d'errors i no com ara, que només es retorna true o false i així treure responsabilitat d'ui als readers
    bool readWithoutShowingError(Volume *volume);

    /// Demana que s'aborti el procés iniciat amb read() o readWithoutShowingError() de lectura d'un Volume
    /// Si no s'ha iniciat cap procés de lectura, no farà res. Un cop cridat la lectura retornarà amb error.
    void requestAbort();

    /// Mostra per pantalla un message box informant de l'error que s'ha produit al llegir el volum.
    /// Si no s'ha produit cap error, no fa res.
    void showMessageBoxWithLastError() const;

    /// Retorna un missatge que es pot mostrar a un usuari amb l'explicació de l'últim error.
    /// Si no hi ha cap "últim error" es retorna un QString buit.
    QString getLastErrorMessageToUser() const;

signals:
    /// Ens indica el progrés del procés de lectura
    /// TODO: De moment quan es vulgui llegir només un fitxer, p.ex. multiframes, mamos, etc. per limitacions de la lectura,
    /// no tindrem cap tipus de progrés.
    void progress(int progress);

private:
    /// Executa el pixel reader i llegeix el volume
    void executePixelDataReader(Volume *volume);

    /// Grava un missatge de warning amb l'ultim error que s'ha produit al llegir el volum.
    /// Si no s'ha produit cap error, no fa res.
    void logWarningLastError(const QStringList &fileList) const;

    /// Arregla l'spacing en els casos que sabem que les llibreries fallen en aquest càlcul
    void fixSpacingIssues(Volume *volume);

private:
    /// Definim els tipus de lectors que podem fer servir segons les dades del volum
    enum PixelDataReaderType { ITKGDCMPixelDataReader, VTKGDCMPixelDataReader };

    /// Ens retorna els arxius que hem de llegir, sense retornar-ne cap de repetit
    QStringList getFilesToRead(Volume *volume) const;

    /// Retorna el tipus de lector mes adequat per llegir el volume que se li passa.
    PixelDataReaderType getSuitableReader(Volume *volume) const;

    /// Ens diu si existeix algun setting que indiqui que s'hagi de forçar la lectura
    /// del volum amb una llibreria específica i quina és aquesta
    /// Els settings comprovats, amb el següent ordre de prioritat, són ForcedImageReaderLibrary,
    /// ForceITKImageReaderForSpecifiedModalities i ForceVTKImageReaderForSpecifiedModalities
    /// En cas que es torni fals, indicarà que s'ha de seguir el comportament normal i el valor de @param forcedReaderLibrary serà indefinit
    /// ATENCIÓ: L'ús d'aquests settings està reservat exclusivament per a casos molt concrets que podrien servir per
    /// proporcionar un workaround temporal en cas que alguna cas funcioni malament
    bool mustForceReaderLibraryBackdoor(Volume *volume, PixelDataReaderType &forcedReaderLibrary) const;

    /// Crea i posa a punt el tipus de reader que se li passa per poder llegir a continuació.
    /// Podem especificar si volem que el reader ens doni el progrés o no.
    void setUpReader(PixelDataReaderType readerType, bool showProgress);

private:
    /// Classe amb la qual llegirem les dades d'imatge del volum
    VolumePixelDataReader *m_volumePixelDataReader;

    /// Guardem l'ultim error que s'ha produit al llegir
    int m_lastError;
};

} // End namespace udg

#endif
