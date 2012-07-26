#ifndef UDGCUSTOMWINDOWLEVELSREADER_H
#define UDGCUSTOMWINDOWLEVELSREADER_H

#include <QXmlSchemaValidator>
#include <QList>

class QXmlStreamReader;

namespace udg {

class WindowLevel;

/**
    Classe encarregada de llegir els fitxers XML que contenen la llista de custom window levels.
    També és l'encarregada de crear els diferent objectes WindowLevel.
  */
class CustomWindowLevelsReader {

public:
    CustomWindowLevelsReader();

    ~CustomWindowLevelsReader();

    /// Mètode que llegeix un fitxer XML i retorna un objecte WindowLevel.
    QList<WindowLevel*> readFile(const QString &path);

private:
    /// Validador que s'utilitza a l'hora de llegir els fitxers
    QXmlSchemaValidator m_customWindowLevelsSchemaValidator;
};

}

#endif
