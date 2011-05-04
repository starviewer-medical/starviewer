#ifndef UDGDICOMDUMPDEFAULTTAGS_H
#define UDGDICOMDUMPDEFAULTTAGS_H

#include "dicomtag.h"
#include "dicomdumpdefaulttagsrestriction.h"
#include <QString>
#include <QList>

namespace udg {

/**
   Classe que ens representa el Mappeig d'un fitxer que conté la llista de DefaulTags utilitzat pel DICOMDump
*/

class DICOMTag;

class DICOMDumpDefaultTags {

public:
    DICOMDumpDefaultTags();

    ~DICOMDumpDefaultTags();

    /// Mètode per definir el SOPClassUID
    void setSOPClassUID(const QString &sopClassUid);

    /// Mètode que retorna el SOPClassUID
    QString getSOPClassUID() const;

    /// Mètode per afegir una restricció a la llista de restriccions
    void addRestriction(const DICOMDumpDefaultTagsRestriction &restriction);

    /// Mètode per definir la llista de restriccions d'un DICOMDumpDefaultTags
    void addRestrictions(const QList<DICOMDumpDefaultTagsRestriction> &restrictionsList);

    /// Mètode que retorna la llista de restriccions d'un DICOMDumpDefaultTags
    QList<DICOMDumpDefaultTagsRestriction> getRestrictions() const;

    /// Mètode que retorna el número de restriccions que té definides un DICOMDumpDefaultTags
    int getNumberOfRestrictions() const;

    /// Mètode per afegir un DICOMTag a la llista de DICOMTags per mostrar
    void addTagToShow(const DICOMTag &dicomTag);

    /// Mètode per afegir la llista de Tags que s'han de visualitzar per defecte en un DICOMDumpDefaultTags
    void addTagsToShow(const QList<DICOMTag> &tagsToShow);

    /// Retorna la llista de Tags que s'ha de visualitzar per defecte per el SOPClassUID definit
    QList<DICOMTag> getTagsToShow() const;

private:
    QString m_SOPClassUID;
    QList<DICOMDumpDefaultTagsRestriction> m_restrictions;
    QList<DICOMTag> m_tagsToShow;
};

}

#endif
