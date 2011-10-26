#ifndef LOCALDATABASEPACSDEVICEDAL_H
#define LOCALDATABASEPACSDEVICEDAL_H

#include "localdatabasebasedal.h"

#include <QtGlobal>

namespace udg {

class DatabaseConnection;
class PacsDevice;

/**
  Classe DAL que gestionar la taula PACSRetrievedImages
  */
class LocalDatabasePACSRetrievedImagesDAL : public LocalDatabaseBaseDAL
{

public:

    /// Constructor de la classe
    LocalDatabasePACSRetrievedImagesDAL(DatabaseConnection *dbConnection);

    /// Retorna un PacsDevice a partir del seu ID a la base de dades
    PacsDevice query(const qlonglong &IDPacsInDatabase);

    /// Retorna un PacsDevice amb el AETitle, address i queryPort passats per paràmetre
    PacsDevice query(const QString AETitle, const QString address, int queryPort);

    /// Inserta el pacs a la base de dades, Si el PACS ja existeix o s'ha produït algun error al inserir retorna -1.
    qlonglong insert(const PacsDevice &pacsDevice);

private:

    /// Retorna el PACSDevice que consulti el select passat per paràmetre
    PacsDevice query(const QString &sqlQuerySentence);

    /// A partir de les dades retornades d'una consulta s'emplena un objecte PACSDevice
    PacsDevice fillPACSDevice(char **reply, int row, int columns);

    /// Construeix sentència per inserir el PACS passat per paràmetre
    QString buildSqlInsert(const PacsDevice &pacsDevice);

    /// Construeix sentència per consultar els PACS de la base de dades sense cap clausula where.
    QString buildSqlSelect();

    /// Construeix sentència per consultar el PACS amb l'ID passat per paràmetre
    QString buildSqlSelect(const qlonglong &IDPACSInDatabase);

    /// Construeix sentència per consultar un PACS a partir dels paràmetres passats
    QString buildSqlSelect(const QString AETitle, const QString address, int queryPort);
};
}
#endif // LOCALDATABASEPACSDEVICEDAL_H
