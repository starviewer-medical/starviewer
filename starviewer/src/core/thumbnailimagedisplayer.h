#ifndef UDGTHUMBNAILIMAGEDISPLAYER_H
#define UDGTHUMBNAILIMAGEDISPLAYER_H

#include "ui_thumbnailimagedisplayer.h"

namespace udg {

class Image;

/**
Widget que s'encarrega de mostrar un llistat d'imatges amb miniatura (thumbnails)

    @author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
//TODO: Aquest control s'hauria de fusionar amb QSeriesListWidget
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

signals:

    /// Determina que cal mostrar la icona amb la informacio de serie i imatge
    void show(const QString &seriesInstanceUID, const QString &imageInstanceUID);

public slots:

    /// Afegeix la previsualitzacio de la imatge image
    void addImage(Image *image);

    /// Elimina totes les previsualitzacions.
    void clearAllThumbnails();

    /// Obte la informacio d'un item i executa el signal de mostrar
    void showItem(QListWidgetItem *item);

private:
    /// Crea les connexions de signals i slots
    void createConnections();

private:
/// Guardem per cada imatge a la serie que pertany
    QHash<QString, QString> m_HashImageSeries;
};

}
#endif
