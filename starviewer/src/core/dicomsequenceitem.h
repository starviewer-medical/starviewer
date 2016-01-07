/*************************************************************************************
  Copyright (C) 2014 Laboratori de Gràfics i Imatge, Universitat de Girona &
  Institut de Diagnòstic per la Imatge.
  Girona 2014. All rights reserved.
  http://starviewer.udg.edu

  This file is part of the Starviewer (Medical Imaging Software) open source project.
  It is subject to the license terms in the LICENSE file found in the top-level
  directory of this distribution and at http://starviewer.udg.edu/license. No part of
  the Starviewer (Medical Imaging Software) open source project, including this file,
  may be copied, modified, propagated, or distributed except according to the
  terms contained in the LICENSE file.
 *************************************************************************************/

#ifndef UDGDICOMSEQUENCEITEM_H
#define UDGDICOMSEQUENCEITEM_H

#include <QMap>

namespace udg {

class DICOMAttribute;
class DICOMValueAttribute;
class DICOMSequenceAttribute;
class DICOMTag;

/**
    Classe encarregada de representar els Items de les seqüències de l'estàndard DICOM. Per treballar amb seqüències s'ha de fer ús de la classe
    \c DICOMSequenceAttribute.
  */
class DICOMSequenceItem {

public:
    DICOMSequenceItem();
    ~DICOMSequenceItem();

    /// Afegeix un DICOMAttribute a la llista d'atributs
    void addAttribute(DICOMAttribute *attribute);

    /// Obté tota la llista de DICOMAttribute
    QList<DICOMAttribute*> getAttributes();

    /// Returns true if this sequence item contains the given tag and false otherwise.
    bool hasAttribute(const DICOMTag &tag) const;

    /// Retorna l'atribut associat al tag passat per parametre. Si no disposa d'ell retorna null.
    DICOMAttribute* getAttribute(const DICOMTag &tag);

    /// Retorna l'atribut associat al tag passat per paràmetre.
    /// Si no disposa d'ell o el tag no és un valor retorna null.
    DICOMValueAttribute* getValueAttribute(const DICOMTag &tag);

    /// Retorna l'atribut associat al tag passat per paràmetre.
    /// Si no disposa d'ell o el tag no és un retorna null.
    DICOMSequenceAttribute* getSequenceAttribute(const DICOMTag &tag);

    /// Retorna el contingut de l'item en forma de text. Útil per analitzar el contingut.
    QString toString();

private:
    /// Atribut per emmagatzemar els artributs que conté l'item. S'utilitza un QMap per optimitzar la cerca d'atributs.
    QMap<QString, DICOMAttribute*> m_attributeList;
};

}

#endif
