/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGDICOMSEQUENCEITEM_H
#define UDGDICOMSEQUENCEITEM_H

#include <QMap>

namespace udg {

class DICOMAttribute;
class DICOMTag;

class DICOMSequenceItem {
public:
    
    DICOMSequenceItem();
    ~DICOMSequenceItem();

    /// Afegeix un DICOMAttribute a la llista d'atributs
    void addAttribute(DICOMAttribute * attribute );

    /// Obté tota la llista de DICOMAttribute
    QList<DICOMAttribute*> getAttributes();

    /// Retorna l'atribut associat al tag passat per parametre. Si no disposa d'ell retorna null.
    DICOMAttribute * getAttribute(DICOMTag tag);

    /// Retorna el contingut de l'item en forma de text. Útil per analitzar el contingut.
    QString toString();

private:

    QMap<QString,DICOMAttribute*> m_attributeList;
};

}

#endif
