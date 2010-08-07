#ifndef UDGKEYIMAGENOTECREATORWIDGET_H
#define UDGKEYIMAGENOTECREATORWIDGET_H

#include "ui_keyimagenotecreatorwidgetbase.h"

#include <QDialog>

namespace udg {

class KeyImageNoteManager;

/**
Widget que s'encarrega de recollir les dades d'un Key Image Note i enviar-les al seu Key Image Note Manager

    @author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class KeyImageNoteCreatorWidget : public QDialog, private Ui::KeyImageNoteCreatorWidgetBase
{
Q_OBJECT
public:
    KeyImageNoteCreatorWidget(QWidget *parent, KeyImageNoteManager *keyImageNoteManager);
    ~KeyImageNoteCreatorWidget();

public slots:
    /// Envia les dades recollides al Key Image Note Manager per a guardar un Key Image Note
    void createKeyImageNote();

    /// Fa visible o invisible el combobox de Rejected For Quality Reasons segons el document Title seleccionat
    void setVisibilityOfRejectedForQualityReasons(const QString &documentTitle);

    /// S'utilitza per actualizar diferents elements segons quines destinacions tenim seleccionades
    void destinationsChanged(bool checked);

private:
    /// Crea les connexions entre el Key Image Note Creator Widget i el seu Key Image Note Manager
    void createConnections();

    /// Estableix els valors per defecte de textedit i combobox del Key Image Note Creator Widget
    void setDefaultValuesOfKeyImageNoteCreatorWidget();

private:
    /// Referencia del Key Image Note Manager amb el que interacturara
    KeyImageNoteManager *m_keyImageNoteManager;
};

}
#endif