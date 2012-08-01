#ifndef UDGLOCALDATABASEIMAGEDAL_H
#define UDGLOCALDATABASEIMAGEDAL_H

#include <QString>
#include <QList>
#include <QHash>

#include "localdatabasebasedal.h"
#include "image.h"

namespace udg {

class DicomMask;
class PacsDevice;
class DICOMSource;

/**
    Aquesta classe conté els mètodes per operar amb l'objecte image en la caché de l'aplicació
  */
class LocalDatabaseImageDAL : public LocalDatabaseBaseDAL {
public:
    LocalDatabaseImageDAL(DatabaseConnection *dbConnection);

    /// Insereix la informació d'una imatge a la caché, actualitzamt l'espai ocupat de la pool, com s'ha de fer un insert i un update aquests dos operacion
    /// es fan dins el marc d'una transaccio, per mantenir coherent l'espai de la pool ocupat. Per això tot i que accedim a dos taules, al haver-se de fer
    /// dins el marc d'una transacció, necessitem fer-les dins el mateix mètode. Ja que sinó ens podríem trobar que altres operacions entressin entre insertar
    /// la imatge i updatar la pool i quedessin incloses dins la tx
    /// @param dades de la imatge
    /// @return retorna estat del mètode
    void insert(Image *newImage);

    /// Esborra les imatges que compleixin el filtre de la màscara, només es té en compte l'StudyUID, SeriesUID i SOPInstanceUID
    void del(const DicomMask &imageMaskToDelete);

    /// Actualitza la imatge passada per paràmetre
    void update(Image *imageToUpdate);

    /// Selecciona les imatges que compleixen la màscara, només es té en compte de la màscara el StudyUID, SeriesUID i SOPInstanceUID
    QList<Image*> query(const DicomMask &imageMaskToSelect);

    /// Compta les imatges que compleixin el filtre de la màscara, només es té en compte l'StudyUID, SeriesUID i SOPInstanceUID
    int count(const DicomMask &imageMaskToCount);

private:
    double m_imageOrientationPatient[6];
    double m_pixelSpacing[2];
    double m_patientPosition[3];

    //Utilitzem aquest QHash com una cache per fer la tradució de la cadena IP+AETitle a ID de PACS per quan inserim/actualitzem imatges
    QHash<QString, QString> m_PACSIDCache;
    //Utilitzem aquest QHash com una cache per fer la traducció del IDPACS a PacsDevice per quan es consulten imatges, d'aquesta manera si ja hem
    //recuperat un pacs amb un ID determinat no farà falta tornar a accedir a la base de dades per obtenir-ne les dades.
    QHash<int, PacsDevice> m_PACSDeviceCacheByIDPACSInDatabase;

    /// Emplena un l'objecte imatge de la fila passada per paràmetre
    Image* fillImage(char **reply, int row, int columns);

    /// Genera la sentència sql per fer selectes d'imatges, de la màscara només té en compte per construir la sentència el StudyUID, SeriesUID i SOPInstanceUID
    QString buildSqlSelect(const DicomMask &imageMaskToSelect);

    /// Genera la sentència sql per comptar número d'imatges, de la màscara només té en compte per construir la sentència el StudyUID, SeriesUID i
    /// SOPInstanceUID
    QString buildSqlSelectCountImages(const DicomMask &imageMaskToSelect);

    /// Genera la sentència sql per inserir la nova imatge a la base de dades
    QString buildSqlInsert(Image *newImage);

    /// Genera la sentència sql per updatar la imatge a la base de dades
    QString buildSqlUpdate(Image *imageToUpdate);

    /// Genera la sentencia Sql per esborrar Imatges, de la màscara només té en compte per construir la sentència el StudyUID, SeriesUID i SOPInstanceUID
    QString buildSqlDelete(const DicomMask &imageMaskToDelete);

    /// Genera la sentència del where a partir de la màscara tenint en compte per construir la sentència el StudyUID, SeriesUID i SOPInstanceUID
    QString buildWhereSentence(const DicomMask &imageMask);

    /// Retorna el Pixel Spacing en format d'string separat per "\\"
    QString getPixelSpacingAsQString(Image *newImage);
    /// Retorna el Pixel spacing en format de double
    double* getPixelSpacingAsDouble(const QString &pixelSpacing);

    /// Retorna l'imagePatientPosition en format d'string separat per "\\";
    QString getPatientPositionAsQString(Image *newImage);
    /// Retorna el Patient Position en format double
    double* getPatientPositionAsDouble(const QString &patientPosition);

    /// Retorna el WindowWidth, WindowCenter i explanation en format d'string separats per "\\" en els strings passats per paràmetre;
    void getWindowLevelInformationAsQString(Image *newImage, QString &windowWidth, QString &windowCenter, QString &explanation);

    /// Retorna el WindowLevelExplanation en una llista d'strings
    QStringList getWindowLevelExplanationAsQStringList(const QString &explanationList);

    /// Si el DICOMSource conté un PACS retorna l'ID d'aquest a la base de dades, (si no existeix l'insereix)
    /// Si el DICOMSource té més d'un PACS només es té en compte el primer, una imatge no hauria de tenir més d'un PACS com a DICOMSource
    /// i si el DICOMSource no conté  cap PACS retorna string contenint la paraula null.
    QString getIDPACSInDatabaseFromDICOMSource(DICOMSource DICOMSourceRetrievedImage);

    /// Obté el ID del PACS a la base de ades sinó existeix li insereix.
    /// Guarda una cache dels PACS consultats i inserits de manera que si ja s'ha demanat el ID d'un PACS el va a buscar directament a la caché
    QString getIDPACSInDatabase(PacsDevice pacsDevice);

    /// A partir del camp retrievedPACSID de la base de dades ens omple el DICOMSource de la imatge
    DICOMSource getImageDICOMSourceByIDPACSInDatabase(const QString &retrievedPACSID);

    /// Retorna un PACSDevice a partir del seu ID a la base de dades, sinó el troba retorna un pacs buit
    /// Guarda en una caché el PACS consultats de manere que si es torna demana un PACS ja consultat anteriorment l'obté directament de la caché
    PacsDevice getPACSDeviceByIDPACSInDatabase(int IDPACSInDatabase);

};

}

#endif
