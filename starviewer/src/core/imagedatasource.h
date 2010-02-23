#ifndef UDGIMAGEDATASOURCE_H
#define UDGIMAGEDATASOURCE_H

#include <QString>

namespace udg {

/**
 * Aquesta classe encapsula la informació font a partir de les quals generem Images
 * Conté informació comuna i relativa a l'arxiu font. Múltiples Image poden apuntar
 * al mateix ImageDataSource.
 */
class ImageDataSource {
public:
    ImageDataSource();
    ~ImageDataSource();

    /// Assigna/obté el SOPInstanceUID de la instància
    void setSOPInstanceUID( const QString &uid );
    QString getSOPInstanceUID() const;

    /// Assigna/obté el número d'instància
    void setInstanceNumber( const QString &number );
    QString getInstanceNumber() const;

    /// Assigna/Obté el nombre de frames
    void setNumberOfFrames( int frames );
    int getNumberOfFrames() const;

    /// Ens diu si les dades són de tipus multi-frame
    bool isMultiFrame() const;

    /// Assignar/Obtenir la descripció del tipus d'imatge
    void setImageType( const QString &imageType );
    QString getImageType() const;
    
    /// Assigna/retorna el path absolut de la font de dades
    void setFilePath( const QString &path );
    QString getFilePath() const;

private:
    /// Atributs DICOM
    
    /// Identificador de la instància (0008,0018)
    QString m_SOPInstanceUID;

    /// Número d'instància (0020,0013) Tipus 2
    QString m_instanceNumber;
    
    /// Tipus d'imatge. Ens pot definir si es tracta d'un localizer, per exemple. Conté els valors separats per '\\'
    /// Es troba al mòdul General Image C.7.6.1 i als mòduls Enhanced MR/CT/XA/XRF Image (C.8.13.1/C.8.15.2/C.8.19.2)
    QString m_imageType;

    /// Nombre de frames de la imatge. (0028,0008) Tipus 1
    int m_numberOfFrames;

    /// Atributs NO-DICOM

    /// Path de l'arxiu font
    QString m_filePath;
};

}

#endif
