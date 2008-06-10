/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGKEYIMAGENOTEFILEREADER_H
#define UDGKEYIMAGENOTEFILEREADER_H

#include <QString>

class DSRDocumentTree;

namespace udg {

class KeyImageNote;

/**
Classe que ens permet llegei un objecte DICOM Key Image Note (KIN) des d'un fitxer.

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class KeyImageNoteFileReader{
public:
    KeyImageNoteFileReader();
    KeyImageNoteFileReader(const QString &filename);

    ~KeyImageNoteFileReader();

    /// Especifica el nom del fitxer del que es llegirà el Key Image Note
    void setFilename(const QString &filename);

    /// Intenta llegir el Key Image Note. Si no el pot llegir retorna NULL
    KeyImageNote* read();

    /// Intenta llegir el Key Image Note del fitxer filename. Si no el pot llegir retorna NULL
    KeyImageNote* read(const QString &filename);

    /// Retorna una descprició de l'últim error que s'ha produït al llegir un fitxer
    QString getLastErrorDescription() const;
private:
    QString searchDocumentTitle(DSRDocumentTree &tree);
    QString searchQualityExplanationDocumentTitle(DSRDocumentTree &tree);
    QString searchObservableContext(DSRDocumentTree &tree);
    QString searchObjectDescription(DSRDocumentTree &tree);

private:
    QString m_filename;
    QString m_lastErrorDescription;
};

}

#endif
