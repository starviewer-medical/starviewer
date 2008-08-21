/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGLOCALDATABASEIMAGEDAL_H
#define UDGLOCALDATABASEIMAGEDAL_H

#include <QString>
#include <QList>

#include "image.h"


namespace udg {

class DicomMask;
class DatabaseConnection;

/** Aquesta classe conté els mètodes per operar amb l'objecte image en la caché de l'aplicació
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/

class LocalDatabaseImageDAL
{
public:

    LocalDatabaseImageDAL();

    /** Insereix la informació d'una imatge a la caché, actualitzamt l'espai ocupat de la pool, com s'ha de fer un insert i un update aquests dos operacions es fan dins el marc d'una transaccio, per mantenir coherent l'espai de la pool ocupat. Per això tot i que accedim a dos taules, al haver-se de fer dins el marc d'una transacció, necessitem fer-les
    dins el mateix mètode. Ja que sinó ens podríem trobar que altres operacions entressin entre insertar la imatge i updatar la pool i quedessin incloses dins la tx
     * @param dades de la imatge
     * @return retorna estat del mètode
     */
    void insert(Image *newImage, int orderNumberInSeries);

    /// Esborra les imatges que compleixin el filtre de la màscara, només es té en compte l'StudyUID, SeriesUID i SOPInstanceUID
    void deleteImage(DicomMask imageMaskToDelete);

    ///Actualitza la imatge passada per paràmetre
    void update(Image *imageToUpdate, int orderNumberInSeries);

    ///Selecciona les imatges que compleixen la màscara, només es té en compte de la màscara el StudyUID, SeriesUID i SOPInstanceUID
    QList<Image*> query(DicomMask imageMaskToSelect);

    ///Indiquem quina és la connexió a la base de dades a utilitzar
    void setDatabaseConnection(DatabaseConnection *dbConnection);

    ///Retorna el codi de l'últim error que s'ha produït
    int getLastError();

    ~LocalDatabaseImageDAL();

private :

    DatabaseConnection *m_dbConnection;

    int m_lastSqliteError;

    double m_imageOrientationPatient[6];
    double m_pixelSpacing[2];
    double m_patientPosition[3];


    ///Emplena un l'objecte imatge de la columna passada per paràmetre
    Image *fillImage(char **reply, int row, int columns);

    ///Genera la sentència sql per fer selectes d'imatges, de la màscara només té en compte per construir la sentència el StudyUID, SeriesUID i SOPInstanceUID 
    QString buildSqlSelect(DicomMask imageMaskToSelect);

    ///Genera la sentència sql per inserir la nova imatge a la base de dades
    QString buildSqlInsert(Image *newImage, int OrderNumberInSeries);

    ///Genera la sentència sql per updatar la imatge a la base de dades
    QString buildSqlUpdate(Image *imageToUpdate, int OrderNumberInSeries);

    ///Genera la sentencia Sql per esborrar Imatges, de la màscara només té en compte per construir la sentència el StudyUID, SeriesUID i SOPInstanceUID 
    QString buildSqlDelete(DicomMask imageMaskToDelete);

    ///Genera la sentència del where a partir de la màscara tenint en compte per construir la sentència el StudyUID, SeriesUID i SOPInstanceUID 
    QString buildWhereSentence(DicomMask imageMask);

    ///Retorna el Pixel Spacing en format d'string separat per "\\"
    QString getPixelSpacingAsQString(Image *newImage);
    ///Retorna el Pixel spacing en format de double
    double* getPixelSpacingAsDouble(QString pixelSpacing);

    ///Retorna l'ImageOrientationPatient en format d'string separat per "\\";
    QString getImageOrientationPatientAsQString(Image *newImage);
    ///Retorna l'ImageOrientationPatient  en format de double
    double* getImageOrientationPatientAsDouble(QString ImageOrientationPatient);

    ///Retorna l'imagePatientPosition en format d'string separat per "\\";
    QString getPatientPositionAsQString(Image *newImage);
    ///Retorna el Patient Position en format double
    double* getPatientPositionAsDouble(QString patientPosition);

    ///Retorna el WindowWidth en format d'string separats per "\\";
    QString getWindowWidthAsQString(Image *newImage);

    ///Retorna el WindowWidth en format d'string separats per "\\";
    QString getWindowCenterAsQString(Image *newImage);

    ///Retorna el WindowLevelExplanation en format d'string separats per "\\";
    QString getWindowLevelExplanationAsQString(Image *newImage);

    ///Retorna el WindowLevelExplanation en una llista d'strings
    QStringList getWindowLevelExplanationAsQStringList(QString explanationList);

    ///Emplena el windowlevel de la imatge
    void setWindowLevel(Image *selectedImage, QString windowLevelWidth, QString windowLevelCenter);

    ///Ens fa un ErrorLog d'una sentència sql
    void logError(QString sqlSentence);
};

}

#endif
