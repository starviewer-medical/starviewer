#ifndef UDGKEYIMAGENOTETOOLWIDGET_H
#define UDGKEYIMAGENOTETOOLWIDGET_H

#include "ui_keyimagenotetoolwidgetbase.h"

#include <QDialog>

namespace udg {

/** Widget que complementa la Key Image Note Tool, donada una llista de Kins d'una imatge
ens els representa, podem escollir el Key Image Note a visualitzar utilitzant un combobox*/
class KeyImageNote;
class KeyImageNoteManager;

class KeyImageNoteToolWidget : public QDialog, private Ui::KeyImageNoteToolWidgetBase
{
Q_OBJECT
public:
    KeyImageNoteToolWidget(QWidget *parent = 0);
    ~KeyImageNoteToolWidget();

    /// Canvia els Key Image Notes del widget
    void setKeyImageNotes(QList<KeyImageNote*> keyImageNotesOfImage);

    /// Estableix el Key Image Note Manager amb el que s'ha de comunicar
    void setKeyImageNoteManager(KeyImageNoteManager *keyImageNoteManager);
    
public slots:
    /// Canviem el Key Image Note mostrat pel que ocupa la posicio index de la llista de Key Image Notes
    void changeKeyImageNoteDisplayed(int index);

private:
    /// Crea les connexions del Key Image Note Tool Widget
    void createConnections();

    /// Emplena el combobox amb la informacio dels diferents Key Image Notes
    void fillKeyImageNoteSelectorComboBox();

private:
    /// Llista de Key Image Notes a mostrar
    QList<KeyImageNote*> m_keyImageNotesOfImage;

    /// Key Image Note Manager amb el que interactua la tool
    KeyImageNoteManager *m_keyImageNoteManager;
};

#endif

}