#ifndef UDGTHUMBNAILIMAGEDISPLAYER_H
#define UDGTHUMBNAILIMAGEDISPLAYER_H

#include "ui_thumbnailimagedisplayer.h"
#include <QMenu>

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

    /// Estableix el menú contextual del Thumbnail Image Displayer
    void setContextMenu(QMenu *contextMenu);

    // TODO: Tambe ha de tornar el numero de frame
    /// Retorna UID de l'element eliminat
    QString removeSelectedItems();

signals:
    /// Determina que cal mostrar la icona amb la informacio de imatge
    void show(const QString &imageInstanceUID);

public slots:
    /// Afegeix la previsualitzacio de la imatge image
    void addImage(Image *image);

    /// Elimina totes les previsualitzacions.
    void clearAllThumbnails();

    /// Obte la informacio d'un item i executa el signal de mostrar
    void showItem(QListWidgetItem *item);

    /// Elimina l'element amb UID sopInstaceUID si existeix
    void removeItem(const QString &sopInstanceUID);

private:
    /// Crea les connexions de signals i slots
    void createConnections();

protected:
    /// Mostra el menu contextual del Thumbnail Image Displayet
    void contextMenuEvent(QContextMenuEvent *event);

private:
    // TODO: Falta considerar el numero de frame, guardar-lo i retornar-lo quan faci falta

    /// Ens guardem el menu contextual
    QMenu  *m_contextMenu;
};

}
#endif
