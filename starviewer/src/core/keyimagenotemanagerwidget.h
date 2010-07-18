#ifndef UDGKEYIMAGENOTEMANAGERWIDGET_H
#define UDGKEYIMAGENOTEMANAGERWIDGET_H

#include "ui_keyimagenotemanagerwidgetbase.h"

namespace udg {

class KeyImageNoteManager;
class KeyImageNote;

/**
Widget que s'encarrega de mostrar els Key Image Notes

    @author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class KeyImageNoteManagerWidget : public QWidget, private Ui::KeyImageNoteManagerWidgetBase
{
Q_OBJECT
public:
    KeyImageNoteManagerWidget(QWidget *parent = 0);
    ~KeyImageNoteManagerWidget();

    /// Assignar el Key Image Note Manager al Key Image Note Manager Widget, establint les seves connexions.
    void setKeyImageNoteManager(KeyImageNoteManager *keyImageNoteManager);

    /// Inicialitza el Key Image Note Manager Widget si no te les dades del Key Image Note Manager carregades
    void showKeyImageNoteManagerWidget();

private:
    /// Inicialitza els Key Image Note Displayers
    void generateKeyImageNoteDisplayers();

    /// Creacio de un Key Image Note Displayer a partir d'un Key Image Note
    void createKeyImageNoteDisplayer(KeyImageNote *keyImageNote);

    /// Crea les connexions de signals i slots del Key Image Note Manager Widget
    void createConnections();

private:
    /// Key Image Note Manager amb el qual interactua el Key Image Note Manager Widget
    KeyImageNoteManager *m_keyImageNoteManager;

    /// Retorna cert o fals dependent de si hem o no carregat les dades del seu Key Image Note Manager
    bool m_isKeyImageNoteManagerDataLoaded;
};

}
#endif
