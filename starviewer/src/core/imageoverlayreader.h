#ifndef UDGIMAGEOVERLAYREADER_H
#define UDGIMAGEOVERLAYREADER_H

#include <QList>

#include "imageoverlay.h"

namespace gdcm {
class Image;
}

namespace udg {

class ImageOverlay;

/**
    Classe per llegir overlays a través d'un arxiu. Per llegir els overlays caldrà assignar primer el nom de l'arxiu
    del que volem llegir els overlays i després fer-ne la lectura. Un cop feta la lectura podrem obtenir els overlays amb getOverlays()
    
    Exemple:
    \code
    ImageOverlayReader reader;
    reader.setFilename("C:\samples\image.dcm");
    if (read())
    {
        return reader.getOverlays();
    }
    \endcode
 */
class ImageOverlayReader {
public:
    ImageOverlayReader();
    ~ImageOverlayReader();

    /// Nom del fitxer a partir del que hem de llegir l'overlay
    void setFilename(const QString &filename);

    /// Un cop donat un nom d'arxiu, en llegeix els overlays. Retorna fals en cas d'error, cert altrament
    bool read();

    /// Retorna la llista d'overlays llegits
    QList<ImageOverlay> getOverlays() const;

private:
    /// Ens retorna una gdcm::Image a partir del nom de fitxer especificat. Retornarà nul en cas d'error
    virtual gdcm::Image* getGDCMImageFromFile(const QString &filename);

private:
    /// Nom de l'arxiu del que hem de llegir els overlays
    QString m_filename;

    /// Llista d'overlays llegits
    QList<ImageOverlay> m_overlaysList;
};

}

#endif
