/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGLOCALDATABASEDICOMREFERENCEDIMAGEDAL_H
#define UDGLOCALDATABASEDICOMREFERENCEDIMAGEDAL_H

#include <QString>

namespace udg {

class DICOMReferencedImage;
class DicomMask;
class DatabaseConnection;
class Series;

/** Classe que conté els mètodes d'accés a la Taula DICOMReferencedImage
    @author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class LocalDatabaseDICOMReferencedImageDAL
{
public:

    LocalDatabaseDICOMReferencedImageDAL();
    ~LocalDatabaseDICOMReferencedImageDAL();

    /// Insereix un nou DICOMReferencedImage
    // TODO: La serie no s'hauria de passar com a paràmetre, sino una nova classe generica DICOMObject d'on podriem obtenir tota la informació per guardar DICOMReferencedImage a la BD
    void insert(DICOMReferencedImage *newDICOMReferencedImage, Series *seriesOfParentObject);

    /// Esborra els DICOM Referenced Images que compleixin el filtre de la màscara, només es té en compte el l'StudyUID, el SeriesUID i SOPInstanceUID del pare
    void del(const DicomMask &DICOMReferencedImageMaskToDelete);

    /// Cerca els DICOM Referenced Images que compleixen amb els criteris de la màscara de cerca, només té en compte l'StudyUID, el SeriesUID i SOPInstanceUID del pare
    QList<DICOMReferencedImage*> query(const DicomMask &DICOMReferencedImageMaskToQuery);

    /// Retorna l'estat de la última operació realitzada
    int getLastError() const;

    /// Connexió de la base de dades a utilitzar
    void setDatabaseConnection(DatabaseConnection *dbConnection);

private:
    /// Construeix la sentència sql per inserir un nou DICOMReferencedImage
    // TODO: La serie no s'hauria de passar com a paràmetre, sino una nova classe generica DICOMObject d'on podriem obtenir tota la informació per guardar DICOMReferencedImage a la BD
    QString buildSqlInsert(DICOMReferencedImage *newDICOMReferencedImage, Series *seriesOfParentObject);

    /// Ens fa un ErrorLog d'una sentència sql
    void logError(const QString &sqlSentence);

    /// Genera la sentencia Sql per esborrar DICOM Referenced Images, de la màscara només té en compte per construir la sentència el StudyUID, SeriesUID i SOPInstanceUID del pare
    QString buildSqlDelete(const DicomMask &DICOMReferencedImageMaskToDelete);

    /// Genera la sentència del where a partir de la màscara tenint en compte per construir la sentència el SOPInstanceUID 
    QString buildWhereSentence(const DicomMask &DICOMReferencedImageMaskToDelete);

    /// Genera la sentència sql per fer selectes de DICOM Referenced Images, de la màscara només té en compte per construir la sentència el StudyUID, SeriesUID i SOPInstanceUID del pare
    QString buildSqlSelect(const DicomMask &DICOMReferencedImageMaskToDelete);

    /// Emplena el DICOM Referenced Image de la fila passada per paràmetre
    DICOMReferencedImage* fillDICOMReferencedImage(char **reply, int row, int columns);

private:
    /// Connexió a la Base de Dades
    DatabaseConnection *m_dbConnection;

    /// Ultim error de Sqlite
    int m_lastSqliteError;
};

}

#endif