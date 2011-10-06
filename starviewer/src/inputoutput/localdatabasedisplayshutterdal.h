#ifndef UDGLOCALDATABASEDISPLAYSHUTTERDAL_H
#define UDGLOCALDATABASEDISPLAYSHUTTERDAL_H

#include "localdatabasebasedal.h"

#include <QList>

namespace udg {

class DicomMask;
class Image;
class DisplayShutter;

/**
    Classe que conté els mètodes d'accés a la taula DisplayShutter de la base de dades
  */
class LocalDatabaseDisplayShutterDAL : public LocalDatabaseBaseDAL {
public:
    LocalDatabaseDisplayShutterDAL(DatabaseConnection *dbConnection);

    /// Insereix un nou DisplayShutter corresponent a la imatge donada
    void insert(const DisplayShutter &shutter, Image *shuttersImage);

    /// Actualitza la llista de DisplayShutters corresponents a la imatge donada
    void update(const QList<DisplayShutter> &shuttersList, Image *shuttersImage);

    /// Esborra els DisplayShutters que coincideixin amb els criteris de la màscara
    void del(const DicomMask &mask);

    /// Cerca els DisplayShutters que coincideixin amb els criteris de la màscara i ens torna els resultats en una llista
    QList<DisplayShutter> query(const DicomMask &mask);

private:
    /// Construeix la sentència SQL per inserir un DisplayShutter
    QString buildSQLInsert(const DisplayShutter &shutter, Image *shuttersImage);

    /// Construeix la sentència SQL per seleccionar els DisplayShutters que coincideixin amb els criteris de la màscara
    QString buildSQLSelect(const DicomMask &mask);

    /// Construeix la sentència SQL per esborrar els DisplayShutters que coincideixin amb els criteris de la màscara
    QString buildSQLDelete(const DicomMask &mask);

    /// Construeix la sentència WHERE segons els criteris de la màscara
    QString buildWhereSentence(const DicomMask &mask);

    /// Omple un objecte DisplayShutter a partir del resultat SQL (reply) segons la fila i columna indicades
    DisplayShutter fillDisplayShutter(char **reply, int row, int columns);
};

} // End namespace udg

#endif
