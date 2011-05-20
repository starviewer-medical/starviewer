#ifndef UDGDICOMDUMPDEFAULTTAGSLOADER_H
#define UDGDICOMDUMPDEFAULTTAGSLOADER_H

#include <QFileInfo>
#include <QStringList>

namespace udg {

/**
    Classe encarregada de recuperar els arxius XML que contenen la informació referent als Default Tags que s'han
    d'utilitzar pel DICOMDump.
  */
class DICOMDumpDefaultTagsLoader {

public:
    DICOMDumpDefaultTagsLoader();

    ~DICOMDumpDefaultTagsLoader();

    /// Mètode que carrega els arxius XML definits a una adreça per defecte. (Hardcode)
    void loadDefaults();

    /// Mètode que carrega els arxius XML que contenen la informació dels Tags. El paràmetre pot ser el path d'un Directori o Fitxer.
    void loadXMLFiles(const QString &path);

private:
    /// Mètodes privats per fer la lectura dels fitxers de default tags
    QStringList loadXMLFiles(const QFileInfo &fileInfo);
    QString loadXMLFile(const QFileInfo &fileInfo);
};

}

#endif
