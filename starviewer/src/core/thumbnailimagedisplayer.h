#ifndef UDGTHUMBNAILIMAGEDISPLAYER_H
#define UDGTHUMBNAILIMAGEDISPLAYER_H

#include "ui_thumbnailimagedisplayer.h"

namespace udg {

class Image;

const int ScaledSmallThumbnailsSizeX = 50;/// Mida de la previsualitzacio de la imatge escalada a l'eix x Petita
const int ScaledSmallThumbnailsSizeY = 50;/// Mida de la previsualitzacio de la imatge escalada a l'eix y Petita

const int ScaledMediumThumbnailsSizeX = 100;/// Mida de la previsualitzacio de la imatge escalada a l'eix x Mitjana
const int ScaledMediumThumbnailsSizeY = 100;/// Mida de la previsualitzacio de la imatge escalada a l'eix y Mitjana

const int ScaledBigThumbnailsSizeX = 200;/// Mida de la previsualitzacio de la imatge escalada a l'eix x Gran
const int ScaledBigThumbnailsSizeY = 200;/// Mida de la previsualitzacio de la imatge escalada a l'eix y Gran
/**
Widget que s'encarrega de mostrar un llistat d'imatges amb miniatura (thumbnails)

    @author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class ThumbnailImageDisplayer : public QWidget, private Ui::ThumbnailImageDisplayer
{
Q_OBJECT
public:
    /// Enum per a les diferents mides, els diferents tamanys estan definits en constants.
    enum ThumbnailSize {Small, Medium, Big};

    ThumbnailImageDisplayer(QWidget *parent = 0);
    ~ThumbnailImageDisplayer();
    
    /// Estableix la mida de les previsualitzacions
    void setThumbnailSize(ThumbnailSize thumbnailSize);

 public slots:

    /// Afegeix la previsualitzacio de la imatge image
    void addImage(Image *image);

    /// Elimina totes les previsualitzacions.
    void clearAllThumbnails();
};

}
#endif
