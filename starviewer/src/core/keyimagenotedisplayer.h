#ifndef UDGKEYIMAGENOTEDISPLAYER_H
#define UDGKEYIMAGENOTEDISPLAYER_H

#include "ui_keyimagenotedisplayerbase.h"

namespace udg {

class Image;
class KeyImageNote;

const int scaledImagesSizeX = 100;/// Mida de la previsualitzacio de la imatge escalada a l'eix x
const int scaledImagesSizeY = 100;/// Mida de la previsualitzacio de la imatge escalada a l'eix y

/**
Widget que s'encarrega de mostrar la informacio referida a un Key Image Note

    @author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class KeyImageNoteDisplayer : public QWidget , private Ui::KeyImageNoteDisplayerBase
{
Q_OBJECT
public:
    
    KeyImageNoteDisplayer(KeyImageNote *keyImageNote, QWidget *parent = 0);
    ~KeyImageNoteDisplayer();

private:
    
    /// Inicialitzacio del widget
    void initialize();

    /// Inicialitzem el widget d'imatges referenciades
    void initializeReferencedImagesWidget();

    /// Inserim una imatge al widget d'imatges referenciades
    void insertImage(Image *image);
    
    /// Referencia al Key Image Note que mostrem
    KeyImageNote *m_keyImageNote;
};

}
#endif