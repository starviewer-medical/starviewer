/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGDICOMATTRIBUTE_H
#define UDGDICOMATTRIBUTE_H

class QString;

namespace udg {

class DICOMTag;

class DICOMAttribute {

public:
    DICOMAttribute();

    virtual ~DICOMAttribute();

    void setTag(DICOMTag tag);

    DICOMTag* getTag();

    virtual bool isValueAttribute() = 0;

    virtual bool isSequenceAttribute() = 0;

    virtual QString toString() = 0;

private:
    DICOMTag *m_tag;

};

}

#endif
