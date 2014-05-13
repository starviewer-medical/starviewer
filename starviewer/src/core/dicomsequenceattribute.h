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

#ifndef UDGDICOMSEQUENCEATTRIBUTE_H
#define UDGDICOMSEQUENCEATTRIBUTE_H

#include "dicomattribute.h"

#include <QList>

class QString;

namespace udg {

class DICOMSequenceItem;

/**
    Classe heredada de \class DICOMAttribute encarregada de representar el atributs DICOM de tipus seqüència. La seva utilització va molt lligada amb
    \class DICOMSequenceItem.
  */
class DICOMSequenceAttribute : public DICOMAttribute {

public:
    DICOMSequenceAttribute();

    ~DICOMSequenceAttribute();

    /// Retorna sempre fals
    virtual bool isValueAttribute();

    /// Retorna sempre cert
    virtual bool isSequenceAttribute();

    /// Afegeix un item a la llista
    void addItem(DICOMSequenceItem *item);

    /// Retorna la llista sencera d'items
    QList<DICOMSequenceItem*> getItems();

    /// Retorna la sequència llegible
    QString toString(bool verbose = false);

private:
    /// Atribut que representa la llista d'items que té la seqüència.
    QList<DICOMSequenceItem*> m_itemList;
};

}

#endif
