#ifndef UDGCUSTOMWINDOWLEVELSLOADER_H
#define UDGCUSTOMWINDOWLEVELSLOADER_H

#include <QFileInfo>
#include <QStringList>

namespace udg {

/**
   Classe encarregada de recuperar els arxius XML que contenen la informació referent als custom window levels
*/

class CustomWindowLevelsLoader {

public:    
    CustomWindowLevelsLoader();

    ~CustomWindowLevelsLoader();

    /// Mètode que carrega els arxius XML definits a una adreça per defecte. (Hardcode)
    void loadDefaults();

    /// Mètode que carrega els arxius XML que contenen la informació dels custom window levels. El paràmetre pot ser el path d'un Directori o Fitxer.
    void loadXMLFiles(const QString &path);

private:
    /// Mètodes privats per fer la lectura dels fitxers de custom window levels
    /// Retorna una llista amb els fitxers llegits
    QStringList loadXMLFiles(const QFileInfo &fileInfo);
    /// Retorna el nom del fitxer llegit
    QString loadXMLFile(const QFileInfo &fileInfo);
};

}

#endif
