#ifndef UDGDICOMDUMPDEFAULTTAGSREADER_H
#define UDGDICOMDUMPDEFAULTTAGSREADER_H

#include <QXmlSchemaValidator>

class QXmlStreamReader;

namespace udg {

class DICOMTag;
class DICOMDumpDefaultTags;
class DICOMDumpDefaultTagsRestriction;

/**
    Classe encarregada de llegir els fitxers XML que contenen la llista de Default Tags
    que s'han de mostrar en el DICOMDump.
    També és l'encarregada de crear els diferent objectes DICOMDumpDefaultTags.
  */
class DICOMDumpDefaultTagsReader {

public:
    DICOMDumpDefaultTagsReader();

    ~DICOMDumpDefaultTagsReader();

    /// Mètode que llegeix un fitxer XML i retorna un objecte DICOMDumpDefaultTags.
    DICOMDumpDefaultTags* readFile(const QString &path);

private:
    /// Mètode que llegeix un DICOMTag i el retorna
    DICOMTag readTag(QXmlStreamReader *reader);

    /// Mètode que llegeix un DICOMDumpDefaultTagsRestriction i el retorna
    DICOMDumpDefaultTagsRestriction readRestriction(QXmlStreamReader *reader);

private:
    /// Validador que s'utilitza a l'hora de llegir els fitxers
    QXmlSchemaValidator m_defaultTagsSchemaValidator;
};

}

#endif
