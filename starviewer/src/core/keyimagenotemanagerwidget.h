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
    KeyImageNoteManagerWidget(KeyImageNoteManager *keyImageNoteManager, QWidget *parent = 0);
    ~KeyImageNoteManagerWidget();

private:
    /// Inicialitza el Key Image Note Manager Widget
    void initialize();
    
    /// Inicialitza els Key Image Note Displayers
    void generateKeyImageNoteDisplayers();

    /// Creacio un Key Image Note Displayer a partir d'un Key Image Note
    void createKeyImageNoteDisplayer(KeyImageNote *keyImageNote);

    /// Key Image Note Manager amb el qual interactua el Key Image Note Manager Widget
    KeyImageNoteManager *m_keyImageNoteManager;
};

}
#endif
