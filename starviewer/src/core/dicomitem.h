/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGDICOMITEM_H
#define UDGDICOMITEM_H

#include <QObject>
#include <QList>

namespace udg {

class DICOMAttribute;

class DICOMItem : public QObject
{
Q_OBJECT
public:
    
    DICOMItem();
    ~DICOMItem();

    void addAttribute(DICOMAttribute * attribute );

private:

    QList<DICOMAttribute*> m_attributeList;
};

}

#endif
