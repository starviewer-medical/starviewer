/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGDICOMSEQUENCEATTRIBUTE_H
#define UDGDICOMSEQUENCEATTRIBUTE_H

#include "dicomattribute.h"

#include <QList>

class QString;

namespace udg {

class DICOMSequenceItem;

class DICOMSequenceAttribute : public DICOMAttribute
{

public:
    
    DICOMSequenceAttribute();

    ~DICOMSequenceAttribute();
   
    /// Retorna sempre fals
    virtual bool isValueAttribute();

    /// Retorna sempre cert
    virtual bool isSequenceAttribute();

    /// Afegeix un item a la llista
    void addItem( DICOMSequenceItem * item );

    /// Retorna la llista sencera d'items
    QList<DICOMSequenceItem*> getItems();

    /// Retorna la sequència llegible
    QString toString();

private:

    QList<DICOMSequenceItem*> m_itemList;
};

}

#endif
