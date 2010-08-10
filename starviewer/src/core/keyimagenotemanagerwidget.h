#ifndef UDGKEYIMAGENOTEMANAGERWIDGET_H
#define UDGKEYIMAGENOTEMANAGERWIDGET_H

#include "ui_keyimagenotemanagerwidgetbase.h"

namespace udg {

class KeyImageNoteManager;
class KeyImageNote;
class KeyImageNoteCreatorWidget;
class KeyImageNoteDisplayer;

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

public slots:
    /// Mostra el dialeg per a crear un Key Image Note
    void showKeyImageNoteCreatorWidget();
    
    /// Creacio de un Key Image Note Displayer a partir d'un Key Image Note
    KeyImageNoteDisplayer* createKeyImageNoteDisplayer(KeyImageNote *keyImageNote);

    /// Esborra els elements seleccionats de la meva seleccio
    void deleteSelectedItemsFromCurrentSelection();

    /// Actualitza els Key Image Note Displayers colocant el KIN passat per parametre a sobre el layout i seguidament la resta
    void updateKeyImageNoteDisplayers(KeyImageNote *keyImageNote);

    /// Mostra el widget de la meva seleccio
    void showMySelectionWidget();

    /// Amaga el widget de la meva seleccio
    void hideMySelectionWidget();

    /// Activa / Desactiva el boto per a guardar el Kin i mostrar o no mostrar la meva seleccio segons tinguem o no imatges seleccionades
    void updateInterfaceStatus();

private:
    /// Inicialitza els Key Image Note Displayers
    void generateKeyImageNoteDisplayers();

    /// Crea les connexions amb el seu Key Image Note Manager
    void createConnectionsWithKeyImageNoteManager();

    /// Crea les connexions propies del Key Image Note Manager Widget
    void createConnections();

    /// Inicialitza el ThumbnailImageDisplayer de la meva seleccio
    void initializeThumbnailImageDisplayer();

private:
    /// Key Image Note Manager amb el qual interactua el Key Image Note Manager Widget
    KeyImageNoteManager *m_keyImageNoteManager;

    /// Retorna cert o fals dependent de si hem o no carregat les dades del seu Key Image Note Manager
    bool m_isKeyImageNoteManagerDataLoaded;

    /// Widget per a crear un Key Image Note
    KeyImageNoteCreatorWidget *m_keyImageNoteCreator;

    /// Llista dels Key Image Note Displayers que gestiona el Key Image Note Manager Widget
    QList<KeyImageNoteDisplayer*> m_keyImageNoteDisplayers;
};

}
#endif
