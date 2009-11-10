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

namespace udg {

class DICOMItem;

class DICOMSequenceAttribute : public DICOMAttribute
{

public:
    
    DICOMSequenceAttribute();

    ~DICOMSequenceAttribute();
   
    bool isValueAttribute();

    bool isSequenceAttribute();

    void addItem( DICOMItem * item );

private:

    QList<DICOMItem*> m_itemList;
};

}

#endif
