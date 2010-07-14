/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGLOCALDATABASEKEYIMAGENOTEDAL_H
#define UDGLOCALDATABASEKEYIMAGENOTEDAL_H

#include <QString>

namespace udg {

class KeyImageNote;
class DicomMask;
class DatabaseConnection;

/** Classe que conté els mètodes d'accés a la Taula KeyImageNote
    @author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class LocalDatabaseKeyImageNoteDAL
{
public:
    LocalDatabaseKeyImageNoteDAL();
    ~LocalDatabaseKeyImageNoteDAL();
    
    /// Insereix un nou Key Image Note
    void insert(KeyImageNote *newKeyImageNote);

    /// Actualitza un Key Image Note
    void update(KeyImageNote *keyImageNoteToUpdate);

    /// Esborra els Key Image Notes que compleixin el filtre de la màscara, només es té en compte l'StudyUID, SeriesUID i SOPInstanceUID
    void del(const DicomMask &keyImageNoteMaskToDelete);

    /// Cerca els Key Image Notes que compleixen amb els criteris de la màscara de cerca, només té en compte l'StudyUID, el SeriesUID i SOPInstanceUID
    QList<KeyImageNote*> query(const DicomMask &keyImageNoteMaskToQuery);
    
    /// Retorna l'estat de la última operació realitzada
    int getLastError() const;

    /// Connexió de la base de dades a utilitzar
    void setDatabaseConnection(DatabaseConnection *dbConnection);

private:
    /// Construeix la sentència sql per inserir el nou Key Image Note
    QString buildSqlInsert(KeyImageNote *newKeyImageNote);

    /// Construeix la sentència sql per actualitzar un Key Image Note
    QString buildSqlUpdate(KeyImageNote *keyImageNoteToUpdate);

    /// Genera la sentencia Sql per esborrar Key Image Notes, de la màscara només té en compte per construir la sentència el StudyUID, SeriesUID i SOPInstanceUID 
    QString buildSqlDelete(const DicomMask &keyImageNoteMaskToDelete);

    /// Genera la sentència sql per fer selectes de Key Image Notes, de la màscara només té en compte per construir la sentència el StudyUID, SeriesUID i SOPInstanceUID 
    QString buildSqlSelect(const DicomMask &keyImageNoteMaskToSelect);

    /// Genera la sentència del where a partir de la màscara tenint en compte per construir la sentència el StudyUID, SeriesUID i SOPInstanceUID 
    QString buildWhereSentence(const DicomMask &keyImageNoteMask);

    /// Emplena el Key Image Note de la fila passada per paràmetre
    KeyImageNote* fillKeyImageNote(char **reply, int row, int columns);

    /// Ens fa un ErrorLog d'una sentència sql
    void logError(const QString &sqlSentence);

private:
    /// Connexió a la Base de Dades
    DatabaseConnection *m_dbConnection;

    /// Ultim error de Sqlite
    int m_lastSqliteError;

};

}

#endif