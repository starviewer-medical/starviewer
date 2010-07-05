#ifndef UDGKEYIMAGENOTEMANAGER_H
#define UDGKEYIMAGENOTEMANAGER_H

#include <QObject>

namespace udg {

class Patient;
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
    
    /// Inicialitza el Key Image Note Manager
    void initialize();

    /// Retorna tots els Key Image Notes que tenim
    QList<KeyImageNote*> getKeyImageNotes() const;
 
private:
    /// Cerca de tots els Key Image Notes
    void searchKeyImageNotes();

private:
    /// El pacient que estem tractant
    Patient *m_patient;

     /// Llistat de Key Image Notes
    QList<KeyImageNote*> m_kinSet;
};

}
#endif