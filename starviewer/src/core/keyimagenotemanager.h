#ifndef UDGKEYIMAGENOTEMANAGER_H
#define UDGKEYIMAGENOTEMANAGER_H

#include <QObject>

namespace udg {

class Patient;
class Image;
class KeyImageNote;

/**
Classe que s'encarrega de gestionar els Key Image Notes d'un pacient i mantenir la seleccio activa de l'usuari

    @author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class KeyImageNoteManager : public QObject
{
Q_OBJECT
public:
    KeyImageNoteManager(Patient *patient);
    ~KeyImageNoteManager();

    /// Retorna tots els Key Image Notes del pacient
    QList<KeyImageNote*> getKeyImageNotesOfPatient();

    /// Afegeix una imatge a la seleccio actual si no la tenim
    void addImageToTheCurrentSelectionOfImages(Image *image);

signals:
    /// Senyala que s'ha afegit una nova imatge a la seleccio
    void imageAddedToTheCurrentSelectionOfImages(Image *image);

    /// Senyala que s'ha netejat la seleccio actual
    void currentSelectionCleared();

private:
    /// Cerca de tots els Key Image Notes
    void searchKeyImageNotes();

private:
    /// El pacient que estem tractant
    Patient *m_patient;

     /// Llistat de Key Image Notes
    QList<KeyImageNote*> m_KeyImageNotesOfPatient;

    /// Seleccio actual d'imatges
    QList<Image*> m_currentSelection;

    /// Retorna cert si hem buscat els Key Image Notes del Pacient
    bool m_KeyImageNotesOfPatientSearched;
};

}
#endif