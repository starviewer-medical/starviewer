#ifndef UDGKEYIMAGENOTEMANAGER_H
#define UDGKEYIMAGENOTEMANAGER_H

#include <QObject>

namespace udg {

class Patient;
class Image;
class KeyImageNote;
class Series;

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

    /// Retorna una llista de tots els Key Image Notes del Pacient en els que hi ha referenciada la imatge
    static QList<KeyImageNote*> KeyImageNoteManager::getKeyImageNotesWhereImageIsReferenced(Patient *patient, Image *image);

    /// Obte les imatges del Key Image Note disponibles i emet un senyal perque siguin visualitzades
    void showKeyImageNote(KeyImageNote *keyImageNote);

public slots:
    /// Crea un Key Image Note donat d'alta per l'usuari
    void generateAndStoreNewKeyImageNote(const QString &documentTitle, const QString &documentTitleQualityReasons, const QString &observerName, const QString &keyObjectDescription, bool storeToLocalDataBase, bool storeToPacs, const QString &pacsNode);

    /// Busquem la imatge segons els parametres i enviem un senyal per a canviar la llesca que ens indica la imatge
    void changeCurrentDisplayedImage(const QString &seriesInstanceUID, const QString &imageInstanceUID);

signals:
    /// Senyala que s'ha afegit una nova imatge a la seleccio
    void imageAddedToTheCurrentSelectionOfImages(Image *image);

    /// Senyala que s'ha netejat la seleccio actual
    void currentSelectionCleared();

    /// Senyala que s'ha afegit un Key Image Note a l'estudi
    void keyImageNoteOfPatientAdded(KeyImageNote *keyImageNote);

    /// Assenyala que cal posicionar-nos a la llesca
    void changeCurrentSlice(int sliceNumber);

    /// Indica que cal mostrar les imatges d'un Key Image Note
    void showImagesReferencedInKeyImageNote(QList<Image*> referencedImage);

private:
    /// Cerca de tots els Key Image Notes
    static QList<KeyImageNote*> searchKeyImageNotes(Patient *patient);

    /// Crea un nou Key Image Note a partir de les dades de parametre.
    KeyImageNote* createNewKeyImageNote(const QString &documentTitle, const QString &documentTitleQualityReasons, const QString &observerName, const QString &keyObjectDescription);

    /// Crea una nova serie per a guardar un Key Image Note
    Series* createNewKeyImageNoteSeries();

    /// Guarda a la BD local la serie creada amb el nou Key Image Note
    void storeKeyImageNoteSeriesToLocalDataBase(Series *newKeyImageNoteSeries);

    /// Retorna cert si totes les imatges seleccionades formen part del mateix estudi.
    bool allImagesInTheSameStudy();

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