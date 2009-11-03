/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGDICOMATTRIBUTE_H
#define UDGDICOMATTRIBUTE_H

#include <QObject>
#include <QString>

namespace udg {

class DICOMTag;

class DICOMAttribute : public QObject
{
Q_OBJECT
public:
    
    DICOMAttribute();

    ~DICOMAttribute();

    void setTag( DICOMTag * tag );

    DICOMTag * getTag();

    QString getValueRepresentation();

    virtual bool isValueAttribute();

    virtual bool isSequenceAttribute();

protected:

    void setValueRepresentation( QString value );

private:

    DICOMTag * m_tag;

    QString m_valueRepresentation;
};

}

#endif
