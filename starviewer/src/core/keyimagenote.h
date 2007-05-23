/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGKEYIMAGENOTE_H
#define UDGKEYIMAGENOTE_H

#include <QString>
#include <QStringList>

namespace udg {

/**
    Classe que representa un objecte DICOM Key Image Note (KIN)
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class KeyImageNote{
public:
    KeyImageNote();

    ~KeyImageNote();

    /// Retorna true si carrega correctament un KIN des del fitxer filename
    bool loadFromFile(const QString &filename);


    QString getDocumentTitle() const;
    void setDocumentTitle(const QString &title);
    bool hasDocumentTitle() const;

    /// Si el document title fa referències a problemes de qualitat indica quin és el motiu
    QString getDocumentTitleQualityReasons() const;
    void setDocumentTitleQualityReasons(const QString &reason);
    bool hasDocumentTitleQualityReasons() const;

    /// Retorna la descripció del Key Object
    QString getKeyObjectDescription() const;
    void setKeyObjectDescription(const QString &description);
    bool hasKeyObjectDescription() const;

    /// Retorna el Observable context si en té
    QString getObservableContext() const;
    void setObservableContext(const QString &context);
    bool hasObservableContext() const;

    /// Retorna tots els atributs de texte composats en un sol string
    QString getPrintableText() const;

    /// Retorna la llista de UID SOP Instances a les que s'aplica el KIN
    QStringList getReferencedSOPInstancesUID() const;
    void setReferencedSOPInstancesUID(const QStringList &list); 

private:
    QString m_keyObjectDescription;
    QString m_documentTitle;
    QString m_observableContext;
    QString m_documentTitleQualityReason;
    QStringList m_referencedSOPInstancesUIDList;
};

}

#endif
