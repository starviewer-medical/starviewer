#ifndef UDGKEYIMAGENOTEDISPLAYER_H
#define UDGKEYIMAGENOTEDISPLAYER_H

#include "ui_keyimagenotedisplayerbase.h"

namespace udg {

class Image;
class KeyImageNote;
class KeyImageNoteManager;

/**
Widget que s'encarrega de mostrar la informacio referida a un Key Image Note

    @author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class KeyImageNoteDisplayer : public QWidget, public Ui::KeyImageNoteDisplayerBase
{
Q_OBJECT
public:
    KeyImageNoteDisplayer(QWidget *parent = 0);
    ~KeyImageNoteDisplayer();

    /// Estableix amb quin Key Image Note Manager s'ha de comunicar i estableix les seves connexions
    void setKeyImageNoteManager(KeyImageNoteManager *m_keyImageNoteManager);

public slots:
    /// Canvia el Key Image Note a mostrar
    void setKeyImageNote(KeyImageNote *keyImageNote);

    /// Indica al Key Image Note Manager que s'ha de mostrar el KIN del Key Image Note displayer
    void showKeyImageNote();

private:
    /// Inicialitzacio del widget
    void fillKeyImageNoteDisplayer();

    /// Emplena el widget d'imatges referenciades
    void fillReferencedImagesWidget();

private:
    /// Referencia al Key Image Note que mostrem
    KeyImageNote *m_keyImageNote;

    /// Referencia al Key Image Note Manager al que pertany i es comunica
    KeyImageNoteManager *m_keyImageNoteManager;
};

}
#endif
